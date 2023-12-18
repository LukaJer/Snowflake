#include <Arduino.h>
#include <tinyNeoPixel.h>

#define NUMLEDS 100
int speed = 100;
uint8_t hue;
uint32_t led;
uint8_t randhue[NUMLEDS];
uint8_t randbright[NUMLEDS];
uint8_t direction[NUMLEDS]; // 1=rising, 0=falling

uint32_t AdjustBrightness(uint32_t color, int brightnessChange);
int8_t getState(uint32_t color);
void newRandom();
void twinkle();

volatile byte mode = 0;

tinyNeoPixel leds = tinyNeoPixel(NUMLEDS, PIN_PA5, NEO_GRB);

void setup()
{
  pinMode(PIN_PA2, INPUT_PULLUP); // pin 14

  PORTA.PIN2CTRL |= 0x02; // ISC = 2 trigger rising - uses |= so current value of PULLUPEN is preserved
  // Note: uses 4 bytes more flash than the first two examples, plus the overhead of calling pinMode().
  PORTA.PIN2CTRL &= PORT_ISC_RISING_gc; // interrupt on rising

  PORTA.PIN1CTRL = 0b00001010; // PULLUPEN = 1, ISC = 2 trigger rising
  leds.begin();
  for (int h = 0; h < NUMLEDS; h++)
  {
    randhue[h] = random(256);
    randbright[h] = random(256);
    direction[h] = random(2);
  }
  cli();
  sei();
}

void loop()
{
  switch (mode - 1)
  {
  case 0:
    for (int i = 0; i < NUMLEDS; i++)
    {
      leds.setPixelColor(i, 255, 255, 255);
    }
    leds.show();
    break;
  case 1:
    twinkle();
    break;
  case 2:
    newRandom();
    break;
  }
}

void finalrandom()
{
  while (true)
  {
    for (int i = 0; i < NUMLEDS; i += 1)
    {
      led = leds.getPixelColor(i);
      if (getState(led) == 1)
      {
        if (direction[i])
          leds.setPixelColor(i, AdjustBrightness(leds.getPixelColor(i), 1));
        else
          leds.setPixelColor(i, AdjustBrightness(leds.getPixelColor(i), -1));
      }
      else if (getState(led) == 2)
      {
        direction[i] = 0; // set to falling
      }
      else
      {
      }
    }
    leds.show();
  }
}

void newRandom()
{
  for (int i = 0; i < NUMLEDS; i += 1)
  {
    if (0 < randbright[i] < 255)
    {
      if (direction[i])
        randbright[i] = randbright[i] + 1;
      else
        randbright[i] = randbright[i] - 1;

      leds.setPixelColor(i, leds.ColorHSV(randhue[i], 255, randbright[i]));
    }
    else if (randbright[i] == 255)
      direction[i] = 0;
    else
    {
      randhue[i] = random(255);
      direction[i] = 1;
    }
  }
  leds.show();
}

void twinkle()
{
  for (int i = 0; i < NUMLEDS; i += 1)
  {
    if (0 < randbright[i] < 255)
    {
      if (direction[i])
        randbright[i] = randbright[i] + 1;
      else
        randbright[i] = randbright[i] - 1;

      leds.setPixelColor(i, randbright[i], randbright[i], randbright[i]);
    }
    else if (randbright[i] == 255)
      direction[i] = 0;
    else
    {
      direction[i] = 1;
    }
  }
  leds.show();
}

ISR(PORTA_PORT_vect)
{
  byte flags = PORTA.INTFLAGS;
  PORTA.INTFLAGS = flags; // clear flags
  mode = mode + 1;
  if (mode > 8)
  {
    mode = 0;
  }
}

int8_t getState(uint32_t color)
{
  uint32_t sum = color & 0xFFFFFF; // Mask out the alpha channel if present

  if (sum == 0)
  {
    return 0; // If the sum of RGB components is zero, it's black
  }
  else if (sum == 0xFFFFFF)
  {
    return 2; // If the sum is maximum (all components are 255), it's white
  }
  else
  {
    return 1; // Otherwise, it's neither black nor white
  }
}

uint8_t CalculateBrightness(uint32_t color)
{
  // Extract individual color components
  uint8_t r = (color >> 16) & 0xFF; // Red component
  uint8_t g = (color >> 8) & 0xFF;  // Green component
  uint8_t b = color & 0xFF;         // Blue component

  // Calculate brightness as an average of RGB components
  uint8_t brightness = (r + g + b) / 3; // You can adjust this formula if needed

  return brightness;
}

uint32_t AdjustBrightness(uint32_t color, int brightnessChange)
{
  // Extract individual color components
  uint8_t r = (color >> 16) & 0xFF; // Red component
  uint8_t g = (color >> 8) & 0xFF;  // Green component
  uint8_t b = color & 0xFF;         // Blue component

  // Adjust brightness by changing each component
  int adjustedR = r + brightnessChange;
  int adjustedG = g + brightnessChange;
  int adjustedB = b + brightnessChange;

  // Ensure the components are within the valid range (0-255)
  adjustedR = (adjustedR > 255) ? 255 : (adjustedR < 0) ? 0
                                                        : adjustedR;
  adjustedG = (adjustedG > 255) ? 255 : (adjustedG < 0) ? 0
                                                        : adjustedG;
  adjustedB = (adjustedB > 255) ? 255 : (adjustedB < 0) ? 0
                                                        : adjustedB;

  // Recombine the adjusted components into a 32-bit color value
  uint32_t adjustedColor = ((uint32_t)adjustedR << 16) | ((uint32_t)adjustedG << 8) | adjustedB;

  return adjustedColor;
}

void RGBtoHSV(uint32_t color, uint8_t *h, uint8_t *s, uint8_t *v)
{
  // Extract individual color components
  uint8_t r = (color >> 16) & 0xFF; // Red component
  uint8_t g = (color >> 8) & 0xFF;  // Green component
  uint8_t b = color & 0xFF;         // Blue component

  // Normalize RGB values to be in the range of 0 to 1
  float R = (float)r / 255.0;
  float G = (float)g / 255.0;
  float B = (float)b / 255.0;

  // Calculate min and max values of RGB
  float cmax = fmaxf(R, fmaxf(G, B));
  float cmin = fminf(R, fminf(G, B));
  float delta = cmax - cmin;

  // Calculate Hue
  float hue = 0.0;
  if (delta == 0)
  {
    hue = 0;
  }
  else if (cmax == R)
  {
    hue = 60 * fmodf(((G - B) / delta), 6);
  }
  else if (cmax == G)
  {
    hue = 60 * (((B - R) / delta) + 2);
  }
  else if (cmax == B)
  {
    hue = 60 * (((R - G) / delta) + 4);
  }

  if (hue < 0)
  {
    hue += 360;
  }

  // Calculate Saturation
  float saturation = (cmax == 0) ? 0 : (delta / cmax);

  // Value (Brightness)
  float value = cmax;

  // Convert HSV values to the range 0-255
  *h = (uint8_t)(hue / 360.0 * 255.0 + 0.5);
  *s = (uint8_t)(saturation * 255.0 + 0.5);
  *v = (uint8_t)(value * 255.0 + 0.5);
}