#include <Servo.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN A5

//Pin Definitions
  int pinTimeDay=A1;
  int pinTimeNight=A0;
  int pinValveHeat=A2;
  int pinValveCool=A3;
  int pinMotor1=3;
  int pinMotor2=5;
  int pinThermistor = A4;
  int Vout=13;
  int pinServo = 6;
  int pinDayNight=7;
  
//Variable Definitions
  int timeDay=0;
  int timeNight=0;
  int valveHeat=0;
  int valveCool=0;
  int motorSpeed=0;
  int motorPercent=0;
  int Vo;
  float Toa;
  int Hoa;
  float R1 = 10000;
  float logR2, R2, Tia;
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  int pos = 0; 
  float OAdampPos;
  float RAdampPos;
  Servo myOAdamper;
  int Tsp=70;
  float Tra;
  float Tdiff;
  

void setup(){
  Serial.begin(9600);
  pinMode(pinTimeDay,OUTPUT);
  pinMode(pinTimeNight,OUTPUT);
  pinMode(pinValveHeat,OUTPUT);
  pinMode(pinValveCool,OUTPUT);
  pinMode(pinMotor1,OUTPUT);
  pinMode(pinMotor2,OUTPUT);
  pinMode(Vout,OUTPUT);
  pinMode(pinDayNight,INPUT);
  myOAdamper.attach(pinServo);
}

void loop(){

//Day and Night System Schedule
  int DayNight = digitalRead(pinDayNight);
  if (DayNight == 0){
    digitalWrite(pinTimeNight,LOW);
    digitalWrite(pinTimeDay,HIGH);
  }
  else{
    motorPercent=0;
    OAdampPos = 0;
    digitalWrite(pinTimeNight,HIGH);
    digitalWrite(pinTimeDay,LOW);   
    digitalWrite(pinValveCool,LOW);
    digitalWrite(pinValveHeat,LOW);
    Tra = Tia+10;   //adding 10F to simulate return air temp in summer
    Tdiff = Toa - Tra;
   }

//INDOOR AIR TEMPERATURE
  Vo = analogRead(pinThermistor);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  Tia = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tia = Tia - 273.15;
  Tia = (Tia * 9.0)/ 5.0 + 32.0; 

//OUTDOOR AIR TEMPERATURE
  int chk = DHT.read11(DHT11_PIN);
  Toa=DHT.temperature*1.8+32; //temp in F
  Hoa=DHT.humidity;
  

//CONTROL LOGIC
  if (DayNight == 0){
  
    motorPercent=60;
    Tra = Tia+10;   //adding 10F to simulate return air temp in summer
    Tdiff = Toa - Tra;
  
    if (-10 <= Tdiff && Tdiff < 0){
      OAdampPos = 100;
    }
  
    if (-40 <= Tdiff && Tdiff < -10) {
      OAdampPos = 100*1.71*(exp(0.0536*Tdiff));
    }
  
    if (Tdiff < -40) {
      OAdampPos = 20;
      digitalWrite(pinValveHeat,HIGH);
    }
      else{
        digitalWrite(pinValveHeat,LOW); 
      }
  
    if (0 <= Tdiff) {
      OAdampPos = 20;
      digitalWrite(pinValveCool,HIGH);
    }
      else{
        digitalWrite(pinValveCool,LOW);
      }
  }
  
//SUPPLY FAN MOTOR CONTROL
  
  motorSpeed = motorPercent*(255./100.);  //converstion for 0-255 to 0-100
  analogWrite(Vout,motorSpeed);
  digitalWrite(pinMotor1,LOW);  //change Low/high outputs to change direction of fan
  digitalWrite(pinMotor2,HIGH);

//OUTSIDE AIR DAMPER
  /*Test: Happy range is 70 to 180 degrees
  Serial.println("Servo position?");
  while (Serial.available()==0) {} 
  pos = Serial.parseInt();
  myPointer.write(pos);*/
  
  pos=-(110./100.)*OAdampPos+180.; //conversion for 70-180 deg to 0-100%
  myOAdamper.write(pos);

//RETURN AIR DAMPER

RAdampPos= 100 - OAdampPos;
    
//PRINT STATEMENTS
  Serial.println("****CURRENT READINGS****");
  Serial.println("Temperatures");
  Serial.print("OAT: ");
  Serial.print(Toa);
  Serial.println(" F");
  
  Serial.print("IAT: "); 
  Serial.print(Tia);
  Serial.println(" F");
  
  Serial.print("RAT: "); 
  Serial.print(Tra);
  Serial.println(" F");

  Serial.print("MAT: "); 
  float MAT = Tra - (OAdampPos/100)*(Tra-Toa); //From OA%=(RAT-MAT)/(RAT-OAT)
  Serial.print(MAT);
  Serial.println(" F");
  
  Serial.print("Tdiff (OAT-RAT): "); 
  Serial.print(Tdiff);
  Serial.println(" F");
  Serial.println(" ");
  
  Serial.println("Damper Positions");
  Serial.print("Outside Air: ");
  Serial.print(OAdampPos);
  Serial.println("%");  
  
  Serial.print("Return Air: ");
  Serial.print(RAdampPos);
  Serial.println("%");
  Serial.println(" ");

  Serial.println("Motor Condition");
  Serial.print("Percent: ");
  Serial.println(motorPercent);
  Serial.println(" ");
  
  Serial.println("Equipment Status");
  Serial.print("LED Day: ");
  int PTD = digitalRead(pinTimeDay);
  Serial.print(PTD);
  Serial.println(" ");
  
  Serial.print("LED Night: ");
  int PTN = digitalRead(pinTimeNight);
  Serial.print(PTN);
  Serial.println(" ");

  Serial.print("LED Heating: ");
  int PVH = digitalRead(pinValveHeat);
  Serial.print(PVH);
  Serial.println(" ");
  
  Serial.print("LED Cooling: ");
  int PVC = digitalRead(pinValveCool);
  Serial.print(PVC);
  Serial.println(" ");

  Serial.print("Light Sensor: ");
  int PDN = digitalRead(pinDayNight);
  Serial.print(PDN);
  Serial.println(" ");
    
  delay(3000);
  
}