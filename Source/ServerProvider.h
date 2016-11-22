#include "HmdDriver.h"
#include "ControllerDriver.h"
#include "TrackerDriver.h"

#include <queue>
#include <mutex>
#include <condition_variable>

class ServerTrackedDeviceProvider : public vr::IServerTrackedDeviceProvider
{
public:
  EVRInitError Init(IDriverLog * pDriverLog, vr::IServerDriverHost * pDriverHost, const char * pchUserDriverConfigDir, const char * pchDriverInstallDir) override;
  void Cleanup() override;
  const char * const * GetInterfaceVersions() override;
  uint32_t GetTrackedDeviceCount() override;
  ITrackedDeviceServerDriver * GetTrackedDeviceDriver(uint32_t unWhich) override;
  ITrackedDeviceServerDriver * FindTrackedDeviceDriver(const char * pchId) override;
  void RunFrame() override;
  bool ShouldBlockStandbyMode() override;
  void EnterStandby() override;
  void LeaveStandby() override;

private:
  std::unique_ptr<HmdDriver> hmd;
  std::unique_ptr<ControllerDriver> left;
  std::unique_ptr<ControllerDriver> right;
  std::vector<std::unique_ptr<TrackerDriver>> trackers;

  std::vector<DeviceDriver *> devices;

  std::thread trackingThread;

public:
  std::queue<TrackingMessage> queue;
  std::mutex qMutex;
  std::condition_variable qVariable;
};
