#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

class Timer
{
public:
  Timer() : running(false)
  {
    startTime.QuadPart = stopTime.QuadPart = 0;
  }

  static Timer startNew()
  {
    Timer timer;
    timer.start();
    return timer;
  }

  void start()
  {
    running = true;

    ::QueryPerformanceCounter(&startTime);
  }

  void stop()
  {
    ::QueryPerformanceCounter(&stopTime);

    running = false;
  }

  LONGLONG elapsed()
  {
    return (stopTime.QuadPart - startTime.QuadPart);
  }

  double elapsedSeconds()
  {
    if (running) {
      ::QueryPerformanceCounter(&stopTime);
    }

    LARGE_INTEGER liFrequency;
    ::QueryPerformanceFrequency(&liFrequency);
    return ((double)elapsed() / (double)liFrequency.QuadPart);
  }

private:
  LARGE_INTEGER startTime;
  LARGE_INTEGER stopTime;
  bool running;
};
