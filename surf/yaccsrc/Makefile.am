include $(top_srcdir)/Makefile.global

EXTRA_DIST = polyyacc.h
YACC = @YACC@ -d

addDefaultSymbols.cc: $(top_srcdir)/defaults.pic
	perl $(top_srcdir)/tools/convertScript.pl $< >$@

noinst_LIBRARIES = libdummy.a
libdummy_a_SOURCES =	polyyacc.y polylex.l polylexyacc.h degree.h \
			lexfunc.cc			\
			bezierarith.cc bezierarith.h	\
			hornerarith.cc hornerarith.h	\
			monomarith.cc monomarith.h	\
			mymemory.cc mymemory.h		\
			polyarith.cc polyarith.h	\
			polyroot.cc polyroot.h		\
			polyx.cc polyx.h		\
			simple.cc simple.h		\
			SymbolTable.cc SymbolTable.h	\
                        ScriptVar.cc ScriptVar.h        \
			Script.cc Script.h		\
			addDefaultSymbols.cc addDefaultSymbols.h

libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)

clean-local:
	rm -f polyyacc.c polyyacc.h polyex.c addDefaultSymbols.cc
