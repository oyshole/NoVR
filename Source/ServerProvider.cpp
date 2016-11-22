#include "ServerProvider.h"

#include "Settings.h"

class TrackingWorker
{
public:
  TrackingWorker(ServerTrackedDeviceProvider * server) : server(server) {}

  HANDLE hPipe;

  void operator()()
  {
    logTrace("Starting tracking thread.");

    hPipe = CreateNamedPipe(
      "\\\\.\\pipe\\driver_sim",
      PIPE_ACCESS_DUPLEX,
      PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_READMODE_BYTE,
      1,
      2048,
      2048,
      0,
      0
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
      logWarning("Could not create pipe.");
    }

    logDebug("Pipe object created.");
    logDebug("Pipe waiting for client...");

    if (!ConnectNamedPipe(hPipe, nullptr)) {
      logWarning("Could not connect pipe.");
      return;
    }

    logDebug("Client connected to pipe. Waiting for messages...");

    while (!done) {
      uint8_t buffer[2048];
      DWORD readBytes = 0;
      ::ReadFile(hPipe, buffer, sizeof(TrackingMessage), &readBytes, nullptr);

      logTrace("Received message with size %d", readBytes);

      if (readBytes != sizeof(TrackingMessage)) {
        logWarning("Wrong message size. Dropping pipe connection.");
        
        if (!Reconnect()) return;

        continue;
      }

      auto & message = *reinterpret_cast<const TrackingMessage *>(buffer);

      if (message.checksum != 0xBAADF00D) {
        logWarning("Wrong message checksum: %d", message.checksum);
        continue;
      }

      if (message.targetId == -1 || readBytes != sizeof(TrackingMessage)) {
        logDebug("Received disconnect message.");

        if (!Reconnect()) return;

        continue;
      }

      std::lock_guard<std::mutex> lock(server->qMutex);
      server->queue.push(message);
      server->qVariable.notify_one();
    }
  }

  bool Reconnect()
  {
    if (!DisconnectNamedPipe(hPipe)) {
      logWarning("Could not disconnect pipe.");
      return false;
    }

    logDebug("Disconnected from pipe client.");

    if (!ConnectNamedPipe(hPipe, nullptr)) {
      logWarning("Could not connect pipe.");
      return false;
    }

    logDebug("Connected to new pipe client.");

    return true;
  }

  bool done = false;
  ServerTrackedDeviceProvider * server;
};

EVRInitError ServerTrackedDeviceProvider::Init(IDriverLog * pDriverLog, vr::IServerDriverHost * pDriverHost, const char * pchUserDriverConfigDir, const char * pchDriverInstallDir)
{
  if (!logger) { logger = std::make_unique<Log>(pDriverLog); }

  logTrace("ServerTrackedDeviceProvider::Init()");

  if (pDriverHost) {
    settings.update(pDriverHost->GetSettings(vr::IVRSettings_Version));
  } else {
    logDebug("No host.");
  }

  if (!settings.enabled) {
    logDebug("Returning VRInitError_Init_HmdNotFound.");
    return VRInitError_Init_HmdNotFound;
  }

  hmd = std::make_unique<HmdDriver>(pDriverHost);
  left = std::make_unique<ControllerDriver>(pDriverHost, true);
  right = std::make_unique<ControllerDriver>(pDriverHost, false);

  devices.push_back(hmd.get());
  devices.push_back(left.get());
  devices.push_back(right.get());

  if (settings.numTrackingStations) {
    logDebug("Creating %d tracking stations.");

    for (int i = 0; i < settings.numTrackingStations; ++i) {
      trackers.emplace_back(std::make_unique<TrackerDriver>(pDriverHost, i));

      devices.push_back(trackers.back().get());
    }
  }

  TrackingWorker worker(this);
  trackingThread = std::thread(worker);

  return VRInitError_None;
}

void ServerTrackedDeviceProvider::Cleanup()
{
  logTrace("ServerTrackedDeviceProvider::Cleanup()");

  if (trackingThread.joinable()) trackingThread.join();

  hmd.release();
  left.release();
  right.release();

  trackers.clear();
}

const char * const * ServerTrackedDeviceProvider::GetInterfaceVersions()
{
  logTrace("ServerTrackedDeviceProvider::GetInterfaceVersions()");

  return vr::k_InterfaceVersions;
}

uint32_t ServerTrackedDeviceProvider::GetTrackedDeviceCount()
{
  logTrace("ServerTrackedDeviceProvider::GetInterfaceVersions()");

  return static_cast<uint32_t>(devices.size());
}

ITrackedDeviceServerDriver * ServerTrackedDeviceProvider::GetTrackedDeviceDriver(uint32_t unWhich)
{
  logTrace("ServerTrackedDeviceProvider::GetTrackedDeviceDriver(%d)", unWhich);

  if (unWhich == -1 || unWhich >= devices.size()) return nullptr;

  return devices[unWhich];
}

ITrackedDeviceServerDriver * ServerTrackedDeviceProvider::FindTrackedDeviceDriver(const char * pchId)
{
  logTrace("ServerTrackedDeviceProvider::FindTrackedDeviceDriver(%s)", pchId);

  for (auto & d : devices) {
    if (d->HasIdentity(pchId)) return d;
  }

  return nullptr;
}

void ServerTrackedDeviceProvider::RunFrame()
{
  logTrace("ServerTrackedDeviceProvider::RunFrame(ThreadId:%d)", std::this_thread::get_id());

  if (devices.size()) {
    {
      std::lock_guard<std::mutex> lock(qMutex);

      while (!queue.empty()) {
        auto message = queue.front();
        queue.pop();

        for (auto d : devices) {
          if (d->GetId() == message.targetId) {
            d->Post(message);
          }
        }
      }
    }

    for (auto d : devices) {
      d->RunFrame();
    }
  }
}

bool ServerTrackedDeviceProvider::ShouldBlockStandbyMode()
{
  logTrace("ServerTrackedDeviceProvider::ShouldBlockStandbyMode()");

  return false;
}

void ServerTrackedDeviceProvider::EnterStandby()
{
  logTrace("ServerTrackedDeviceProvider::EnterStandby()");
}

void ServerTrackedDeviceProvider::LeaveStandby()
{
  logTrace("ServerTrackedDeviceProvider::LeaveStandby()");
}
