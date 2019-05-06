#include <config.h>
#include <ds3231.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Stepper.h>

#define DS3231_I2C_ADDRESS 0x68

const int STEPS_PER_REV = 32;
const int GEAR_REDUCTION = 64;

const int STEPS_PER_OUTER_REV = STEPS_PER_REV * GEAR_REDUCTION;
int search_speed = 200;
struct ts t;

Stepper stepmotor(STEPS_PER_REV, 8, 10, 9, 11);
Stepper stepmotor2(STEPS_PER_REV,4,6,5,7);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_INTCN);
  Initializacion();

 

  stepmotor.setSpeed(search_speed);
  stepmotor2.setSpeed(search_speed);
  Initmotor1();
  Initmotor2();
  apuntar_estrella();
  
}

void loop() {
  //readTime();
  //stepmotor1();
 //motor2();
 //hora_sid_ms();

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
t.hour=19;
t.min=9;
t.sec=0;
t.mday=03;
t.mon=05;
t.year=2019;
DS3231_set(t);
Serial.println("init finished");
}
void stepmotor1()
{
  Serial.println("number of steps 1: ");
  while(Serial.available()<=0){}
      
     long angle = Serial.readString().toInt();
     long steps = STEPS_PER_OUTER_REV * angle / 360;
    
     Serial.print("steps1:");
     Serial.println(steps);
     stepmotor.step(steps);
     long sum_steps=sum_steps+steps;
     if(sum_steps<0)
     {
      sum_steps=2048+sum_steps;
     }
     
    EEPROM.write(1,sum_steps/8);
    Serial.print("memoire:");
    Serial.println(EEPROM.read(1));
  
}
void motor2(){
 Serial.println("number of steps 2: ");
  while(Serial.available()<=0){}
      
     long angle2 = Serial.readString().toInt();
     long steps2 = STEPS_PER_OUTER_REV * angle2 / 360;
    
     Serial.print("steps2:");
     Serial.println(steps2);
     stepmotor2.step(steps2);
     long sum_steps2=sum_steps2+steps2;
     if(sum_steps2<0)
     {
      sum_steps2=2048+sum_steps2;
     }
     
    EEPROM.write(2,sum_steps2/8);
    Serial.print("memoire:");
    Serial.println(EEPROM.read(2));

}
void Initmotor1()
{
  int test_steps=EEPROM.read(1)*8;
  
  Serial.print("Init_steps: ");
  Serial.println(test_steps);
  if(test_steps!=0)
  {
    stepmotor.step(-test_steps);
  }
  EEPROM.write(1,0);
}
void Initmotor2(){
    int test_steps2=EEPROM.read(2)*8;
  
  Serial.print("Init_steps2: ");
  Serial.println(test_steps2);
  if(test_steps2!=0)
  {
    stepmotor2.step(-test_steps2);
  }
  EEPROM.write(2,0);
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
  long ascencion_ms =horas_ms+minutos_ms+segundos_ms;
  Serial.println(ascencion_ms);
  unsigned long mov_steps = ascencion_ms/42072;
  Serial.print("mov_steps :");
  Serial.println(mov_steps);
  stepmotor2.step(mov_steps);
  EEPROM.write(2,mov_steps/8);

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
  stepmotor.step(grados_step);
  EEPROM.write(1,grados_step/8);
  
  
  
  
  

  

  
  
  
  
}

void calculo(){
  
  
 
  
  
}

void hora_sid_ms(){
  unsigned long horas_ms=t.hour*3600000;
  Serial.println(horas_ms);
  unsigned long minutos_ms=t.min*60000;
  unsigned long segundos_ms=t.sec*1000;
  unsigned long hora_ms =horas_ms+minutos_ms+segundos_ms;
  Serial.print("hora_sideral_ms");
  unsigned long hora_sideral_ms=hora_ms*0.9972695833;
  Serial.println(hora_sideral_ms);
}
