/****************************************************************************************************************************
  SecuredTwoWay-ESP8266-Client.ino
  For ESP8266.
  
  Based on and modified from Gil Maimon's ArduinoWebsockets library https://github.com/gilmaimon/ArduinoWebsockets
  to support STM32F/L/H/G/WB/MP1, nRF52 and SAMD21/SAMD51 boards besides ESP8266 and ESP32


  The library provides simple and easy interface for websockets (Client and Server).
  
  Built by Khoi Hoang https://github.com/khoih-prog/Websockets2_Generic
  Licensed under MIT license
  Version: 1.0.7

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      14/07/2020 Initial coding/porting to support nRF52 and SAMD21/SAMD51 boards. Add SINRIC/Alexa support
  1.0.1   K Hoang      16/07/2020 Add support to Ethernet W5x00 to nRF52, SAMD21/SAMD51 and SAM DUE boards
  1.0.2   K Hoang      18/07/2020 Add support to Ethernet ENC28J60 to nRF52, SAMD21/SAMD51 and SAM DUE boards
  1.0.3   K Hoang      18/07/2020 Add support to STM32F boards using Ethernet W5x00, ENC28J60 and LAN8742A 
  1.0.4   K Hoang      27/07/2020 Add support to STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 using 
                                  Ethernet W5x00, ENC28J60, LAN8742A and WiFiNINA. Add examples and Packages' Patches.
  1.0.5   K Hoang      29/07/2020 Sync with ArduinoWebsockets v0.4.18 to fix ESP8266 SSL bug.
  1.0.6   K Hoang      06/08/2020 Add non-blocking WebSocketsServer feature and non-blocking examples.       
  1.0.7   K Hoang      03/10/2020 Add support to Ethernet ENC28J60 using EthernetENC and UIPEthernet v2.0.9
 *****************************************************************************************************************************/
/*
  Secured Esp8266 Websockets Client

  This sketch:
        1. Connects to a WiFi network
        2. Connects to a Websockets server (using WSS)
        3. Sends the websockets server a message ("Hello Server from ......")
        4. Sends the websocket server a "ping"
        5. Prints all incoming messages while the connection is open

    NOTE:
    The sketch dosen't check or indicate about errors while connecting to
    WiFi or to the websockets server. For full example you might want
    to try the example named "Esp8266-Client" (And use the ssl methods).

  Hardware:
        For this sketch you only need an ESP8266 board.

  Originally Created  : 15/02/2019
  Original Author     : By Gil Maimon
  Original Repository : https://github.com/gilmaimon/ArduinoWebsockets

*****************************************************************************************************************************/

#include "defines.h"

#include <WebSockets2_Generic.h>
#include <ESP8266WiFi.h>

using namespace websockets2_generic;


// Need to update certs. Otherwise, can't connect to server using invalid certs.
// The hardcoded certificate authority for this example.
// Don't use it on your own apps!!!!!
const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC1TCCAb2gAwIBAgIJAMPt1Ms37+hLMA0GCSqGSIb3DQEBCwUAMCExCzAJBgNV
BAYTAlVTMRIwEAYDVQQDDAkxMjcuMC4wLjMwHhcNMTgwMzE0MDQyMTU0WhcNMjkw
NTMxMDQyMTU0WjAhMQswCQYDVQQGEwJVUzESMBAGA1UEAwwJMTI3LjAuMC4zMIIB
IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxsa4qU/tlzN4YTcnn/I/ffsi
jOPc8QRcwClKzasIZNFEye4uThl+LGZWFIFb8X8Dc+xmmBaWlPJbqtphgFKStpar
DdduHSW1ud6Y1FVKxljo3UwCMrYm76Q/jNzXJvGs6Z1MDNsVZzGJaoqit2H2Hkvk
y+7kk3YbEDlcyVsLOw0zCKL4cd2DSNDyhIZxWo2a8Qn5IdjWAYtsTnW6MvLk/ya4
abNeRfSZwi+r37rqi9CIs++NpL5ynqkKKEMrbeLactWgHbWrZeaMyLpuUEL2GF+w
MRaAwaj7ERwT5gFJRqYwj6bbfIdx5PC7h7ucbyp272MbrDa6WNBCMwQO222t4wID
AQABoxAwDjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCmXfrC42nW
IpL3JDkB8YlB2QUvD9JdMp98xxo33+xE69Gov0e6984F1Gluao0p6sS7KF+q3YLS
4hjnzuGzF9GJMimIB7NMQ20yXKfKpmKJ7YugMaKTDWDhHn5679mKVbLSQxHCUMEe
tEnMT93/UaDbWBjV6zu876q5vjPMYgDHODqO295ySaA71UkijaCn6UwKUT49286T
V9ZtzgabNGHXfklHgUPWoShyze+G3g29I1BR0qABoJI63zaNu8ua42v5g1RldxsW
X8yKI14mFOGxuvcygG8L2xxysW7Zq+9g+O7gW0Pm6RDYnUQmIwY83h1KFCtYCJdS
2PgozwkkUNyP
-----END CERTIFICATE-----
)EOF";

