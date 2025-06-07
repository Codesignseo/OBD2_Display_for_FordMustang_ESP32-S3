// The OBD2 connector has an always-on 12V pin where the device will be powered from. Since it's always on, it will unnecessary draw power when the car
// is turned off. One option would be to just always unplug the device when not driving, another might be to add a button to the device to manually
// switch it on/off. In this project, we simply detect if the car is turned on and if not, we put the device into a lite sleep mode by reducing the
// processor clock speed to 80Mhz and switching off the display. For some reason switching in and out of actual deep sleep mode drains the car's battery,
// it's probably waking up some electronic components in the car? When we detect that the car is on, the processor is switched back to 240Hz and the
// dispay is switched on again.
//
// Running at 240MHz with the display on = 100mA @ 5.12V
// Running at 80Mhz with display off = 30mA @ 5.12V
// Running at 40Mhz with display off = less than 10mA @ 5.12V, BUT drains battery
// Deepsleep with display off = ~0mA @ 5.12V, BUT drains battery

#ifndef _HANDLE_LOW_POWER_STATE
#define _HANDLE_LOW_POWER_STATE

// CPU clock speeds in MHz
const uint32_t ClockSpeed_Normal = 240;
const uint32_t ClockSpeed_LiteSleep = 80;

void LiteSleep()
{
#ifdef DISABLE_POWER_SAVING
  return;
#endif
  
  DebugPrintln("Going into lite sleep");
  digitalWrite(LED_BUILTIN, HIGH);   // Turn on-board LED off

  // Stop the thread that's updating the display
  if (g_TaskDisplayInfo)
  {
    vTaskSuspend(g_TaskDisplayInfo);  // Suspend the thread that's updating the display
    TurnDisplayOff();                 // Clear the display and turn it off
    vTaskDelete(g_TaskDisplayInfo);   // Delete the thread that's updating the display
    g_TaskDisplayInfo = nullptr;
  }

#ifdef DEBUG
  Serial.flush();
#endif

  // Go into lite sleep
  setCpuFrequencyMhz(ClockSpeed_LiteSleep);
  DebugPrintf("Clock frequency = %d\n", getCpuFrequencyMhz());
}

// It's a good idea to reboot the device into a clean state and just start fresh, especially since we're working with multiple threads
void Reboot()
{
#ifdef DISABLE_POWER_SAVING
  return;
#endif

  DebugPrintln("Rebooting device into clean state");

#ifdef DEBUG
  Serial.flush();
#endif

  ESP.restart();
}

void WaitForCarToTurnOn()
{
#ifdef DISABLE_POWER_SAVING
  return;
#endif

  // We can be in lite sleep and still monitor incoming CAN frames
  LiteSleep();

  DebugPrintln("Waiting for car to turn on");

  while (true)
  {
    // Blink to indicate we're in lite sleep
    digitalWrite(LED_BUILTIN, LOW);   // Turn on-board LED on

    // When the car is on, there could be hundreds of CAN frames per second. Therefore we inspect frames for 1 second
    auto startTime = millis();
    while ((millis() - startTime) < 1000)
    {
      CollectCarData();

      // It's good enough for this project to check RPM to determine if the car is turned on
      if (g_EngineRPM > 0)
      {
        DebugPrintln("Car turned ON");

        setCpuFrequencyMhz(ClockSpeed_Normal);
        digitalWrite(LED_BUILTIN, LOW); // Turn on-board LED on
        DebugPrintf("Clock frequency = %d\n", getCpuFrequencyMhz());

        return;
      }
    }

    digitalWrite(LED_BUILTIN, HIGH);  // Turn on-board LED off
    delay(3000);
  }
}

void CheckIfCarIsStillOn()
{
#ifdef DISABLE_POWER_SAVING
  return;
#endif

  if (g_CurrentCarData.EngineRPM <= 0)
  {
    DebugPrintln("Car turned OFF");
    TurnDisplayOff();
    LiteSleep();
    Reboot();
  }
}

#endif  // _HANDLE_LOW_POWER_STATE