add_library(WaveSabreVstLib
	../Vst3.x/public.sdk/source/vst2.x/audioeffect.cpp
	../Vst3.x/public.sdk/source/vst2.x/audioeffectx.cpp
	../Vst3.x/public.sdk/source/vst2.x/vstplugmain.cpp
	../Vst3.x/vstgui.sf/libpng/png.c
	../Vst3.x/vstgui.sf/libpng/pngerror.c
	../Vst3.x/vstgui.sf/libpng/pnggccrd.c
	../Vst3.x/vstgui.sf/libpng/pngget.c
	../Vst3.x/vstgui.sf/libpng/pngmem.c
	../Vst3.x/vstgui.sf/libpng/pngpread.c
	../Vst3.x/vstgui.sf/libpng/pngread.c
	../Vst3.x/vstgui.sf/libpng/pngrio.c
	../Vst3.x/vstgui.sf/libpng/pngrtran.c
	../Vst3.x/vstgui.sf/libpng/pngrutil.c
	../Vst3.x/vstgui.sf/libpng/pngset.c
	../Vst3.x/vstgui.sf/libpng/pngtrans.c
	../Vst3.x/vstgui.sf/libpng/pngvcrd.c
	../Vst3.x/vstgui.sf/libpng/pngwio.c
	../Vst3.x/vstgui.sf/libpng/pngwrite.c
	../Vst3.x/vstgui.sf/libpng/pngwtran.c
	../Vst3.x/vstgui.sf/libpng/pngwutil.c
	../Vst3.x/vstgui.sf/vstgui/aeffguieditor.cpp
	../Vst3.x/vstgui.sf/vstgui/cfileselector.cpp
	../Vst3.x/vstgui.sf/vstgui/cvstguitimer.cpp
	../Vst3.x/vstgui.sf/vstgui/cscrollview.cpp
	../Vst3.x/vstgui.sf/vstgui/vstcontrols.cpp
	../Vst3.x/vstgui.sf/vstgui/vstgui.cpp
	../Vst3.x/vstgui.sf/vstgui/vstguidebug.cpp
	../Vst3.x/vstgui.sf/zlib/adler32.c
	../Vst3.x/vstgui.sf/zlib/compress.c
	../Vst3.x/vstgui.sf/zlib/crc32.c
	../Vst3.x/vstgui.sf/zlib/deflate.c
	../Vst3.x/vstgui.sf/zlib/gzio.c
	../Vst3.x/vstgui.sf/zlib/infback.c
	../Vst3.x/vstgui.sf/zlib/inffast.c
	../Vst3.x/vstgui.sf/zlib/inflate.c
	../Vst3.x/vstgui.sf/zlib/inftrees.c
	../Vst3.x/vstgui.sf/zlib/minigzip.c
	../Vst3.x/vstgui.sf/zlib/trees.c
	../Vst3.x/vstgui.sf/zlib/uncompr.c
	../Vst3.x/vstgui.sf/zlib/zutil.c
	include/WaveSabreVstLib.h
	src/ImageManager.cpp
	src/NoTextCOptionMenu.cpp
	src/CEnhancedKnob.cpp
	src/VstEditor.cpp	
	src/VstPlug.cpp)

target_include_directories(WaveSabreVstLib PUBLIC
	include
	../WaveSabreCore/include
	../Vst3.x/vstgui.sf/zlib
	../Vst3.x/vstgui.sf/vstgui
	../Vst3.x/vstgui.sf/libpng
	../Vst3.x/public.sdk/source/vst2.x
	../Vst3.x)

target_compile_definitions(WaveSabreVstLib PUBLIC USE_LIBPNG)

if(MSVC)
	target_compile_definitions(WaveSabreVstLib PUBLIC _CRT_SECURE_NO_WARNINGS)
	target_compile_options(WaveSabreVstLib PUBLIC /EHsc)
endif()
