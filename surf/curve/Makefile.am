include $(top_srcdir)/Makefile.global

noinst_LIBRARIES = libdummy.a
libdummy_a_SOURCES = RBTree.cc RBTree.h		\
        Array.h				\
        defs.h				\
        Matrix.h			\
        Monom.h				\
        RefCounter.h			\
        TreePolynom.h			\
	doubleMath.h			\
	resultant.cc resultant.h	\
	RootFinder2d.cc RootFinder2d.h	\
	RootFinder3d.cc RootFinder3d.h  \
	DrawCurve.cc DrawCurve.h	\
	mygmp.cc mygmp.h		\
	BigInteger.cc BigInteger.h	\
	Bezout.h

EXTRA_DIST = examples.txt
libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)
