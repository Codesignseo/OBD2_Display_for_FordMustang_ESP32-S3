// Continously collect car data on one of the ESP32-S3 cores. This uses the built-in CAN controller of the ESP32-S3 together with a
// connected SN65HVD230 CAN bus transceiver. ESP32 TWAI (Two-Wire Automotive Interface) is used to communicate with the CAN bus.

#include <stdint.h>
#include <stdlib.h>

#ifndef _COLLECT_CAR_DATA
#define _COLLECT_CAR_DATA

#include <ESP32-TWAI-CAN.hpp>   // TWAI = Two-Wire Automotive Interface
#include "OBD2Calculations.h"   // Callback functions for OBD2 PIDs
#include "OBD2Utils.h"          // Misc helper functions for OBD2

// Struct to define CAN IDs
struct CanID
{
  char Name[64];
  uint32_t ID;
  int32_t (*CalculateValue)(const uint8_t* pData);
  void (*PrintInformation)(void);
};

// CAN IDs of CAN frames that are continously broadcasted which carries encoded information without the need to send an OBD2 request
const int32_t NumCanIDs = 3;
CanID CanIDs[NumCanIDs] = { { "Currrent Gear",  0x230,  &CalcCurrentGear, PrintCurrentGear },
                            { "Engine RPM",     0x204,  &CalcEngineRPM,   PrintEngineRPM },
                            { "Gearbox Mode",   0x171,  &CalcGearboxMode, PrintGearboxMode } };

// Configuration to set SN65HVD230 in "Listen Only" mode
twai_general_config_t listenOnlyConfig = TWAI_GENERAL_CONFIG_DEFAULT(gpio_num_t(SN65HVD230_TXPin), gpio_num_t(SN65HVD230_RXPin), TWAI_MODE_LISTEN_ONLY);

// Switch SN65HVD230 to low power "Listen Only" mode
void ListenOnlyMode_SN65HVD230()
{
  ESP32Can.begin(TWAI_SPEED_500KBPS, SN65HVD230_TXPin, SN65HVD230_RXPin, 0, 1024, nullptr, &listenOnlyConfig);
}

void SetupCollectCarData()
{
  DebugPrintln("SetupCollectCarData()");

  memset(&g_CurrentCarData, 0, sizeof(g_CurrentCarData));

  // Car data will be collected on ESP32-S3 core 1 and used on core 0, so we need to use a semaphore to make sure data is handled safely between the cores
  g_SemaphoreCarData = xSemaphoreCreateBinary();
  xSemaphoreGive(g_SemaphoreCarData);

  // We don't need to send any OBD2 requests, since we're only reading broadcasted CAN frames that are already flowing between car modules
  ListenOnlyMode_SN65HVD230();
}

// Helper function to help with debugging while device is not attached to car
void EmulateEngineRPM()
{
#ifdef DEBUG
  if (Serial.available() > 0)
  {
    uint8_t input = Serial.read() - '0';
    if (input < 10)
    {
      g_EngineRPM = input * 1000;
      PrintEngineRPM();
      g_CurrentCarData.EngineRPM = g_EngineRPM;
    }
  }
#endif
}

// Helper function to help with debugging while device is not attached to car
void EmulateCurrentGear()
{
#ifdef DEBUG
  if (Serial.available() > 0)
  {
    uint8_t input = Serial.read() - '0';
    if (input < 10)
    {
      g_CurrentGear = input;
      PrintCurrentGear();
      g_CurrentCarData.CurrentGear = g_CurrentGear;
    }
  }
#endif
}

// Listen for CAN frames and process them
void CollectCarData()
{
  CanFrame receivedCANFrame;

  if (ESP32Can.readFrame(receivedCANFrame), 10)
  {
    if (receivedCANFrame.data_length_code == 8)
    {
      auto canID = receivedCANFrame.identifier;

      for (int i = 0; i < NumCanIDs; i++)
      {
        if (canID == CanIDs[i].ID)
        {
          CanIDs[i].CalculateValue(receivedCANFrame.data);
          //CanIDs[i].PrintInformation();
        }
      }
    }
  }

  // Update data to be shared with the other ESP32-S3 core
  xSemaphoreTake(g_SemaphoreCarData, portMAX_DELAY);
  g_CurrentCarData.EngineRPM = g_EngineRPM;
  g_CurrentCarData.CurrentGear = g_CurrentGear;
  g_CurrentCarData.GearboxMode = g_GearboxMode;
  
#ifdef DEBUG_RPM
  EmulateEngineRPM();
#elif defined(DEBUG_GEAR)
  EmulateCurrentGear();
#endif

  xSemaphoreGive(g_SemaphoreCarData);
}

#endif  // _COLLECT_CAR_DATA