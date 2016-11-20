Driver for OpenVR usage without hardware
========================================

Provides a simple virtual driver for simulating HMD and controller behavior during OpenVR development.

Preparation
-----------

- IMPORTANT: Backup the existing null driver from SteamVR
- Set the OpenVRDir environment variable to point at your OpenVR SDK location
- Edit Build/premake5.lua and adjust the Steam path if necessary

Build
-----

This tool has only been built/tested using Visual Studio 2015 on Windows 10 64 bit.

- cd Build
- ../Tools/premake5 vs2015
- Open NoVR.sln
- Select your platform (e.g x64)
- Build

Install
-------

The projects build output is by default set to overwrite the null driver from SteamVR.

Run
---

Run any application using OpenVR/SteamVR

Build and run the NoVRRemote application to remote control the driver.
