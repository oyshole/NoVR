#include "ServerProvider.h"
#include "ClientProvider.h"

std::unique_ptr<Log> logger;

namespace
{
  ServerTrackedDeviceProvider serverProvider;
  ClientTrackedDeviceProvider clientProvider;
}

#define HMD_DLL_EXPORT extern "C" __declspec(dllexport)

HMD_DLL_EXPORT void * HmdDriverFactory(const char * pInterfaceName, int * pReturnCode)
{
  if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName)) {
    return &serverProvider;
  }

  if (0 == strcmp(IClientTrackedDeviceProvider_Version, pInterfaceName)) {
    return &clientProvider;
  }

  if (pReturnCode) {
    *pReturnCode = VRInitError_Unknown;
  }

  return nullptr;
}
