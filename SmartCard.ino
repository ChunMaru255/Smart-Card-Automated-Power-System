#include <SPI.h>
#include <MFRC522.h>

// Definisi Pin
#define SS_PIN 10
#define RST_PIN 9
#define PIN_RELAY 2    // Sambung ke terminal IN/Signal relay
#define PIN_BUZZER 4   // Sambung ke terminal I/O buzzer

MFRC522 rfid(SS_PIN, RST_PIN);

// ID Kad anda yang telah dikesan
String idKadSah = "96 18 A8 20"; 

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // Keadaan awal: Relay OFF (HIGH), Buzzer OFF (LOW)
  digitalWrite(PIN_RELAY, HIGH); 
  digitalWrite(PIN_BUZZER, LOW);  
  
  Serial.println("--- SISTEM KESELAMATAN AKTIF ---");
  Serial.println("Sila imbas kad anda...");
}

void loop() {
  // Semak jika ada kad baru
  if ( ! rfid.PICC_IsNewCardPresent()) return;
  if ( ! rfid.PICC_ReadCardSerial()) return;

  // Membaca UID kad
  String uidDikesan = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidDikesan.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uidDikesan.concat(String(rfid.uid.uidByte[i], HEX));
  }
  uidDikesan.toUpperCase();
  String idFinal = uidDikesan.substring(1); // Buang ruang kosong pertama

  Serial.print("Kad Diimbas: ");
  Serial.println(idFinal);

  // Semak ID Kad
  if (idFinal == idKadSah) {
    Serial.println("AKSES DITERIMA! Menghidupkan LED & Motor...");
    
    digitalWrite(PIN_RELAY, LOW);  // Hidupkan litar (Relay ON)
    digitalWrite(PIN_BUZZER, LOW); 
    
    delay(5000); // Lampu & Kipas hidup selama 5 saat
    
    digitalWrite(PIN_RELAY, HIGH); // Matikan litar (Relay OFF)
    Serial.println("Sistem dikunci semula.");
  } 
  else {
    Serial.println("AKSES DITOLAK! Kad Salah.");
    
    // Bunyi bip amaran 3 kali
    for(int i=0; i<3; i++) {
      digitalWrite(PIN_BUZZER, HIGH);
      delay(150);
      digitalWrite(PIN_BUZZER, LOW);
      delay(150);
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
