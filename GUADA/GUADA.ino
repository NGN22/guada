#include <EEPROM.h>
#include <config.h>
#include <ds3231.h> 
#include <Wire.h>
#include <Stepper.h>
#include <QMC5883L.h>

float STEPS_PER_REV = 32.0;
float GEAR_REDUCTION = 64.0;
const long solar_ms_per_sideral_day = 86164092;

const int STEPS_PER_OUTER_REV = STEPS_PER_REV * GEAR_REDUCTION;
int search_speed = 3;
int total_steps_motorDec;
int total_steps_motorRA;
int var = 0;
struct ts t;

Stepper motorDec(STEPS_PER_OUTER_REV, 9, 11, 8, 10);
Stepper motorRA(STEPS_PER_OUTER_REV, 4, 6, 5, 7);

QMC5883L compass;
int minX,minY,maxX,maxY,offX,offY;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  //DS3231_init(DS3231_INTCN);
  motorDec.setSpeed(search_speed);
  motorRA.setSpeed(search_speed);
  compass.init();
  compass.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
}


void loop() {
  switch (var) {
    case 0:
      var = print_menu();
      break;
    case 1:
      Serial.println("Input number of degrees RA: ");
      assure_serial_avail();
      mov_motorRA (angle_to_steps(Serial.readString().toInt()));
      var = 0;
      break;
    case 2:
      Serial.println("Input number of degrees Dec: ");
      assure_serial_avail();
      mov_motorDec (angle_to_steps(Serial.readString().toInt()));
      var = 0;
      break;
    case 3:
      point();
      var = 0;
      break;
    case 4:
      point_the_star();
      var = 0;
      break;
    case 5:
      tracking();      
      break;
    case 6:
      Select_Time();
      var = 0;
      break;
    case 7:
      readTime();
      var = 0;
      break;
    case 8:
      move_to_origin_motorDec();
      move_to_origin_motorRA();
      var = 0;
      break;
    case 9:
  while (Serial.available() <= 0) {
    calibrate();
    }
      var=0;
      break;
    case 10:
      Point_South();
      var=0;
      break;
    case 11:
      total_steps_motorRA = 0;
      total_steps_motorDec = 0;
      var = 0;
      break;
    default :
      Serial.print("default case");
      break;
  }
}

void assure_serial_avail() {
  while (Serial.available() <= 0) {}
}

int print_menu() {
  Serial.println("====Menu====");
  Serial.println("1. Move motorRA");
  Serial.println("2. Move motorDec");
  Serial.println("3. Point");
  Serial.println("4. Point the star");
  Serial.println("5. Track");
  Serial.println("6. Init Time");
  Serial.println("7. Time");
  Serial.println("8. Init Motor");
  Serial.println("9. Calibrate");
  Serial.println("10. Point a Direction");
  Serial.println("11. Reset Position");
  assure_serial_avail();
  return Serial.readString().toInt();
}

void readTime() {
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
}

void Select_Time() {
  Serial.println("Select your Time");
  Serial.print("Hour:");
  assure_serial_avail();
  int hour = Serial.readString().toInt();
  Serial.println(hour);
  Serial.print("Min:");
  assure_serial_avail();
  int min = Serial.readString().toInt();
  Serial.println(min);
  Serial.print("sec:");
  assure_serial_avail();
  int sec = Serial.readString().toInt();
  Serial.println(sec);
  Serial.print("mDay:");
  assure_serial_avail();
  int mday = Serial.readString().toInt();
  Serial.println(mday);
  Serial.print("Month:");
  assure_serial_avail();
  int month = Serial.readString().toInt();
  Serial.println(month);
  Serial.print("Year:");
  assure_serial_avail();
  int year = Serial.readString().toInt();
  Serial.println(year);
  t.hour = hour;
  t.min = min;
  t.sec = sec;
  t.mday = mday;
  t.mon = month;
  t.year = year;
  DS3231_set(t);
  Serial.println("init finished");

}

long angle_to_steps(long angle) {
  long steps = STEPS_PER_OUTER_REV * angle / 360;
  Serial.print("Steps:");
  Serial.println(steps);
  return steps;
}

long move_motor(Stepper motor, long steps) {
  motor.step(steps);
  return steps;
}

void mov_motorDec(long steps) {
  move_motor(motorDec, steps);
  total_steps_motorDec = total_steps_motorDec + steps;
}

void mov_motorRA(long steps) {
  move_motor(motorRA, steps);
  total_steps_motorRA = total_steps_motorRA + steps;  
}


void move_to_origin_motorDec() {
  Serial.print("Moving motorDec to origin: ");
  Serial.println(-total_steps_motorDec);
  move_motor(motorDec, -total_steps_motorDec);
  total_steps_motorDec = 0;
}

void move_to_origin_motorRA() {
  Serial.print("Moving motorRA to origin: ");
  Serial.println(-total_steps_motorRA);
  move_motor(motorRA, -total_steps_motorRA);
  total_steps_motorRA = 0;
}

unsigned long ms_to_make_a_step() {
  return solar_ms_per_sideral_day / (STEPS_PER_REV * GEAR_REDUCTION);
}

float degrees_to_make_a_step() {
  float degrees_to_make_a_step = 360 / (STEPS_PER_REV * GEAR_REDUCTION);
  return degrees_to_make_a_step;
}

