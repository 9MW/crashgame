workspace "cage"

project "TextureLoader"
kind "StaticLib"
language "C++"
targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")
staticruntime "Off"
depdent={}
depdent["zlib"]="../ThirdParty/zlib-1.2.8"
depdent["libjpg"]="../ThirdParty/libjpeg-9a"
depdent["libtiff"]="../ThirdParty/libtiff"
includedirs {
    "include",
    "interface",
    "../ThirdParty/libtiff",
    "../ThirdParty/lpng-1.6.17",
    "../ThirdParty/libjpeg-9a",
     "%{depdent.zlib}",
     "%{depdent.libjpg}",
    "%{IncludeDir.DiligentCore}/Common/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Win32/interface",
    "%{IncludeDir.DiligentCore}/Platforms/Basic/interface",
    "%{IncludeDir.DiligentCore}/Primitives/interface",
    "%{IncludeDir.DiligentCore}/Platforms/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsEngine/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsAccessories/interface",
    "%{IncludeDir.DiligentCore}/Graphics/GraphicsTools/interface"

}
files {
    "src/DDSLoader.cpp",
    "src/JPEGCodec.c",
    "src/Image.cpp",
    "src/KTXLoader.cpp",
    "src/PNGCodec.c",
    "src/TextureLoader.cpp",
    "src/TextureUtilities.cpp",
    "interface/Image.h",
    "interface/TextureLoader.h",
    "interface/TextureUtilities.h",
    "include/DDSLoader.h",
    "include/dxgiformat.h",
    "include/JPEGCodec.h",
    "include/pch.h",
    "include/PNGCodec.h",
    "%{depdent.zlib}/adler32.c",
    "%{depdent.zlib}/compress.c",
    "%{depdent.zlib}/crc32.c",
    "%{depdent.zlib}/deflate.c",
    "%{depdent.zlib}/gzclose.c",
    "%{depdent.zlib}/gzlib.c",
    "%{depdent.zlib}/gzread.c",
    "%{depdent.zlib}/gzwrite.c",
    "%{depdent.zlib}/infback.c",
    "%{depdent.zlib}/inffast.c",
    "%{depdent.zlib}/inflate.c",
    "%{depdent.zlib}/inftrees.c",
    "%{depdent.zlib}/trees.c",
    "%{depdent.zlib}/uncompr.c",
    "%{depdent.zlib}/zutil.c",
 "%{depdent.libjpg}/jaricom.c",
 "%{depdent.libjpg}/jcapimin.c",
 "%{depdent.libjpg}/jcapistd.c",
 "%{depdent.libjpg}/jcarith.c",
 "%{depdent.libjpg}/jccoefct.c",
 "%{depdent.libjpg}/jccolor.c",
 "%{depdent.libjpg}/jcdctmgr.c",
 "%{depdent.libjpg}/jchuff.c",
 "%{depdent.libjpg}/jcinit.c",
 "%{depdent.libjpg}/jcmainct.c",
 "%{depdent.libjpg}/jcmarker.c",
 "%{depdent.libjpg}/jcmaster.c",
 "%{depdent.libjpg}/jcomapi.c",
 "%{depdent.libjpg}/jcparam.c",
 "%{depdent.libjpg}/jcprepct.c",
 "%{depdent.libjpg}/jcsample.c",
 "%{depdent.libjpg}/jctrans.c",
 "%{depdent.libjpg}/jdapimin.c",
 "%{depdent.libjpg}/jdapistd.c",
 "%{depdent.libjpg}/jdarith.c",
 "%{depdent.libjpg}/jdatadst.c",
 "%{depdent.libjpg}/jdatasrc.c",
 "%{depdent.libjpg}/jdcoefct.c",
 "%{depdent.libjpg}/jdcolor.c",
 "%{depdent.libjpg}/jddctmgr.c",
 "%{depdent.libjpg}/jdhuff.c",
 "%{depdent.libjpg}/jdinput.c",
 "%{depdent.libjpg}/jdmainct.c",
 "%{depdent.libjpg}/jdmarker.c",
 "%{depdent.libjpg}/jdmaster.c",
 "%{depdent.libjpg}/jdmerge.c",
 "%{depdent.libjpg}/jdpostct.c",
 "%{depdent.libjpg}/jdsample.c",
 "%{depdent.libjpg}/jdtrans.c",
 "%{depdent.libjpg}/jerror.c",
 "%{depdent.libjpg}/jfdctflt.c",
 "%{depdent.libjpg}/jfdctfst.c",
 "%{depdent.libjpg}/jfdctint.c",
 "%{depdent.libjpg}/jidctflt.c",
 "%{depdent.libjpg}/jidctfst.c",
 "%{depdent.libjpg}/jidctint.c",
 "%{depdent.libjpg}/jmemmgr.c",
 "%{depdent.libjpg}/jmemnobs.c",
 "%{depdent.libjpg}/jquant1.c",
 "%{depdent.libjpg}/jquant2.c",
 "%{depdent.libjpg}/jutils.c",

    "../ThirdParty/lpng-1.6.17/png.c",
    "../ThirdParty/lpng-1.6.17/pngerror.c",
    "../ThirdParty/lpng-1.6.17/pngget.c",
    "../ThirdParty/lpng-1.6.17/pngmem.c",
    "../ThirdParty/lpng-1.6.17/pngpread.c",
    "../ThirdParty/lpng-1.6.17/pngread.c",
    "../ThirdParty/lpng-1.6.17/pngrio.c",
    "../ThirdParty/lpng-1.6.17/pngrtran.c",
    "../ThirdParty/lpng-1.6.17/pngrutil.c",
    "../ThirdParty/lpng-1.6.17/pngset.c",
    "../ThirdParty/lpng-1.6.17/pngtrans.c",
    "../ThirdParty/lpng-1.6.17/pngwio.c",
    "../ThirdParty/lpng-1.6.17/pngwrite.c",
    "../ThirdParty/lpng-1.6.17/pngwtran.c",
    "../ThirdParty/lpng-1.6.17/pngwutil.c",
 "%{depdent.libtiff}/tif_aux.c",
 "%{depdent.libtiff}/tif_close.c",   
 "%{depdent.libtiff}/tif_codec.c",
 "%{depdent.libtiff}/tif_color.c",
 "%{depdent.libtiff}/tif_compress.c",
 "%{depdent.libtiff}/tif_dir.c",
 "%{depdent.libtiff}/tif_dirinfo.c",
 "%{depdent.libtiff}/tif_dirread.c",
 "%{depdent.libtiff}/tif_dirwrite.c",
 "%{depdent.libtiff}/tif_dumpmode.c",
 "%{depdent.libtiff}/tif_error.c",
 "%{depdent.libtiff}/tif_extension.c",
 "%{depdent.libtiff}/tif_fax3.c",
 "%{depdent.libtiff}/tif_fax3sm.c",
 "%{depdent.libtiff}/tif_flush.c",
 "%{depdent.libtiff}/tif_getimage.c",
 "%{depdent.libtiff}/tif_jbig.c",
 "%{depdent.libtiff}/tif_jpeg.c",
 "%{depdent.libtiff}/tif_jpeg_12.c",
 "%{depdent.libtiff}/tif_luv.c",
 "%{depdent.libtiff}/tif_lzma.c",
 "%{depdent.libtiff}/tif_lzw.c",
 "%{depdent.libtiff}/tif_next.c",
 "%{depdent.libtiff}/tif_ojpeg.c",
 "%{depdent.libtiff}/tif_open.c",
 "%{depdent.libtiff}/tif_packbits.c",
 "%{depdent.libtiff}/tif_pixarlog.c",
 "%{depdent.libtiff}/tif_predict.c",
 "%{depdent.libtiff}/tif_print.c",
 "%{depdent.libtiff}/tif_read.c",
 "%{depdent.libtiff}/tif_stream.cxx",
 "%{depdent.libtiff}/tif_strip.c",
 "%{depdent.libtiff}/tif_swab.c",
 "%{depdent.libtiff}/tif_thunder.c",
 "%{depdent.libtiff}/tif_tile.c",
 "%{depdent.libtiff}/tif_version.c",
 "%{depdent.libtiff}/tif_warning.c",
 "%{depdent.libtiff}/tif_write.c",
 "%{depdent.libtiff}/tif_zip.c",
}

filter "system:windows"
systemversion "latest"
cppdialect "C++17"
staticruntime "On"
defines {
    "PLATFORM_WIN32"
}
files{
    "%{depdent.libtiff}/tif_win32.c",
}
filter {"system:linux","system:android"}
files{
    "%{depdent.libtiff}/tif_unix.c",
}
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