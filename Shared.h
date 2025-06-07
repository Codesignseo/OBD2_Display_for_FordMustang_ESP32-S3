#ifndef _SHARED
#define _SHARED

#ifdef DEBUG
#define DebugPrintf(...) Serial.printf(__VA_ARGS__)
#define DebugPrintln(...) Serial.println(__VA_ARGS__)
#else
#define DebugPrintln(...)
#define DebugPrintf(...)
#endif

// ESP32-S3 pins
#define GC9A01_SCK        SCK
#define GC9A01_MOSI       MOSI
#define GC9A01_MISO       GFX_NOT_DEFINED
#define GC9A01_CS         SS
#define GC9A01_DC         MISO
#define GC9A01_RST        D0
#define GC9A01_BL         D1
#define MOSFET_GATE       D2
#define SN65HVD230_RXPin  D4
#define SN65HVD230_TXPin  D5

// Car data needed for the information to be displayed on LCD
struct CarData
{
  int32_t EngineRPM;
  int32_t CurrentGear;
  int32_t GearboxMode;
};

// This data is shared between two ESP32-S3 cores
CarData g_CurrentCarData { 0 };
SemaphoreHandle_t g_SemaphoreCarData = nullptr;
TaskHandle_t g_TaskDisplayInfo = nullptr;

#endif  // _SHARED