// Calculate car data values from OBD2 data. These calculations specifically work for the 2016 Ford Mustang Ecoboost. They might not work on other cars, feel free to experiment

#ifndef _OBD2_CALCULATIONS
#define _OBD2_CALCULATIONS

// --------------------------------------------------------
// ******** Engine RPM ************************************
// --------------------------------------------------------

static int32_t g_EngineRPM = 0;

int32_t CalcEngineRPM(const uint8_t* pData)
{
  uint8_t A = pData[3];
  uint8_t B = pData[4];
  g_EngineRPM = (A * 256 + B) * 2;

  return g_EngineRPM;
}

void PrintEngineRPM()
{
  Serial.printf("Engine RPM = %d\n", g_EngineRPM);
}

// --------------------------------------------------------
// ******** Currently Engaged Gear ************************
// --------------------------------------------------------

static int32_t g_CurrentGear = 0;    // 0 = Neutral, -1 = Reverse

int32_t CalcCurrentGear(const uint8_t* pData)
{
  if (pData[1] == 2)
  {
    g_CurrentGear = -1; // Reverse
  }
  else if (pData[1] == 4)
  {
    g_CurrentGear = 0;  // Neutral
  }
  else
  {
    g_CurrentGear = pData[0] / 16;
  }

  return g_CurrentGear;
}

void PrintCurrentGear()
{
  char gearStr[32] = "Neutral";

  if (g_CurrentGear == -1)
  {
    sprintf(gearStr, "Reverse");
  }
  else if (g_CurrentGear > 0)
  {
    sprintf(gearStr, "%d", g_CurrentGear);
  }

  Serial.printf("Current Engaged Gear = %s\n", gearStr);
}

// --------------------------------------------------------
// ******** Gearbox Mode ********************************
// --------------------------------------------------------

// Gearbox  mode
enum GearboxMode
{
  P = 0x00,   // Park
  R = 0x20,   // Reverse
  N = 0x40,   // Neutral
  D = 0x60,   // Drive
  S = 0x80,   // Sport
};

static int32_t g_GearboxMode = GearboxMode::P;

int32_t CalcGearboxMode(const uint8_t* pData)
{
  g_GearboxMode = pData[1];
  return g_GearboxMode;
}

void PrintGearboxMode()
{
  Serial.printf("Gearbox Mode: %#04x %s\n", g_GearboxMode, (g_GearboxMode == GearboxMode::D) ? "P" :
                                                           (g_GearboxMode == GearboxMode::R) ? "R" :
                                                           (g_GearboxMode == GearboxMode::N) ? "N" :
                                                           (g_GearboxMode == GearboxMode::D) ? "D" :
                                                           (g_GearboxMode == GearboxMode::S) ? "S" : "ERROR: Unknown Drive Mode");
}

#endif  // _OBD2_CALCULATIONS
