-- Solution definition
solution "IntroToGraphicsCPP"
  location "../build"

  -- Only Debug and Release configurations
  configurations {
    "Debug",
    "Release",
  }

  -- Support for X86, X64 and 'native' (for other OSes)
  platforms {
    "x32",
    "x64",
    "Native",
  }

  -- C++
  language "C++"

-- Folder for the 'workspace'
WORKSPACE_DIR = path.getabsolute("..")

-- For when we have a 3rd party library set (AssImp, any font libs, etc)
local THIRD_PARTY_DIR = path.join(WORKSPACE_DIR, "3rdparty")

-- Add in the toolchain.lua scrip and fire off the main script
-- to set up the build environment
dofile (path.join(WORKSPACE_DIR, "scripts/toolchain.lua"))
if not toolchain(WORKSPACE_DIR, THIRD_PARTY_DIR) then
	return -- no action specified
end

local PDB_DIR = path.join(path.join(path.join(WORKSPACE_DIR,"projects"), _ACTION), "pdbs")
os.mkdir(PDB_DIR)

-- configureations for Debug
configuration {"Debug", "x32"}
  defines { "WIN32", "_DEBUG", "_WINDOWS", "_UNICODE", "UNICODE", "%(PreprocessorDefinitions)" }
  libdirs { path.join(THIRD_PARTY_DIR, "assimp/lib/win32/Debug")}
  links {"D3D11", "D3DCompiler"}
  links {"assimp-vc140-mt","zlibstaticd"}
  links {"kernel32","user32","gdi32","winspool","comdlg32","advapi32","shell32","ole32","oleaut32","uuid","odbc32","odbccp32", "winmm", "dxgi"}
  flags {"ExtraWarnings"}
  -- To reproduce the linker bug reported in https://github.com/bkaradzic/GENie/issues/266
  -- comment out the two lines below.
  linkoptions {"/PDB:pdbs/$(TargetName)output-dx32.pdb"}
  targetsuffix "-d"
  postbuildcommands { "xcopy ..\\..\\3rdparty\\assimp\\bin\\Debug\\assimp-vc140-mt.dll $(TargetDir) /Y ",
                      "xcopy ..\\..\\..\\assets\\raw\\*.*  $(TargetDir)assets\\raw\\ /Y /E",
                      "xcopy ..\\..\\..\\data\\Shaders\\*.*  $(TargetDir)data\\Shaders\\ /Y /E"
                    }

configuration {"Debug", "x64"}
  defines { "WIN32", "_DEBUG", "_WINDOWS", "_UNICODE", "UNICODE", "%(PreprocessorDefinitions)" }
  libdirs { path.join(THIRD_PARTY_DIR, "assimp/lib/win64/Debug")}
  links {"D3D11", "D3DCompiler"}
  links {"assimp-vc140-mt","zlibstaticd"}
  links {"kernel32","user32","gdi32","winspool","comdlg32","advapi32","shell32","ole32","oleaut32","uuid","odbc32","odbccp32", "winmm", "dxgi"}
  flags {"ExtraWarnings"}
  -- To reproduce the linker bug reported in https://github.com/bkaradzic/GENie/issues/266
  -- comment out the two lines below.
  linkoptions {"/PDB:pdbs/$(TargetName)output-dx64.pdb"}
  targetsuffix "-d"
  postbuildcommands { "xcopy ..\\..\\3rdparty\\assimp\\bin\\Debug\\assimp-vc140-mt.dll $(TargetDir) /Y ",
                      "xcopy ..\\..\\..\\assets\\raw\\*.*  $(TargetDir)assets\\raw\\ /Y /E",
                      "xcopy ..\\..\\..\\data\\Shaders\\*.*  $(TargetDir)data\\Shaders\\ /Y /E"
                    }

