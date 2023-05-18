//tasks schedule 
#include <ExampleConstants.h>
#include <SchedBase.h>
#include <SchedTask.h>
#include <SchedTaskT.h>

//DHT 
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//Soil salinity sensor
#include <SoftwareSerial.h>
#include <Wire.h>
#define RE 9
#define DE 8
const byte ec[] = {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xCE};
const byte salinity[] = {0x01, 0x03, 0x00, 0x14, 0x00, 0x01, 0xC4, 0x0E};
byte values[8];
SoftwareSerial mod(2, 3);

//soil moisture 
const int Moisture_Signal=A0;

//Water level
const int pingPin = 4; // Trigger Pin 
const int echoPin = 5; // Echo Pin 

//Water pump
const int Pump = 10;

const int Valve = 11;

//Methods for scheduling
void PrintVariables();
String getSoilEc();
void irrigationM();
void vpdControl();
SchedTask printV(0,30000,PrintVariables);
SchedTask irrigation(3000,30000,irrigationM);
SchedTask vpdC(5000,30000,vpdControl);

// Variables of the system
float Humeditaty, Tempreature, VPD;
int soil_ec, soil_salinity, Moisture, cm;
int counter=0;
int work=0;

void setup(){
      Serial.begin(9600);
      
      //DHT
      dht.begin();
      
      //Soil salinity 
      mod.begin(9600);
      pinMode(RE, OUTPUT);
      pinMode(DE, OUTPUT);
      
      //Water level
      pinMode(pingPin, OUTPUT);
      pinMode(echoPin, INPUT);

      //pump
      pinMode(Pump, OUTPUT);

      //Valve
      pinMode(Valve, OUTPUT);
      
      digitalWrite(Pump, LOW);
      digitalWrite(Valve, HIGH);
      
}

void loop(){
      SchedBase::dispatcher();
}

float getTemp1(){
    float tempreature = dht.readTemperature();
    return tempreature;
}

float getHum(){
    float Humeditaty = dht.readHumidity();
    return Humeditaty;
}

String getSoilEc(){
  /**************Soil EC Reading*******************/  
 digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
 
  if (mod.write(ec, sizeof(ec)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
    }
  }
  int soil_ec = int(values[3]<<8|values[4]);
  delay(1000);
/**************Soil Salinity Reading*******************/ 
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(salinity, sizeof(salinity)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
    }
  }
  int soil_salinity = int(values[4]);
  delay(1000);
  String Soil= String(soil_ec)+"C"+String(soil_salinity)+"D";
//  Serial.print(soil_ec);       Serial.print("C");
//  Serial.print(soil_salinity); Serial.print("D");
  return Soil;
  
}

int soilMoisture(){
    int Moisture=analogRead(Moisture_Signal);
    return Moisture;
}

int waterTank(){
    int duration;
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    int cm = duration / 29 / 2;
    return cm;
}

void irrigationM(){
  if( cm <= 26  ){
     
    int Moisture=soilMoisture();
    
    if( Moisture>540){ //changed based on the plant needs
      //activate pump 
        digitalWrite(Pump, HIGH);
        delay(10);
        int i=0;
        while(i<6){
          //activate pump 
          digitalWrite(Pump, HIGH);
          delay(3000);
          digitalWrite(Pump, LOW);
          delay(10000);
          i=i+1;
//          Moisture=soilMoisture();
//          delay(50);
//          if(Moisture<480){
//            //deactivate pump
//            digitalWrite(Pump, LOW);
//            delay(10);
//            work=work+1;
//            break;
//          }
        }
      }
    return;
  }
}

float getVpdValue(){
     float Tempreature= getTemp1();
     float Humeditaty= getHum();
     float SVP = 0.6108 * exp((17.27*Tempreature)/(237.3+Tempreature)); 
     float AVP= SVP* Humeditaty/100;
     float VPD= SVP-AVP;
     return VPD;
}

void vpdControl(){
  if( cm <= 34  ){
    
  if (counter>100){
     float VPD= getVpdValue();
     counter=0;
     if ( (VPD> 1.8) ){
      digitalWrite(Pump, HIGH);
      digitalWrite(Valve, LOW);
      delay(40000);
      digitalWrite(Pump, LOW); //five minutes 
      digitalWrite(Valve, HIGH);
      work=work+1;
     }else {
      digitalWrite(Valve, HIGH);
     }
  }
  }
     return;
}

void PrintVariables(){
       digitalWrite(Pump, LOW);
      digitalWrite(Valve, HIGH);
       Tempreature= getTemp1();
       Humeditaty= getHum();
       String Soil=getSoilEc();
       Moisture= soilMoisture();
       cm= waterTank();
       VPD= getVpdValue();
       if ( !(isnan(Tempreature) |  isnan(Humeditaty) |  isnan(VPD) | isnan(soil_ec) | isnan(soil_salinity) | isnan(Moisture) | isnan(cm)) ){
        
        if (VPD>1.8){
        counter=counter+1;
       }
       
       Serial.print("M");
       Serial.print(Tempreature);   Serial.print("A");
       Serial.print(Humeditaty);    Serial.print("B");
       Serial.print(Soil);
       Serial.print(Moisture);    Serial.print("E");
       Serial.print(VPD);           Serial.print("F");
       //Serial.print(cm);           Serial.print("G");
       
       if(cm <= 16) 
       {
        Serial.print("2");
       }
       if(  (cm > 16) && (cm <= 34)  ) 
       {
        Serial.print("1");
       } else if(  (cm > 34)   ) 
       {
        Serial.print("0");
       }
       
       Serial.print("G");
       Serial.print(work);           Serial.print("H");
       
       Serial.print("\n");
       }
       
       return;
}
