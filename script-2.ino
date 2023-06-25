#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SIM800L.h>
#include <UniversalTelegramBot.h>

SoftwareSerial neo(3, 2);   // SoftwareSerial untuk modul GPS

String textMessage;
String lati = "";
String longi = "";

int nyala = 5;
int led = 13;

TinyGPSPlus gps;

const char* telegramToken = "secret";
const char* apn = "secret";   // APN dari penyedia layanan seluler

SIM800L gsm;
UniversalTelegramBot bot(telegramToken, gsm);

void setup() {
  pinMode(nyala, OUTPUT);
  digitalWrite(nyala, HIGH);
  delay(2000);
  digitalWrite(nyala, LOW);
  
  pinMode(led, OUTPUT);
  
  Serial.begin(9600);
  neo.begin(9600);
  
  Serial.println("GSM siap...");
  
  gsm.begin(9600);
  
  gsm.setAPN(apn);
  gsm.enableGPRS();
  
  delay(5000);   // Tunggu koneksi GSM stabil
  
  Serial.println("GPRS terhubung");
  
  gsm.initGPRS();
}

void loop() {
  while (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    if (command.indexOf("MSG") >= 0) {
      String chatId = command.substring(4, command.indexOf(";"));
      String message = command.substring(command.indexOf(";") + 1);
      sendMessage(chatId, message);
    }
  }
  
  if (gsm.available()) {
    textMessage = gsm.readString();
    Serial.print(textMessage);
  }
  
  neo.listen();
  
  if (textMessage.indexOf("LOKASI") >= 0) {
    sendLocation();
    textMessage = "";
  }
  
  bot.loop();
}

void sendLocation() {
  smartDelay(1000);
  
  Serial.println("Permintaan data lokasi");
  
  String location = "Latitude: " + lati + "\nLongitude: " + longi;
  sendMessage("ChatIDTelegram", location);
}

void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  
  do {
    while (neo.available()) {
      gps.encode(neo.read());
    }
  } while (millis() - start < ms);
  
  lati = String(gps.location.lat(), 8);
  longi = String(gps.location.lng(), 8);
  
  Serial.println("Latitude: " + lati);
  Serial.println("Longitude: " + longi);
}

void sendMessage(String chatId, String message) {
  gsm.sendTelegramMessage(chatId, message);
}