-- configuration for Release
configuration {"Release", "x32"}
  defines { "WIN32", "NDEBUG", "_WINDOWS", "_UNICODE", "UNICODE", "%(PreprocessorDefinitions)" }
  libdirs { path.join(THIRD_PARTY_DIR, "assimp/lib/win32/Release") }
  links {"D3D11", "D3DCompiler"}
  links {"assimp-vc140-mt","zlibstatic"}
  links {"kernel32","user32","gdi32","winspool","comdlg32","advapi32","shell32","ole32","oleaut32","uuid","odbc32","odbccp32", "winmm", "dxgi"}
  flags {"Optimize", "ExtraWarnings"}
  postbuildcommands { "xcopy ..\\..\\3rdparty\\assimp\\bin\\Release\\assimp-vc140-mt.dll $(TargetDir) /Y ",
                      "xcopy ..\\..\\..\\assets\\raw\\*.*  $(TargetDir)assets\\raw\\ /Y /E",
                      "xcopy ..\\..\\..\\data\\Shaders\\*.*  $(TargetDir)data\\Shaders\\ /Y /E"
                    }

configuration {"Release", "x64"}
  defines { "WIN32", "NDEBUG", "_WINDOWS", "_UNICODE", "UNICODE", "%(PreprocessorDefinitions)" }
  libdirs { path.join(THIRD_PARTY_DIR, "assimp/lib/win64/Release") }
  links {"D3D11", "D3DCompiler"}
  links {"assimp-vc140-mt","zlibstatic"}
  links {"kernel32","user32","gdi32","winspool","comdlg32","advapi32","shell32","ole32","oleaut32","uuid","odbc32","odbccp32", "winmm", "dxgi"}
  flags {"Optimize", "ExtraWarnings"}
  postbuildcommands { "xcopy ..\\..\\3rdparty\\assimp\\bin\\Release\\assimp-vc140-mt.dll $(TargetDir) /Y ",
                      "xcopy ..\\..\\..\\assets\\raw\\*.*  $(TargetDir)assets\\raw\\ /Y /E",
                      "xcopy ..\\..\\..\\data\\Shaders\\*.*  $(TargetDir)data\\Shaders\\ /Y /E"
                    }

-- our first project
project "intro01"
  PROJ_DIR = path.join(WORKSPACE_DIR, "intro01")
  flags { "WinMain", "NoExceptions"}

  kind "WindowedApp"
  debugdir "$(TargetDir)"

  includedirs {
    path.join(PROJ_DIR, "src"),
    path.join(THIRD_PARTY_DIR, "assimp/include")
  }

  files {
    path.join(PROJ_DIR, "src/**.h"),
    path.join(PROJ_DIR, "src/**.cpp"),
    path.join(PROJ_DIR, "src/Intro01.rc"),
  }

  resoptions {
    "src/Intro01.rc"
  }

-- A new project
project "tutorial01"
  PROJ_DIR = path.join(WORKSPACE_DIR, "tutorial01")
  flags { "WinMain", "NoExceptions" }

  kind "WindowedApp"
  debugdir "$(TargetDir)"

  includedirs {
    path.join(PROJ_DIR, "src")
  }

  files {
    path.join(PROJ_DIR, "src/**.h"),
    path.join(PROJ_DIR, "src/**.cpp"),
    path.join(PROJ_DIR, "src/tutorial01.rc")
  }

  resoptions {
    "src/tutorial01.rc"
  }

-- A stub project
project "testbed"
  PROJ_DIR = path.join(WORKSPACE_DIR, "testbed")
  flags { "WinMain", "NoExceptions" }

  kind "WindowedApp"
  debugdir "$(TargetDir)"

  includedirs {
    path.join(PROJ_DIR, "src")
  }

  files {
    path.join(PROJ_DIR, "src/**.h"),
    path.join(PROJ_DIR, "src/**.cpp"),
    path.join(PROJ_DIR, "src/testbed.rc")
  }

  resoptions {
    "src/testbed.rc"
  }
