#include <dht.h>
#include <Adafruit_Sensor.h>
 
#define dht_apin A3 // Analog Pin sensor is connected to
int pinled1 = 22;
int pinled2 = 23;
 
dht DHT;
bool transmit;
void setup(){
  pinMode(pinled1, OUTPUT);
  pinMode(pinled2, OUTPUT);
  //Serial.begin(57600);
  Serial.begin(9600);
  
  //delay(500);
 
}//end "setup()"
 
void loop(){
  //Start of Program 

    int digit1s = 0;
    int digit10s = 0;
    int digit100s = 0;
    int digit1000s = 0;
    int weightval = 98;
    digit1s = weightval%10;
    digit10s = (weightval%100)/10;
    digit100s = (weightval%1000)/100;
    digit1000s = (weightval%10000)/1000;
    char arr[4]={digit1000s,digit100s,digit10s,digit1s};
    delay(100);
    for(int i=0; i<4; i++)
    {
       Serial.write(arr[i]);
       //delay(10);
       
    }
    DHT.read11(dht_apin);
    if(DHT.humidity>80)
    {
      digitalWrite(pinled1, HIGH);
      digitalWrite(pinled2, LOW);
      Serial.write('A');
      /*Serial.write(digit1000s);
      
      Serial.write(digit100s);
      delay(50);
      Serial.write(digit10s);
      delay(50);
      Serial.write(digit1s);
      delay(20);*/
     
    }

    else
    {
      digitalWrite(pinled2, HIGH);
      digitalWrite(pinled1, LOW);
      Serial.write('B');
    }
    //delay(500);
 
}
