
#include <Keypad.h>
#include <string.h>
#include <ctype.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position


#define button 52

//WIFI
String agAdi = "Madhyam";                 // We write the name of our network here.    
String agSifresi = "12345678";           // We write the password of our network here.
int rxPin = 10;                                               //ESP8266 RX pini
int txPin = 11;
String ip = "184.106.153.149"; 
SoftwareSerial esp(rxPin, txPin);


//ULTRASONIC SENSOR
const int trigPin = 34;
const int echoPin = 36;
const int buzzer = 2;
// const int ledPin = 13;
long duration;
int distance=24;
int safetyDistance;



//KEYPAD AND LCD

char customKey;
LiquidCrystal lcd(30, 32, 22, 24, 26, 28);
long randnum=0;
// int Lock = 32; // Connecting the relay to the 5th pin
// const byte ROWS = 4; //four rows
// const byte COLS = 3; //four columns
// //define the cymbols on the buttons of the keypads
// char hexaKeys[ROWS][COLS] = {
//   {'1','2','3'},
//   {'4','5','6'},
//   {'7','8','9'},
//   {'*','0','#'}
// };
// byte rowPins[ROWS] = {3, 4, 5, 6}; //connect to the row pinouts of the keypad
// byte colPins[COLS] = {7, 8, 9}; //connect to the column pinouts of the keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {3, 4, 5, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 8, 9, 13}; //connect to the column pinouts of the keypad
String temp;
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 
String randstr="";
char inputString[16];
int stringIndex = 0;
int bp=0;
int oldbp=256;



void setup(){
  Serial.begin(9600);
  myservo.attach(14);
  myservo.write(0);
  lcd.begin(16, 2);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(buzzer, OUTPUT);
  // pinMode(ledPin, OUTPUT);
  // Starts the serial communication

  pinMode(button, INPUT);
  Serial.println("Started");
  esp.begin(115200);                                          // We are starting serial communication with ESP8266.
  esp.println("AT");                                         // We do the module control with the AT command.
  Serial.println("AT  sent ");
  while(!esp.find("OK")){                                     // We wait until the module is ready.
    esp.println("AT");
    Serial.println("ESP8266 Not Find.");
  }
  Serial.println("OK Command Received");
  esp.println("AT+CWMODE=1");                                 // We set the ESP8266 module as a client.
  while(!esp.find("OK")){                                     // We wait until the setting is done.
    esp.println("AT+CWMODE=1");
    Serial.println("Setting is ....");
  }
  Serial.println("Set as client");
  Serial.println("Connecting to the Network ...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    // We are connecting to our network.
  while(!esp.find("OK"));                                     // We wait until it is connected to the network.
  Serial.println("connected to the network.");
  delay(1000);
}

// bp=0;

void loop()
{
  
  Serial.print("Button: ");
  Serial.println(bp);
  // Serial.println(bp);
  randomSeed(distance);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
 
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
 
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
 
// Calculating the distance
distance= duration*0.034/2;
 
safetyDistance = distance;
if (safetyDistance <= 25)   // You can change safe distance from here changing value Ex. 20 , 40 , 60 , 80 , 100, all in cm
{
  digitalWrite(buzzer, HIGH);
}
else{
  digitalWrite(buzzer, LOW);
  // digitalWrite(ledPin, LOW);
}
 
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);

esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           // We connect to Thingspeak.
if(esp.find("Error")){                                      // We check the connection error.
    Serial.println("AT+CIPSTART Error");
  }
   String veri = "GET https://api.thingspeak.com/update?api_key=0TCQ3AKGHLIQFC1D";   // Thingspeak command. We write our own api key in the key part.                                   
  veri += "&field1=";
  veri += String(bp);

    
   if(bp==0)
  {
    oldbp=bp;
    if(randnum==0)
    {
      randnum=random(1000,10000);
      Serial.println(randnum);
      temp=String(randnum);
      // otp=1552;
    // digital=0;
    }
    veri += "&field2=";
    veri += String(randnum); 
    veri += "\r\n\r\n"; 
    esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
    esp.println(veri.length()+2);
    delay(200);
    if(esp.find(">")){                                          // The commands in it are running when ESP8266 is ready..
      esp.print(veri);                                          // We send the data.
      Serial.println(veri);
      Serial.println("Data sent.");
  
    Serial.println("Connection Closed.");
    esp.println("AT+CIPCLOSE");                                // we close the link
    delay(100);
    

    }

    
    verifyotp(temp);
  }
  else{
    // otp=1552;
    veri += "&field2=";
    veri += String(randnum);
    veri += "\r\n\r\n"; 
    esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
    esp.println(veri.length()+2);
    delay(200);
    if(esp.find(">")){                                          // The commands in it are running when ESP8266 is ready..
      esp.print(veri);                                          // We send the data.
      Serial.println(veri);
      Serial.println("Data sent.");
      // delay(100);
    }
    Serial.println("Connection Closed.");
    esp.println("AT+CIPCLOSE");                                // we close the link
    delay(100);

  }
  
  
  
}
  
  




void verifyotp(String temp)
{
  customKey = customKeypad.getKey();
            
      if(customKey!=NO_KEY)
      {  if(customKey=='#')
        {
            int size=randstr.length();
            randstr.remove(size-1);
             if (stringIndex > 0) {
              stringIndex--;
              inputString[stringIndex] = '\0'; // Null-terminate string
              lcd.setCursor(stringIndex, 0);
              lcd.print(" "); // Clear LCD at current position
              lcd.setCursor(stringIndex, 0);
      }            
        }
        else
        {
          // lcd.print(customKey);
          if (stringIndex < 15) {
          inputString[stringIndex] = customKey;
          stringIndex++;
          inputString[stringIndex] = '\0'; // Null-terminate string
          lcd.print(customKey);
      }  
          randstr+=customKey;
          
          if ((randstr)==(temp))
          {
            lcd.clear();
            lcd.print("Lock Opened");
            Serial.println("Same");
            // for (pos = 0; pos <= 90; pos += 1) { 
            //   myservo.write(pos);
            //   // delay(15);
            // }
            myservo.write(90);
            delay(60000);
            myservo.write(0);
            // digitalWrite(Lock, LOW);
            randstr="";
          }
        }                
      }
                            
  // delay(7000);
}