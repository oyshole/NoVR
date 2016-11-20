#pragma once

#include <stdio.h>
#include <cstring>
#include <string>
#include <thread>
#include <memory>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <openvr_driver.h>

using namespace vr;

inline HmdQuaternion_t toHmd(const glm::quat & q)
{
  HmdQuaternion_t quat;
  quat.w = q.w;
  quat.x = q.x;
  quat.y = q.y;
  quat.z = q.z;
  return quat;
}

inline void HmdMatrix_SetIdentity(HmdMatrix34_t * pMatrix)
{
  *pMatrix = *reinterpret_cast<HmdMatrix34_t *>(&glm::mat4x3(1.0f));
}

struct TrackingMessage
{
  uint32_t checksum;
  int targetId;
  glm::dvec3 position;
  glm::dquat rotation;
  int trigger;
};

extern std::unique_ptr<class Log> logger;

class Log
{
public:
  Log(vr::IDriverLog * pDriverLog) : driverLog(pDriverLog)
  {
    print("Initialized log");
  }

  ~Log()
  {
    print("Shutdown log");
  }

  void logVar(const char *pMsgFormat, va_list args)
  {
    char buffer[16384];

#ifdef _WIN32
    int pos = vsprintf_s(buffer, pMsgFormat, args);
    sprintf_s(buffer + pos, 16384 - pos, "\n");
#else
    vsnprintf(buffer, sizeof(buffer), pMsgFormat, args);
#endif

    if (driverLog) {
      driverLog->Log(buffer);
    }
  }

  void log(const char *pMsgFormat, ...)
  {
    va_list args;
    va_start(args, pMsgFormat);

    logVar(pMsgFormat, args);

    va_end(args);
  }

  template<typename... Args>
  void print(const char * text, Args... args)
  {
    log(text, args...);
  }

private:
  vr::IDriverLog * driverLog = nullptr;
};

template<typename... Args>
void logDebug(const char * text, Args... args)
{
  if (logger) {
    logger->print(text, args...);
  }
}