long parse_ascension(String ascension) {
  unsigned long hours_ms = ascension.substring(0, 2).toInt() * 3600;
  unsigned long minutes_ms = ascension.substring(3, 5).toInt() * 60;
  unsigned long seconds_ms = ascension.substring(6, 8).toInt();
  Serial.print("hours:");
  Serial.println(hours_ms);
  Serial.print("minutes:");
  Serial.println(minutes_ms);
  Serial.print("seconds:");
  Serial.println(seconds_ms);
  return (hours_ms + minutes_ms + seconds_ms) * 1000;
}

float parse_declination(String declination) {
  float degrees_dec = 90 - declination.substring(0, 2).toFloat();
  float minutes_dec = declination.substring(3, 5).toFloat() / 60;
  float seconds_dec = declination.substring(6, 8).toFloat() / 3600;
  Serial.print("degrees:");
  Serial.println(degrees_dec);
  Serial.print("minutes:");
  Serial.println(minutes_dec);
  Serial.print("seconds:");
  Serial.println(seconds_dec);
  Serial.print("Total:");
  Serial.println(degrees_dec + minutes_dec + seconds_dec);

  return (degrees_dec + minutes_dec + seconds_dec);
}

float process_ascension_inputs() {
  Serial.println("Load the Ascension data in format hh:mm:ss");
  assure_serial_avail();
  String ascension = Serial.readString();
  return parse_ascension(ascension);
}

unsigned long process_declination_inputs() {
  Serial.println("Load the Declination data in format dd,mm,ss");
  assure_serial_avail();
  String declination = Serial.readString();
  return parse_declination(declination);
}

void point() {
  unsigned long steps = process_ascension_inputs() / ms_to_make_a_step();
  Serial.print("Steps needed in RA: ");
  Serial.println(steps);
  mov_motorRA(steps);

  steps = process_declination_inputs() / degrees_to_make_a_step();
  Serial.print("Steps needed in DEC: ");
  Serial.println(steps);
  mov_motorDec(steps);
}

unsigned long get_sideral_time_in_ms() {
  DS3231_get(&t);
  unsigned long hora_sideral_ms=0;
  unsigned long hours_ms = t.hour * 3600000;
  unsigned long minutes_ms = t.min * 60000;
  unsigned long seconds_ms = t.sec;
  seconds_ms = seconds_ms*1000;
  hora_sideral_ms = (hours_ms + minutes_ms + seconds_ms) % solar_ms_per_sideral_day;
  Serial.print("Actual sideral time by RTC: ");
  Serial.println(hora_sideral_ms);
  return hora_sideral_ms;
}

unsigned long get_steps_to_point_star() {

  unsigned long steps = get_sideral_time_in_ms() / ms_to_make_a_step();
  Serial.print("steps");
  Serial.println(steps);
  return steps;
}
//esto es inútil si usamos directamente las coordenadas actuales de la estrella y no las coordenadas 0 0 
void point_the_star() {
  unsigned long steps_to_point = get_steps_to_point_star();
  Serial.print("Number of steps to point the star: ");
  Serial.println(steps_to_point);
  mov_motorRA(steps_to_point);

}
void tracking () {

  Serial.println("Press any key to stop tracking");
  unsigned long sideral_time_when_to_make_step = get_sideral_time_in_ms() + ms_to_make_a_step();
  Serial.println("tracking...");
  do {
    while (get_sideral_time_in_ms() < sideral_time_when_to_make_step) {
      delay(1);
    }
    sideral_time_when_to_make_step = sideral_time_when_to_make_step + ms_to_make_a_step();
    mov_motorRA(1);
    Serial.println("Step done ");
  } while (Serial.available() <= 0);
  var=0;
}
void calibrate(){
  
  int x,y,z;
  compass.read(&x,&y,&z);

  // Determine Min / Max values
  if (x < minX) minX = x;
  if (x > maxX) maxX = x;
  if (y < minY) minY = y;
  if (y > maxY) maxY = y;

  // Calculate offsets
  offX = (maxX + minX)/2;
  offY = (maxY + minY)/2;

  Serial.print(x);
  Serial.print(":");
  Serial.print(y);
  Serial.print(":");
  Serial.print(minX);
  Serial.print(":");
  Serial.print(maxX);
  Serial.print(":");
  Serial.print(minY);
  Serial.print(":");
  Serial.print(maxY);
  Serial.print(":");
  Serial.print(offX);
  Serial.print(":");
  Serial.print(offY);
  Serial.print("\n");
}
int Select_Direction()
{
  Serial.print("Choose the direction in which you want to Point(from 0 to 360°)");
  assure_serial_avail();
  int Point = Serial.readString().toInt();
  return Point;
}

void Point_South()
{
  int x,y,z;
  float headingDegrees=0;
  int Point=Select_Direction();
      while(headingDegrees<Point-1 || headingDegrees>Point+1)
    {
  compass.read(&x,&y,&z);
  x=x-offX;
  y=y-offY;

 // Calculate heading when the magnetometer is level, then correct for signs of axis.
  // Atan2() automatically check the correct formula taking care of the quadrant you are in
  float heading = atan2(y, x);

    float declinationAngle = -8.51;
  heading += declinationAngle;
  // Find yours here: http://www.magnetic-declination.com/

   // Correct for when signs are reversed.
  while(heading < 0)
  {
    heading += 2*PI;
  }

  // Check for wrap due to addition of declination.
  while(heading > 2*PI)
    {
     heading -= 2*PI;
    }

  // Convert radians to degrees for readability.
   headingDegrees= heading * 180/M_PI; 
  Serial.print("degrees");
  Serial.println(headingDegrees);

      motorRA.step(1);
    }
}
