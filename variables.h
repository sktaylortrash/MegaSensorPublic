//Ethernet MAC and IP definition
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(172, 16, 33, 200);
OneWire ds(4);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&ds);

void readDS() {
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 client.publish("controller/1/temp", String(sensors.getTempCByIndex(0)).c_str());

}

// the timer object
SimpleTimer timer;

//Button GPIO Definitions
#define BUTTON_PIN0 0
Bounce debouncer0 = Bounce(); 
#define BUTTON_PIN1 1
Bounce debouncer1 = Bounce(); 



//PIR Sensor Definitions
int PIR10 = 10;  
unsigned long motiontimer10;
boolean inmotion10 = false; 

int PIR11 = 11;  
unsigned long motiontimer11;
boolean inmotion11 = false; 

unsigned long motionDelay = 5000; // Motion Delay Timer

void initPIR() {
  pinMode(PIR10,INPUT);
  digitalWrite(PIR10,LOW);
  pinMode(PIR11,INPUT);
  digitalWrite(PIR11,LOW);
}

void readPIR() {
  if (digitalRead(PIR10) == HIGH && !inmotion10)
  {
    client.publish("controller/1/pin/10/pir", String(digitalRead(PIR10)).c_str());
    Serial.println("Motion10 Detected");
    motiontimer10 = millis();
    inmotion10 = true;
  } 
  else if (millis() - motiontimer10 >= motionDelay) 
  {
    inmotion10 = false;
  }

  if (digitalRead(PIR11) == HIGH && !inmotion11)
  {
    client.publish("controller/1/pin/11/pir", String(digitalRead(PIR11)).c_str());
    Serial.println("Motion11 Detected");
    motiontimer11 = millis();
    inmotion11 = true;
  } 
  else if (millis() - motiontimer11 >= motionDelay) 
  {
    inmotion11 = false;
  }


//  client.publish("controller/1/pin/2/pir", String(digitalRead(PIR11)).c_str());
//  Serial.println(digitalRead(PIR11));
}
//DHT Sensor Definitions
DHT dht4(38, DHT11);
DHT dht5(39, DHT11);
DHT dht6(40, DHT11);
DHT dht7(41, DHT11);

// Now declare the variables that will hold the sensor readings
float dht4_temp,
      dht5_temp,
      dht6_temp,
      dht7_temp;

float dht4_humi,
      dht5_humi,
      dht6_humi,
      dht7_humi;

void initDHT() {
    dht4.begin();
    dht5.begin();
    dht6.begin();
    dht7.begin();
}

void readDHT() {
  dht4_temp = dht4.readTemperature();
  dht4_humi = dht4.readHumidity();
    client.publish("controller/1/pin/2/temp", String(dht4_temp).c_str());
    client.publish("controller/1/pin/2/humi", String(dht4_humi).c_str());
    
  dht5_temp = dht5.readTemperature();
  dht5_humi = dht5.readHumidity();
    client.publish("controller/1/pin/3/temp", String(dht5_temp).c_str());
    client.publish("controller/1/pin/3/humi", String(dht5_humi).c_str());

  dht6_temp = dht6.readTemperature();
  dht6_humi = dht6.readHumidity();
    client.publish("controller/1/pin/12/temp", String(dht6_temp).c_str());
    client.publish("controller/1/pin/12/humi", String(dht6_humi).c_str());

  dht7_temp = dht7.readTemperature();
  dht7_humi = dht7.readHumidity();
    client.publish("controller/1/pin/29/temp", String(dht7_temp).c_str());
    client.publish("controller/1/pin/29/humi", String(dht7_humi).c_str());
}
