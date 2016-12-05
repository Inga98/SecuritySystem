#include "DHT.h"

DHT dht;

#include <Ethernet.h>
#include <SPI.h>

int ledPin = 4;                // choose the pin for the LED
int inputPin = 9;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x2A, 0x8D };
byte ip[] = { 10,220,216,83};
byte gw[] = {10,220,216,1};
byte subnet[] = { 255, 255, 255, 0 };

EthernetClient client;//(server, 80);

byte server[] = { 178, 62, 112, 183 }; // Server IP
int mq7_analogPin = A0;


void setup() {
  Ethernet.begin(mac, ip, gw, gw, subnet);
  Serial.begin(9600);
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C) (F) \t\tGas ");

  dht.setup(6); // data pin 2
  delay(5000);
}

void loop()
{
    val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH

    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      digitalWrite(ledPin, HIGH);  // turn LED ON
      delay(1000);
      
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } 
    else {
    digitalWrite(ledPin, LOW); // turn LED OFF
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;

      }

  int mq7_value = analogRead(mq7_analogPin);
  delay(5000);

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float temperatureF = dht.toFahrenheit(temperature);
  float gas = mq7_value;

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  Serial.println(gas, 1);
  
  if (isnan(temperature) || isnan(humidity) || isnan(temperatureF) || isnan(gas)){
    Serial.println("Failed to read from DHT");
  } else {
    senddata(humidity,temperature,temperatureF, gas);
  }
  
    }
  delay(dht.getMinimumSamplingPeriod());

  
}

void senddata(float humidity, float temperature, float temperatureF, int gas)
{
Serial.println();
Serial.println("ATE :)");
delay(5000);                                   //Keeps the connection from freezing

if (client.connect(server, 80)) {
Serial.println("Connected");
client.print("GET /include/actions/add.php?data=");
client.print(humidity);
client.print("&data2=");
client.print(temperature);
client.print("&data3=");
client.print(temperatureF);
client.print("&data4=");
client.print(gas);
client.println(" HTTP/1.1");
client.println("Host: 10.200.10.24");
client.println("Connection: close");
client.println();
Serial.println();
while(client.connected()) {
  while(client.available()) {
    Serial.write(client.read());
    }
   }
}

else
{
Serial.println("Connection unsuccesful");
}
//}
 //stop client
 client.stop();
 while(client.status() != 0)
{
  delay(5);
}
}

