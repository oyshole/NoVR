#include "ServerProvider.h"

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

  hmd = std::make_unique<HmdDriver>(pDriverHost);
  left = std::make_unique<ControllerDriver>(pDriverHost, true);
  right = std::make_unique<ControllerDriver>(pDriverHost, false);

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
}

const char * const * ServerTrackedDeviceProvider::GetInterfaceVersions()
{
  logTrace("ServerTrackedDeviceProvider::GetInterfaceVersions()");

  return vr::k_InterfaceVersions;
}

uint32_t ServerTrackedDeviceProvider::GetTrackedDeviceCount()
{
  logTrace("ServerTrackedDeviceProvider::GetInterfaceVersions()");

  return 3;
}

ITrackedDeviceServerDriver * ServerTrackedDeviceProvider::GetTrackedDeviceDriver(uint32_t unWhich)
{
  logTrace("ServerTrackedDeviceProvider::GetTrackedDeviceDriver(%d)", unWhich);

  if (unWhich == 0) {
    return hmd.get();
  } else if (unWhich == 1) {
    return left.get();
  } else if (unWhich == 2) {
    return right.get();
  }

  return nullptr;
}

ITrackedDeviceServerDriver * ServerTrackedDeviceProvider::FindTrackedDeviceDriver(const char * pchId)
{
  logTrace("ServerTrackedDeviceProvider::FindTrackedDeviceDriver(%s)", pchId);

  if (left->HasIdentity(pchId)) return left.get();
  if (right->HasIdentity(pchId)) return right.get();
  if (hmd->HasIdentity(pchId)) return hmd.get();

  return nullptr;
}

void ServerTrackedDeviceProvider::RunFrame()
{
  logTrace("ServerTrackedDeviceProvider::RunFrame(ThreadId:%d)", std::this_thread::get_id());

  if (hmd) {
    {
      std::lock_guard<std::mutex> lock(qMutex);

      while (!queue.empty()) {
        auto message = queue.front();
        queue.pop();

        if (message.targetId == 0) {
          hmd->message = message;
          hmd->hasMessage = true;
        } else if (message.targetId == 1) {
          left->message = message;
          left->hasMessage = true;
        } else if (message.targetId == 2) {
          right->message = message;
          right->hasMessage = true;
        }
      }
    }

    hmd->RunFrame();
    left->RunFrame();
    right->RunFrame();
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
