#include <PubSubClient.h>
#include <WiFi.h>
#include <ctime>

#define ONE_SECOND (1000)
#define ONE_MINUTE (ONE_SECOND * 60)
#define ONE_HOUR (ONE_MINUTE * 60)

const char* ssid = "";
const char* password = "";
const char* mqtt_server = ""; 
const char* mqtt_user = "";
const char* mqtt_pass= "";

const u_int16_t mqtt_port = 1883;
const u_int32_t one_day = 60 * 60 * 24;

const int SensorPin1 = 33;
const int SensorPin2 = 34;
const int SensorPin3 = 35;

const u_int16_t pump1 = 23;
const u_int16_t pump2 = 26;
const u_int16_t pump3 = 27;

u_int16_t soilMoistureValue1 = 0;
u_int16_t soilMoistureValue2 = 0;
u_int16_t soilMoistureValue3 = 0;

time_t lastWateredPlant1 = time(0);
time_t lastWateredPlant2 = time(0);
time_t lastWateredPlant3 = time(0);

time_t last_watered_daily = time(0);

u_int16_t thresholdPlant1 = 80;
u_int16_t thresholdPlant2 = 80;
u_int16_t thresholdPlant3 = 80;


WiFiClient espClient;
PubSubClient client(espClient);



void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;
  int extracted_number;
  uint16_t newThreshold;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  if (String(topic) == "esp.in") {
    if(messageTemp == "waterAllPlants"){
        pump_water(pump1);
        pump_water(pump2);
        pump_water(pump3);
      }
    else{
      Serial.println("Bad Input");
    }
  }
}


void setup_wifi() {
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
      client.subscribe("esp.in");
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
  char msg[32];
  pinMode(pump_number, OUTPUT);
  delay(ONE_SECOND * 5);  
  pinMode(pump_number, INPUT_PULLUP); 
  delay(ONE_SECOND);
  sprintf(msg, "watered:plant %d", pump_number);
  client.publish("esp.watering",  msg);
 }

void loop() {
  char msg1[32];
  char msg2[32];
  char msg3[32];

  if (!client.connected()) { reconnect(); }

  soilMoistureValue1 = analogRead(SensorPin1);  
  soilMoistureValue1 = map(soilMoistureValue1, 3600, 1346, 0, 100);
  Serial.println(soilMoistureValue1);
  
  if (soilMoistureValue1 < thresholdPlant1 && (difftime( time(0), lastWateredPlant1) > one_hour)){
    pump_water(pump1);
    lastWateredPlant1 = time(0);
  }

  soilMoistureValue2 = analogRead(SensorPin2);
  soilMoistureValue2 = map(soilMoistureValue2, 3550, 1346, 0, 100);
  Serial.println(soilMoistureValue2);

  if (soilMoistureValue2 < thresholdPlant2 && (difftime( time(0), lastWateredPlant2) > one_hour)){
    pump_water(pump2);
    lastWateredPlant2 = time(0);
  }

  soilMoistureValue3 = analogRead(SensorPin3);  
  soilMoistureValue3 = map(soilMoistureValue3, 2800, 1200, 0, 100);
  Serial.println(soilMoistureValue3);

  if (soilMoistureValue3 < thresholdPlant3 && (difftime( time(0), lastWateredPlant3) > one_hour)){
    pump_water(pump3);
    lastWateredPlant3 = time(0);
  }

  if (difftime(time(0), last_watered_daily) > one_day){
        pump_water(pump1);
        pump_water(pump2);
        pump_water(pump3);

        last_watered_daily = time(0);
  }
  
  sprintf(msg1, "plant1:%d", soilMoistureValue1);
  sprintf(msg2, "plant2:%d", soilMoistureValue2);
  sprintf(msg3, "plant3:%d", soilMoistureValue3);

  client.publish("esp.humidity",  msg1);
  client.publish("esp.humidity",  msg2);
  client.publish("esp.humidity",  msg3);

  delay(ONE_HOUR);
}
