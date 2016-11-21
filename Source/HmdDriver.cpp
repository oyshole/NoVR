#include "HmdDriver.h"

HmdDriver::HmdDriver(vr::IServerDriverHost * pDriverHost) :
  DeviceDriver(pDriverHost)
{
  serialNumber = "CogsVR1";
  modelNumber = "Cogs Virtual HMD";

  windowX = 100;
  windowY = 100;
  windowWidth = 1920;
  windowHeight = 1080;
  renderWidth = 1344;
  renderHeight = 1512;
  secondsFromVsyncToPhotons = 0.01111111f;
  displayFrequency = 90.0;
  ipd = 0.065f;
}

EVRInitError HmdDriver::Activate(uint32_t unObjectId)
{
  logTrace("HmdDriver::Activate()");

  objectId = unObjectId;

  return VRInitError_None;
}

void HmdDriver::RunFrame()
{
  logTrace("HmdDriver::RunFrame()");

  if (objectId != vr::k_unTrackedDeviceIndexInvalid) {
    host->TrackedDevicePoseUpdated(objectId, GetPose());
  }
}

void HmdDriver::Deactivate()
{
  logTrace("HmdDriver::Deactivate()");

  objectId = vr::k_unTrackedDeviceIndexInvalid;
}

void HmdDriver::EnterStandby()
{
  logTrace("HmdDriver::EnterStandby()");
}

void * HmdDriver::GetComponent(const char * pchComponentNameAndVersion)
{
  logTrace("HmdDriver::GetComponent()");

  if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
  {
    return (vr::IVRDisplayComponent*)this;
  }

  return nullptr;
}

void HmdDriver::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
  logTrace("HmdDriver::DebugRequest()");
}

bool HmdDriver::GetBoolTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  *pError = vr::TrackedProp_Success;

  switch (prop)
  {
  case vr::Prop_IsOnDesktop_Bool:
    // Avoid "not fullscreen" warnings from vrmonitor
    return false;
  }

  *pError = vr::TrackedProp_ValueNotProvidedByDevice;

  return false;
}

float HmdDriver::GetFloatTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("HmdDriver::GetFloatTrackedDeviceProperty(%d)", prop);

  vr::ETrackedPropertyError error = vr::TrackedProp_ValueNotProvidedByDevice;
  float fRetVal = 0;
  switch (prop)
  {
  case vr::Prop_UserIpdMeters_Float:
    fRetVal = ipd;
    error = TrackedProp_Success;
    break;

  case vr::Prop_UserHeadToEyeDepthMeters_Float:
    error = vr::TrackedProp_Success;
    fRetVal = 0.f;
    break;

  case vr::Prop_DisplayFrequency_Float:
    error = vr::TrackedProp_Success;
    fRetVal = displayFrequency;
    break;

  case vr::Prop_SecondsFromVsyncToPhotons_Float:
    error = vr::TrackedProp_Success;
    fRetVal = secondsFromVsyncToPhotons;
    break;

  }

  if (pError)
  {
    *pError = error;
  }

  return fRetVal;
}

int32_t HmdDriver::GetInt32TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  int32_t nRetVal = 0;
  vr::ETrackedPropertyError error = vr::TrackedProp_UnknownProperty;

  switch (prop)
  {
  case vr::Prop_DeviceClass_Int32:
    nRetVal = vr::TrackedDeviceClass_HMD;
    error = vr::TrackedProp_Success;
    break;
  }

  if (pError)
  {
    *pError = error;
  }

  return nRetVal;
}

uint64_t HmdDriver::GetUint64TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  *pError = vr::TrackedProp_Success;

  switch (prop)
  {
  case vr::Prop_CurrentUniverseId_Uint64:
    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    return 2;
  }

  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  return 0;
}

HmdMatrix34_t HmdDriver::GetMatrix34TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  HmdMatrix34_t matIdentity;
  HmdMatrix_SetIdentity(&matIdentity);
  return matIdentity;
}

uint32_t HmdDriver::GetStringTrackedDeviceProperty(ETrackedDeviceProperty prop, char * pchValue, uint32_t unBufferSize, ETrackedPropertyError * pError)
{
  std::string sValue = GetStringTrackedDeviceProperty(prop, pError);
  
  if (*pError == vr::TrackedProp_Success) {
    if (sValue.size() + 1 > unBufferSize) {
      *pError = vr::TrackedProp_BufferTooSmall;
    } else {
      strcpy_s(pchValue, unBufferSize, sValue.c_str());
    }

    return (uint32_t)sValue.size() + 1;
  }

  return 0;
}

std::string HmdDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  std::string sRetVal;

  switch (prop) {
  case vr::Prop_ModelNumber_String:
    sRetVal = modelNumber;
    *pError = vr::TrackedProp_Success;
    break;

  case vr::Prop_SerialNumber_String:
    sRetVal = serialNumber;
    *pError = vr::TrackedProp_Success;
    break;
  }

  return sRetVal;
}

void HmdDriver::GetWindowBounds(int32_t * pnX, int32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
  *pnX = windowX;
  *pnY = windowY;
  *pnWidth = windowWidth;
  *pnHeight = windowHeight;
}

void HmdDriver::GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight)
{
  *pnWidth = renderWidth;
  *pnHeight = renderHeight;
}

void HmdDriver::GetEyeOutputViewport(EVREye eEye, uint32_t * pnX, uint32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
  *pnY = 0;
  *pnWidth = windowWidth / 2;
  *pnHeight = windowHeight;

  if (eEye == Eye_Left) {
    *pnX = 0;
  } else {
    *pnX = windowWidth / 2;
  }
}

void HmdDriver::GetProjectionRaw(EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom)
{
  *pfLeft = -1.0;
  *pfRight = 1.0;
  *pfTop = -1.0;
  *pfBottom = 1.0;
}

DistortionCoordinates_t HmdDriver::ComputeDistortion(EVREye eEye, float fU, float fV)
{
  DistortionCoordinates_t coordinates = {};
  coordinates.rfBlue[0] = fU;
  coordinates.rfBlue[1] = fV;
  coordinates.rfGreen[0] = fU;
  coordinates.rfGreen[1] = fV;
  coordinates.rfRed[0] = fU;
  coordinates.rfRed[1] = fV;
  return coordinates;
}

DriverPose_t HmdDriver::GetPose()
{
  DriverPose_t pose = {};
  pose.poseIsValid = true;
  pose.result = TrackingResult_Running_OK;
  pose.deviceIsConnected = true;

  auto t = timer.elapsedSeconds();

  glm::quat headRotation = glm::angleAxis(glm::sin((float)t * 0.5f) * 0.3f, glm::vec3(0, 1, 0)) *
    glm::angleAxis(-0.2f + glm::sin((float)t * 0.3f) * 0.1f, glm::vec3(1, 0, 0));

  pose.qRotation = toHmd(headRotation);

  pose.qWorldFromDriverRotation = toHmd(glm::quat());
  pose.qDriverFromHeadRotation = toHmd(glm::quat());

  if (!hasMessage) {
    pose.vecPosition[0] = 0.1 * sin(t * 1.76);
  } else {
    pose.vecPosition[0] = message.position.x;
  }

  pose.vecPosition[1] = 1.8 + 0.1 * sin(t * 0.2);
  pose.vecPosition[2] = 0;

  return pose;
}
