

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "Adafruit_CCS811.h"
#include "credentials.h"


#define LEDPIN_1 12
#define LEDPIN_2 14
#define LDRPIN 35

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int this_thing = 0;
float voltage = 0.0;


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;


//CCS811 global variables
uint16_t co2 = 0;
uint16_t tvoc = 0;

float temp = 0.0;
float humidity = 0.0;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  


Adafruit_CCS811 ccs;

DHT dht(26, DHT11);


void setup() {
  Serial.begin(9600);

  //-------------------Setting up WIFI and accessing firebase-----------------------
  WiFi.begin(SSID,PASS);
  Serial.print("Connecting to wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = URL;
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signup OK");
    signupOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);



  //-----------------Setting up SARACEn devices--------------------------------
  
  //CCS811 SETUP
  Serial.println("CCS811 test");
  if(!Wire.begin()){
    Serial.println("Failed to establish I2C connection.");
    while(1);
  }
  if(!ccs.begin()){
    Serial.println("Failed to start CCS811! Please check your wiring.");
    while(1);
  }
  while(!ccs.available());


  //DHT SETUP
  dht.begin();

  //LCD SETUP
  lcd.init();
  lcd.backlight();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SARACEn by");
  lcd.setCursor(0,1);
  lcd.print("HACKERSCHOOL");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Special thanks");
  lcd.setCursor(0,1);
  lcd.print("to OPENLAB crew");
  delay(2000);

}

void loop() {
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    //-------------------------- SARASEn measure gathering and emission to LCD's ------------------------------

    if(ccs.available()){
      if(!ccs.readData()){
        Serial.print("CO2: ");
        Serial.print(co2 = ccs.geteCO2());
        Serial.print("ppm, TVOC: ");
        Serial.println(tvoc = ccs.getTVOC());
      }
      else{
        Serial.println("ERROR!");
        while(1);
      }
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CCS811 report:");
    lcd.setCursor(0,1);
    lcd.print("C:");
    // C stands for Carbon dioxide
    lcd.print(co2);
    lcd.print(" TV:");
    // TV stands for Total Volatile Organic Compounds
    lcd.print(tvoc);
    delay(2000);

    temp = dht.readTemperature();
    humidity = dht.readHumidity();
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" C ");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" % ");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT11 report:");
    lcd.setCursor(0,1);
    lcd.print("T:");
    // T stands for temperature
    lcd.print(temp);
    lcd.print(" H:");
    // H stands for humidity
    lcd.print(humidity);
    delay(2000);

    //---------------- End of measure gathering and emmission to LCD's------------------



    //---------------- Send data to real time database ----------------------

    if(Firebase.RTDB.setInt(&fbdo,"Sensor/CO2_(ppm)", co2)){
      Serial.println(); Serial.print(temp);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
        if(Firebase.RTDB.setFloat(&fbdo,"Sensor/TVOC_levels_(ppb)", tvoc)){
      Serial.println(); Serial.print(humidity);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
    if(Firebase.RTDB.setInt(&fbdo,"Sensor/Temperature_(Celcius)", temp)){
      Serial.println(); Serial.print(temp);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
        if(Firebase.RTDB.setFloat(&fbdo,"Sensor/Humidity_(percentage)", humidity)){
      Serial.println(); Serial.print(humidity);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");

    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
  }
}