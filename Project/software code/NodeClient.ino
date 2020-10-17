#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // Define type of the DHT sensor DHT 11
#define dht_dpin 0

DHT dht(dht_dpin, DHTTYPE); 

// Update values suitable for the network.
const char* ssid = "Eng-Student bridge";
const char* password = "123456789";
const char* mqtt_server = "192.168.43.16";

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    // digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    // digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

WiFiClient espClient;
// PubSubClient client(espClient);
PubSubClient client(mqtt_server,1883,callback,espClient);
long lastMsg = 0;
char msgT[50];
char msgH[50];
int value = 0;

void setup() {
  // pinMode(BUILTIN_LED, OUTPUT);   // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);

  setup_wifi();

  dht.begin();
  Serial.begin(9600);
  Serial.println("Humidity and temperature\n\n");
  delay(700);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("not connected");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Temperature", "Temperature Node1");
      client.publish("Humidity", "Humidity Node1");
     
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


int sensorValue = 0;//initialize sensor value

void loop() {
  // Read the sensor
  sensorValue = random(5, 20); //= analogRead(sensorPin);

  if (!client.connected()) { //check whether client is not connected
    reconnect();//if client is not connected, reconnect the client

  }
  client.loop();
  delay(100);
  long now = millis();
  if (now - lastMsg > 2000) {


    Serial.print("Raw Sensor Value:");
    Serial.println(sensorValue);

    float h = dht.readHumidity(); //read temperature values from the sensor
    float t = dht.readTemperature(); //read humidity values from the sensor

    // Initialize char arrays for temperature & humidity
    char T[100];
    char H[100];
    
    sprintf(T, "%f", t);//convert float values of temperature to char array
    sprintf(H, "%f", h);//convert float values of humidity to char array
     
    lastMsg = now;
    ++value;
    snprintf (msgT, 75, T);//print current temperature values on the client monitor
    snprintf (msgH, 75, H);//print current humidity values on the client monitor

    Serial.print("Temperature = "); //print current temperature values on the serial monitor
    Serial.print(msgT);
    Serial.println("C  ");
    Serial.print("Humidity = ");//print current humidity values on the serial monitor
    Serial.print(msgH);
    Serial.println("%  ");
    client.publish("Temperature", msgT);
    client.publish("Humidity", msgH);
    
  }

  delay(800);
}
