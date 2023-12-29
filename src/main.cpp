#include <Arduino.h>
#include <tinyNeoPixel.h>

#define NUMLEDS 30
int speed = 100;
uint8_t hue;
uint32_t led;
uint8_t maxBrightness = 255;
volatile byte mode = 0;
uint8_t randhue[NUMLEDS];
uint8_t randbright[NUMLEDS];
uint8_t direction[NUMLEDS]; // 1=rising, 0=falling

void newRandom();
void twinkle();
void expandRainbow();
void expand();

tinyNeoPixel leds = tinyNeoPixel(NUMLEDS, PIN_PA5, NEO_GRB);

void setup()
{
	cli();
	pinMode(PIN_PA2, INPUT); // pin 14

	PORTA.PIN2CTRL |= 0x02; // ISC = 2 trigger rising - uses |= so current value of PULLUPEN is preserved
	// Note: uses 4 bytes more flash than the first two examples, plus the overhead of calling pinMode().
	PORTA.PIN2CTRL &= PORT_ISC_RISING_gc; // interrupt on rising

	PORTA.PIN1CTRL = 0b00000010; // NOPULLUP = 0, ISC = 2 trigger rising
	leds.begin();
	for (int h = 0; h < NUMLEDS; h++)
	{
		randhue[h] = random(256);
		randbright[h] = random(maxBrightness + 1);
		direction[h] = random(2);
	}
	sei();
}

void loop()
{
	switch (mode - 1)
	{
	case 0:
		for (int i = 0; i < NUMLEDS; i++)
		{
			leds.setPixelColor(i, 0, 0, 0);
		}
		leds.show();
	case 1:
		for (int i = 0; i < NUMLEDS; i++)
		{
			leds.setPixelColor(i, maxBrightness, maxBrightness, maxBrightness);
		}
		leds.show();
		break;
	case 2:
		twinkle();
		break;
	case 3:
		newRandom();
		break;
	case 4:
		expand();
		break;
	case 5:
		expandRainbow();
		break;
	}
	delay(speed);
}

void newRandom()
{
	for (int i = 0; i < NUMLEDS; i += 1)
	{
		if (0 < randbright[i] < maxBrightness)
		{
			if (direction[i])
				randbright[i] = randbright[i] + 1;
			else
				randbright[i] = randbright[i] - 1;

			leds.setPixelColor(i, leds.ColorHSV(randhue[i], 255, randbright[i]));
		}
		else if (randbright[i] == maxBrightness)
			direction[i] = 0;
		else
		{
			randhue[i] = random(maxBrightness);
			direction[i] = 1;
		}
	}
	leds.show();
}

void twinkle()
{
	for (int i = 0; i < NUMLEDS; i += 1)
	{
		if (0 < randbright[i] < maxBrightness)
		{
			if (direction[i])
				randbright[i] = randbright[i] + 1;
			else
				randbright[i] = randbright[i] - 1;

			leds.setPixelColor(i, randbright[i], randbright[i], randbright[i]);
		}
		else if (randbright[i] == maxBrightness)
			direction[i] = 0;
		else
		{
			direction[i] = 1;
		}
	}
	leds.show();
}

void expandRainbow()
{
	static uint8_t hue;
	for (int i = 0; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue, 255, maxBrightness));
	}
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue + 20, 255, maxBrightness));
	}
	for (int i = 2; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue + 40, 255, maxBrightness));
		leds.setPixelColor(i + 1, leds.ColorHSV(hue + 40, 255, maxBrightness));
	}
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue + 80, 255, maxBrightness));
	}
	hue++;
}

void expandRainbowAsync()
{
	static uint8_t hue1, hue2, hue3, hue4;
	for (int i = 0; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue, 255, maxBrightness));
	}
	hue++;
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue1, 255, maxBrightness));
	}
	hue1 = +2;
	for (int i = 2; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue3, 255, maxBrightness));
		leds.setPixelColor(i + 1, leds.ColorHSV(hue3, 255, maxBrightness));
	}
	hue3 = +3;
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue4, 255, maxBrightness));
	}
	hue4 = +4;
}

void expand()
{
	static uint8_t wave;
	for (int i = 0; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.sine8(wave), leds.sine8(wave), leds.sine8(wave));
	}
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.sine8(wave + 85), leds.sine8(wave + 85), leds.sine8(wave + 85));
	}
	for (int i = 2; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.sine8(wave + 170), leds.sine8(wave + 170), leds.sine8(wave + 170));
		leds.setPixelColor(i + 1, leds.sine8(wave + 170), leds.sine8(wave + 170), leds.sine8(wave + 170));
	}
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.sine8(wave + 255), leds.sine8(wave + 255), leds.sine8(wave + 255));
	}
	wave++;
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