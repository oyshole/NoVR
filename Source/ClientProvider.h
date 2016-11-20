#pragma once

#include "Common.h"

class ClientTrackedDeviceProvider : public vr::IClientTrackedDeviceProvider
{
public:
  EVRInitError Init(EClientDriverMode eDriverMode, IDriverLog * pDriverLog, vr::IClientDriverHost * pDriverHost, const char * pchUserDriverConfigDir, const char * pchDriverInstallDir) override;
  void Cleanup() override;
  bool BIsHmdPresent(const char * pchUserConfigDir) override;
  EVRInitError SetDisplayId(const char * pchDisplayId) override;
  HiddenAreaMesh_t GetHiddenAreaMesh(EVREye eEye, EHiddenAreaMeshType type) override;
  uint32_t GetMCImage(uint32_t * pImgWidth, uint32_t * pImgHeight, uint32_t * pChannels, void * pDataBuffer, uint32_t unBufferLen) override;

private:
  vr::IClientDriverHost * host;
  vr::EClientDriverMode driverMode;
};
