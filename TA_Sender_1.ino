#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_INA219.h>  //memanggil library dari INA219

Adafruit_INA219 ina219; //memberikan nama pada modul sensor dengan nama "ina219"

//membuat variabel yang akan dipakai untuk perhitungan, tipe datanya adalah bilangan desimal
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;

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
  Wire.begin();
  uint32_t currentFrequency; //mengatur current frequency
  if (! ina219.begin()) { //inisiasi untuk memulai modul sensor ina219
    Serial.println("Failed to find INA219 chip"); //jika gagal akan ada tulisan tsb
    while (1) { delay(10); } //jika gagal, program tidak akan lanjut
  }
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
  
  myData.id = 2;
  shuntvoltage = ina219.getShuntVoltage_mV(); 
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  //menampilkan hasil pengukuran
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
  
  myData.x = current_mA;
  myData.y = loadvoltage;
  myData.z = power_mW;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.print("Arus Listrik: ");
    Serial.print(myData.x);
    Serial.println("Tegangan Listrik: ");
    Serial.print(myData.y);
    Serial.println("Daya Listrik: ");
    Serial.print(myData.z);
  }
  else {
    Serial.println("Error sending the data");
  } 

}