// The client's private key which must be kept secret
const char client_private_key[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAsRNVTvqP++YUh8NrbXwE83xVsDqcB3F76xcXNKFDERfVd2P/
LvyDovCcoQtT0UCRgPcxRp894EuPH/Ru6Z2Lu85sV//i7ce27tc2WRFSfuhlRxHP
LJWHxTl1CEfXp/owkECQ4MB3pw6Ekc16iTEPiezTG+T+mQ/BkiIwcIK6CMlpR9DI
eYUTqv0f9NrUfAjdBrqlEO2gpgFvLFrkDEU2ntAIc4aPOP7yDOym/xzfy6TiG8Wo
7nlh6M97xTZGfbEPCH9rZDjo5istym1HzF5P+COq+OTSPscjFGXoi978o6hZwa7i
zxorg4h5a5lGnshRu2Gl+Ybfa14OwnIrv/yCswIDAQABAoIBAHxwgbsHCriTcEoY
Yx6F0VTrQ6ydA5mXfuYvS/eIfIE+pp1IgMScYEXZobjrJPQg1CA1l0NyFSHS97oV
JPy34sMQxcLx6KABgeVHCMJ/EeJtnv7a3SUP0GIhhsVS95Lsl8RIG4hWub+EzFVK
eZqAB9N9wr4Pp3wZPodbz37B38rb1QPyMFmQOLlHjKTOmoxsXhL2ot+R3+aLYSur
oPO1kQo7/d0UAZoy8h9OQN4a2EXvawh4O2EvFGbc5X/yXwAdEQ4NPp9VZhkNIRkV
+XZ3FcIqEVOploKtRF/tVBTz3g61/lFz21L9PMmV5y8tvSafr2SpJugGVmp2rrVQ
VNyGlIECgYEA10JSI5gmeCU3zK6kvOfBp54hY/5dDrSUpjKkMxpmm7WZQ6Il/k7A
hMcLeMzHiriT7WhRIXF8AOr2MoEkHkH3DhVNN4ccieVZx2SE5P5mVkItZGLrrpfU
dysR/ARAI1HYegGUiKacZtf9SrRavU0m7fOVOiYwbFRhjyX+MyuteYkCgYEA0pbz
4ZosetScP68uZx1sGlTfkcqLl7i15DHk3gnj6jKlfhvC2MjeLMhNDtKeUAuY7rLQ
guZ0CCghWAv0Glh5eYdfIiPhgqFfX4P5F3Om4zQHVPYj8xHfHG4ZP7dKQTndrO1Q
fLdGDTQLVXabAUSp2YGrijC8J9idSW1pYClvF1sCgYEAjkDn41nzYkbGP1/Swnwu
AEWCL4Czoro32jVxScxSrugt5wJLNWp508VukWBTJhugtq3Pn9hNaJXeKbYqVkyl
pgrxwpZph7+nuxt0r5hnrO2C7eppcjIoWLB/7BorAKxf8REGReBFT7nBTBMwPBW2
el4U6h6+tXh2GJG1Eb/1nnECgYAydVb0THOx7rWNkNUGggc/++why61M6kYy6j2T
cj05BW+f2tkCBoctpcTI83BZb53yO8g4RS2yMqNirGKN2XspwmTqEjzbhv0KLt4F
X4GyWOoU0nFksXiLIFpOaQWSwWG7KJWrfGJ9kWXR0Xxsfl5QLoDCuNCsn3t4d43T
K7phlwKBgHDzF+50+/Wez3YHCy2a/HgSbHCpLQjkknvgwkOh1z7YitYBUm72HP8Z
Ge6b4wEfNuBdlZll/y9BQQOZJLFvJTE5t51X9klrkGrOb+Ftwr7eI/H5xgcadI52
tPYglR5fjuRF/wnt3oX9JlQ2RtSbs+3naXH8JoherHaqNn8UpH0t
-----END RSA PRIVATE KEY-----
)EOF";

