include $(top_srcdir)/Makefile.global

noinst_LIBRARIES = libdummy.a
libdummy_a_SOURCES = ImageFormats.h ImageFormats.cc \
        AvailableImageFormats.h AvailableImageFormats.cc \
	ByExtension.h ByExtension.cc \
	JPEG.h JPEG.cc   TIFF.h TIFF.cc     Postscript.h Postscript.cc \
        Sun.h Sun.cc     EPS.h EPS.cc       \
	XBitmap.h XBitmap.cc PPM.h PPM.cc   PBM.h PBM.cc \
	XPM.h XPM.cc \
        PNG.h PNG.cc

libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)

