#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "main.h"
#include "config.h"

// Configure for your panel(s) as appropriate!
#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32
#define PANELS_NUMBER 1 // Number of chained panels, if just a single panel, obviously set to 1
#define PANE_WIDTH PANEL_WIDTH *PANELS_NUMBER
#define PANE_HEIGHT PANEL_HEIGHT
#define NUM_LEDS PANE_WIDTH *PANE_HEIGHT

MatrixPanel_I2S_DMA matrix;

const uint16_t centreX = PANE_WIDTH / 2;
const uint16_t centreY = PANE_HEIGHT / 2;

const uint16_t black = matrix.color565(0, 0, 0);
const uint16_t red = matrix.color565(255, 0, 0);
const uint16_t green = matrix.color565(0, 255, 0);
const uint16_t yellow = matrix.color565(255, 255, 0);
const uint16_t blue = matrix.color565(0, 0, 255);
const uint16_t magenta = matrix.color565(255, 0, 255);
const uint16_t cyan = matrix.color565(0, 255, 255);
const uint16_t white = matrix.color565(255, 255, 255);
const uint16_t orange = matrix.color565(255, 128, 0);
const uint16_t dark_blue = matrix.color565(0, 0, 128);
const uint16_t dark_orange = matrix.color565(128, 64, 0);

// serial debug port baud rate
const unsigned int BAUD_RATE = 115200;

void setup()
{
  Serial.begin(BAUD_RATE);

  HUB75_I2S_CFG::i2s_pins pins = {HUB75_R1_PIN, HUB75_G1_PIN, HUB75_B1_PIN, HUB75_R2_PIN, HUB75_G2_PIN, HUB75_B2_PIN,
                                  HUB75_A_PIN, HUB75_B_PIN, HUB75_C_PIN, HUB75_D_PIN, HUB75_E_PIN,
                                  HUB75_LAT_PIN, HUB75_OE_PIN, HUB75_CLK_PIN};
  HUB75_I2S_CFG mxconfig(PANEL_WIDTH, PANEL_HEIGHT, PANELS_NUMBER, pins);
  mxconfig.driver = HUB75_I2S_CFG::FM6124;
  mxconfig.clkphase = false;
  mxconfig.double_buff = true;

  matrix = MatrixPanel_I2S_DMA(mxconfig);
  matrix.begin();
  matrix.setBrightness8(100);
}

void loop()
{
  static uint16_t i = 0;
  const uint16_t dot_colours[] = {blue, blue, dark_blue, dark_blue, orange, orange, dark_orange, dark_orange};
  const uint8_t num_colours = sizeof(dot_colours) / sizeof(dot_colours[0]);

  matrix.clearScreen();

  for (uint8_t x = 0; x < PANE_WIDTH; x++)
  {
    uint16_t col = dot_colours[(x + i) % num_colours];
    matrix.drawPixel(x, 0, col);
    matrix.drawPixel(PANE_WIDTH - x, PANE_HEIGHT - 1, col);
  }
  for (uint8_t y = 0; y < PANE_HEIGHT; y++)
  {
    uint16_t col = dot_colours[(y + i) % num_colours];
    matrix.drawPixel(PANE_WIDTH - 1, y, col);
    matrix.drawPixel(0, PANE_HEIGHT - y, col);
  }

  centreText("Otley", centreY - 5, white);
  matrix.setCursor(2, 18);
  matrix.setTextColor(blue);
  matrix.print("MAKER");
  matrix.setCursor(33, 18);
  matrix.setTextColor(orange);
  matrix.print("SPACE");

  matrix.flipDMABuffer();
  delay(50);
  i++;
}

void testCard()
{
  int barWidth = PANE_WIDTH / 8;
  uint16_t barColours[] = {black, blue, red, magenta, green, cyan, yellow, white};
  for (int colIndex = 0; colIndex < 8; colIndex++)
  {
    uint16_t barColour = barColours[colIndex];
    for (int x = 0; x < barWidth; x++)
    {
      matrix.drawFastVLine(colIndex * barWidth + x, 0, PANE_HEIGHT, barColour);
    }
  }
}

void centreText(char *string, uint8_t y, uint16_t colour)
{
  int16_t x1;
  int16_t y1;
  uint16_t w;
  uint16_t h;
  matrix.getTextBounds(string, 0, 0, &x1, &y1, &w, &h);
  matrix.setCursor(centreX - w / 2, y - h / 2);
  matrix.setTextColor(colour);
  matrix.print(string);
}
