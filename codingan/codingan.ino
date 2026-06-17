#include <EEPROM.h>  // Include EEPROM library

#define BLYNK_TEMPLATE_ID "TMPL6f6Mb2upM"
#define BLYNK_TEMPLATE_NAME "smart door lock"
#define BLYNK_AUTH_TOKEN "ViJgYWgWu78aF8dNuoWXB-RS2os6s7Rw"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WebServer.h>

// Konfigurasi WiFi
const char* ssid = "Xtroy06";         
const char* password = "19531953"; 

// Pin untuk RC522
#define RST_PIN D2   
#define SS_PIN D3    
#define RELAY_PIN D0 

MFRC522 rfid(SS_PIN, RST_PIN); 
ESP8266WebServer server(80);    // Web server berjalan di port 80

// Variabel untuk status tombol Blynk
bool disableCard1 = false;
bool disableCard2 = false;
bool disableCard3 = false;
bool disableCard4 = false;

// Fungsi untuk membaca status tombol Blynk
BLYNK_WRITE(V1) { 
  disableCard1 = param.asInt(); 
  EEPROM.write(0, disableCard1);
  EEPROM.commit();
}
BLYNK_WRITE(V2) { 
  disableCard2 = param.asInt(); 
  EEPROM.write(1, disableCard2);
  EEPROM.commit();
}
BLYNK_WRITE(V3) { 
  disableCard3 = param.asInt(); 
  EEPROM.write(2, disableCard3);
  EEPROM.commit();
}
BLYNK_WRITE(V4) { 
  disableCard4 = param.asInt(); 
  EEPROM.write(3, disableCard4);
  EEPROM.commit();
}

// Fungsi untuk kontrol relay melalui Blynk
BLYNK_WRITE(V5) {
  int relayblynk = param.asInt();
  digitalWrite(RELAY_PIN, relayblynk);
  
  if (relayblynk == LOW) {
    Serial.println("Solenoid Terbuka (Blynk)");
  } else {
    Serial.println("Solenoid Tertutup (Blynk)");
  }

  rfid.PCD_Init();  // Reset RFID setelah pakai Blynk
}

// Routing Web Server
void handleControl() {
  Serial.println("Received request to /control");
  if (server.hasArg("action")) {
    String action = server.arg("action");
    Serial.println("Action: " + action);
    if (action == "open") {
      digitalWrite(RELAY_PIN, LOW);
      server.send(200, "text/plain", "Solenoid Opened (Web)");
    } else if (action == "close") {
      digitalWrite(RELAY_PIN, HIGH);
      server.send(200, "text/plain", "Solenoid Closed (Web)");
    } else {
      server.send(400, "text/plain", "Invalid Action");
    }
  } else {
    server.send(400, "text/plain", "No Action Provided");
  }
}


void setup() {
  Serial.begin(19200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Pintu dalam keadaan tertutup pada awalnya

  EEPROM.begin(512);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Scan kartu RFID!");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Wi-Fi connected!");
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  disableCard1 = EEPROM.read(0);
  disableCard2 = EEPROM.read(1);
  disableCard3 = EEPROM.read(2);
  disableCard4 = EEPROM.read(3);
  EEPROM.commit();  // Pastikan nilai EEPROM tersimpan dengan benar

  server.on("/control", HTTP_GET, handleControl); // Routing ke /control
  server.begin();
}

void loop() {
  Blynk.run();           // Menjalankan Blynk
  server.handleClient(); // Menangani permintaan client

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  Serial.println(uid);

  if (uid == "f31e4728" && !disableCard1) {
    Serial.println("Kartu BAPAK diterima!");
    openDoor();
  } else if (uid == "86f5e2ad" && !disableCard2) {
    Serial.println("Kartu MAMAK diterima!");
    openDoor();
  } else if (uid == "a338b315" && !disableCard3) {
    Serial.println("Kartu ADEK diterima!");
    openDoor();
  } else if (uid == "d3c4a915" && !disableCard4) {
    Serial.println("Kartu NUR diterima!");
    openDoor();
  } else if (uid == "f31e4728" && disableCard1) {
    Serial.println("Kartu 1 tidak bisa dipakai!");
  } else if (uid == "86f5e2ad" && disableCard2) {
    Serial.println("Kartu 2 tidak bisa dipakai!");
  } else if (uid == "a338b315" && disableCard3) {
    Serial.println("Kartu 3 tidak bisa dipakai!");
  } else if (uid == "d3c4a915" && disableCard4) {
    Serial.println("Kartu 4 tidak bisa dipakai!");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  rfid.PCD_Init();  // Reset RFID agar bisa membaca ulang kartu
}

// Fungsi untuk membuka kunci
void openDoor() {
  digitalWrite(RELAY_PIN, LOW);
  delay(5000);  // Pintu terbuka selama 5 detik
  digitalWrite(RELAY_PIN, HIGH);
  delay(5000);  // Pintu tertutup selama 5 detik
}
