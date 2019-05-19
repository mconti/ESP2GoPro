/*
 * Autoscatto per GoPro Hero3+
 * Versione per IDE Arduino su ESP8266
 * 
 * posta@chiaraconti.com
 * posta@maurizioconti.com
 * 25 Aprile 2019
 * 
 * Idee e spunti da
 * 
 * https://github.com/KonradIT/goprowifihack/blob/master/HERO3/WifiCommands.md
 * https://medium.com/@christopherwong/control-your-gopro-hero-camera-anywhere-with-http-protocol-153a12a6529b
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dove visualizziamo i messaggi
Adafruit_SSD1306 display(0);

const char* ssid = "GoGoMalliPro"; 
const char* password = "GoGoMalliPro"; 

const char* host = "10.5.5.9";
#define SWITCH 12
#define LED 14

int INTERVALLO = 15*60*1000;
unsigned long previousMillis=0;
int oldQuantoManca;

void setup() {
  Serial.begin(115200);
  pinMode(SWITCH, INPUT);
  pinMode(LED, OUTPUT);

  // oled init
  display.begin( SSD1306_SWITCHCAPVCC, 0x3C );
  display.clearDisplay();
  
  /*
  // Disegna un rettangolo
  display.drawRect(0, 1, display.width(), display.height()-1, WHITE);
  display.display();

  display.setTextColor( WHITE );
  display.setCursor( 3, 1 );
  display.setTextSize( 1 );
  display.println("\nsearching goPro...");
  display.display();

  Serial.print("searching goPro...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  display.clearDisplay();
  display.setCursor( 1, 1 );
  display.setTextSize( 2 );
  display.setTextColor( WHITE );
  display.println("\nConnected");
  display.display();
  Serial.println("");
  Serial.print("GoPro connessa usando l'indirizzo");
  Serial.println(WiFi.localIP());

  */
  // Inizializza display
  display.clearDisplay();
  display.setCursor( 2, 2 );
  display.setTextSize( 2 );
  display.setTextColor( WHITE );
  display.drawRect(0, 1, display.width(), display.height()-1, WHITE);
  display.display();
}

void loop() {
  

  if ( WiFi.status() == WL_CONNECTED ) {
    if( digitalRead( SWITCH ) == HIGH ) {
      delay( 10 );
      if( digitalRead( SWITCH ) == HIGH ) {

        // Scatta una foto
        shoot();
        
        // Attendi rilascio del pulsante
        while( digitalRead( SWITCH ) == HIGH ) { delay(10); };
      }
    }
    
    // Dopo INTERVALLO scatta foto
    unsigned long currentMillis = millis();
    unsigned long currentTime = (unsigned long)(currentMillis - previousMillis);
    if ( currentTime >= INTERVALLO ) {

      // Scatta una foto
      shoot();
      previousMillis = currentMillis;
    }
    else {
      unsigned long quantoManca = (INTERVALLO - currentTime)/1000;
      if (oldQuantoManca != quantoManca ) {
        
        oldQuantoManca = quantoManca;
        
        // Inizializza display
        display.clearDisplay();
        display.setCursor( 2, 2 );
        display.setTextSize( 2 );
        display.setTextColor( WHITE );
        display.drawRect(0, 1, display.width(), display.height()-1, WHITE);
        display.display();
        display.println( String(quantoManca) );
        display.display();
      }
    }
    
  }
  else {
    display.println("\ncerco goPro...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      display.print(".");
      display.display();
    }
  }
}
  
void shoot() {
  WiFiClient client;
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("Connessione alla goPro fallita");
    return;
  }

  String urlPowerOn( "GET /bacpac/PW?t=GoGoMalliPro&p=%01 HTTP/1.1\r\nHost: 10.5.5.9\r\nConnection: close\r\n\r\n" );
  String urlPowerOff( "GET /bacpac/PW?t=GoGoMalliPro&p=%00 HTTP/1.1\r\nHost: 10.5.5.9\r\nConnection: close\r\n\r\n" );
  String urlShoot( "GET /bacpac/SH?t=GoGoMalliPro&p=%01 HTTP/1.1\r\nHost: 10.5.5.9\r\nConnection: close\r\n\r\n" );
  //Serial.println(urlOn);

  display.clearDisplay();
  display.setCursor( 1, 1 );
  display.setTextSize( 2 );
  display.setTextColor( WHITE );
  display.println( "GoPro ON" );
  display.display();

  client.print( urlPowerOn );
  delay(3000);

  digitalWrite( LED, HIGH );
  client.print( urlShoot );

  display.println( "Shot!" );
  display.display();

  delay(2000);
  digitalWrite( LED, LOW );

  display.println( "GoPro OFF" );
  display.display();

  client.print( urlPowerOff );
  delay(500);
  client.print( urlPowerOff );
  delay(500);
}

