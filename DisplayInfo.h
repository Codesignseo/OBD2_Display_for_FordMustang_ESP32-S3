#ifndef _DISPLAY_INFO
#define _DISPLAY_INFO

#include <Arduino_GFX_Library.h>
#include "FreeMonoBold54pt7b.h"
#include "MirrorFont.h"
#include "AsyncTimer.h"

// Setup gfx library for the GC9A01 display
Arduino_DataBus* bus = new Arduino_ESP32SPI(GC9A01_DC, GC9A01_CS, GC9A01_SCK, GC9A01_MOSI, GC9A01_MISO);
Arduino_GFX* gfx = new Arduino_GC9A01(bus, GC9A01_RST, 0, true);
bool bIsDisplayOn = false;

// Define a simple shift indicator based only on RPM. It would be better to setup something using dyno numbers, looking at torque, etc.
struct ShiftIndicatorLights
{
  int32_t EngineRPM;
  uint16_t Color;
};

const int32_t NumLights = 5;
const int32_t ShiftNowIndex = 0;  // Index into shiftIndicatorLights for ideal shift time, i.e. when the indicator turns red
const ShiftIndicatorLights shiftIndicatorLights[NumLights] = { {4500, RED}, {3500, YELLOW}, {2500, GREEN}, {1500, BLUE}, {0, DARKGREY} };

const int32_t MaxRPM  = 6000;       // Max RPM to show on LCD
const int32_t Reverse = -1;         // Define Reverse as a special gear number
const int32_t Park    = -2;         // Define Park as a special gear number
int32_t  previousGear = -12;        // A randomly chosen initial number
uint16_t previousGearColor = WHITE;

// g_CurrentCarData is populated by the SN65HVD230 transceiver on another ESP32-S3 core. Since the data needs to be thread safe, we keep a
// local copy of the data on this thread, copying it safely using g_SemaphoreCarData
CarData carData;

// Make a local copy of car data that was gathered on the other ESP32-S3 core
void CopyCarData()
{
  xSemaphoreTake(g_SemaphoreCarData, portMAX_DELAY);
  memcpy(&carData, &g_CurrentCarData, sizeof(CarData));
  xSemaphoreGive(g_SemaphoreCarData);

  // If Neutral, then check if we're in Park, since we'd rather display "P" than "N" when in Park
  if (carData.CurrentGear == 0 &&
      carData.GearboxMode == GearboxMode::P)
  {
    carData.CurrentGear = Park;
  }
}

// Turn power for display ON
void TurnMosfetSwitchOn()
{
  DebugPrintln("Turn Mosfet switch ON");
  pinMode(MOSFET_GATE, OUTPUT);
  digitalWrite(MOSFET_GATE, HIGH);
}

// Turn power for display OFF
void TurnMosfetSwitchOff()
{
  DebugPrintln("Turn Mosfet switch OFF");
  pinMode(MOSFET_GATE, OUTPUT);
  digitalWrite(MOSFET_GATE, LOW);
}

void TurnDisplayOn()
{
  DebugPrintln("TurnDisplayOn()");

  // Turn power on
  TurnMosfetSwitchOn();

  // Init display
  while (!gfx->begin())
  {
    DebugPrintln("gfx->begin() failed!");
    delay(500);
  }

  bIsDisplayOn = true;

  gfx->fillScreen(RGB565_BLACK);
  gfx->setRotation(2);
  gfx->setFont(&FreeMonoBold54pt7b);
  gfx->setTextSize(2);
}

void TurnDisplayOff()
{
  DebugPrintln("TurnDisplayOff()");
  gfx->fillScreen(RGB565_BLACK);
  TurnMosfetSwitchOff();
  bIsDisplayOn = false;
  delay(200);
}

