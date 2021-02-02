#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "libraries/DHT.h"
#include "libraries/ThingSpeak.h"

#define DHTTYPE DHT11
#define DHTPIN  5

const char* API_KEY = "YourKey"; // Put your ThingSpeak API KEY here
const char* SSID = "YourSSID"; // Put your WiFi SSID here
const char* PASSWORD = "YourRouterPassword"; // Put your WiFi password here

const char* SERVER_API = "api.thingspeak.com";
const unsigned long PORT = 80;

unsigned long channelNumber = 1000001; // Put your channel number here

// Initialize DHT sensor
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {

        Serial.begin(115200);
        dht.begin();
        WiFi.begin(SSID, PASSWORD);

        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }

        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        ThingSpeak.begin(client);
}

void loop() {
        delay(2000);

        // Reading temperature or humidity takes about 250 milliseconds!
        float humidity = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float temp_celsius = dht.readTemperature();
        // Read temperature as Fahrenheit (isFahrenheit = true)
        float temp_fahrenheit = dht.readTemperature(true);
        // Compute heat index in Fahrenheit (the default)
        float hif = dht.computeHeatIndex(temp_fahrenheit, humidity);
        // Compute heat index in Celsius (isFahreheit = false)
        float hic = dht.computeHeatIndex(temp_celsius, humidity, false);

        // Check if any reads failed and exit early (to try again).
        if (isnan(humidity) || isnan(temp_celsius) || isnan(temp_fahrenheit)) {
                Serial.println("Failed to read from DHT sensor!");
                return;
        }

        if (client.connect(SERVER_API, PORT)) {
           ThingSpeak.setField(1, temp_fahrenheit);
           ThingSpeak.setField(2, temp_celsius);
           ThingSpeak.setField(3, humidity);
           ThingSpeak.setField(4, hif);
           ThingSpeak.setField(5, hic);

           int x = ThingSpeak.writeFields(channelNumber, API_KEY);
           if(x == 200){
            Serial.println("Channel update successful.");
           } else {
            Serial.println("Problem updating channel. HTTP error code " + String(x));
           }
        }

        client.stop();
}
