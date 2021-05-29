/*
 * BLDC Motor Test Bed
 */


//Thrust Measurment
#include <HX711.h>

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

HX711 scale;
float calibration_factor = 10000;
int thr, maxThr = 0;        //  DEFINE Thrust AND MAXIMUM Thrust


//Speed Control
#include <Servo.h>
#define ESC_PIN  9
#define POT_PIN  A0
Servo ESC;    // create servo object to control the ESC
int potValue; // value from the analog pin


//RPM Measurment
#define CNY70_INTRPT_PIN  0
#define CNY70_VCC_PIN  4
#define CNY70_GND_PIN  5
volatile byte REV;       //  VOLATILE DATA TYPE TO STORE REVOLUTIONS
unsigned long int rpm, maxRPM;  //  DEFINE RPM AND MAXIMUM RPM
unsigned long time;         //  DEFINE TIME TAKEN TO COVER ONE REVOLUTION
long prevtime = 0;       //  STORE IDLE TIME TO TOGGLE MENU
byte x=0;


void setup() {
  Serial.begin(9600);  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); //Reset the scale to 0

  ESC.attach(ESC_PIN,1000,2000); // (pin, min pulse width, max pulse width in ms)

  attachInterrupt(CNY70_INTRPT_PIN, RPMCount, RISING);     //  ADD A HIGH PRIORITY ACTION ( AN INTERRUPT)  WHEN THE SENSOR GOES FROM LOW TO HIGH
  REV = 0;      //  START ALL THE VARIABLES FROM 0   
  rpm = 0;
  time = 0;
  pinMode(CNY70_VCC_PIN, OUTPUT);
  pinMode(CNY70_GND_PIN, OUTPUT);
  digitalWrite(CNY70_VCC_PIN, HIGH);             //  VCC PIN FOR SENSOR
  digitalWrite(CNY70_GND_PIN, LOW);              // GND PIN FOR SENSOR
}

void loop() {
  thr = scale.get_units(); //scale.get_units() returns a float
  Serial.print("Thrust: ");
  Serial.print(thr, 5);
  Serial.print(" KG - ");
  if(thr > maxThr)
  maxThr=thr;


  potValue = analogRead(POT_PIN); // read the A0 pin (this value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 0, 180); // scale the potValue (0,1023)>>(0,180)
  ESC.write(potValue); // send signal to the ESC


  long currtime = millis();                 // GET CURRENT TIME
  long idletime = currtime - prevtime;        //  CALCULATE IDLE TIME
    
    if(REV >= 5 )                  //  IT WILL UPDATE AFTER EVERY 5 READINGS
   {     
     rpm = 30*1000/(millis() - time)*REV;       //  CALCULATE  RPM USING REVOLUTIONS AND ELAPSED TIME
     
     if(rpm > maxRPM)
     maxRPM = rpm;                             //  GET THE MAX RPM THROUGHOUT THE RUN
    
     time = millis();                            
     
     REV = 0;

     Serial.print("Revulation: ");
     Serial.print(rpm,5);                        //  PRINT RPM IN DECIMAL SYSTEM
     Serial.println("RPM");
     delay(500);
     
     prevtime = currtime;                        // RESET IDLETIME
     x=0;
     }
   
   if(idletime > 5000 || x == 0 )                      //  IF THERE ARE NO READING FOR 5 SEC , THE SCREEN WILL SHOW MAX RPM
   { 
     Serial.print("MAXIMUM Thrust:  ");
     Serial.print(maxThr,5);                     // DISPLAY MAX RPM
     Serial.print(" KG - ");
     Serial.print("MAXIMUM RPM: ");
     Serial.print(maxRPM,5);                     // DISPLAY MAX RPM
     Serial.println(" RPM");
     delay(2000);
     Serial.println("IDLE STATE - READY TO MEASURE");
     delay(2000);
     prevtime = currtime;
     x++;
   }
}


void RPMCount()                                // EVERYTIME WHEN THE SENSOR GOES FROM LOW TO HIGH , THIS FUNCTION WILL BE INVOKED 
 {
   REV++;                                         // INCREASE REVOLUTIONS
 }
