info_TEXINFOS = surf.texi
man_MANS = surf.1

EXTRA_DIST = $(man_MANS)

surf.1: $(top_srcdir)/src/main.cc
	-$(HELP2MAN) --output=surf.1.tmp --name='visualization of real algebraic geometry' $(top_srcdir)/surf
	-$(SED) s/FSF// surf.1.tmp >$@
	-rm -f surf.1.tmp

html: surf.texi
	texi2html -subdir html -split chapters -no-section_navigation surf.texi

clean-local:
	rm -rf html/
