#include "Settings.h"

Settings settings;

namespace
{
  constexpr char * kSection = "novr";

  constexpr char * kEnabled = "enabled";

  constexpr char * kNumTrackingStations = "numTrackingStations";
}

void Settings::update(vr::IVRSettings * pSettings)
{
  logTrace("Settings::update()");

  if (!pSettings) return;

  logDebug("Updating settings...");

  EVRSettingsError error;
  enabled = pSettings->GetBool(kSection, kEnabled, &error);

  if (error != VRSettingsError_None) {
    logWarning("Settings error: %s", pSettings->GetSettingsErrorNameFromEnum(error));
  }

  numTrackingStations = pSettings->GetInt32(kSection, kNumTrackingStations);

  logDebug("Setting enabled: %s", enabled ? "true" : "false");
}
