#include <config.h>
#include <ds3231.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Stepper.h>

const int STEPS_PER_REV = 32;
const int GEAR_REDUCTION = 64;
const int STEPS_PER_OUTER_REV = STEPS_PER_REV * GEAR_REDUCTION;
long steps;
long steps2;
long angle;
long angle2;
long sum_steps;
long sum_steps2;
long test_steps;
long test_steps2;
long prev_steps;
long prev_steps2;
int search_speed = 200;
struct ts t;
int horas;
int minutos;
int segundos;

Stepper stepmotor(STEPS_PER_REV, 8, 10, 9, 11);
Stepper stepmotor2(STEPS_PER_REV,4,6,5,7);

void setup() {
  sum_steps=0;
  prev_steps = 0;
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_INTCN);
  Initializacion();
  ascencion();

  stepmotor.setSpeed(search_speed);
  stepmotor2.setSpeed(search_speed);
  
}

void loop() {
  readTime();
  stepmotor1();
  motor2();
  calculo();

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
t.hour=14;
t.min=53;
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
      
      angle = Serial.readString().toInt();
      steps = STEPS_PER_OUTER_REV * angle / 360;
    
     Serial.print("steps1:");
     Serial.println(steps);
     stepmotor.step(steps);
     sum_steps=sum_steps+steps;
     if(sum_steps<0)
     {
      sum_steps=2048+sum_steps;
     }
     
    EEPROM.write(1,sum_steps/8);
    Serial.print("memoire:");
    Serial.println(EEPROM.read(1));

   prev_steps = steps;
  
}
void motor2(){
 Serial.println("number of steps 2: ");
  while(Serial.available()<=0){}
      
      angle2 = Serial.readString().toInt();
      steps2 = STEPS_PER_OUTER_REV * angle2 / 360;
    
     Serial.print("steps2:");
     Serial.println(steps2);
     stepmotor2.step(steps2);
     sum_steps2=sum_steps2+steps2;
     if(sum_steps2<0)
     {
      sum_steps2=2048+sum_steps2;
     }
     
    EEPROM.write(2,sum_steps2/8);
    Serial.print("memoire:");
    Serial.println(EEPROM.read(2));

   prev_steps2 = steps2;
}
void Initmotor1()
{
  test_steps=EEPROM.read(1)*8;
  
  Serial.print("Init_steps: ");
  Serial.println(test_steps);
  if(test_steps!=0)
  {
    stepmotor.step(-test_steps);
  }
  EEPROM.write(1,0);
}
void Initmotor2(){
    test_steps2=EEPROM.read(2)*8;
  
  Serial.print("Init_steps2: ");
  Serial.println(test_steps2);
  if(test_steps2!=0)
  {
    stepmotor.step(-test_steps2);
  }
  EEPROM.write(2,0);
}
void ascencion(){
  Serial.println("Ascencion");
  Serial.print("Horas:");
  while(Serial.available()<=0){}
  horas = Serial.readString().toInt();
  Serial.println(horas);
  Serial.print("Minutos:");
  while(Serial.available()<=0){}
  minutos = Serial.readString().toInt();
  Serial.println(minutos);
  Serial.print("Segundos:");
  while(Serial.available()<=0){}
  segundos = Serial.readString().toInt();
  Serial.println(segundos);
  
  
}

void calculo(){
  
  unsigned long horas_ms=t.hour*3600000;
  Serial.println(horas_ms);
  unsigned long minutos_ms=t.min*60000;
  Serial.println(minutos_ms);
  unsigned long segundos_ms=t.sec*1000;
  Serial.println(segundos_ms);
  unsigned long hora_ms =horas_ms+minutos_ms+segundos_ms;
  
  Serial.print("hora_ms :");
  Serial.println(hora_ms);
  unsigned long  hora_sid_ms=hora_ms*0.997269583;
  Serial.print("hora_sid_ms :");
  Serial.println(hora_sid_ms);
  unsigned long mov_steps = hora_sid_ms/42072;
  Serial.print("mov_steps :");
  Serial.println(mov_steps);
  stepmotor2.step(mov_steps);
  
}
