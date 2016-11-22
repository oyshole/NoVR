#pragma once

#include "Common.h"
#include "Timer.h"

class DeviceDriver : public vr::ITrackedDeviceServerDriver
{
public:
  DeviceDriver(vr::IServerDriverHost * host);
  virtual ~DeviceDriver() {}

  bool HasIdentity(const char * id) { return serialNumber == id; }

  uint32_t GetId() const { return objectId; }
  void Post(TrackingMessage message) { this->message = message; hasMessage = true; }

  virtual void RunFrame() = 0;

protected:
  vr::IServerDriverHost * host;
  uint32_t objectId;

  std::string serialNumber;
  std::string modelNumber;

  TrackingMessage message;
  bool hasMessage = false;

  Timer timer = Timer::startNew();
};
