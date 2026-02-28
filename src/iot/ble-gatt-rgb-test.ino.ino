#include <BLE2901.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Preferences.h>
#include <nvs_flash.h>

const uint8_t BUTTON_PIN = 21;
const uint8_t LED_PIN = 10;
const uint8_t RGB_RED_PIN = 0;
const uint8_t RGB_GREEN_PIN = 1;
const uint8_t RGB_BLUE_PIN = 3;

#define SERVICE_UUID "95dd0623-3ad2-402d-b3dd-2618296ab725"
#define RED_CHAR_UUID "e542d3f6-eeab-4cad-9905-e8be6fc9700a"
#define GREEN_CHAR_UUID "10e3098b-5cdc-4fd8-8a39-bc9dfc07129d"
#define BLUE_CHAR_UUID "6fa88b76-2482-48cc-bfea-dade929a0d61"

BLEServer *pServer = NULL;
bool deviceConnected = false;
bool isAdvertising = true;

unsigned long previousBlinkTime = 0;
const long blinkInterval = 500;
bool ledState = LOW;

unsigned long buttonPressStart = 0;
bool isButtonPressed = false;

Preferences preferences;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    isAdvertising = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("CONNECTED");
  }

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    isAdvertising = true;
    Serial.println("DISCONNECTED - Restarting Advertising...");
    BLEDevice::startAdvertising();
  }
};

class ColorCallback : public BLECharacteristicCallbacks {
  uint8_t configuredPin;
  String colorName;

public:
  ColorCallback(uint8_t pin, String name)
      : configuredPin(pin), colorName(name) {}
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t *rxData = pCharacteristic->getData();
    if (pCharacteristic->getLength() > 0) {
      uint8_t pwmValue = rxData[0];
      analogWrite(configuredPin, pwmValue);
      pCharacteristic->setValue(&pwmValue, 1);

      if (colorName == "RED")
        preferences.putUInt("red", pwmValue);
      else if (colorName == "GREEN")
        preferences.putUInt("green", pwmValue);
      else if (colorName == "BLUE")
        preferences.putUInt("blue", pwmValue);

      Serial.printf("GATT: %s set to %d\n", colorName.c_str(), pwmValue);
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  preferences.begin("rgb_colors", false);
  uint8_t r = preferences.getUInt("red", 0);
  uint8_t g = preferences.getUInt("green", 0);
  uint8_t b = preferences.getUInt("blue", 0);
  analogWrite(RGB_RED_PIN, r);
  analogWrite(RGB_GREEN_PIN, g);
  analogWrite(RGB_BLUE_PIN, b);

  BLEDevice::init("ESP32-C3_RGB");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);

  auto setupChar = [&](const char *uuid, uint8_t pin, String name,
                       uint8_t val) {
    BLECharacteristic *pChar = pService->createCharacteristic(
        uuid,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
    pChar->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                ESP_GATT_PERM_WRITE_ENCRYPTED);
    pChar->setCallbacks(new ColorCallback(pin, name));
    pChar->setValue(&val, 1);
    BLE2901 *desc = new BLE2901();
    desc->setValue((name + " Brightness").c_str());
    pChar->addDescriptor(desc);
  };

  setupChar(RED_CHAR_UUID, RGB_RED_PIN, "RED", r);
  setupChar(GREEN_CHAR_UUID, RGB_GREEN_PIN, "GREEN", g);
  setupChar(BLUE_CHAR_UUID, RGB_BLUE_PIN, "BLUE", b);

  pService->start();

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("SYSTEM READY. Hold button 3s for Factory Reset.");
}

void loop() {
  unsigned long currentTime = millis();
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == HIGH) {
    if (!isButtonPressed) {
      buttonPressStart = currentTime;
      isButtonPressed = true;
    } else if (currentTime - buttonPressStart >= 3000) {
      Serial.println("!!! FACTORY RESET !!! Wiping all memory and bonds...");

      nvs_flash_erase();
      nvs_flash_init();

      for (int i = 0; i < 15; i++) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(70);
      }
      Serial.println("Restarting...");
      ESP.restart();
    }
  } else {
    isButtonPressed = false;
  }

  if (isAdvertising && !deviceConnected) {
    if (currentTime - previousBlinkTime >= blinkInterval) {
      previousBlinkTime = currentTime;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  } else if (deviceConnected) {
    digitalWrite(LED_PIN, LOW);
  }
}
