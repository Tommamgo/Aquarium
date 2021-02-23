// Library für WiFi-Verbindung
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>



// Daten des WiFi-Netzwerks
const char* ssid     = "Telemannstr3";
const char* password = "SadFHAimIQ51s.";

//tine and date
//Zeitverschiebung UTC <-> MEZ (Winterzeit) = 3600 Sekunden (1 Stunde)
//Zeitverschiebung UTC <-> MEZ (Sommerzeit) = 7200 Sekunden (2 Stunden)
const long utcOffsetInSeconds = 3600;


char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);



// GPIO where the DS18B20 is connected to
const int oneWireBus = 5;   //D1
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);


// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


// LED Pin
const unsigned int gruen = 14; //D5
const unsigned int blau = 12; // D6
const unsigned int rot = 13; //  D7

//Heizplatte
const unsigned int platte = 4; // D2
const unsigned int platte_aus = 12; // S3

//pumpe 
const unsigned int pumpen_pin = 15; // D8

// Port des Web Servers auf 80 setzen
WiFiServer server(80);

// Variable für den HTTP Request
String header;

void setup() {
  Serial.begin(115200);

  // Mit dem WiFi-Netzwerk verbinden
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Lokale IP-Adresse im Seriellen Monitor ausgeben und Server starten
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  //LED Pinmode 
  pinMode(gruen, OUTPUT);
  pinMode(blau, OUTPUT);
  pinMode(rot, OUTPUT);

  //Heizplatte
  pinMode(platte, OUTPUT);

  //Pumpe
  pinMode(pumpen_pin, OUTPUT);

  //time and Date
  Serial.println("Ich bin mit dem Internet verbunden!");

  timeClient.begin();
  

  
}

void loop() {
  server_main();
  unsigned int wahlderFarbe = 0;

  //soll Farbe von Server bekommen
  LEDs(wahlderFarbe); // 0 = gruen, 1 = blau, rot = 2
  time_date(); //time and date funktion
  heizplatte(sensor1(), sensor2());

  //time_date()
  delay(10);

  
 
}

void server_main(){
   WiFiClient client = server.available();   // Auf Clients (Server-Aufrufe) warten

  if (client) {                             // Bei einem Aufruf des Servers
    Serial.println("Client available");
    String currentLine = "";                // String definieren für die Anfrage des Clients

    while (client.connected()) { // Loop, solange Client verbunden ist

      if (client.available()) {
        char c = client.read();             // Ein (1) Zeichen der Anfrage des Clients lesen
        Serial.write(c);                    // und es im Seriellen Monitor ausgeben
        header += c;
        if (c == '\n') {                    // bis eine Neue Zeile ausgegeben wird

          // Wenn der Client eine Leerzeile sendet, ist das Ende des HTTP Request erreicht
          if (currentLine.length() == 0) {

            // Der Server sendet nun eine Antwort an den Client
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
//
            
//            
//            const char index_html[] PROGMEM = R"rawliteral(
//            <!DOCTYPE HTML><html>
//            <head>
//              <title>ESP Web Server</title>
//              <meta name="viewport" content="width=device-width, initial-scale=1">
//              <style>
//                html {font-family: Arial; display: inline-block; text-align: center;}
//                h2 {font-size: 3.0rem;}
//                p {font-size: 3.0rem;}
//                body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
//                .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
//                .switch input {display: none}
//                .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
//                .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
//                input:checked+.slider {background-color: #2196F3}
//                input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
//              </style>
//            </head>
//            <body>
//              <h2>ESP Web Server</h2>
//              %BUTTONPLACEHOLDER%
//            <script>function toggleCheckbox(element) {
//              var xhr = new XMLHttpRequest();
//              if(element.checked){ xhr.open("GET", "/update?state=1", true); }
//              else { xhr.open("GET", "/update?state=0", true); }
//              xhr.send();
//            }
//            
//            setInterval(function ( ) {
//              var xhttp = new XMLHttpRequest();
//              xhttp.onreadystatechange = function() {
//                if (this.readyState == 4 && this.status == 200) {
//                  var inputChecked;
//                  var outputStateM;
//                  if( this.responseText == 1){ 
//                    inputChecked = true;
//                    outputStateM = "On";
//                  }
//                  else { 
//                    inputChecked = false;
//                    outputStateM = "Off";
//                  }
//                  document.getElementById("output").checked = inputChecked;
//                  document.getElementById("outputState").innerHTML = outputStateM;
//                }
//              };
//              xhttp.open("GET", "/state", true);
//              xhttp.send();
//            }, 1000 ) ;
//            </script>
//            </body>
//            </html>
//            )rawliteral";
//




            // Die Webseite anzeigen
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\"></head>");
            client.println("<body><h1 align=\"center\">Hier spricht dein Server! :)</h1>");
            client.println("");
            client.println("<script>function toggleCheckbox(element){");
            client.println("var xhr = new XHLHttpRequest():");
            client.println("if(element.checked){ xhr.open('GET', '/update?state=1', true): }");
            client.println("else{ xhr.open('GET','/update?state=0', true); }");
            client.println("xhr.send()");
            client.println("}");
            //client.println("</body></html>");
            client.println("</body></html>");
            // Die Antwort mit einer Leerzeile beenden
            client.println();
            // Den Loop beenden
            break;
          } else { // Bei einer Neuen Zeile, die Variable leeren
            currentLine = "";
          }
        } else if (c != '\r') {  // alles andere als eine Leerzeile wird
          currentLine += c;      // der Variable hinzugefüht
        }
      }
    }
    // Variable für den Header leeren
    header = "";
    // Die Verbindung beenden
    client.stop();
    Serial.println("Client disconnected");
    Serial.println("");
  }
  
}

void pumpe(int zustand){
  if (zustand){
    digitalWrite(pumpen_pin, HIGH);
  }else{
    digitalWrite(pumpen_pin, LOW);
  }
  
}

void heizplatte(float sensor1, float sensor2){
  if (sensor1 < 24 && sensor2 < 60){
    digitalWrite(platte_aus, LOW);
    digitalWrite(platte, HIGH);
    
  }else{
    digitalWrite(platte, LOW);
    digitalWrite(platte_aus, HIGH);
  }
  
}

void LEDs(int farbe){

  switch (farbe){
    case 0: 
      digitalWrite(gruen, HIGH);
      digitalWrite(blau, LOW);
      digitalWrite(rot, LOW);
      break;
    case 1:
      digitalWrite(blau, HIGH);
      digitalWrite(rot, LOW);
      digitalWrite(gruen, LOW);
      break;
    case 2: 
      digitalWrite(rot, HIGH);
      digitalWrite(gruen, LOW);
      digitalWrite(blau, LOW);
       break;
    default:
      break;
  }
  
  
  
  
}

float sensor1(){
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  
  Serial.print(temperatureC);
  Serial.println("ºC");


  return temperatureC;
}
  
//Hier muss noch die Berechung hin
float sensor2(){
  float temperatur = 0.0;
  
  return temperatur;
}

void time_date(){
  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.println(timeClient.getFormattedTime());
  
}


//Funktion muss noch gemacht werden 
//hier muss der HTMP-Code rein
String http_string(){
  return "Hallo"; 
}
