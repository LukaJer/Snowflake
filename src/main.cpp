#include <Arduino.h>
#include <tinyNeoPixel.h>

#define NUMLEDS 30
int speed = 100;
uint8_t hue;
uint32_t led;
uint8_t maxBrightness = 50;
volatile byte mode = 0, change = 0;
uint16_t randhue[NUMLEDS];
uint8_t randbright[NUMLEDS];
uint8_t direction[NUMLEDS]; // 1=rising, 0=falling
uint8_t ledDisplay[] = {30, 29, 27, 28, 26, 5, 4, 2, 3, 1};

uint8_t mygamma(uint8_t value);
uint8_t mygamma2(uint8_t value);
void rainbowTwinkle();
void twinkle();
void expandRainbow();
void expandRainbowAsync();
void expand();
void fullRainbow();
void singleRainbow();

tinyNeoPixel leds = tinyNeoPixel(NUMLEDS, PIN_PA5, NEO_GRB);

void setup()
{
	cli();
	pinMode(PIN_PA2, INPUT); // pin 14
	Serial.begin(9600);
	PORTA.PIN2CTRL |= 0x02; // ISC = 2 trigger rising - uses |= so current value of PULLUPEN is preserved
	// Note: uses 4 bytes more flash than the first two examples, plus the overhead of calling pinMode().
	PORTA.PIN2CTRL &= PORT_ISC_RISING_gc; // interrupt on rising

	PORTA.PIN1CTRL = 0b00000010; // NOPULLUP = 0, ISC = 2 trigger rising
	leds.begin();
	leds.clear();
	for (int h = 0; h < NUMLEDS; h++)
	{
		randhue[h] = random(65535);
		randbright[h] = random(maxBrightness + 1);
		direction[h] = random(2);
	}
	sei();
	Serial.println("Hi :)");
}

void loop()
{
	if (change)
	{
		Serial.println(mode);
		leds.clear();
		leds.show();
		for (int i = 0; i < mode; i++)
		{
			leds.setPixelColor(ledDisplay[i] - 1, maxBrightness, maxBrightness, maxBrightness);
		}
		leds.show();
		change = 0;
		delay(1000);
	}
	else
	{
		switch (mode)
		{
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
			expand();
			break;
		case 4:
			rainbowTwinkle();
			break;
		case 5:
			expandRainbow();
			break;
		case 6:
			expandRainbowAsync();
			break;
		case 7:
			fullRainbow();
			break;
		case 8:
			singleRainbow();
			break;
		}
	}
	delay(speed);
}

void twinkle()
{
	uint8_t val;
	for (int i = 0; i < NUMLEDS; i += 1)
	{
		if (randbright[i] >= maxBrightness)
		{
			direction[i] = 0;
		}
		if (randbright[i] <= 1)
		{
			direction[i] = 1;
		}

		if (direction[i])
			randbright[i] = randbright[i] + 1;
		else
			randbright[i] = randbright[i] - 1;

		val = mygamma(randbright[i]);
		leds.setPixelColor(i, val, val, val);
	}
	leds.show();
}

uint8_t mygamma(uint8_t value)
{
	const float maxBrightnessFloat = static_cast<float>(maxBrightness);
	return round((value-2) * pow(value / maxBrightnessFloat, 5)+2);
}

uint8_t mysin(uint8_t value)
{
	return round(maxBrightness / 2 * (1 - cos(value / 40.055)));
}

uint8_t mygamma2(uint8_t value)
{
	return round((maxBrightness - 10) * pow(value / 255.0, 1)) + 10;
}

void expand()
{
	static uint8_t wave;
	for (int i = 0; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(0, 0, mygamma2(wave)));
	}
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(0, 0, mygamma2(wave + 10)));
		leds.setPixelColor(i + 1, leds.ColorHSV(0, 0, mygamma2(wave + 10)));
	}
	for (int i = 3; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(0, 0, mygamma2(wave + 20)));
	}
	for (int i = 4; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(0, 0, mysin(wave + 30)));
	}
	wave = wave + 10;
	leds.show();
}

void rainbowTwinkle()
{
	for (int i = 0; i < NUMLEDS; i += 1)
	{
		if (randbright[i] >= maxBrightness)
		{
			direction[i] = 0;
		}
		if (randbright[i] <= 1)
		{
			randhue[i] = random(65535);
			direction[i] = 1;
		}

		if (direction[i])
			randbright[i] = randbright[i] + 1;
		else
			randbright[i] = randbright[i] - 1;

		leds.setPixelColor(i, leds.ColorHSV(randhue[i], 255, randbright[i]));
	}
	leds.show();
}

void expandRainbow()
{
	static uint16_t hue;
	for (int i = 0; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue, 255, maxBrightness));
	}
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue + 5000, 255, maxBrightness));
		leds.setPixelColor(i + 1, leds.ColorHSV(hue + 5000, 255, maxBrightness));
	}
	for (int i = 3; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue + 10000, 255, maxBrightness));
	}
	for (int i = 4; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue + 15000, 255, maxBrightness));
	}
	hue = hue + 255;
	leds.show();
}

void expandRainbowAsync()
{
	static uint16_t hue1, hue2, hue3, hue4;
	for (int i = 0; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue1, 255, maxBrightness));
	}
	hue1 = hue1 + 250;
	for (int i = 1; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue2, 255, maxBrightness));
		leds.setPixelColor(i + 1, leds.ColorHSV(hue2, 255, maxBrightness));
	}
	hue2 = hue2 + 300;
	for (int i = 3; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue3, 255, maxBrightness));
	}
	hue3 = hue3 + 350;
	for (int i = 4; i < NUMLEDS; i += 5)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue4, 255, maxBrightness));
	}
	hue4 = hue4 + 400;
	leds.show();
}

void fullRainbow()
{
	static uint16_t hue;
	for (int i = 0; i < NUMLEDS; i += 1)
	{
		leds.setPixelColor(i, leds.ColorHSV(hue, 255, maxBrightness));
	}
	hue = hue + 250;
	leds.show();
}

void singleRainbow()
{
	for (int i = 0; i < NUMLEDS; i += 1)
	{
		leds.setPixelColor(i, leds.ColorHSV(randhue[i], 255, maxBrightness));
		randhue[i] = randhue[i] + 250;
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
	change = 1;
}