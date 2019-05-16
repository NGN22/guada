#include <EEPROM.h>
#include <config.h>
#include <ds3231.h>
#include <Wire.h>
#include <Stepper.h>


const int STEPS_PER_REV = 32;
const int GEAR_REDUCTION = 64;
const long solar_ms_per_sideral_day=86164092;

const int STEPS_PER_OUTER_REV = STEPS_PER_REV * GEAR_REDUCTION;
int search_speed = 200;
long ascension_ms;
int total_steps_motorDec;
int total_steps_motorRA;
int var=0;
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

}

void loop() {
 switch(var){
  case 0:
 Serial.println("====Menu====");
 Serial.println("1. Moove motorRA");
 Serial.println("2. Moove motorDec");
 Serial.println("3. Point");
 Serial.println("4. Track");
 Serial.println("5. Init Time");
 Serial.println("6. Time");
 Serial.println("7. Init Motor");
 Serial.println("8. Save Position");
 Serial.println("9. Reset Position");
 Serial.println("10. Exit");
    while(Serial.available()<=0){}
    var=Serial.readString().toInt();
    
    break;
  case 1:
    mov_motorRA();
    var=0;

    break;
  case 2:
  mov_motorDec();
  var=0;
    break;
  case 3:
  Point();
  var=0;
    break;
  case 4:
  tracking();
    break;
  case 5:
  Select_Time();
  var=0;
    break;
  case 6:
  readTime();
  var=0;
    break;
  case 7:
  Init_motorDec();
  Init_motorRA();
  var=0;
    break;
   case 8:
   Write_EEPROM(total_steps_motorRA,0);
   Write_EEPROM(total_steps_motorDec,2);
   var=0;
    break;
   case 9:
  total_steps_motorRA=0;
  total_steps_motorDec=0;
  var =0;
    break;
   case 10:
  Init_motorDec();
  Init_motorRA();

    break;

  default :

  break;
}
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
void Select_Time(){
  Serial.println("Select your Time");
  Serial.print("Hour:");
  while(Serial.available()<=0){}
  int hour = Serial.readString().toInt();
  Serial.print("Min:");
  while(Serial.available()<=0){}
  int min = Serial.readString().toInt();
  Serial.print("sec:");
  while(Serial.available()<=0){}
  int sec = Serial.readString().toInt();
  Serial.print("mDay:");
  while(Serial.available()<=0){}
  int mday = Serial.readString().toInt();
  Serial.print("Month:");
  while(Serial.available()<=0){}
  int month = Serial.readString().toInt();
  Serial.print("Year:");
  while(Serial.available()<=0){}
  int year = Serial.readString().toInt();
  t.hour=hour;
  t.min=min;
  t.sec=sec;
  t.mday=mday;
  t.mon=month;
  t.year=year;
  DS3231_set(t);
  Serial.println("init finished");
  
}
void mov_motorDec()
{
  Serial.println("number of degres Dec: ");
  while(Serial.available()<=0){}
      
     long angle = Serial.readString().toInt();
     long steps = STEPS_PER_OUTER_REV * angle / 360;
    
     Serial.print("steps1:");
     Serial.println(steps);
     motorDec.step(steps);
     total_steps_motorDec=total_steps_motorDec+steps;
     
  
}
void mov_motorRA(){
 Serial.println("number of degres RA: ");
  while(Serial.available()<=0){}
      
     long angle2 = Serial.readString().toInt();
     long steps2 = STEPS_PER_OUTER_REV * angle2 / 360;
    
     Serial.print("steps2:");
     Serial.println(steps2);
     motorRA.step(steps2);
     total_steps_motorRA=total_steps_motorRA+steps2;
     

}
void Init_motorDec()
{ 
  
  Serial.print("Init_stepsDec: ");
  Serial.println(-total_steps_motorDec);
  motorDec.step(-total_steps_motorDec);
  total_steps_motorDec=0;
}
void Init_motorRA(){
  Serial.print("Init_stepsRA: ");
  Serial.println(-total_steps_motorRA);
  motorRA.step(-total_steps_motorRA);
  total_steps_motorRA=0;
  
}
void Position0(){
  int stepsRA=Read_EEPROM(0);
  int stepsDec=Read_EEPROM(2);

  motorRA.step(-stepsRA);
  motorDec.step(-stepsDec);
  
}
void Point(){
  Serial.println("Ascension");
  Serial.print("Hour:");
  while(Serial.available()<=0){}
  long hours = Serial.readString().toInt();
  Serial.println(hours);
  Serial.print("Minute:");
  while(Serial.available()<=0){}
  long minutes = Serial.readString().toInt();
  Serial.println(minutes);
  Serial.print("Seconds:");
  while(Serial.available()<=0){}
  long seconds = Serial.readString().toInt();
  Serial.println(seconds);
  unsigned long hours_ms=hours*3600000;
  unsigned long minutes_ms=minutes*60000;
  unsigned long seconds_ms=seconds*1000;
  ascension_ms =hours_ms+minutes_ms+seconds_ms;
  position=ascension_ms;
  unsigned long mov_steps = ascension_ms/42072;
  Serial.print("mov_steps :");
  Serial.println(mov_steps);
  motorRA.step(mov_steps);
  float mem_step2=mov_steps/8;
  total_steps_motorRA=total_steps_motorRA+mov_steps;
  

  Serial.println("Declination: ");
  Serial.print("Degrees: ");
  while(Serial.available()<=0){}
  int degree = Serial.readString().toInt();
  Serial.println(degree);
  Serial.print("Minutes: ");
  while(Serial.available()<=0){}
  int min_degrees = Serial.readString().toInt();
  Serial.println(min_degrees);
  Serial.print("Seconds: ");
  while(Serial.available()<=0){}
  int sec_degrees = Serial.readString().toInt();
  Serial.println(sec_degrees);
  min_degrees = min_degrees/60;
  sec_degrees=sec_degrees/3600;
  degree=degree+min_degrees+sec_degrees;
  int degrees_step= degree/0.17578125;
  Serial.print("step: ");
  Serial.println(degrees_step);
  motorDec.step(degrees_step);
  float mem_step=degrees_step/8;
  total_steps_motorDec=total_steps_motorDec+degrees_step;
  
}
unsigned long hora_sid_ms(){
  DS3231_get(&t);
  unsigned long horas_ms=t.hour*3600000;
  unsigned long minutos_ms=t.min*60000;
  unsigned long segundos_ms=t.sec*1000;
  unsigned long hora_ms =horas_ms+minutos_ms+segundos_ms;
  //Serial.print("hora_sideral_ms");
  unsigned long hora_sideral_ms=hora_ms%solar_ms_per_sideral_day;
  return (unsigned long) hora_sideral_ms;
}

void tracking (){
  unsigned long time=hora_sid_ms();
  int steps=0;
  Serial.println("Press '0' to Stop the Tracking");
  while(Serial.available()<=0){
  if(time>position)
  {
    position=position+42072;
    motorRA.step(1);
    total_steps_motorRA=total_steps_motorRA+1;
    
  }
    if(time<=position){
    position=time;
    Serial.println(position);
    Serial.println(time);
  }
  }
  var=Serial.readString().toInt();
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
