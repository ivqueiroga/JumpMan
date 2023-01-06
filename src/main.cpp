// libraries 
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <gamma.h>
#include <ColorPalette.h>

// defines and constants
#define PIN     18              // data pin on ESP-32 board
#define mW      16              // matrix width
#define mH      16              // matrix height
#define NUMPIXELS mW*mH         // matrix length
#define BRIGHTNESS 40           // max 255, 32 is a good default and more current safe
#define gravity 1               // gravity applied to fall effect
const int REFRESH = 65;         // refresh rate

// variables that will change
bool gameOver;
int pebbleX,lifeBar,jumpCount;
unsigned long previousTimer = 0;



Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(mW, mH, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

// Pixel class
class Pixel {
  private:
    int color;

  public:
    int pX, pY;
    float velocity=.5;

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

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

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

void Draw() {
  bg.update();
  matrix.fillRect(4,0,lifeBar,1,GreenYellow);
  player1.update();
  pebble1.update();
  matrix.show();
}

void Input() {
  if (jumpCount > 0){
    if (pebble1.pX == 6)
    {
      player1.velocity = -2;
      // jumpCount--; // life loss and gameOver simulation after jump count ends
    }
  }
}

void Logic() {
  if(--pebbleX >= 0) {
      pebble1.pX = pebbleX;
    } else pebbleX = mW;
  if (player1.pX == pebble1.pX && player1.pY == pebble1.pY){
  lifeBar--;
  }
  if (lifeBar < 0) gameOver = true;
}

void loop() {
while (!gameOver) {
  unsigned long currentTimer = millis();
    if (currentTimer - previousTimer >= REFRESH)
    {
      Draw();
      Input();
      Logic();
      previousTimer = currentTimer;
    }
  }
}