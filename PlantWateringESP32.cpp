#include <PubSubClient.h>
#include <WiFi.h>
#include <ctime>


const char* ssid = "";
const char* password = "";
const char* mqtt_server = ""; 
const char* mqtt_user = "";
const char* mqtt_pass= "";
const u_int16_t mqtt_port = 1883;

const u_int16_t diffTime = 3600;
const u_int16_t sleepTimeInMinutes = 60;

const int SensorPin1 = 33;
const int SensorPin2 = 34;
const int SensorPin3 = 35;

const int pump1 = 23;
const int pump2 = 26;
const int pump3 = 27;

int soilMoistureValue1 = 0;
int soilMoistureValue2 = 0;
int soilMoistureValue3 = 0;


time_t lastWateredPlant1 = time(0);
time_t lastWateredPlant2 = time(0);
time_t lastWateredPlant3 = time(0);


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  Serial.println("In reconnect...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Arduino_Gas", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  pinMode(pump1, INPUT_PULLUP);
  pinMode(pump2, INPUT_PULLUP);
  pinMode(pump3, INPUT_PULLUP);
}

void pump_water(int pump_number){
  Serial.println("watering");
  pinMode(pump_number, OUTPUT);
  delay(5000);  
  pinMode(pump_number, INPUT_PULLUP); 
  delay(1000);
 }
void loop() {
  char msg1[32];
  char msg2[32];
  char msg3[32];

  if (!client.connected()) { reconnect(); }

  soilMoistureValue1 = analogRead(SensorPin1);  
  soilMoistureValue1 = map(soilMoistureValue1, 3600, 1346, 0, 100);
  Serial.println(soilMoistureValue1);
  
  if (soilMoistureValue1 < 50 && (difftime( time(0), lastWateredPlant1) > diffTime)){
    pump_water(pump1);
    lastWateredPlant1 = time(0);
  }

  soilMoistureValue2 = analogRead(SensorPin2);  //put Sensor insert into soil
  soilMoistureValue2 = map(soilMoistureValue2, 3550, 1346, 0, 100);
  Serial.println(soilMoistureValue2);

  if (soilMoistureValue2 < 50 && (difftime( time(0), lastWateredPlant2) > diffTime)){
    pump_water(pump2);
    lastWateredPlant2 = time(0);
  }

  soilMoistureValue3 = analogRead(SensorPin3);  //put Sensor insert into soil
  soilMoistureValue3 = map(soilMoistureValue3, 2800, 1200, 0, 100);
  Serial.println(soilMoistureValue3);

  if (soilMoistureValue3 < 50 && (difftime( time(0), lastWateredPlant3) > diffTime)){
    pump_water(pump3);
    lastWateredPlant3 = time(0);
  }
  
  sprintf(msg1, "plant1:%d", soilMoistureValue1);
  sprintf(msg2, "plant2:%d", soilMoistureValue2);
  sprintf(msg3, "plant3:%d", soilMoistureValue3);

  client.publish("amq.topic",  msg1);
  client.publish("amq.topic",  msg2);
  client.publish("amq.topic",  msg3);

  delay(1000  * 1 * sleepTimeInMinutes);
}