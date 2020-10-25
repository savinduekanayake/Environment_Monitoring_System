#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>

WiFiClientSecure client;

#include <stdarg.h>
#include <string.h>

String url;
char column_name[ ][20] = {"date", "time", "temperature", "humidity", "pressure", "rain", "altitude"};;

double random_values[100];

const char* host = "script.google.com";
const int httpsPort = 443;

char Sensor_Values[10][20];
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID;
int Count;


char column_name_in_sheets[ ][20] = {"value1", "value2", "value3", "value4", "value5", "value6", "value7"};
String Sheets_GAS_ID = "AKfycbzJUP3VPH8CqILhf9m2_hZvE2mu1WgGKxDKBN108pAaggtuE4I";
int No_of_Parameters = 7;


#include "PageIndex.h"
ESP8266WebServer server2(80);


void handleRoot() {
  server2.send(200, "text/html", MAIN_page);
}


#include <dht11.h>
#define DHT11PIN D4
dht11 DHT11;


#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

float temperature;
float humidity;
float pressure;
float rain;
float altitude;

int chk;




float temperaturearray[1000];
float humidityarray[1000];
float pressurearray[1000];
float rainarray[1000];
float altitudearray[1000];

int counter1 = 1;



#include <SPI.h>
#include <SD.h>

File myFile;

const int chipSelect = D3;

double counter = 0;


// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int currentYear;
int currentMonth;
int currentDay;
int currentHour;
int currentMinutes;
String date;

String currentDate1;
String currentTime1;


long time1;
long time2;
void setup()
{

  Serial.begin(115200);

  pinMode(A0, INPUT);               // Rain sensor pin


  while (!bmp.begin())
  {
    Serial.println("Not connected with BMP180/BMP085 sensor, check connections ");
  }

  while (!SD.begin(D3)) {
    Serial.println("SD card not found!");
  }

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  /*uncoment for reset the time*/

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }




  while (!Serial);


  WIFI_Connect();

  server2.on("/", handleRoot);
  server2.on("/readTemperature", handleTemperature);
  server2.on("/readHumidity", handleHumidity);
  server2.on("/readPressure", handlePressure);
  server2.on("/readRain", handleRain);
  server2.on("/readAltitude", handleAltitude);

  server2.begin();

  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );



}

void loop()
{
  time1 = millis();
  time2 = millis();
  while ((time2 - time1) < 900000) {
    time2 = millis();
    temperature = bmp.readTemperature();

    chk = DHT11.read(DHT11PIN);
    humidity = DHT11.humidity;

    pressure = bmp.readPressure();

    rain = (100 - map(analogRead(A0), 0, 1024, 0, 100));

    altitude = bmp.readAltitude();


    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.print(" *C   ");

    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.print(" %   ");

    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.print(" Pa   ");

    Serial.print("Rain = ");
    Serial.print(rain);
    Serial.print(" %   ");

    Serial.print("Altitude = ");
    Serial.print(altitude);
    Serial.println(" m   ");



    for (int x = 0; x < 50; x++) {
      handleTemperature();
      handleHumidity();
      handlePressure();
      handleRain();
      handleAltitude();
      server2.handleClient();
      delay(100);
    }
    delay(1);
    counter++;
  }
  getTime ();
  myFile = SD.open("Environmental_Monitering_System.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("Writing to SD card");
    myFile.println(String(currentYear) + "/" + String(currentMonth) + "/" + String(currentDay) + " " + date + " " + String(currentHour) + ":" + String(currentMinutes) + " " + "Temperature = " + String(temperature) + " *C   " + "Humidity = " + String(humidity) + " %   " + "Pressure = " + String(pressure) + " Pa   " + "Rain = " + String(rain) + " %   " + "Altitude = " + String(altitude) + " m   ");
    myFile.close();
  }




  if (WiFi.status() != WL_CONNECTED)
  {
    temperaturearray[counter1];
    humidityarray[counter1];
    pressurearray[counter1];
    rainarray[counter1];
    altitudearray[counter1];
    Serial.print("no wifi");
    counter1++;
  }
  else {

    //    if (counter1 > 1) {
    //
    //      for (int x = 1; x <= counter1; x++) {
    //        Data_to_Sheets(No_of_Parameters, 0, 0, temperaturearray[x], humidityarray[x],  pressurearray[x], rainarray[x], altitudearray[x]);
    //        delay(500);
    //      }
    //    } else {
    Data_to_Sheets(No_of_Parameters, 0, 0, temperature, humidity, pressure, rain, altitude);
    // }

    // counter1 = 1;
  }



}



