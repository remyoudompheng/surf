include $(top_srcdir)/Makefile.global

noinst_LIBRARIES = libdummy.a

libdummy_a_SOURCES = SurfaceCalc.cc SurfaceCalc.h \
	NewClip.cc               NewClip.h \
	NewSimpleClip.cc         NewSimpleClip.h \
	NewClipSphere.cc         NewClipSphere.h \
	NewClipSphereParallel.cc NewClipSphereParallel.h \
	NewClipSphereCentral.cc  NewClipSphereCentral.h \
	NewClipAlgebraic.cc      NewClipAlgebraic.h \
	NewClipAlgebraicTetra.cc NewClipAlgebraicTetra.h \
	NewClipAlgebraicCube.cc  NewClipAlgebraicCube.h \
	NewClipAlgebraicOcta.cc  NewClipAlgebraicOcta.h \
	NewClipAlgebraicDode.cc  NewClipAlgebraicDode.h \
	NewClipAlgebraicIco.cc   NewClipAlgebraicIco.h \
	NewClipAlgebraicCyl.cc   NewClipAlgebraicCyl.h \
	NewClipAlgebraicUser.cc  NewClipAlgebraicUser.h \
	                         NewClipNone.h \
	Preview.h

libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)

