#pragma once

#include "Common.h"

class Settings
{
public:
  void update(vr::IVRSettings * pSettings);

  bool enabled = false;
  int numTrackingStations = 0;
};

extern Settings settings;
