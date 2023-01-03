#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 2);

int rotatetext = 1;
int motorPin = 3;
int throttle = 13;
int brake = 12;
int clutch = 11;
int JS_B = 10;
int JS_X = A0;
int JS_Y = A1;
int off = 0; //0 car is off, 1 is on
float rev = 1000; //rpm counter going to have car rev limit at 7000
int stall = 0;
int gear = 0;
float mph;
int shift;
int speed;
//to calc mph rpm/1000 * 2*gear + gear^2

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);

} task;

int delay_gcd;
const unsigned short tasksNum = 5;
task tasks[tasksNum];

enum START_States {START_INIT, on};
int START_Tick(int state1) {
  switch (state1) {
    case START_INIT://transition
      state1 = on;
      break;

    case on:
      state1 = on;
      break;

  }
  switch (state1) { //action
    case on:
      int jsb = digitalRead(JS_B);
      int sclu = digitalRead(clutch);
      if (jsb != 1 && sclu) {
        if (off != 1) {
          off = 1;
          stall = 0;
        }
        else {
          off = 0;
        }
      }
      break;
  }
  return state1;
}

enum RPM_States {RPM_INIT, ped_off, thr_on, bra_on};
int RPM_Tick(int state2) {
  int thr = digitalRead(throttle);
  int bra = digitalRead(brake);
  switch (state2) {
    case RPM_INIT://transition
      if (off == 1) {
        state2 = ped_off;
      }
      break;

    case ped_off:
      if (thr) {
        state2 = thr_on;
      }
      if (bra) {
        state2 = bra_on;
      }
      if (!thr && !bra) {
        state2 = ped_off;
      }
      break;

    case thr_on:
      if (thr) {
        state2 = thr_on;
      }
      if (bra) {
        state2 = bra_on;
      }
      if (!thr && !bra) {
        state2 = ped_off;
      }
      break;

    case bra_on:
      if (thr) {
        state2 = thr_on;
      }
      if (bra) {
        state2 = bra_on;
      }
      if (!thr && !bra) {
        state2 = ped_off;
      }
      break;
  }
  switch (state2) { //action
    case ped_off:
      if (rev > 0) {
        rev -= 5;
      }
      break;
    case thr_on:
      if (rev <= 7000) {
        rev += 25;
      }
      break;
    case bra_on:
      if (rev > 0) {
        rev -= 30;
      }
      break;
  }
  return state2;
}

enum GEAR_States {GEAR_INIT, Neu, One, Two, Three, Four, Five, Six};
int GEAR_Tick(int state3) {
  int clu = digitalRead(clutch);
  shift = analogRead(JS_X);
  switch (state3) {
    case GEAR_INIT://transition
      if (off == 1) {
        state3 = Neu;
      }
      break;

    case Neu:
      if (clu && shift > 490) {
        state3 = One;
      }
      break;

    case One:
      if (clu && shift > 490) {
        state3 = Two;
      }
      else if (clu && shift < 160) {
        state3 = Neu;
      }
      if(stall == 1){
        state3 = Neu;
      }
      break;
    case Two:
      if (clu && shift > 490) {
        state3 = Three;
      }
      else if (clu && shift < 160) {
        state3 = One;
      }
      if(stall == 1){
        state3 = Neu;
      }     
      break;

    case Three:
      if (clu && shift > 490) {
        state3 = Four;
      }
      else if (clu && shift < 160) {
        state3 = Two;
      }
      if(stall == 1){
        state3 = Neu;
      }      
      break;
    case Four:
      if (clu && shift > 490) {
        state3 = Five;
      }
      else if (clu && shift < 160) {
        state3 = Three;
      }
       if(stall == 1){
        state3 = Neu;
      }
      break;

    case Five:
      if (clu && shift > 490) {
        state3 = Six;
      }
      else if (clu && shift < 160) {
        state3 = Four;
      }
            if(stall == 1){
        state3 = Neu;
      }
      break;
    case Six:
      if (clu && shift < 160) {
        state3 = Five;
      }
      if(stall == 1){
        state3 = Neu;
      }
      break;
  }
  switch (state3) { //action
    case Neu:
      gear = 0.0;
      break;
    case One:
      gear = 1.0;
      if(rev<1){
        //stall = 1;
        off = 0;
      }
      break;
    case Two:
      gear = 2.0;
      if(rev<100){
        stall = 1;
        off = 0;
      }
      break;
    case Three:
      gear = 3.0;
      if(rev<100){
        stall = 1;
        off = 0;
      }
      break;
    case Four:
      gear = 4.0;
      if(rev<100){
        stall = 1;
        off = 0;
      }
      break;
    case Five:
      gear = 5.0;
      if(rev<100){
        stall = 1;
        off = 0;
      }
      break;
    case Six:
      gear = 6.0;
      if(rev<100){
        stall = 1;
        off = 0;
      }
      break;

  }
  return state3;
}

