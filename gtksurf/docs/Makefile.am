info_TEXINFOS = gtksurf.texi
man_MANS = gtksurf.1

EXTRA_DIST = $(man_MANS)

gtksurf.1: $(top_srcdir)/src/main.cc
	-$(HELP2MAN) --output=gtksurf.1.tmp --name='a graphical frontend for surf' $(top_srcdir)/src/gtksurf
	-$(SED) s/FSF// gtksurf.1.tmp >$@
	-rm -f gtksurf.1.tmp

html: gtksurf.texi
	texi2html -subdir html -split chapters -no-section_navigation gtksurf.texi

clean-local:
	rm -rf html/

