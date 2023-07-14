#include <TinyGPS++.h>
#include <SoftwareSerial.h>
SoftwareSerial GPRS(8, 7);
SoftwareSerial neo(3, 2);
String textMessage;
String lampState;
int nyala = 5;
String lati = "";
String longi = "";
int led = 13;
TinyGPSPlus gps;
void setup()
{
    pinMode(nyala, OUTPUT);
    delay(10000);
    digitalWrite(nyala, HIGH);
    delay(2000);
    digitalWrite(nyala, LOW);
    pinMode(led, OUTPUT);
    Serial.begin(9600);
    GPRS.begin(9600);
    neo.begin(9600);
    GPRS.listen();
    delay(5000);
    digitalWrite(led, HIGH);
    Serial.print("GPRS ready...\r\n");
    GPRS.print("AT+CMGF=1\r\n");
    delay(1000);
    GPRS.print("AT+CNMI=2,2,0,0,0\r\n");
    delay(1000);
    digitalWrite(led, LOW);
}

void loop()
{
    GPRS.listen();
    delay(2);
    while (GPRS.available() > 0)
    {
        digitalWrite(led, HIGH);
        textMessage = GPRS.readString();
        Serial.print(textMessage);
        delay(10);
        digitalWrite(led, LOW);
    }
    neo.listen();
    if (textMessage.indexOf("LOKASI") >= 0)
    {
        smartDelay(1000);
        Serial.println("Permintaan data lokasi\r\n");
        textMessage = "";
        GPRS.println("AT+CMGS=\"08xxx\"");
        delay(500);
        String pesan = "https://maps.google.com/?q=" + lati + "," + longi;
        GPRS.print(pesan);
        GPRS.write(0x1a);
        delay(1000);
        GPRS.println("AT+CMGD=1,4");
    }
}
static void smartDelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        neo.listen();
        delay(2);
        while (neo.available())
            gps.encode(neo.read());
    } while (millis() - start < ms);
    lati = String(gps.location.lat(), 8);
    longi = String(gps.location.lng(), 6);
    Serial.println(lati);
    Serial.println(longi);
}
