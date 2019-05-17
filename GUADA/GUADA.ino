#include <EEPROM.h>
#include <config.h>
#include <ds3231.h>
#include <Wire.h>
#include <Stepper.h>

const int STEPS_PER_REV = 32;
const int GEAR_REDUCTION = 64;
const long solar_ms_per_sideral_day = 86164092;

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


void assure_serial_avail(){
    while(Serial.available()<=0){}
}

int print_menu() {
    Serial.println("====Menu====");
    Serial.println("1. Move motorRA");
    Serial.println("2. Move motorDec");
    Serial.println("3. Point");
    Serial.println("4. Track");
    Serial.println("5. Init Time");
    Serial.println("6. Time");
    Serial.println("7. Init Motor");
    Serial.println("8. Save Position");
    Serial.println("9. Reset Position");
    Serial.println("10. Exit");
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

    delay(500);  
}

void Select_Time() {
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

void mov_motorDec(long steps){
    move_motor(motorDec, steps)
        total_steps_motorDec = total_steps_motorDec + steps;
}

void mov_motorRA(long steps){
    move_motor(motorRA, steps);
    total_steps_motorRA = total_steps_motorRA + steps;
}


void move_to_origin_motorDec() { 
    Serial.print("Moving motorDec to origin: ");
    Serial.println(-total_steps_motorDec);
    move_motor(motorDec, -total_steps_motorDec);
    total_steps_motorDec=0;
}

void move_to_origin_motorRA() { 
    Serial.print("Moving motorDec to origin: ");
    Serial.println(-total_steps_motorRA);
    move_motor(motorDec, -total_steps_motorRA);
    total_steps_motorRA=0;
}

unsigned long ms_to_make_a_step() {
    return solar_ms_per_sideral_day / (STEPS_PER_REV * GEAR_REDUCTION);
}

unsigned long degrees_to_make_a_step() {
    return 360 / (STEPS_PER_REV * GEAR_REDUCTION);
}

long parse_ascension(string ascension){
    unsigned long hours_ms = ascension.substring(0,1).toInt() * 3600;
    unsigned long minutes_ms = ascension.substring(3,4).toInt() * 60;
    unsigned long seconds_ms = ascension.substring(5,6).toInt(); 

    return (hours_ms + minutes_ms + seconds_ms) * 1000;
}

unsigned long parse_declination(string declination){
    long degrees_dec = ascension.substring(0,2).toInt();
    long minutes_dec = ascension.substring(4,5).toInt() / 60;
    long seconds_dec = ascension.substring(6,7).toInt() / 3600; 

    return (hours_ms + minutes_ms + seconds_ms) * 1000;
}

void process_ascension_inputs() {
    Serial.println("Load the Ascension data in format hh:mm:ss");
    assure_serial_avail();
    string ascension = Serial.readString();
    return parse_ascension(ascension);
}

void process_declination_inputs() {
    Serial.println("Load the Declination data in format ddd,mm,ss");
    assure_serial_avail();
    string declination = Serial.readString();
    return parse_declination(declination);
}

void point() {
    unsigned long steps = process_ascension_inputs() / ms_to_make_a_step();
    Serial.print("Steps needed in RA: ");
    Serial.println(steps);
    move_motorRA.(steps);

    steps = process_declination_inputs() / degrees_to_make_a_step();
    Serial.print("Steps needed in DEC: ");
    Serial.println(steps);
    move_motorDec(steps);
}

unsigned long get_sideral_time_in_ms(){
    DS3231_get(&t);
    unsigned long hours_ms = t.hour*3600000;
    unsigned long minutes_ms = t.min*60000;
    unsigned long seconds_ms = t.sec*1000;
    Serial.println("Actual sideral time by RTC: ")
        Serial.println((hours_ms + minutes_ms + seconds_ms) % solar_ms_per_sideral_day);
    return (unsigned long) hora_sideral_ms = (hours_ms + minutes_ms + seconds_ms) % solar_ms_per_sideral_day;
}

void tracking () {
    Serial.println("Press any key to stop tracking");
    unsigned long sideral_time_when_to_make_step = get_sideral_time_in_ms() + ms_to_make_a_step();
    do {
        while ( (long ) (get_sideral_time_in_ms() < sideral_time_when_to_make_step)) {
            delay(1);
        }
        sideral_time_when_to_make_step = sideral_time_when_to_make_step + ms_to_make_a_step();
        move_motorRA.(1);
    } while (Serial.available() > 0);
}

void loop() {
    switch(var){
        case 0:
            var=print_menu;
            break;
        case 1:
            Serial.println("Input number of degrees RA: ");
            assure_serial_avail();
            mov_motorRA (angle_to_steps(Serial.readString().toInt()));
            var=0;
            break;
        case 2:
            Serial.println("Input number of degrees Dec: ");
            assure_serial_avail();
            mov_motorDec (angle_to_steps(Serial.readString().toInt()));
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

