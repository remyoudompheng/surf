include $(top_srcdir)/Makefile.global
CPPFLAGS = @CPPFLAGS@ @GTK_CFLAGS@
noinst_LIBRARIES = libdummy.a

libdummy_a_SOURCES = welcomeMessage.cc welcomeMessage.h	\
	Misc.cc Misc.h \
	mystrdup.cc mystrdup.h \
	FileWriter.h

libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)
