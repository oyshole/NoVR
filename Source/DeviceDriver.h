#pragma once

#include "Common.h"
#include "Timer.h"

class DeviceDriver
{
public:
  DeviceDriver(vr::IServerDriverHost * host);
  virtual ~DeviceDriver() {}

  bool HasIdentity(const char * id) { return serialNumber == id; }

  TrackingMessage message;
  bool hasMessage = false;

protected:
  vr::IServerDriverHost * host;
  uint32_t objectId;

  std::string serialNumber;
  std::string modelNumber;

  Timer timer = Timer::startNew();
};
