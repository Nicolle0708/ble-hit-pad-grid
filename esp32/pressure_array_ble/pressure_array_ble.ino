#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int sensorPins[10] = {4, 5, 6, 7, 15, 18, 16, 17, 8, 9};
const char* sensorNames[10] = {
  "S1", "S2", "S3", "S4", "S5",
  "S6", "S7", "S8", "S9", "S10"
};

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

int baseline[10];
int peakValues[10];

const int threshold = 300;
const unsigned long windowMs = 120;

// BLE UUIDs (custom)
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // notify
#define CHARACTERISTIC_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // write (optional)

BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("BLE client connected");
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("BLE client disconnected");
    BLEDevice::startAdvertising();
    Serial.println("Advertising restarted");
  }
};

int readSmooth(int pin) {
  long sum = 0;
  for (int i = 0; i < 3; i++) {
    sum += analogRead(pin);
    delay(1);
  }
  return sum / 3;
}

void calibrateSensors() {
  for (int i = 0; i < 10; i++) {
    long sum = 0;
    for (int j = 0; j < 30; j++) {
      sum += analogRead(sensorPins[i]);
      delay(5);
    }
    baseline[i] = sum / 30;
  }
}

void resetPeaks() {
  for (int i = 0; i < 10; i++) {
    peakValues[i] = 0;
  }
}

void capturePeaks(unsigned long durationMs) {
  resetPeaks();
  unsigned long startTime = millis();

  while (millis() - startTime < durationMs) {
    for (int i = 0; i < 10; i++) {
      int value = readSmooth(sensorPins[i]) - baseline[i];
      if (value < 0) value = 0;
      if (value > peakValues[i]) {
        peakValues[i] = value;
      }
    }
  }
}

int getStrongestIndex() {
  int maxIndex = 0;
  for (int i = 1; i < 10; i++) {
    if (peakValues[i] > peakValues[maxIndex]) {
      maxIndex = i;
    }
  }
  return maxIndex;
}

String makeCsvPacket() {
  String packet = "CSV,";
  for (int i = 0; i < 10; i++) {
    packet += String(peakValues[i]);
    if (i < 9) packet += ",";
  }
  return packet;
}

void notifyPacket(const String& packet) {
  Serial.println(packet);

  if (deviceConnected) {
    //pTxCharacteristic->setValue(packet.c_str());
    //pTxCharacteristic->notify();
    String out = packet + "\n";
    pTxCharacteristic->setValue(out.c_str());
    pTxCharacteristic->notify();
  }
}

void setupBLE() {
  BLEDevice::init("ESP32_HIT_PAD_BLE");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_RX,
    BLECharacteristic::PROPERTY_WRITE
  );

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE advertising started");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  analogReadResolution(12);

  Serial.println("Calibrating... do not touch sensors.");
  calibrateSensors();
  Serial.println("Calibration done.");

  setupBLE();
}

void loop() {
  capturePeaks(windowMs);

  int maxIndex = getStrongestIndex();

  if (peakValues[maxIndex] > threshold) {
    // Main packet for app / phone / PC
    String packet = makeCsvPacket();
    notifyPacket(packet);

    // Optional human-readable packet
    String summary = "HIT," + String(sensorNames[maxIndex]) + "," + String(peakValues[maxIndex]);
    notifyPacket(summary);
  } 

  delay(50);
}
