project "AssetLoader"
kind "StaticLib"
language "C++"
targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")
--include "%{IncludeDir.rootdir}/pj2/3rdparty/DiligentTools/TextureLoader"
includedirs{
    "%{IncludeDir.trdparty}",
    "%{IncludeDir.rootdir}/3rdparty/DiligentTools/TextureLoader/interface",
    "%{IncludeDir.DiligentCore}/Common/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Win32/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Basic/interface",
    "%{IncludeDir.DiligentCore}/Primitives/interface",
    "%{IncludeDir.DiligentCore}/Platforms/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsEngine/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsAccessories/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsTools/interface"
}

files {"./**.cpp"}

filter "system:windows"
systemversion "latest"
defines {
    "PLATFORM_WIN32"
}