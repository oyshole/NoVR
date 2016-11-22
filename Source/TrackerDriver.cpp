#include "TrackerDriver.h"

TrackerDriver::TrackerDriver(vr::IServerDriverHost * pDriverHost, int trackerIndex) :
  DeviceDriver(pDriverHost),
  trackerIndex(trackerIndex)
{
  serialNumber = "CogsVRTracker" + std::to_string(trackerIndex);
  modelNumber = "Cogs Virtual Tracking Station";
}

EVRInitError TrackerDriver::Activate(uint32_t unObjectId)
{
  logTrace("TrackerDriver::Activate(%d)", unObjectId);

  objectId = unObjectId;

  return VRInitError_None;
}

void TrackerDriver::RunFrame()
{
  logTrace("TrackerDriver::RunFrame()");

  if (objectId != vr::k_unTrackedDeviceIndexInvalid) {
    host->TrackedDevicePoseUpdated(objectId, GetPose());
  }
}

void TrackerDriver::Deactivate()
{
  logTrace("TrackerDriver::Deactivate()");
  objectId = vr::k_unTrackedDeviceIndexInvalid;
}

void TrackerDriver::EnterStandby()
{
  logTrace("TrackerDriver::EnterStandby()");
}

void * TrackerDriver::GetComponent(const char * pchComponentNameAndVersion)
{
  logTrace("TrackerDriver::GetComponent(%s)", pchComponentNameAndVersion);

  return nullptr;
}

void TrackerDriver::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
  logTrace("TrackerDriver::DebugRequest()");
}

bool TrackerDriver::GetBoolTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("TrackerDriver::GetBoolTrackedDeviceProperty(%d)", prop);

  *pError = vr::TrackedProp_Success;

  switch (prop)
  {
  case vr::Prop_DeviceIsWireless_Bool:
    return true;
  }

  *pError = vr::TrackedProp_ValueNotProvidedByDevice;

  return false;
}

float TrackerDriver::GetFloatTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("TrackerDriver::GetFloatTrackedDeviceProperty(%d)", prop);

  vr::ETrackedPropertyError error = vr::TrackedProp_ValueNotProvidedByDevice;
  float fRetVal = 0;
  switch (prop)
  {
  case vr::Prop_DeviceBatteryPercentage_Float:
    error = vr::TrackedProp_Success;
    fRetVal = 100.f;
    break;
  }

  if (pError)
  {
    *pError = error;
  }

  return fRetVal;
}

int32_t TrackerDriver::GetInt32TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("TrackerDriver::GetInt32TrackedDeviceProperty(%d)", prop);

  int32_t nRetVal = 0;
  vr::ETrackedPropertyError error = vr::TrackedProp_UnknownProperty;

  switch (prop)
  {
  case vr::Prop_DeviceClass_Int32:
    nRetVal = vr::TrackedDeviceClass_TrackingReference;
    error = vr::TrackedProp_Success;
    break;
  }

  if (pError) {
    *pError = error;
  }

  return nRetVal;
}

uint64_t TrackerDriver::GetUint64TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("TrackerDriver::GetUint64TrackedDeviceProperty(%d)", prop);

  *pError = vr::TrackedProp_Success;

  switch (prop) {
  case vr::Prop_CurrentUniverseId_Uint64:
    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    return 2;
  }

  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  return 0;
}

HmdMatrix34_t TrackerDriver::GetMatrix34TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  HmdMatrix34_t matIdentity;
  HmdMatrix_SetIdentity(&matIdentity);
  return matIdentity;
}

uint32_t TrackerDriver::GetStringTrackedDeviceProperty(ETrackedDeviceProperty prop, char * pchValue, uint32_t unBufferSize, ETrackedPropertyError * pError)
{
  logTrace("TrackerDriver::GetStringTrackedDeviceProperty(%d)", prop);

  std::string sValue = GetStringTrackedDeviceProperty(prop, pError);

  if (*pError == vr::TrackedProp_Success) {
    if (sValue.size() + 1 > unBufferSize)
    {
      *pError = vr::TrackedProp_BufferTooSmall;
    } else
    {
      strcpy_s(pchValue, unBufferSize, sValue.c_str());
    }
    return (uint32_t)sValue.size() + 1;
  }
  return 0;
}

std::string TrackerDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
{
  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  std::string sRetVal;

  switch (prop)
  {
  case vr::Prop_ModelNumber_String:
    sRetVal = modelNumber;
    *pError = vr::TrackedProp_Success;
    break;

  case vr::Prop_SerialNumber_String:
    sRetVal = serialNumber;
    *pError = vr::TrackedProp_Success;
    break;

  case vr::Prop_RenderModelName_String:
    sRetVal = "lh_basestation_vive";
    *pError = vr::TrackedProp_Success;
    break;
  }

  return sRetVal;
}

DriverPose_t TrackerDriver::GetPose()
{
  DriverPose_t pose = {};
  pose.poseIsValid = true;
  pose.result = TrackingResult_Running_OK;
  pose.deviceIsConnected = true;

  pose.qWorldFromDriverRotation = toHmd(glm::quat());
  pose.qDriverFromHeadRotation = toHmd(glm::quat());

  glm::dquat rotation = glm::angleAxis(0.0, glm::dvec3(1, 0, 0));

  pose.qRotation = toHmd(rotation);

  glm::dvec3 position = hasMessage ? message.position : glm::dvec3(-1 + trackerIndex, 2, -2);

  pose.vecPosition[0] = position.x;
  pose.vecPosition[1] = position.y;
  pose.vecPosition[2] = position.z;

  return pose;
}
