#include "DeviceDriver.h"

DeviceDriver::DeviceDriver(vr::IServerDriverHost * host) :
  host(host),
  objectId(vr::k_unTrackedDeviceIndexInvalid)
{

}