enum MPH_States {MPH_INIT, MPH, rev_match}; // would also spin fan
int MPH_Tick(int state4) { //to calc mph (rpm/1000 * 2*gear + gear^2)/255+50
  shift = analogRead(JS_X);
  switch (state4) {
    case MPH_INIT://transition
      if (off == 1) {
        state4 = MPH;
      }
      break;

    case MPH:
      if (shift < 160 || shift > 490) {
        state4 = rev_match;
      }

    case rev_match:
      if (shift > 160 && shift < 490) {
        state4 = MPH;
      }
      break;
  }
  switch (state4) { //action
    case MPH:
      mph = ((rev / 1000) * 2.0 * gear + gear * gear); //+50 is for the fan starting at 50+ speed;
      speed = rev/34.15+50;
      
      Serial.println(speed);
      if(stall == 0){
      analogWrite(motorPin, speed);
      }
      else if(stall == 1){
        analogWrite(motorPin, 0);
      }
      break;
    case rev_match:
      rev = (1000 * (mph - gear * gear)) / (2.0 * gear);
      //Serial.println(rev);
      break;
  }
  return state4;
}

enum LCD_States{LCD_INIT, LCD_On, LCD_stall};
int LCD_Tick(int state5){
  switch (state5) {
    case LCD_INIT://transition
    if(off==1){
      state5 = LCD_On;
    }
    break;

    case LCD_On:
      if(stall ==  1){
        state5 = LCD_stall;
      }
    case LCD_stall:
      if(stall == 0){
        state5 = LCD_On;
      }
      break;

  }
  switch (state5){//action
    case LCD_On:
    //Serial.println("works");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0,0);
   
    display.print("RPM: ");//first line "RPM: . . . 
    for(int i = 0; i<(rev/1000);i++){
      display.setTextColor(WHITE, BLACK);
      display.print("-");
    }
   
   
   display.setTextColor(WHITE);
   display.println(".");
   display.print("RPM: ");
   display.setTextColor(BLACK);
   display.print("1234567");
    
    display.setCursor(0,16);
    display.print("Gear: ");//line 3 Gear: #
    display.print(gear/1);

    display.setCursor(0,24);
    display.print("MPH:  ");//Line 4 MPH: #
    display.print(mph);
    display.display();
    break;

    case LCD_stall:
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Stalled!!!");
    display.setCursor(0,8);
    display.println("Please press  clutch and    restart Car");
    display.display();

  }
  return state5;
}

void setup()
{
  pinMode(motorPin, OUTPUT);
  pinMode(JS_B, INPUT_PULLUP);

  //del for submission
  display.begin();
  display.setContrast(57);
  display.clearDisplay();
  
  unsigned char i = 0;
  tasks[i].state = START_INIT;//start feature
  tasks[i].period = 50;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &START_Tick;
  i++;
  tasks[i].state = RPM_INIT;//pedals
  tasks[i].period = 10;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &RPM_Tick;
  i++;
  tasks[i].state = GEAR_INIT;//gears neutral-6
  tasks[i].period = 200;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &GEAR_Tick;
  i++;
  tasks[i].state = MPH_INIT;//speed
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &MPH_Tick;
  i++;
  tasks[i].state = LCD_INIT;//display
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &LCD_Tick;

  delay_gcd = 5; // GCD

  Serial.begin(9600);
}


void loop()
{
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }

}
