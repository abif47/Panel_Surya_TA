#include "ThingSpeak.h"
#include <esp_now.h>
#include <WiFi.h>

const char* ssid = "NurShafira";
const char* password = "1721083132";
WiFiClient client;
//Channel number dan APIKey Thingspeak
unsigned long channelNumber = 2272428;
// Service API Key
const char* apiKey = "072HI33ZFWQOQEWG";

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  float x;
  float y;
  float z;
  float a;
}struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;

// Create an array with all the structures
struct_message boardsStruct[3] = {board1, board2, board3};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].x = myData.x;
  boardsStruct[myData.id-1].y = myData.y;
  boardsStruct[myData.id-1].z = myData.z;
  boardsStruct[myData.id-1].a = myData.a;
  Serial.printf("x value: %d \n", boardsStruct[myData.id-1].x);
  Serial.printf("y value: %d \n", boardsStruct[myData.id-1].y);
  Serial.printf("z value: %d \n", boardsStruct[myData.id-1].z);
  Serial.printf("a value: %d \n", boardsStruct[myData.id-1].a);
  Serial.println();
}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi...");
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
}
 
void loop() {
  // Acess the variables for each board
  float board1X = boardsStruct[0].x;
  float board1Y = boardsStruct[0].y;
  float board1Z = boardsStruct[0].z;
  float board1A = boardsStruct[0].a;
  float board2X = boardsStruct[1].x;
  float board2Y = boardsStruct[1].y;
  float board2Z = boardsStruct[1].z;
  ThingSpeak.setField(1, board1X);
  ThingSpeak.setField(2, board1Y);
  ThingSpeak.setField(3, board1Z);
  ThingSpeak.setField(4, board1A);
  ThingSpeak.setField(5, board2X);
  ThingSpeak.setField(6, board2Y);
  ThingSpeak.setField(7, board2Z);
  int x = ThingSpeak.writeFields(channelNumber, apiKey);
  if(x == 200){
    Serial.println("You've been connected to ThingSpeak");
    Serial.println(board1X);
    Serial.println(board1Y);
    Serial.println(board1Z);
    Serial.println(board1A);
    Serial.println(board2X);
    Serial.println(board2Y);
    Serial.println(board2Z);
    } 
  if(x==500){
    Serial.println("server eror");
  }
  delay(60000);  
}
