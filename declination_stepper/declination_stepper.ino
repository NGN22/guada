#include <config.h>
#include <ds3231.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Stepper.h>

const int STEPS_PER_REV = 32;
const int GEAR_REDUCTION = 64;

const int STEPS_PER_OUTER_REV = STEPS_PER_REV * GEAR_REDUCTION;
int search_speed = 200;
long ascencion_ms;
int total_steps_motorDec;
int total_steps_motorRA;
unsigned long position;
struct ts t;

Stepper motorDec(STEPS_PER_REV, 8, 10, 9, 11);
Stepper motorRA(STEPS_PER_REV,4,6,5,7);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_INTCN);
 

  motorDec.setSpeed(search_speed);
  motorRA.setSpeed(search_speed);
  Init_motorDec();
  Init_motorRA();
  apuntar_estrella();
  position=ascencion_ms;
}

void loop() {
 //readTime();
 //mov_motorDec();
 //mov_motorRA();
 hora_sid_ms();
 tracking();
 Write_EEPROM(total_steps_motorDec,0);
 Write_EEPROM(total_steps_motorRA,2);
}
void readTime()
{
    DS3231_get(&t);
  Serial.print("date: ");
  Serial.print(t.mday);
  Serial.print("/");
  Serial.print(t.mon);
  Serial.print("/");
  Serial.println(t.year);
  Serial.print("Heure :");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(".");
  Serial.println(t.sec);

  delay(500);
  
}
void Initializacion(){
t.hour=16;
t.min=21;
t.sec=0;
t.mday=8;
t.mon=05;
t.year=2019;
DS3231_set(t);
Serial.println("init finished");
}
void mov_motorDec()
{
  Serial.println("number of degres 1: ");
  while(Serial.available()<=0){}
      
     long angle = Serial.readString().toInt();
     long steps = STEPS_PER_OUTER_REV * angle / 360;
    
     Serial.print("steps1:");
     Serial.println(steps);
     motorDec.step(steps);
     total_steps_motorDec=total_steps_motorDec+steps;
     if(total_steps_motorDec<0)
     {
      total_steps_motorDec=2048+total_steps_motorDec;
     }
     
  
}
void mov_motorRA(){
 Serial.println("number of degres 2: ");
  while(Serial.available()<=0){}
      
     long angle2 = Serial.readString().toInt();
     long steps2 = STEPS_PER_OUTER_REV * angle2 / 360;
    
     Serial.print("steps2:");
     Serial.println(steps2);
     motorRA.step(steps2);
     total_steps_motorRA=total_steps_motorRA+steps2;
     if(total_steps_motorRA<0)
     {
      total_steps_motorRA=2048+total_steps_motorRA;
     }
     
    Write_EEPROM(total_steps_motorRA,2);
    Serial.print("memoire:");
    Serial.println(Read_EEPROM(2));

}
void Init_motorDec()
{
  int init_steps_motorDec=Read_EEPROM(0);
  if(init_steps_motorDec>2048){
    init_steps_motorDec=init_steps_motorDec-2048;
  }
  Serial.print("Init_steps: ");
  Serial.println(init_steps_motorDec);
  if(init_steps_motorDec!=0)
  {
    motorDec.step(-init_steps_motorDec);
  }
  Write_EEPROM(0,0);
}
void Init_motorRA(){
  int init_steps_motorRA =Read_EEPROM(2);
  if(init_steps_motorRA>2048){
    init_steps_motorRA=init_steps_motorRA-2048;
  }
  Serial.print("Init_steps2: ");
  Serial.println(init_steps_motorRA);
  if(init_steps_motorRA!=0)
  {
    motorRA.step(-init_steps_motorRA);
  }
  Write_EEPROM(0,2);
}
void apuntar_estrella(){
  Serial.println("Ascencion");
  Serial.print("Horas:");
  while(Serial.available()<=0){}
  long horas = Serial.readString().toInt();
  Serial.println(horas);
  Serial.print("Minutos:");
  while(Serial.available()<=0){}
  long minutos = Serial.readString().toInt();
  Serial.println(minutos);
  Serial.print("Segundos:");
  while(Serial.available()<=0){}
  long segundos = Serial.readString().toInt();
  Serial.println(segundos);
  unsigned long horas_ms=horas*3600000;
  unsigned long minutos_ms=minutos*60000;
  unsigned long segundos_ms=segundos*1000;
  Serial.print("ascencion_ms");
  ascencion_ms =horas_ms+minutos_ms+segundos_ms;
  Serial.println(ascencion_ms);
  unsigned long mov_steps = ascencion_ms/42072;
  Serial.print("mov_steps :");
  Serial.println(mov_steps);
  motorRA.step(mov_steps);
  float mem_step2=mov_steps/8;
  EEPROM.write(2,mem_step2);
  Serial.print("memoire:");
  Serial.println(EEPROM.read(2));
  total_steps_motorRA=total_steps_motorRA+mov_steps;
  

  Serial.println("Declinacion: ");
  Serial.print("Grados: ");
  while(Serial.available()<=0){}
  int grados = Serial.readString().toInt();
  Serial.println(grados);
  Serial.print("Minutos: ");
  while(Serial.available()<=0){}
  int min_grados = Serial.readString().toInt();
  Serial.println(min_grados);
  Serial.print("Segundos: ");
  while(Serial.available()<=0){}
  int sec_grados = Serial.readString().toInt();
  Serial.println(sec_grados);
  min_grados = min_grados/60;
  sec_grados=sec_grados/3600;
  grados=grados+min_grados+sec_grados;
  Serial.print("total grados: ");
  Serial.print(grados);
  int grados_step= grados/0.17578125;
  Serial.print("step: ");
  Serial.print(grados_step);
  motorDec.step(grados_step);
  float mem_step=grados_step/8;
  EEPROM.write(1,mem_step);
  Serial.print("memoire:");
  Serial.println(EEPROM.read(1));
  total_steps_motorDec=total_steps_motorDec+grados_step;
  
}
unsigned long hora_sid_ms(){
  DS3231_get(&t);
  unsigned long horas_ms=t.hour*3600000;
  unsigned long minutos_ms=t.min*60000;
  unsigned long segundos_ms=t.sec*1000;
  unsigned long hora_ms =horas_ms+minutos_ms+segundos_ms;
  //Serial.print("hora_sideral_ms");
  unsigned long hora_sideral_ms=hora_ms*0.9972695833;
  return (unsigned long) hora_sideral_ms;
}

void tracking (){
  unsigned long time=hora_sid_ms();
  if(time-position>42072)
  {
    motorRA.step(1);
    position=position+42072;
    total_steps_motorRA=total_steps_motorRA+1;
  }
  if(position>=time){
    position=time;
    delay(1000);
    Serial.println(position);
    Serial.println(time);
  }
  
  
  }

  void Write_EEPROM(int var,int address)
 {
  int var1=var>>8;
  EEPROM.write(address,var1);

  EEPROM.write(address+1,var);

 }
 int Read_EEPROM(int address)
 {
  int var1=EEPROM.read(address);
  int var2=EEPROM.read(address+1);
  var1=var1<<8;
  int var=var1+var2;
  return (int) var;
  Serial.print(var1);
  Serial.print(var2);
 }
