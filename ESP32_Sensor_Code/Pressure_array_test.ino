const int sensorPins[10] = {4, 5, 6, 7, 15, 18, 16, 17, 8, 9};
const char* sensorNames[10] = {
  "S1", "S2", "S3", "S4", "S5",
  "S6", "S7", "S8", "S9", "S10"
};

int sensorValues[10];
int baseline[10];
int threshold = 300;   // adjust later

int readSmooth(int pin) {
  long sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(pin);
    delay(1);
  }
  return sum / 5;
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

char levelChar(int v) {
  if (v < 100) return '.';
  if (v < 400) return '-';
  if (v < 1200) return '*';
  return '#';
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  analogReadResolution(12);

  Serial.println("Calibrating... do not touch sensors.");
  calibrateSensors();
  Serial.println("Calibration done.");
}

void loop() {
  int maxIndex = 0;

  for (int i = 0; i < 10; i++) {
    sensorValues[i] = readSmooth(sensorPins[i]) - baseline[i];
    if (sensorValues[i] < 0) sensorValues[i] = 0;

    if (sensorValues[i] > sensorValues[maxIndex]) {
      maxIndex = i;
    }
  }

  Serial.println("Raw values:");
  for (int i = 0; i < 10; i++) {
    Serial.print(sensorNames[i]);
    Serial.print(": ");
    Serial.print(sensorValues[i]);
    Serial.print("\t");
    if (i == 4) Serial.println();
  }
  Serial.println();

  Serial.println("Heat map:");
  for (int i = 0; i < 5; i++) {
    Serial.print("[");
    Serial.print(levelChar(sensorValues[i]));
    Serial.print("]");
  }
  Serial.println();

  for (int i = 5; i < 10; i++) {
    Serial.print("[");
    Serial.print(levelChar(sensorValues[i]));
    Serial.print("]");
  }
  Serial.println();

  if (sensorValues[maxIndex] > threshold) {
    Serial.print("Strongest zone: ");
    Serial.print(sensorNames[maxIndex]);
    Serial.print("  Value: ");
    Serial.println(sensorValues[maxIndex]);
  } else {
    Serial.println("No hit detected");
  }

  Serial.println("----------------------------");
  delay(120);
}
