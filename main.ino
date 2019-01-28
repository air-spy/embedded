#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

// Event loop errata
unsigned long duration;
unsigned long starttime;

// Input pin
int pin = 5;

// Dust sensor settings
float ratio = 0;
float concentration = 0;

// Sample frequency
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;

void setup() {
  // Wireless stuff:
  Serial.begin(115200);
  Serial.println();

  WiFi.begin("SSID", "PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.print("Connecting..");

  }
  pinMode(pin,INPUT);
  starttime = millis();

}
void loop() {
  // Variables for managing the sensor calculations
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  // Sensor reporting (through serial)
  if ((millis()-starttime) > sampletime_ms) {
    // Integer percentage 0=>100
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);

    // Using spec sheet curve
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;

    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      http.begin("http://192.168.87.22:8000/"
      +"sensor_graph/default/submit_reading.html?reading="+String(ratio)
      +"&SSK=PASSWORDGOESHEREBUTITSHOULDNT");
      int httpCode = http.GET();

      // Check the returned code
      if (httpCode > 0) {

        // Get the request response payload
        String payload = http.getString();

        // Print the response payload
        Serial.println(payload);

      }
      else {
        Serial.print("it didn't work");
      }

      http.end();
    }

    lowpulseoccupancy = 0;
    starttime = millis();
  }
}
