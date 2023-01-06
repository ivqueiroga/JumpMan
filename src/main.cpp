// libraries 
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <gamma.h>

// defines and constants
#define PIN     18              // data pin on ESP-32 board
#define mW      16              // matrix width
#define mH      16              // matrix height
#define NUMPIXELS mW*mH         // matrix length
#define BRIGHTNESS 32           // max 255, 32 is a good default and more current safe
#define gravity .2              // gravity applied to fall effect
const int REFRESH = 63;         // refresh rate


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
bool jumpLastState,crowLastState,gameOver,launch,jumpSide;
int pebbleX,pebbleY,pebbleDX,pebbleDY,lifeBar,jumpCount;
unsigned long previousTimer = 0;



Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(mW, mH, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

// player class
class Player {
  public:
    int pX, pY, velocity=1, color;

    void set_values (int x, int y, int col)
      { pX=x; pY=y; color=col; }

    void draw () {
      matrix.drawPixel(pX, pY, color);
    };

    void update () {
      this->draw();
      this->pY += this->velocity;
      if (this->pY + 1 + this->velocity < mH-3) this->velocity += gravity;
      else this->velocity = 0;
    };

};

Player player1;
Player pebble1;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  matrix.begin();
  matrix.setBrightness(40);
  player1.set_values(1,0,Orange);
  pebble1.set_values(15,0,Black);
  previousTimer = millis();
}

void Draw() {
  matrix.fillScreen(Cyan);
  matrix.fillRect(0,mH-3,16,3,Maroon);
  matrix.fillRect(0,mH-4,16,1,DarkGreen);
  player1.update();
  pebble1.update();
  matrix.show();
}

void Input() {

}

void Logic() {
  if(pebbleX < 0) pebbleX = mW;
  pebble1.set_values(pebbleX,mH-5,Black);
  pebbleX--;
}

void loop() {
unsigned long currentTimer = millis();
  if (currentTimer - previousTimer >= REFRESH)
  {
    Draw();
    Input();
    Logic();
    previousTimer = currentTimer;
  }

}