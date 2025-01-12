#include <SPI.h>
#include <MFRC522.h>

// Define RFID pins
#define RST_PIN 9  // Reset pin
#define SS_PIN 10  // Slave Select pin

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Authorized UIDs (example: "AB 12 CD 34")
String authorizedUIDs[] = {
  "B9 96 50 7B"
};
int numAuthorizedUIDs = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

void setup() {
  Serial.begin(9600); // Initialize serial communication
  SPI.begin();        // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522
  Serial.println("Place your RFID card near the scanner...");
}

void loop() {
  // Look for a card
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Read the card UID
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();

  // Print the UID to Serial Monitor
  Serial.print("Card UID: ");
  Serial.println(uid);

  // Check if UID is authorized
  if (isAuthorized(uid)) {
    Serial.println("Access Granted!");
    // Add actions for authorized access (e.g., unlocking a door)
  } else {
    Serial.println("Access Denied!");
    // Add actions for unauthorized access
  }

  delay(1000); // Wait for 1 second before scanning again
  mfrc522.PICC_HaltA(); // Halt card
  mfrc522.PCD_StopCrypto1(); // Stop encryption
}

// Function to check if UID is authorized
bool isAuthorized(String uid) {
  for (int i = 0; i < numAuthorizedUIDs; i++) {
    if (uid == authorizedUIDs[i]) {
      return true;
    }
  }
  return false;
}