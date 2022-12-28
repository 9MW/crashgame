workspace "cage"

project "ImGui"
kind "StaticLib"
language "C++"
targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")
staticruntime "Off"

-- IncludeDir = {}
-- IncludeDir["DiligentCore"] = "G:/CPPLib/DiligentCore-Windows-x64-%{cfg.buildcfg}/include"

includedirs {
    "../ThirdParty/imgui",
    "interface",
    "../ThirdParty/imGuIZMO.quat",
    "%{IncludeDir.DiligentCore}/Common/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Win32/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Basic/interface",
    "%{IncludeDir.DiligentCore}/Primitives/interface",
    "%{IncludeDir.DiligentCore}/Platforms/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsEngine/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsAccessories/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsTools/interface",
    "../../"--used for include glfw

}
files {
    "src/ImGuiDiligentRenderer.cpp",
    "src/ImGuiImplDiligent.cpp",
    "src/ImGuiImplWin32.cpp",
    "interface/ImGuiImplWin32.hpp",
    "interface/ImGuiImplDiligent.hpp",
    "interface/ImGuiDiligentRenderer.hpp",
    "./*h",
    --"../ThirdParty/imgui/misc/*h",
    "../ThirdParty/imgui/*cpp",
    "src/imGuiimpGLFW.cpp",
    "../ThirdParty/imgui/backends/imgui_impl_glfw.cpp"
}

filter "system:windows"
systemversion "latest"
cppdialect "C++17"
staticruntime "On"
files {
    "src/ImGuiImplWin32.cpp",
    "interface/ImGuiImplWin32.hpp"
}
defines {
    "PLATFORM_WIN32"
}
filter "system:linux"
pic "On"
systemversion "latest"
cppdialect "C++17"
staticruntime "On"

filter "configurations:Debug"
runtime "Debug"
symbols "on"
buildoptions "/MDd"

filter "configurations:Release"
runtime "Release"
optimize "on"
buildoptions "/MD"
