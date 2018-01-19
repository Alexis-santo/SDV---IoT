/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network. 
const char* ssid = "PX-0.1";
const char* password = "557744Aa";
const char* mqtt_server = "192.168.43.56";

const char* lightChannel = "/ESP8266/sensors/light_sensor";
const char* moistureChannel = "/ESP8266/sensors/moisture_sensor";
const char* logChannel = "/ESP8266/log";
const char* testChannel = "/ESP8266/test";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMessage = 0;
char message[50];
int value = 0;
int dots = 0;


int moistureSensorPin = A0;  
int moistureSensorValue = 0;  
int moisturePercent = 0;

int lightSensorPin = D0;  
int lightSensorValue = 0;  

// the setup routine runs once when you press reset
void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// the loop routine runs over and over again forever
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (dots == 20)
    {
      Serial.print("\n");
      dots = 0;
    }
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Wemos D1 mini pro"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(logChannel, "Wemos D1 mini pro connected on server");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void printMoistureValuesToSerial()
{
  Serial.print("\n\nMoisture analog Value: ");
  Serial.print(moistureSensorValue);
  Serial.print("\nPercent: ");
  Serial.print(moisturePercent);
  Serial.print("%");
}

void printLightValuesToSerial()
{
  Serial.print("\n\nLight analog Value: ");
  Serial.print(lightSensorValue);
}

int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 465, 0, 100);
  return percentValue;
}

void lightSensorChecking()
{
  lightSensorValue = analogRead(lightSensorPin);
  char* valueToPublish = "light sensor : ";
  sprintf(valueToPublish, "%d", lightSensorValue);
  client.publish(lightChannel, valueToPublish);
  printLightValuesToSerial();
  delay(1000);
}

void moistureSensorChecking()
{
  moistureSensorValue = analogRead(moistureSensorPin);
  moisturePercent = convertToPercent(moistureSensorValue);
  char* valueToPublish = "moisture sensor : ";
  sprintf(valueToPublish, "%d", moisturePercent);
  valueToPublish = strcat(valueToPublish, "%");
  client.publish(moistureChannel, valueToPublish);
  printMoistureValuesToSerial();
  delay(1000);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  //lightSensorChecking();
  delay(3000);
  moistureSensorChecking();
}
