workspace "cage"
location "./"
architecture "x86_64"
startproject "cage"

function buildArguments(cfg)
    local args = {}

    if cfg.name:find("Debug") then
        table.insert(args, "DEBUG")
    end

    if cfg.name:find("Release") then
        table.insert(args, "RELEASE")
    end

    return table.concat(args, " ")
end
premake.override(premake.tools.msc, "getLibraryExtensions", function(oldfn)
  local extensions = oldfn()
  extensions["a"] = true
  return extensions
end)
configurations {
    "Debug",
    "Release",
    "Dist"
}

flags {
    "MultiProcessorCompile"
}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["trdparty"] = path.getabsolute("./3rdparty")
IncludeDir["rootdir"] = path.getabsolute(".")
IncludeDir["spdlog"] = "%{IncludeDir.trdparty}/spdlog/include"
IncludeDir["eigen"] = "%{IncludeDir.trdparty}/Eigen"
IncludeDir["ImGui"] = "%{IncludeDir.trdparty}/DiligentTools/imgui/interface"
IncludeDir["DiligentCore1"] = "D:/CPPLib/DiligentEngine_v2.5.2/DiligentEngine_v2.5.2/build/Win64/DiligentCore"
IncludeDir["DiligentCore"] ="D:/CPPLib/DiligentEngine_v2.5.2/DiligentEngine_v2.5.2/DiligentCore"
IncludeDir["Effekseer"] = "D:/CPPLib/EffekseerRuntime152m/src/include/Effekseer"
IncludeDir["outlibdir"] = path.getabsolute(".") .. "/libs/%{cfg.buildcfg}"
group "Dependencies"
include "3rdparty/DiligentTools/imgui"
include "3rdparty/DiligentTools/TextureLoader"
--include "cage/src/include/physics"
group ""

project "cage"
kind "WindowedApp"
language "C++"
cppdialect "C++20"
staticruntime "on"

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")
-- need switch use to create on vs precompile head setting

entrypoint "WinMainCRTStartup"

files {
    "src/include/**.h",
    "src/include/**.cpp",
     "src/*.cpp",
    "src/include/**.hpp",
    "%{IncludeDir.trdparty}/ImGuizmo/ImGuizmo.cpp",
}
removefiles{"%{prj.name}/src/include/Render/structs.cpp","%{prj.name}/src/include/Test/**.cpp"
}
--files{"%{prj.name}/src/include/Test/tentry.cpp","%{prj.name}/src/include/app.ixx"}
defines {
    "_CRT_SECURE_NO_WARNINGS"
}

includedirs {
    "src/include/Common",
    "src/include/Modules",
    "src/include",
    "3rdparty/DiligentTools/ThirdParty/imgui",
    "%{IncludeDir.spdlog}",
    "%{IncludeDir.eigen}",
    "%{IncludeDir.ImGui}",
    "3rdparty/DiligentTools/TextureLoader/interface",
    "%{IncludeDir.libcds}",
    "%{IncludeDir.trdparty}",
    "%{IncludeDir.DiligentCore}",
    "%{IncludeDir.DiligentCore}/Common/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Win32/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Basic/interface",
    "%{IncludeDir.DiligentCore}/Primitives/interface",
    "%{IncludeDir.DiligentCore}/Platforms/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsEngine/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsAccessories/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsTools/interface",
    "%{IncludeDir.Effekseer}"
}
staticruntime "Off"

libdirs {
    "libs/%{cfg.buildcfg}"
    -- "G:/CPPLib/DiligentCore-Windows-x64-%{cfg.buildcfg}/lib/%{cfg.buildcfg}"
}
links {
    
    "TextureLoader","ImGui",
    "glfw3.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/GraphicsTools/%{cfg.buildcfg}/Diligent-GraphicsTools.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/GraphicsEngineD3DBase/%{cfg.buildcfg}/Diligent-GraphicsEngineD3DBase.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/ShaderTools/%{cfg.buildcfg}/Diligent-ShaderTools.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/HLSL2GLSLConverterLib/%{cfg.buildcfg}/Diligent-HLSL2GLSLConverterLib.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/SPIRV-Tools/source/opt/%{cfg.buildcfg}/SPIRV-Tools-opt.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/SPIRV-Tools/source/%{cfg.buildcfg}/SPIRV-Tools.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/GraphicsEngine/%{cfg.buildcfg}/Diligent-GraphicsEngine.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/GraphicsEngineVulkan/%{cfg.buildcfg}/Diligent-GraphicsEngineVk-static.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/GraphicsAccessories/%{cfg.buildcfg}/Diligent-GraphicsAccessories.lib",
    "%{IncludeDir.DiligentCore1}/Common/%{cfg.buildcfg}/Diligent-Common.lib",
    "%{IncludeDir.DiligentCore1}/Platforms/Win32/%{cfg.buildcfg}/Diligent-Win32Platform.lib",
    "%{IncludeDir.DiligentCore1}/Platforms/Basic/%{cfg.buildcfg}/Diligent-BasicPlatform.lib",
    "%{IncludeDir.DiligentCore1}/Primitives/%{cfg.buildcfg}/Diligent-Primitives.lib",
}
filter "system:windows"
systemversion "latest"
defines {
    "PLATFORM_WIN32"
}
excludes {
    "src/include/Common/**.*"
}
files {
    "src/include/Common/*.*",
    --"src/include/Modules/*.*",
    "src/include/Common/Win32/*.cpp",
    "src/include/Common/*.hpp",
    "src/include/Common/*.cpp",
}
--suf=""
buildoptions "/bigobj"
filter "configurations:Debug"
defines "DEBUG"
runtime "Debug"
symbols "on"
buildoptions{"/MDd"} 
surf="d"
links {
    "libcds-x64-dbg.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/GraphicsEngineVulkan/%{cfg.buildcfg}/GraphicsEngineVk_64d.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/SPIRV-Cross/%{cfg.buildcfg}/spirv-cross-cored.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/%{cfg.buildcfg}/glslangd.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/SPIRV/%{cfg.buildcfg}/SPIRVd.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/%{cfg.buildcfg}/MachineIndependentd.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/OGLCompilersDLL/%{cfg.buildcfg}/OGLCompilerd.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/OSDependent/Windows/%{cfg.buildcfg}/OSDependentd.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/%{cfg.buildcfg}/GenericCodeGend.lib",
}
filter "configurations:Release"
defines "RELEASE"
runtime "Release"
optimize "on"
buildoptions "/MD"
surf="r"
links {
    "libcds-x64.lib",
    "%{IncludeDir.DiligentCore1}/Graphics/GraphicsEngineVulkan/%{cfg.buildcfg}/GraphicsEngineVk_64%{surf}.lib", 
    "%{IncludeDir.DiligentCore1}/ThirdParty/SPIRV-Cross/%{cfg.buildcfg}/spirv-cross-core.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/%{cfg.buildcfg}/glslang.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/SPIRV/%{cfg.buildcfg}/SPIRV.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/%{cfg.buildcfg}/MachineIndependent.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/OGLCompilersDLL/%{cfg.buildcfg}/OGLCompiler.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/OSDependent/Windows/%{cfg.buildcfg}/OSDependent.lib",
    "%{IncludeDir.DiligentCore1}/ThirdParty/glslang/glslang/%{cfg.buildcfg}/GenericCodeGen.lib",
}