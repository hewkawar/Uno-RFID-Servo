#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Define RFID pins
#define RST_PIN 9  // Reset pin
#define SS_PIN 10  // Slave Select pin

#define LED_OK_PIN 7     // Green LED for authorized access
#define LED_WRONG_PIN 6  // Red LED for unauthorized access

#define SERVO_PIN 3      // Servo pin

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
Servo lockServo;                 // Create Servo instance

// Authorized UIDs (example: "B9 96 50 7B")
String authorizedUIDs[] = {
  "B9 96 50 7B" // Replace with your RFID card UID
};
int numAuthorizedUIDs = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

void setup() {
  Serial.begin(9600);     // Initialize serial communication
  SPI.begin();            // Initialize SPI bus
  mfrc522.PCD_Init();     // Initialize MFRC522

  // Set up LEDs
  pinMode(LED_OK_PIN, OUTPUT);
  pinMode(LED_WRONG_PIN, OUTPUT);

  // Set up servo
  lockServo.attach(SERVO_PIN); // Attach servo to pin 3
  lockServo.write(0);          // Locked position

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
    grantAccess(); // Unlock the door for authorized UID
  } else {
    denyAccess(); // Deny access for unauthorized UID
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

// Function to grant access
void grantAccess() {
  Serial.println("Access Granted!");

  // Turn on green LED
  digitalWrite(LED_OK_PIN, HIGH);
  digitalWrite(LED_WRONG_PIN, LOW);

  // Unlock the door
  lockServo.write(90); // Move servo to unlocked position
  delay(3000);         // Keep the door unlocked for 3 seconds

  // Lock the door
  lockServo.write(0);  // Move servo back to locked position

  // Turn off green LED
  digitalWrite(LED_OK_PIN, LOW);
}

// Function to deny access
void denyAccess() {
  Serial.println("Access Denied!");

  // Turn on red LED
  digitalWrite(LED_WRONG_PIN, HIGH);
  digitalWrite(LED_OK_PIN, LOW);

  delay(2000); // Keep red LED on for 2 seconds

  // Turn off red LED
  digitalWrite(LED_WRONG_PIN, LOW);
}
