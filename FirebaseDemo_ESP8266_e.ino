#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ExampleConstants.h>
#include <SchedBase.h>
#include <SchedTask.h>
#include <SchedTaskT.h>

// Set these to run example.
#define FIREBASE_HOST "fasela-6c09f-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "e9oW51sWf7LlXWRssvRAHiWgojEbhgFPZUdV7Njn"
#define WIFI_SSID "HUAWEI-E5330-E402"
#define WIFI_PASSWORD "7tftiq48"

//recieve 
char sensor_data;
String dataIn; 
String checkM;
int8_t indexOfA, indexOfB,indexOfC,indexOfD,indexOfE,indexOfF,indexOfG, indexOfH;
String Humeditaty, Tempreature, VPD, soil_ec, soil_salinity, Moisture, cm, work;


//methods 
void recieveMdata();


SchedTask recieveM(0,10000,recieveMdata);

void setup() {
    Serial.begin(9600);

    // connect to wifi.
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
       Serial.print(".");
       delay(500);
    }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


void loop() {

      SchedBase::dispatcher();

      // update value
      //Firebase.setFloat("number", 43.0);
            

      // remove value
      //Firebase.remove("number");
      //delay(10);

}

void parseTheData(){
    indexOfA=dataIn.indexOf("A");
    indexOfB=dataIn.indexOf("B");
    indexOfC=dataIn.indexOf("C");
    indexOfD=dataIn.indexOf("D");
    indexOfE=dataIn.indexOf("E");
    indexOfF=dataIn.indexOf("F");
    indexOfG=dataIn.indexOf("G");
    indexOfH=dataIn.indexOf("H");
    
    checkM=dataIn.substring(0,1);
    Tempreature=dataIn.substring(1,indexOfA);
    Humeditaty=dataIn.substring(indexOfA+1,indexOfB);
    soil_ec=dataIn.substring(indexOfB+1,indexOfC);
    soil_salinity=dataIn.substring(indexOfC+1,indexOfD);
    Moisture=dataIn.substring(indexOfD+1,indexOfE);
    VPD=dataIn.substring(indexOfE+1,indexOfF);
    cm=dataIn.substring(indexOfF+1,indexOfG);
    work=dataIn.substring(indexOfG+1,indexOfH);
    
}

void recieveMdata(){
     while(Serial.available()){
        //get sensor data from serial put in sensor_data
        sensor_data=Serial.read(); 
        
        if(sensor_data=='\n') {
          break;
        }
        else {
          dataIn+=sensor_data;
        }      
       }
  
      
      if(sensor_data=='\n'){  
        parseTheData();
        //show
        if(checkM=="M"){
        Firebase.setString("/tomato/airTemperature", Tempreature); 
          delay(10);
        Firebase.setString("/tomato/airHumidity", Humeditaty); 
          delay(10);
        Firebase.setString("/tomato/soilEC", soil_ec); 
          delay(10);
        Firebase.setString("/tomato/soilSalinity", soil_salinity); 
          delay(10);
        Firebase.setString("/tomato/soilMoisture", Moisture); 
          delay(10);
        Firebase.setString("/tomato/VPDValue", VPD); 
          delay(10);
        Firebase.setString("/tomato/waterLevel", cm); 
          delay(10);
        Firebase.setString("/tomato/PumpOnFor", work); 
          delay(10);
        }
        //reset
        sensor_data=0;
        dataIn="";
      }
       if (Firebase.failed()) {  
        return;
      }
}