void handleTemperature() {
  String Temperature_Value = String(temperature);

  server2.send(200, "text/plane", Temperature_Value);

}

void handleHumidity() {
  String Humidity_Value = String(humidity);

  server2.send(200, "text/plane", Humidity_Value);

}



void handlePressure() {
  String Pressure_Value = String(pressure);

  server2.send(200, "text/plane", Pressure_Value);

}

void handleRain() {
  String Rain_Value = String(rain);

  server2.send(200, "text/plane", Rain_Value);

}

void handleAltitude() {
  String Altitude_Value = String(altitude);

  server2.send(200, "text/plane", Altitude_Value);

}



void Google_Sheets_Init(char test[ ][20], String sheets_gas_id, int param_size)
{
  GAS_ID = sheets_gas_id;
  Count = param_size;

  for (int i = 0; i < Count; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      column_name[i][j] = test[i][j];
    }
  }


  for (int i = 0; i < Count; i++)
  {
    Serial.print("column_name= ");
    Serial.println(column_name[i]);
  }

}



void Data_to_Sheets(int num, ...)
{
  va_list lst;
  va_start(lst, num);

  for (int i = 0; i < num; i++)
  {
    random_values[i] = va_arg(lst, double);
  }
  va_end(lst);

  float_to_string();
  Send_Data();
}



void float_to_string()
{

  currentDate1 = String(currentYear) + "/" + String(currentMonth) + "/" + String(currentDay) + +"(" + date + ")";
  currentTime1 = String(currentHour) + ":" + String(currentMinutes);
  currentDate1.toCharArray( Sensor_Values[0], 20);
  currentTime1.toCharArray( Sensor_Values[1], 20);
  for (int j = 2; j < Count; j++)
  {
    sprintf(Sensor_Values[j], "%.02f", random_values[j - 1]);

    Serial.print("Sensor Values : ");
    Serial.println(Sensor_Values[j]);

  }
}


void Send_Data()
{
  sheets_initialization();

  String url = "/macros/s/" + GAS_ID + "/exec?";
  int i = 0;
  while (i != Count)
  {
    if (i == 0)
    {
      url = url + column_name[i] + "=" + Sensor_Values[i];
      i++;
    }
    if (i == Count)
      break;
    url = url + "&" + column_name[i] + "=" + Sensor_Values[i];
    i++;
  }

  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");


  Serial.println("request sent");

  while (client.connected())
  {
    String line = client.readStringUntil('\n');

    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');

  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  }

  else {
    Serial.println("esp8266/Arduino CI has failed");
  }


  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");

}


void sheets_initialization()
{
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);


  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  }

  else {

    Serial.println("certificate doesn't match");
  }

}




void WIFI_Connect()
{
  WiFiManager wifiManager;
  Serial.println("Conecting.....");
  wifiManager.autoConnect("ENVIRONMENTAL MONITORING SYSTEM");
  Serial.println("connected");



  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}


void getTime () {
  DateTime now = rtc.now();

  currentYear = now.year();
  currentMonth = now.month();
  currentDay = now.day();
  currentHour = now.hour();
  currentMinutes = now.minute();
  date = daysOfTheWeek[now.dayOfTheWeek()];

  Serial.print(currentYear);
  Serial.print('/');
  Serial.print(currentMonth);
  Serial.print('/');
  Serial.print(currentDay);
  Serial.print(" (");
  Serial.print(date);
  Serial.print(") ");
  Serial.print(currentHour);
  Serial.print(':');
  Serial.println(currentMinutes);


}
