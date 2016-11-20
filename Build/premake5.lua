solution "NoVR"
  configurations { "Debug", "Release" }
  platforms { "x64", "x32" }

  configuration "Debug"
    flags { "Symbols" }

  configuration "Release"
    flags { "Optimize", "Symbols" }

  project "NoVR"
    kind "SharedLib"
    language "C++"
    flags { "C++14" }

    targetname "driver_null"

    files {
      "../Source/**.cpp",
      "../Source/**.h",
      "../Source/**.inl",
      "*.cmd",
      "*.lua",
    }
      
    includedirs { "../Source", "$(OpenVRDir)/headers", "../Libraries/glm" }
    links { "openvr_api" }
    
    defines { "NOMINMAX" }
    
    configuration "x64"
      libdirs { "$(OpenVRDir)/lib/win64" }
      targetdir "C:/Program Files (x86)/Steam/SteamApps/common/SteamVR/drivers/null/bin/win64"
    
    configuration "x32"
      libdirs { "$(OpenVRDir)/lib/win32" }
      targetdir "C:/Program Files (x86)/Steam/SteamApps/common/SteamVR/drivers/null/bin/win32"
      
    configuration "vs*"
      flags { "NoMinimalRebuild" }
      buildoptions { "/MP", "/Zo", "/wd4512 /wd4458" }
      defines { "_CRT_SECURE_NO_WARNINGS" }
      
      vpaths {
        ["*"] = "..",
      }
      
  project "NoVRRemote"
    language "C#"
    kind "ConsoleApp"
    architecture "x64"

    location "../Tools/NoVRRemote/"
    
    files { "../Tools/NoVRRemote/**.cs" }
    links {
      "System",
      "System.Drawing",
      "System.Windows.Forms",
      "System.Data",
    }
