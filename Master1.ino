#include <SPI.h>
#include <WiFi.h>
#include<String.h>
#include <WiFiUdp.h>
#define MASTER_ID 1
#define photoCellPin 0
#define sensor_1 2
#define sensor_2 4
#define sensor_3 7
char ssid[] = "the g2"; //  your network SSID (name)
char pass[] = "hello123";    // your network password (use for WPA, or use as key for WEP)
String updateQuery = String("GET /update_count/?");
String getQuery = String("GET /get_count/?");
int status = WL_IDLE_STATUS;

IPAddress servername(128, 199, 164, 21);
int localPort = 1729;

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

//Controller Configurables
int smartFeature = 1;
int alwaysOn = 0;
int minimum_1 = 20, minimum_2 = 20, minimum_3 = 20;

//constants
int THRESHOLD = 30;
int MAXLIGHT = 220;
// pins and variables
int ambientLight = 0;
int night = 0;
int count_m1 = 0, count_m2 = 0, count_m3 = 0;
int m1 = 3 , m2 = 6, m3 = 10, sm1 = 5 , sm2 = 9, sm3 = 11;

WiFiUDP Udp;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(sm1, OUTPUT);
  pinMode(sm2, OUTPUT);
  pinMode(sm3, OUTPUT);
  pinMode(photoCellPin, INPUT);
  pinMode(sensor_1, INPUT);
  pinMode(sensor_2, INPUT);
  pinMode(sensor_3, INPUT);
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while (true);
  }
  else {
    Serial.println("Starting UDP server on 1729");
    Udp.begin(localPort);

    Serial.println("Connected to wifi");
    Serial.println(WiFi.localIP());
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
  }
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer[0]);
    if (packetBuffer[0] == '8')
    {
      alwaysOn = packetBuffer[1] - 48;
    }
    if (packetBuffer[0] == '9')
    {
      smartFeature = packetBuffer[1] - 48;
    }
    else if (packetBuffer[0] == '1')
    {

      minimum_1 = getValue(packetBuffer);
      Serial.print("minimum_1 is :  ");
      Serial.println(minimum_1);
    }
    else if (packetBuffer[0] == '2')
    {
      minimum_2 = getValue(packetBuffer);
      Serial.print("minimum_2 is :  ");
      Serial.println(minimum_2);
    }
    else if (packetBuffer[0] == '3')
    {
      minimum_3 = getValue(packetBuffer);
      Serial.print("minimum_3 is :  ");
      Serial.println(minimum_3);
    }
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
  ambientLight = analogRead(photoCellPin);
  Serial.print("ambientLight Val is :  ");
  Serial.println(ambientLight);
  Serial.print("alwaysOn :  ");
  Serial.println(alwaysOn);
  Serial.print("SmartFeature :  ");
  Serial.println(smartFeature);
  if(ambientLight < THRESHOLD)
   { 
      Serial.println("Night!!!!!!");
     night = 1;
   }
    else
    {
      night=0;
      Serial.println("Day!!!!!!");
    }
      if(night ==0 && alwaysOn == 0)
  {
    Serial.print("Still Day = ");
    Serial.println(ambientLight);
//    analogWrite(m1, 0);
//    analogWrite(m2, LOW);
//    analogWrite(m3, LOW);
//    analogWrite(sm1, LOW);
//    analogWrite(sm2, LOW);
//    analogWrite(sm3, LOW);
    digitalWrite(m1, LOW);
    digitalWrite(m2, LOW);
    digitalWrite(m3, LOW);
    digitalWrite(sm1, LOW);
    digitalWrite(sm2, LOW);
    digitalWrite(sm3, LOW);
  }

    if (digitalRead(sensor_1))
    {
      count_m1++;
    }
//    Serial.println("Counters at Sensor 1");
//    Serial.println(count_m1);
//    Serial.println(count_m2);
//    Serial.println(count_m3);
    update(1, count_m1);
    if (count_m1 > 0 && night==1)
    {
      Serial.println("M1 High");
      analogWrite(m1, MAXLIGHT);
      analogWrite(sm1, MAXLIGHT);
    }
    else if(night==1)
    {
      Serial.println("M1 Min");
      analogWrite(m1, minimum_1);
      analogWrite(sm1, minimum_1);
    }
   // delay(1000);
    if (digitalRead(sensor_2) )
    {
      count_m1--;
      count_m2++;
    }
//    Serial.println("Counters at Sensor 2");
//    Serial.println(count_m1);
//    Serial.println(count_m2);
//    Serial.println(count_m3);
  update(2, count_m2);
    if (count_m2 > 0 && night==1)
    {
      Serial.println("M2 High");
      analogWrite(m2, MAXLIGHT);
      analogWrite(sm2, MAXLIGHT);
    }
    else if(night==1)
    {
      Serial.println("M2 Min");
      analogWrite(m2, minimum_2 );
      analogWrite(sm2, minimum_2);
    }
   // delay(1000);
//    if (digitalRead(sensor_3))
//    {
//      count_m1--;
//      count_m3++;
//    }
////    Serial.println("Counters at Sensor 3");
////    Serial.println(count_m1);
////    Serial.println(count_m2);
////    Serial.println(count_m3);
//    update(3, count_m3);
//    if (count_m3 > 0 && night==1)
//    {
//      Serial.println("M3 High");
//      analogWrite(m3, MAXLIGHT);
//      analogWrite(sm3, MAXLIGHT);
//    }
//    else if(night==1)
//    {
//      Serial.println("M3 Min");
////      analogWrite(m3, minimum_3);
////      analogWrite(sm3, minimum_3);
////digitalWrite(sm3, HIGH);
////digitalWrite(m3, HIGH);
//    }
  //  delay(1000);

  if (alwaysOn == 1 )
  {
    Serial.println("AlwaysOn");
    digitalWrite(m1, HIGH);
    digitalWrite(m2, HIGH);
    digitalWrite(m3, HIGH);
    digitalWrite(sm1, HIGH);
    digitalWrite(sm2, HIGH);
    digitalWrite(sm3, HIGH);
  }
  delay(500);

}


void update(int masterid, int count)
{
  WiFiClient client;
  String query = updateQuery;
  query.concat("id=");
  query.concat(masterid);
  query.concat("&");
  query.concat("count=");
  query.concat(count);
  Serial.println("Sending Request");
  Serial.println(query);
  if (client.connect(servername, 8000)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println(query);
    client.println();
  }

  Serial.println("DONE!!!!!!");
}

int getValue(char* p)
{
  int val = 0;
  val =  (p[1] - 48) * 100;
  val =  val + (p[2] - 48) * 10;
  val = val + (p[3] - 48);
  return val;
}


