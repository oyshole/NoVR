#include "ControllerDriver.h"

ControllerDriver::ControllerDriver(vr::IServerDriverHost * pDriverHost, bool left) :
  DeviceDriver(pDriverHost),
  left(left)
{
  serialNumber = "CogsVRController" + std::to_string(left ? 0 : 1);
  modelNumber = "Cogs Virtual Controller";
}

EVRInitError ControllerDriver::Activate(uint32_t unObjectId)
{
  logTrace("ControllerDriver::Activate(%d)", unObjectId);

  objectId = unObjectId;

  return VRInitError_None;
}

void ControllerDriver::RunFrame()
{
  logTrace("ControllerDriver::RunFrame()");

  if (objectId != vr::k_unTrackedDeviceIndexInvalid) {
    host->TrackedDevicePoseUpdated(objectId, GetPose());
    
    for (uint32_t i = 0; i < k_unControllerStateAxisCount; ++i) {
      host->TrackedDeviceAxisUpdated(objectId, i, GetControllerState().rAxis[i]);
    }
  }
}

VRControllerState_t ControllerDriver::GetControllerState()
{
  logTrace("ControllerDriver::GetControllerState()");

  VRControllerState_t state = {};

  static uint32_t packet = 0;
  state.unPacketNum = packet++;

  double t = timer.elapsedSeconds();
  double dSec;
  double frac = modf(t, &dSec);

  int sec = static_cast<int>(dSec);

  for (uint32_t i = 0; i < k_unControllerStateAxisCount; ++i) {
    state.rAxis[i].x = (sec % 3 == 0 && frac < 0.5) ? 1.0f : 0.0f;
  }

  return state;
}

bool ControllerDriver::TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds)
{
  return false;
}

void ControllerDriver::Deactivate()
{
  logTrace("ControllerDriver::Deactivate()");
  objectId = vr::k_unTrackedDeviceIndexInvalid;
}

void ControllerDriver::EnterStandby()
{
  logTrace("ControllerDriver::EnterStandby()");
}

void * ControllerDriver::GetComponent(const char * pchComponentNameAndVersion)
{
  logTrace("ControllerDriver::GetComponent(%s)", pchComponentNameAndVersion);

  if (!_stricmp(pchComponentNameAndVersion, vr::IVRControllerComponent_Version)) {
    logTrace("ControllerDriver::ReturnComponent", pchComponentNameAndVersion);
    return (vr::IVRControllerComponent *)this;
  }

  // override this to add a component to a driver
  return nullptr;
}

void ControllerDriver::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
  logTrace("ControllerDriver::DebugRequest()");
}

bool ControllerDriver::GetBoolTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("ControllerDriver::GetBoolTrackedDeviceProperty(%d)", prop);

  *pError = vr::TrackedProp_Success;

  switch (prop)
  {
  case vr::Prop_DeviceIsWireless_Bool:
    return true;
  }

  *pError = vr::TrackedProp_ValueNotProvidedByDevice;

  return false;
}

float ControllerDriver::GetFloatTrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("ControllerDriver::GetFloatTrackedDeviceProperty(%d)", prop);

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

int32_t ControllerDriver::GetInt32TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("ControllerDriver::GetInt32TrackedDeviceProperty(%d)", prop);

  int32_t nRetVal = 0;
  vr::ETrackedPropertyError error = vr::TrackedProp_UnknownProperty;

  switch (prop)
  {
  case vr::Prop_DeviceClass_Int32:
    nRetVal = vr::TrackedDeviceClass_Controller;
    error = vr::TrackedProp_Success;
    break;

  case vr::Prop_ControllerRoleHint_Int32:
    nRetVal = left ? vr::TrackedControllerRole_LeftHand : vr::TrackedControllerRole_RightHand;
    error = vr::TrackedProp_Success;
    break;
  }

  if (pError) {
    *pError = error;
  }

  return nRetVal;
}

uint64_t ControllerDriver::GetUint64TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  logTrace("ControllerDriver::GetUint64TrackedDeviceProperty(%d)", prop);

  *pError = vr::TrackedProp_Success;

  switch (prop) {
  case vr::Prop_CurrentUniverseId_Uint64:
    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    return 2;
  }

  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  return 0;
}

HmdMatrix34_t ControllerDriver::GetMatrix34TrackedDeviceProperty(ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
  *pError = vr::TrackedProp_ValueNotProvidedByDevice;
  HmdMatrix34_t matIdentity;
  HmdMatrix_SetIdentity(&matIdentity);
  return matIdentity;
}

uint32_t ControllerDriver::GetStringTrackedDeviceProperty(ETrackedDeviceProperty prop, char * pchValue, uint32_t unBufferSize, ETrackedPropertyError * pError)
{
  logTrace("ControllerDriver::GetStringTrackedDeviceProperty(%d)", prop);

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

std::string ControllerDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError)
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
    sRetVal = "vr_controller_vive_1_5";
    *pError = vr::TrackedProp_Success;
    break;
  }

  return sRetVal;
}

DriverPose_t ControllerDriver::GetPose()
{
  DriverPose_t pose = {};
  pose.poseIsValid = true;
  pose.result = TrackingResult_Running_OK;
  pose.deviceIsConnected = true;

  pose.qWorldFromDriverRotation = toHmd(glm::quat());
  pose.qDriverFromHeadRotation = toHmd(glm::quat());

  auto t = timer.elapsedSeconds();

  glm::dquat headRotation = glm::angleAxis(glm::sin(t * 0.5) * 0.0, glm::dvec3(0, 1, 0));

  double xOffset = left ? -0.3 : 0.3;
  glm::dvec3 offset = glm::dvec3(
    xOffset + 0.01 * sin(t * 1.76),
    1.5 + 0.01 * sin(t * 1.0 + (left ? 0.7 : 0)),
    -0.5f
  );

  glm::dquat rotation = glm::angleAxis(glm::sin(t * 11.0) * 0.02, glm::dvec3(1, 0, 0));

  pose.qRotation = toHmd(headRotation * rotation);

  glm::dvec3 position = headRotation * offset;

  if (!hasMessage) {
    pose.vecPosition[0] = position.x;
  } else {
    pose.vecPosition[0] = message.position.x;
  }
  pose.vecPosition[1] = position.y;
  pose.vecPosition[2] = position.z;

  return pose;
}
