#include <Servo.h>
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

Servo myservo;//initilize the servo
int pos=0;

//initialize memory
int mem=1;
int memo=0;

//initialize sensor to pin6 & pin8
int sensor1=6;
int sensor2=8;
int off=0;
//initialize the location1 and location2
int dis1;
int dis2;
//initialize led to pin13
int ledPin=13;

//intialize the motor rotation and speed to pin4 & pin5
int DIR=4;
int PWM=5;

void setup()
{
  Serial.begin(9600);
  //set servo to pin9
  myservo.attach(9);
  //set pin6, pin8 & pin10 as input
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
  pinMode(10,INPUT);
  //initialize servo at zero degree
  myservo.write(0);
  //set pin4,pin5 & pin13 as output
  pinMode(ledPin,OUTPUT);
  pinMode(DIR,OUTPUT);
  pinMode(PWM,OUTPUT);
  //intialize led & motor to be off
  digitalWrite(ledPin,LOW);
  digitalWrite(DIR,HIGH);
  analogWrite(PWM,0);
  //intialize the display
  lcd.init();
  lcd.backlight();
}

void loop()
{
  com();//get into serial communication
  if(memo==1)//pass if fan is on
  {
    //read input from sensors & switch
    dis1=digitalRead(sensor1);
    dis2=digitalRead(sensor2);
    off=digitalRead(10);
    //servo movement based on the sensor detection
    if(mem==1 && dis1==HIGH && dis2==LOW)
    {
      moveTo(0,180);//get into function moveTo
      mem=2;//save the last fan's location
    }
    else if(mem==1 && dis1==LOW && dis2==LOW)
    {
      moveTo(0,180);
      mem=2;
    }
    else if(mem==1 && dis1==LOW && dis2==HIGH)
    {
      myservo.write(0);
      mem=1;
    }
    else if(mem==1 && dis1==HIGH && dis2==HIGH)
    {
      myservo.write(0);
      mem=1;
    }
    else if(mem==2 && dis1==LOW && dis2==HIGH)
    {
      moveFrom(180,0);
      mem=1;
    }
    else if(mem==2 && dis1==LOW && dis2==LOW)
    {
      moveFrom(180,0);
      mem=1;
    }
     else if(mem==2 && dis1==HIGH && dis2==LOW)
    {
      myservo.write(180);
      mem=2;
    }
    else if(mem==2 && dis1==HIGH && dis2==HIGH)
    {
      myservo.write(180);
      mem=2;
    }
    if(off==LOW)
    {
      digitalWrite(ledPin,LOW);
      analogWrite(PWM,0);
      myservo.write(0);
      lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
      lcd.print("FAN IS OFF~~");
      memo=0;//save either the fan is on or not(mem=0:off)
      Serial.println(memo);
    }
  }
}

void moveTo(int pos1,int pos2)
{
  //servo moves degree by degree from low to high
  for (pos=pos1;pos<=pos2;pos+=1)
  {
    myservo.write(pos);
    delay(15);
  }
}

void moveFrom(int pos1,int pos2)
{
  //servo moves degree by degree from high to low
  for (pos=pos1;pos>=pos2;pos-=1)
  {
    myservo.write(pos);
    delay(15);
  }
}

void com()
{
  //receive serial commnication from python
  if (Serial.available() > 0)
  {
    //transform string into int
    String payload = Serial.readStringUntil('\r\n');
    int Incoming_value=payload.toInt();
    if (memo==0 && Incoming_value == 1)//turn on the fan if the condition is satisfied
    {
      myservo.write(0);
      digitalWrite(ledPin,HIGH);
      analogWrite(PWM,155);
      //display
      lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
      lcd.print("FAN IS ON~~");
      delay(3000);
      lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
      lcd.print("FAN SPEED:LOW");
      delay(1000);
      memo=1;
    }
    //control the speed of the motor(fan)
    else if (memo==1 && Incoming_value == 1) 
    {
      digitalWrite(ledPin,HIGH);
      analogWrite(PWM,155);
      lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
      lcd.print("FAN SPEED:LOW   ");
      memo=1;
    }
    else if (memo==1 && Incoming_value == 2) 
    {
      digitalWrite(ledPin,HIGH);
      analogWrite(PWM,205);
      lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
      lcd.print("FAN SPEED:MED"); 
      memo=1;
    }
    else if (memo==1 && Incoming_value == 3) 
    {
      digitalWrite(ledPin,HIGH);
      analogWrite(PWM,255);
      lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
      lcd.print("FAN SPEED:HIGH   "); 
      memo=1;
    }
  }
}   
