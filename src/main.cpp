// libraries 
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <gamma.h>
#include <ColorPalette.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// defines and constants
#define PIN     18              // data pin on ESP-32 board
#define mW      16              // matrix width
#define mH      16              // matrix height
#define NUMPIXELS mW*mH         // matrix length
#define BRIGHTNESS 40           // max 255, 32 is a good default and more current safe
#define gravity 1               // gravity applied to fall effect
#define SERVICE_UUID   "13e0ba4e-9678-11ed-a1eb-0242ac120002"
#define CHARACTERISTIC_UUID "311ffe9e-9678-11ed-a1eb-0242ac120002"
const int REFRESH = 65;         // refresh rate

// variables that will change
bool gameOver, lastJumpState;
int pebbleX,lifeBar,jumpCount;
unsigned long previousTimer = 0;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(mW, mH, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

BLECharacteristic *pCharacteristic;
bool characteristicValue = true;

// Pixel class
class Pixel {
  private:
    int color;

  public:
    int pX, pY;
    float velocity=0;

    void set_values (int x, int y, int col)
      { pX=x; pY=y; color=col; }

    void draw () {
      matrix.drawPixel(pX, pY, color);
    };

    void update () {
      this->draw();
      this->pY += this->velocity;
      if (this->pY+ 2 + this->velocity < mH-3) this->velocity += gravity;
      else this->velocity = 0;
    };

};

// Background class
class Background {
  public:
    int sky, grass, soil;

    void set_values (int x, int y, int z)
      { sky=x; grass=y; soil=z; }

    void draw () {
      matrix.fillScreen(sky);
      matrix.fillRect(0,mH-4,16,1,grass);
      matrix.fillRect(0,mH-3,16,3,soil);
    };

    void update () {
      this->draw();
    };

};

Pixel player1;
Pixel pebble1;
Background bg;

void Draw() {
  bg.update();
  matrix.fillRect(4,0,lifeBar,1,GreenYellow);
  player1.update();
  pebble1.update();
  matrix.show();
}

void Input() {
  std::string value = pCharacteristic->getValue();
  bool val = std::stoi(value.c_str());
  if (lastJumpState != val) {
    lastJumpState = val;
    if (val == 0) {
      player1.velocity = -2;
    };
  }
}

void Logic() {
  if(--pebbleX >= 0) {
      pebble1.pX = pebbleX;
    } else pebbleX = mW;
  // if (player1.pX == pebble1.pX && player1.pY == pebble1.pY){
  // lifeBar--;
  // }
  // if (lifeBar < 0) gameOver = true;
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Device disconnected");
    pServer->startAdvertising(); // Start advertising when a device disconnects
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      if (value[0] == '0') {
        Input();
        Serial.println("Jump");
        characteristicValue = true;
        pCharacteristic->setValue("1");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Pixel Frame");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setValue("1");
  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();  // Get advertising handle
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  pServer->startAdvertising();  // Start advertising

  Serial.println("Characteristic defined! Now you can read it on your phone!");
  lastJumpState = 0;
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  bg.set_values(Cyan, DarkGreen, Maroon);
  player1.set_values(2,11,Orange);
  pebble1.set_values(15,11,Black);
  previousTimer = millis();
  jumpCount = 4;
  lifeBar = 8;
  pebbleX = mW;
}

void loop() {
  while (!gameOver) {
  unsigned long currentTimer = millis();
    if (currentTimer - previousTimer >= REFRESH)
      {
        Draw();
        Input();
        Logic();
        if (characteristicValue) {
          pCharacteristic->setValue("1");
          pCharacteristic->notify();
          characteristicValue = false;
        }
      previousTimer = currentTimer;
    }
  }
}