// This will be called from the main setup() function, which will get called each time the device wakes up from deep sleep
void SetupDisplay()
{
  DebugPrintln("SetupDisplay()");

  // If we want to display the text on a projective film as a holographic effect on the windshield, then we need to mirror the text
#ifdef MIRROR_TEXT_FOR_HOLOGRAPHIC_REFLECTION
  const uint8_t fontStartCharacter = FreeMonoBold54pt7b.first;

  // Mirror text: P, R, N, 0-9 (takes only ~15ms)
  uint16_t index = 'P' - fontStartCharacter;
  MirrorText(FreeMonoBold54pt7bBitmaps, FreeMonoBold54pt7bGlyphs[index].bitmapOffset,  FreeMonoBold54pt7bGlyphs[index].width, FreeMonoBold54pt7bGlyphs[index].height);
  index = 'R' - fontStartCharacter;
  MirrorText(FreeMonoBold54pt7bBitmaps, FreeMonoBold54pt7bGlyphs[index].bitmapOffset,  FreeMonoBold54pt7bGlyphs[index].width, FreeMonoBold54pt7bGlyphs[index].height);
  index = 'D' - fontStartCharacter;
  MirrorText(FreeMonoBold54pt7bBitmaps, FreeMonoBold54pt7bGlyphs[index].bitmapOffset,  FreeMonoBold54pt7bGlyphs[index].width, FreeMonoBold54pt7bGlyphs[index].height);
  index = 'N' - fontStartCharacter;
  MirrorText(FreeMonoBold54pt7bBitmaps, FreeMonoBold54pt7bGlyphs[index].bitmapOffset,  FreeMonoBold54pt7bGlyphs[index].width, FreeMonoBold54pt7bGlyphs[index].height);

  for (int i = 0; i < 10; i++)
  {
    index = '0' + i - fontStartCharacter;
    MirrorText(FreeMonoBold54pt7bBitmaps, FreeMonoBold54pt7bGlyphs[index].bitmapOffset,  FreeMonoBold54pt7bGlyphs[index].width, FreeMonoBold54pt7bGlyphs[index].height);
  }
#endif
}

char GenerateGearText(const int32_t gear)
{
  if ((gear > 0) && (gear <= 8))
  {
    return '0' + gear;
  }
  else
  {
    return (gear == Park) ? 'P' : (gear == Reverse) ? 'R' : (gear == 0) ? 'N' : 'D';
  }
}

// Clear the current gear text by redrawing it in black
void ClearGearText(const int32_t gear)
{
  gfx->setCursor(60, 180);
  gfx->setTextColor(RGB565_BLACK);
  gfx->printf("%c", GenerateGearText(gear));
}

void DrawGearText()
{
  // Determine and keep track of the gear color
  uint16_t color = WHITE;
  if (carData.EngineRPM > shiftIndicatorLights[ShiftNowIndex].EngineRPM)
  {
    color = RED;
  }

  // Update the text when the gear changes
  if (carData.CurrentGear != previousGear)
  {
    ClearGearText(previousGear);

    // Show the new gear
    gfx->setCursor(60, 180);
    gfx->setTextColor(color);
    gfx->printf("%c", GenerateGearText(carData.CurrentGear));

    previousGear = carData.CurrentGear;
    previousGearColor = color;
  }

  // Update the text when the gear color changes
  if (color != previousGearColor)
  {
    // Show the current gear with the new color
    gfx->setCursor(60, 180);
    gfx->setTextColor(color);
    gfx->printf("%c", GenerateGearText(carData.CurrentGear));

    previousGearColor = color;
  }
}

void DrawShiftIndicator()
{
  uint16_t color = DARKGREY;

  for (int i = 0; i < NumLights; i++)
  {
    if (carData.EngineRPM > shiftIndicatorLights[i].EngineRPM)
    {
      color = shiftIndicatorLights[i].Color;
      break;
    }
  }

  const uint16_t rotate = 90;
  const uint16_t endRadius = (min(MaxRPM, carData.EngineRPM) / float(MaxRPM)) * 180.0f;

  // Draw left side bar
  gfx->fillArc(120, 120, 120, 95, 0 + rotate, endRadius + rotate, color);
  gfx->fillArc(120, 120, 120, 95, endRadius + rotate, 180 + rotate, BLACK);

  // Draw right side bar
  gfx->fillArc(120, 120, 120, 95, 360 - endRadius + rotate, 360 + rotate, color);
  gfx->fillArc(120, 120, 120, 95, 360 - 180 + rotate, 360 - endRadius + rotate, BLACK);
}

// Main function of the thread task running on a seperate ESP32-S3 core
void DisplayInfo(void* params)
{
  DebugPrintf("Core %d: DisplayInfo()\n", xPortGetCoreID());

  SetupDisplay();
  TurnDisplayOn();

  while (true)
  {
    CopyCarData();

    if (bIsDisplayOn)
    {
      DrawGearText();
      DrawShiftIndicator();
    }

    delay(1);  // Make sure something is happening on the thread, otherwise a watchdog timer will kick in after 5 seconds and reboot device
  }
}

#endif  // _DISPLAY_INFO