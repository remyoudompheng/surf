include $(top_srcdir)/Makefile.global

noinst_LIBRARIES = libdummy.a

libdummy_a_SOURCES = mystrdup.cc mystrdup.h \
	             FileWriter.h \
	             IO.h IO.cc

libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)