// The client's public certificate which must be shared
const char client_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDTzCCAjcCCQDPXvMRYOpeuDANBgkqhkiG9w0BAQsFADCBpjESMBAGA1UEAwwJ
MTI3LjAuMC4xMQswCQYDVQQGEwJVUzElMCMGA1UECgwcTXkgT3duIENlcnRpZmlj
YXRlIEF1dGhvcml0eTEUMBIGA1UECAwLQXJkdWlub0xhbmQxFTATBgNVBAcMDEFy
ZHVpbm9WaWxsZTEVMBMGA1UECgwMRVNQODI2NlVzZXJzMRgwFgYDVQQLDA9FU1A4
MjY2LUFyZHVpbm8wHhcNMTgwMzE0MDQwMDAwWhcNMjkwMjI0MDQwMDAwWjAsMRYw
FAYDVQQKDA1NeSBTZXJ2ZXIgT3JnMRIwEAYDVQQDDAkxMjcuMC4wLjMwggEiMA0G
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCxE1VO+o/75hSHw2ttfATzfFWwOpwH
cXvrFxc0oUMRF9V3Y/8u/IOi8JyhC1PRQJGA9zFGnz3gS48f9G7pnYu7zmxX/+Lt
x7bu1zZZEVJ+6GVHEc8slYfFOXUIR9en+jCQQJDgwHenDoSRzXqJMQ+J7NMb5P6Z
D8GSIjBwgroIyWlH0Mh5hROq/R/02tR8CN0GuqUQ7aCmAW8sWuQMRTae0Ahzho84
/vIM7Kb/HN/LpOIbxajueWHoz3vFNkZ9sQ8If2tkOOjmKy3KbUfMXk/4I6r45NI+
xyMUZeiL3vyjqFnBruLPGiuDiHlrmUaeyFG7YaX5ht9rXg7Cciu//IKzAgMBAAEw
DQYJKoZIhvcNAQELBQADggEBAEnG+FNyNCOkBvzHiUpHHpScxZqM2f+XDcewJgeS
L6HkYEDIZZDNnd5gduSvkHpdJtWgsvJ7dJZL40w7Ba5sxpZHPIgKJGl9hzMkG+aA
z5GMkjys9h2xpQZx9KL3q7G6A+C0bll7ODZlwBtY07CFMykT4Mp2oMRrQKRucMSV
AB1mKujLAnMRKJ3NM89RQJH4GYiRps9y/HvM5lh7EIK/J0/nEZeJxY5hJngskPKb
oPPdmkR97kaQnll4KNsC3owVlHVU2fMftgYkgQLzyeWgzcNa39AF3B6JlcOzNyQY
seoK24dHmt6tWmn/sbxX7Aa6TL/4mVlFoOgcaTJyVaY/BrY=
-----END CERTIFICATE-----
)EOF";

void onMessageCallback(WebsocketsMessage message) 
{
  Serial.print("Got Message: ");
  Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) 
{
  if (event == WebsocketsEvent::ConnectionOpened) 
  {
    Serial.println("Connnection Opened");
  } 
  else if (event == WebsocketsEvent::ConnectionClosed) 
  {
    Serial.println("Connnection Closed");
  } 
  else if (event == WebsocketsEvent::GotPing) 
  {
    Serial.println("Got a Ping!");
  } 
  else if (event == WebsocketsEvent::GotPong) 
  {
    Serial.println("Got a Pong!");
  }
}

WebsocketsClient client;

void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStarting WebSockets SecuredTwoWay-ESP8266-Client on " + String(BOARD_NAME));
  
  // Connect to wifi
  WiFi.begin(ssid, password);
  
  // Wait some time to connect to wifi
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) 
  {
    Serial.print(".");
    delay(1000);
  }
  
  // Check if connected to wifi
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("No Wifi!");
    return;
  }
  
  Serial.print("Connected to Wifi, Connecting to WebSockets Server @");
  Serial.println(websockets_connection_string);
  
  // run callback when messages are received
  client.onMessage(onMessageCallback);
  
  // run callback when events are occuring
  client.onEvent(onEventsCallback);
  
  // Before connecting, set the ssl certificates and key of the server
  X509List cert(ca_cert);
  client.setTrustAnchors(&cert);
  
  X509List *serverCertList = new X509List(client_cert);
  PrivateKey *serverPrivKey = new PrivateKey(client_private_key);
  client.setClientRSACert(serverCertList, serverPrivKey);
  
  // Connect to server
  bool connected = client.connect(websockets_connection_string);

  Serial.println("Called Connect to server.");
  
  if (connected) 
  {
    Serial.println("Connected!");

    String WS_msg = String("Hello to Server from ") + BOARD_NAME;
    client.send(WS_msg);

    // Send a ping
    client.ping();
  } 
  else 
  {
    Serial.println("Not Connected!");
  }
}

void loop() 
{
    client.poll();
}
