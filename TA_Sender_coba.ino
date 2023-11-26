#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <GP2Y1010AU0F.h>


int measurePin = 34;
int ledPin     = 5;  

GP2Y1010AU0F dustSensor(ledPin, measurePin);

BH1750 lightMeter;
#define DHT_PIN 4
const char* ssid = "NurShafira";
const char* password = "1721083132";
DHT dht(DHT_PIN,DHT11);
// REPLACE WITH THE RECEIVER'S MAC Address C8:F0:9E:30:3F:BC

uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0x30, 0x3F, 0xBC};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    float x;
    float y;
    float z;
    float a;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(DHT_PIN, INPUT_PULLUP);
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  
  dustSensor.begin();
  pinMode(ledPin,OUTPUT);
  WiFi.begin(ssid, password);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
 
  // Set values to send
  myData.id = 1;
  
  myData.x = dht.readTemperature();
  myData.y = dht.readHumidity();
  myData.z = lightMeter.readLightLevel();
  float dustDensity = dustSensor.read();
  
  if(dustDensity < 0){
    dustDensity = 0; 
  }
  myData.a = dustDensity;

  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.print("Temperature: ");
    Serial.print(myData.x);
    Serial.print("Humidity: ");
    Serial.print(myData.y);
    Serial.print("Intensitas Cahaya: ");
    Serial.print(myData.z);
    Serial.print("Dust Density = ");
    Serial.print(myData.a); 
  }
  else {
    Serial.println("Error sending the data");
  }
}
