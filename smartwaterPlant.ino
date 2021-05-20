
#include <Servo.h> // including servo library.
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
char auth[] = "43YrsYMEWJ7nEE3Ka4v5j5d2ivrep9YA"; //Auth Blynk
char ssid[] = "TerCyduck";                        //SSID WiFi
char pass[] = "bungaimut";                        //Pass WiFi
BlynkTimer timer;

float persen;
const int relay = D0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo; // Giving name to servo.
#define servopin D3

int soilMoistureValue = 0;
int pos = 0;

#define DHTPIN D4
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

void relayON()
{
    Serial.println("Nearly dry, Pump turning on");
    digitalWrite(relay, LOW); // Low percent high signal to relay to turn on pump
    lcd.setCursor(0, 1);
    lcd.print("menyiram           ");
    for (pos = 0; pos <= 180; pos += 1)
    { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        servo.write(pos); // tell servo to go to position in variable 'pos'
        delay(15);        // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1)
    {                     // goes from 180 degrees to 0 degrees
        servo.write(pos); // tell servo to go to position in variable 'pos'
        delay(15);        // waits 15ms for the servo to reach the position
    }
}

void relayOFF()
{
    Serial.println("Nearly wet, Pump turning off");
    digitalWrite(relay, HIGH); // high percent water high signal to relay to turn off pump
}

void analogSense()
{                                       //Sensor Kelembaban Tanah
    soilMoistureValue = analogRead(A0); //Mention where the analog pin is connected on NodeMCU
    soilMoistureValue = map(soilMoistureValue, 0, 1024, 0, 100);
    lcd.setCursor(0, 0);
    lcd.print("klmbbn tnh : ");
    lcd.print(soilMoistureValue);
    lcd.print("      ");
    Blynk.virtualWrite(V7, soilMoistureValue);

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        Serial.println(F("Error reading temperature!"));
        lcd.setCursor(0, 1);
        lcd.print("err T & H ");
    }
    else
    {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
        lcd.setCursor(0, 1);
        lcd.print(event.temperature);
        lcd.print("^C");
        Blynk.virtualWrite(V1, event.temperature);
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println(F("Error reading humidity!"));
    }
    else
    {
        Serial.print(F("Humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
        lcd.setCursor(8, 1);
        lcd.print(event.relative_humidity);
        lcd.print("%");
        Blynk.virtualWrite(V2, event.relative_humidity);
    }

    if (soilMoistureValue > 80) // change this at what level the pump turns on
    {
        relayON();
    }
    else // max water level should be
    {
        relayOFF();
    }
    delay(500);
    lcd.clear();
    delay(200);
}

void setup()
{
    pinMode(relay, OUTPUT); // pin where relay trigger connected
    servo.attach(servopin);
    Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
    lcd.begin();
    lcd.backlight();
    dht.begin();
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    dht.humidity().getSensor(&sensor);

    Blynk.begin(auth, ssid, pass);
    timer.setInterval(1000L, analogSense);
}

void loop()
{
    Blynk.run();
    timer.run();
}