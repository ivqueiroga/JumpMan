// libraries 
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <gamma.h>

// defines and constants
#define PIN     18              // data pin on ESP-32 board
#define mW      16              // matrix width
#define mH      16              // matrix height
#define NUMPIXELS mW*mH         // matrix length
#define BRIGHTNESS 40           // max 255, 32 is a good default and more current safe
#define gravity 1               // gravity applied to fall effect
const int REFRESH = 65;         // refresh rate


// color pallet
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255,   0 */
#define White           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */

// variables that will change
bool jumpLastState,gameOver,launch,jumpSide;
int pebbleX,pebbleY,pebbleDX,pebbleDY,lifeBar,jumpCount;
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
  player1.set_values(2,7,Orange);
  pebble1.set_values(15,10,Black);
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
      // jumpCount--;
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