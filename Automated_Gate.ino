
#include <Servo.h>
#include <LiquidCrystal.h>
#include "arduinoFFT.h"

Servo myservo; // create Servo object to control a servo
//Parameters:(rs, enable, d4,d5,d6, d7)
LiquidCrystal lcd(12,11,5,4,3,2); // create LiquidCrystal object to control the LCD screen
arduinoFFT  FFT = arduinoFFT();
 
#define SAMPLES 128             //SAMPLES-pt FFT. Must  be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
 
const int trigPin = 7;
const int echoPin = 8;
const int analogPin = 17;

unsigned int samplingPeriod;
unsigned long microSeconds;
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double  vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
double peak;

long microsecondsToCentimeters(long microseconds) {

// The speed of sound is 340 m/s or 29 microseconds per centimeter.

// The ping travels out and back, so to find the distance of the

// object we take half of the distance travelled.

return microseconds / 29 / 2;

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //Baud rate for the Serial Monitor
  myservo.attach(9); //assigns pin 9 to the servo output
  
  lcd.begin(16,2); //set up the LCD's number of columns and rows
  lcd.clear(); // clears the LCD screen
  
  samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds
}

void loop() {
  // put your main code here, to run repeatedly:
  long duration, distance;
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin,LOW);
  duration = pulseIn(echoPin,HIGH);
  // converts time to distance turned
  distance = microsecondsToCentimeters(duration);
  // determines what direction to turn the servo
  if ((distance > 7 && distance < 14) || peak >= 500){
    myservo.write(140);
    delay(400);
  }
  else{
    myservo.write(40);
    delay(100);
  }
  //Serial.print(distance);
  //Serial.print("cm");
  //Serial.println();
  //delay(100);
  
    /*Sample SAMPLES times*/
    for(int  i=0; i<SAMPLES; i++)
    {
        microSeconds = micros();    //Returns the  number of microseconds since the Arduino board began running the current script.  
     
        vReal[i] = analogRead(0); //Reads the value from analog pin  0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes  imaginary term 0 always

        /*remaining wait time between samples if  necessary*/
        while(micros() < (microSeconds + samplingPeriod))
        {
          //do nothing
        }
    }
 
    /*Perform FFT on samples*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal,  vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    /*Find peak frequency and print peak*/
    peak = FFT.MajorPeak(vReal,  SAMPLES, SAMPLING_FREQUENCY);
    Serial.println(peak);     //Print out the most  dominant frequency.
    lcd.setCursor(3,0);
    lcd.print(peak); // prints a message to the LCD.
    lcd.setCursor(9,0); // cursor set to column 0 and row 1
    lcd.print("Hz ");
     
}




