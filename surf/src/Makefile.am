include $(top_srcdir)/Makefile.global

noinst_LIBRARIES = libdummy.a
libdummy_a_SOURCES = main.cc \
        compfn.h compfn.cc \
        cube.h cube.cc \
        float_buffer.h float_buffer.cc \
        init_parser.h init_parser.cc \
        oct_quan.h oct_quan.cc \
        roots.h roots.cc \
        color.h \
        def.h \
	bit_buffer.cc bit_buffer.h			\
	SurfaceDataStruct.cc SurfaceDataStruct.h	\
	CanvasDataStruct.cc CanvasDataStruct.h		\
	RgbBuffer.cc RgbBuffer.h			\
	WindowGeometry.cc WindowGeometry.h		\
	antialiasing_factor.cc antialiasing_factor.h	\
	RGBToNetscape.cc RGBToNetscape.h		\
	mytypes.h


libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)

