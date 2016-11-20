#pragma once

#include "DeviceDriver.h"

class HmdDriver : public DeviceDriver, public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent
{
public:
  HmdDriver(vr::IServerDriverHost * pDriverHost);
  
  EVRInitError Activate(uint32_t unObjectId) override;
  void Deactivate() override;
  void EnterStandby() override;
  void * GetComponent(const char * pchComponentNameAndVersion) override;
  void DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize) override;
  bool GetBoolTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError) override;
  float GetFloatTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError) override;
  int32_t GetInt32TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError) override;
  uint64_t GetUint64TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError) override;
  HmdMatrix34_t GetMatrix34TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError) override;
  uint32_t GetStringTrackedDeviceProperty(ETrackedDeviceProperty prop, char * pchValue, uint32_t unBufferSize, ETrackedPropertyError * pError) override;
  std::string GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError);
  
  void RunFrame();

  // IVRDisplayComponent
  bool IsDisplayOnDesktop() override { return true; }
  bool IsDisplayRealDisplay() override { return false; }
  void GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight) override;
  void GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight);
  void GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight) override;
  void GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom) override;
  DistortionCoordinates_t ComputeDistortion(EVREye eEye, float fU, float fV) override;
  DriverPose_t GetPose() override;

private:
  int32_t windowX;
  int32_t windowY;
  int32_t windowWidth;
  int32_t windowHeight;
  int32_t renderWidth;
  int32_t renderHeight;
  float secondsFromVsyncToPhotons;
  float displayFrequency;
  float ipd;
};
