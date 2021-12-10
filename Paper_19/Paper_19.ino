#include <dht11.h>
//#include <MQ2.h>
#include <SoftwareSerial.h>

//MQ2 mq2(smokeA0);
int smokeA0 = A0;
// utlrasonic pinout
#define ULTRASONIC_TRIG_PIN     5   // pin TRIG to D5
#define ULTRASONIC_ECHO_PIN     3   // pin ECHO to D3
int dht11Pin = 2;
float temp,hum;
dht11 DHT11;

int rxPin = 00;
int txPin = 01;
String ip = "184.106.153.149";
// user config: TODO
String ssid = "SSID";             // SSID
String password = "PWD";         // WIFI

String host = "GET https://api.thingspeak.com/update?api_key=VJUMSAVZRS5D1WS9";
SoftwareSerial esp(rxPin, txPin);

void setup() {
  
  Serial.begin(115200);  // We are starting our communication with the serial port.
  Serial.println("Started");
  esp.begin(115200);                                          // We are starting serial communication with ESP8266.
  esp.println("AT");                                         // We do the module control with the AT command.
  Serial.println("AT  sent ");
  while(!esp.find("OK")){                                     // We wait until the module is ready.
    esp.println("AT");
    Serial.println("ESP8266 Not Found.");
  }
  Serial.println("OK Command Received");
  esp.println("AT+CWMODE=1");                                 // We set the ESP8266 module as a client.
  while(!esp.find("OK")){                                     // We wait until the setting is done.
    esp.println("AT+CWMODE=1");
    Serial.println("Setting is ....");
  }
  Serial.println("Set as client");
  Serial.println("Connecting to the Network ...");
  esp.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");    // We are connecting to our network.
  while(!esp.find("OK"));                                     // We wait until it is connected to the network.
  Serial.println("Connected to the Network.");
  delay(1000);

  // ultraonic setup 
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  //mq2.begin();
  //pinMode(smokeA0, INPUT);
}


void loop() {
    
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           // We connect to Thingspeak.
  if(esp.find("Error")){                                      // We check the connection error.
    Serial.println("AT+CIPSTART Error");
  }
//int values= mq2.read(false); //set it false if you don't want to print the values in the Serial
        DHT11.read(dht11Pin);
        temp = (float)DHT11.temperature;
        hum = (float)DHT11.humidity;
        long duration, distance;
        digitalWrite(ULTRASONIC_TRIG_PIN, LOW);  
        delayMicroseconds(2); 
        
        digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
        delayMicroseconds(10); 
        
        digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
        duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
        distance = (duration/2) / 29.1;
        Serial.print("********** Ultrasonic Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

//MQ2 Sensor Value
        
        int analogSensor = analogRead(smokeA0);
        Serial.println("MQ2 Value:");
        Serial.print(analogSensor);  
        // Create a URL for the request
        String url = "";
        url += host;
        url += "&field1=";
        url += String(temp);// The Temperature value will be sent
        url += "&field2=";
        url += String(hum);// The Humidity Value will be sent
        url += "&field3=";
        url += distance;// The distance will be sent
        url += "&field4=";
        url += String(analogSensor);// The analogSensor value we will sent
        url += "\r\n\r\n";
        esp.print("AT+CIPSEND=");
        esp.println(url.length()+2);
        delay(2000);
        //if(esp.find(">")){
          esp.print(url);
          Serial.print(url);
          Serial.println("Data Sent");
          delay(1000);
          //}
        Serial.println("Connection Closed");
        esp.println("AT+CIPCLOSE");
        delay(1000);
}
