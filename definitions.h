// Dallas Temperature. 
OneWire ds(3);
DallasTemperature sensors(&ds);

void readDS() {
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 client.publish("controller/1/temp", String(sensors.getTempCByIndex(0)).c_str());
}


/********************************************************************/
// Hearbeat for WatchDog
#define hbPin 2
int hwHeartbeat = HIGH;
timer hbTimer = timer(1000 * 3); // 3 second heartbeat

void hbTimerFunc() {
  if (hbTimer.update() == 1) {
    if (hwHeartbeat == HIGH) {
      digitalWrite(hbPin, LOW);
      hwHeartbeat = LOW;
      Serial.println( "LOW" );
    } else {
      digitalWrite(hbPin, HIGH);
      hwHeartbeat = HIGH;
      Serial.println( "HIGH" );
    }
    hbTimer.reset();
  }
}

/********************************************************************/
// This timer object is for scheduling device reads
SimpleTimer timer;


/********************************************************************/
//Button Definitions
#define BUTTON_PIN22 22
Bounce debouncer22 = Bounce(); 
#define BUTTON_PIN23 23
Bounce debouncer23 = Bounce(); 

void initButton() {
  pinMode(BUTTON_PIN22,INPUT_PULLUP);
  debouncer22.attach(BUTTON_PIN22);
  debouncer22.interval(200);

  pinMode(BUTTON_PIN23,INPUT_PULLUP);
  debouncer23.attach(BUTTON_PIN23);
  debouncer23.interval(200);
}

void checkButton() {
   debouncer22.update();
   if ( debouncer22.fell() ) {
    client.publish("controller/1/pin/22/btn", "1");
   }
   debouncer23.update();
   if ( debouncer23.fell() ) {
    client.publish("controller/1/pin/23/btn", "1");
   }
}

/********************************************************************/
//PIR Sensor Definitions
int PIR10 = 10;  
unsigned long motiontimer10;
boolean inmotion10 = false; 

int PIR11 = 11;  
unsigned long motiontimer11;
boolean inmotion11 = false; 

int PIR12 = 12;  
unsigned long motiontimer12;
boolean inmotion12 = false; 

unsigned long motionDelay = 5000; // Motion Delay Timer

void initPIR() {
  pinMode(PIR10,INPUT);
  digitalWrite(PIR10,LOW);
  pinMode(PIR11,INPUT);
  digitalWrite(PIR11,LOW);
  pinMode(PIR12,INPUT);
  digitalWrite(PIR12,LOW);
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

  if (digitalRead(PIR12) == HIGH && !inmotion12)
  {
    client.publish("controller/1/pin/12/pir", String(digitalRead(PIR11)).c_str());
    Serial.println("Motion12 Detected");
    motiontimer12 = millis();
    inmotion12 = true;
  } 
  else if (millis() - motiontimer12 >= motionDelay) 
  {
    inmotion12 = false;
  }
}


/********************************************************************/
//DHT Sensor Definitions
DHT dht4(38, DHT22);
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


/********************************************************************/
//Ethernet MAC and IP definition
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(172, 16, 33, 200);

void getMAC() {
  byte i;
  byte dsAddress[8];
  Serial.println( "Searching for DS18B20..." );
  ds.reset_search();  // Start the search with the first device
  if( !ds.search(dsAddress) )
  {
    Serial.println( "none found. Using default MAC address." );
  } else {
    Serial.println( "success. Setting MAC address:" );
    Serial.print( " DS18B20 ROM  =" );
    for( i = 0; i < 8; i++)
    {
      Serial.write(' ');
      Serial.print( dsAddress[i], HEX );
    }
    Serial.println();
    // Offset array to skip DS18B20 family code, and skip mac[0]
    mac[1] = dsAddress[3];
    mac[2] = dsAddress[4];
    mac[3] = dsAddress[5];
    mac[4] = dsAddress[6];
    mac[5] = dsAddress[7];
  }    
  Serial.print( " Ethernet MAC =" );
  for( i = 0; i < 6; i++ )
  {
    Serial.write( ' ' );
    Serial.print( mac[i], HEX );
  }
  Serial.println();
}
