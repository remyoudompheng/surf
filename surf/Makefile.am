AUTOMAKE_OPTIONS = foreign 1.4

DITHER_OBJ = dither/dither_brute.o dither/dither_clustered.o dither/dither_dispersed.o dither/dither_floyd_steinberg.o dither/dither_jarvis_judis_ninke.o dither/dither_stucki.o dither/dither_dot_diffusion.o dither/dither_smooth_dot_diffusion.o dither/copy_rgb_to_float.o dither/copy_rgb_to_float_curve.o dither/dither_pixel_radius_adjust.o
DEBUG_OBJ = debug/debug.o
MISC_OBJ = misc/welcomeMessage.o misc/Misc.o misc/mystrdup.o

DRAW_OBJ = draw/NewClip.o draw/NewClipSphere.o draw/NewClipSphereCentral.o draw/NewClipSphereParallel.o draw/NewClipAlgebraic.o draw/NewClipAlgebraicCube.o draw/NewClipAlgebraicTetra.o draw/NewClipAlgebraicOcta.o draw/NewClipAlgebraicDode.o draw/NewClipAlgebraicIco.o draw/NewClipAlgebraicCyl.o draw/NewClipAlgebraicUser.o draw/NewSimpleClip.o draw/SurfaceCalc.o
MT_OBJ = mt/GuiThread.o mt/Thread.o mt/TSDrawingArea.o mt/cthread.o

GTKGUI_OBJ = gtkgui/start.o gtkgui/MainWindowController.o gtkgui/mycolor.o gtkgui/TextWidget.o gtkgui/showAbout.o gtkgui/ProgressDialog.o gtkgui/ColorWindow.o gtkgui/DrawingArea.o gtkgui/PositionWindow.o gtkgui/ConfigurationWindow.o gtkgui/NumericWindow.o gtkgui/ColorSelectionButton.o gtkgui/IlluminationWindow.o gtkgui/Requester.o gtkgui/DisplayWindow.o gtkgui/WidgetReadWriter.o gtkgui/DitherWindow.o gtkgui/ClipWindow.o gtkgui/Document.o gtkgui/RGBToStandard.o gtkgui/mygtk.o gtkgui/SaveImageDialog.o gtkgui/NavigationWindow.o

CURVE_OBJ = curve/RBTree.o curve/resultant.o curve/RootFinder2d.o curve/RootFinder3d.o curve/DrawCurve.o curve/BigInteger.o

DRAWFUNC_OBJ =  drawfunc/Bezier.o drawfunc/Clip.o drawfunc/DrawfuncData.o drawfunc/DrawFunc.o drawfunc/Hornergroup.o drawfunc/Horner.o drawfunc/MappingMatrix.o drawfunc/MultiVariatePolynom.o drawfunc/Position.o drawfunc/Sturmchain.o drawfunc/UniVariatePolynom.o drawfunc/Vector.o drawfunc/HornergroupXY.o drawfunc/HornergroupXYZ.o drawfunc/RationalHornerXY.o

IMAGE_OBJ =  image-formats/ImageFormats.o image-formats/AvailableImageFormats.o image-formats/ByExtension.o image-formats/JPEG.o image-formats/TIFF.o image-formats/Postscript.o image-formats/Sun.o image-formats/EPS.o image-formats/XWD.o image-formats/XBitmap.o image-formats/PPM.o image-formats/PBM.o image-formats/XPM.o image-formats/Pipe.o

SRC_OBJ = src/RGBToNetscape.o src/compfn.o src/cube.o src/float_buffer.o src/gui_config.o src/init_parser.o src/main.o src/oct_quan.o src/RgbBuffer.o src/roots.o src/bit_buffer.o src/SurfaceDataStruct.o src/CanvasDataStruct.o src/symtab.o src/WindowGeometry.o src/antialiasing_factor.o src/stop.o

YACCSRC_OBJ = yaccsrc/bezierarith.o yaccsrc/hornerarith.o yaccsrc/monomarith.o yaccsrc/mymemory.o yaccsrc/polyarith.o yaccsrc/polylex.o yaccsrc/polyroot.o yaccsrc/polyyacc.o yaccsrc/simple.o yaccsrc/polyx.o yaccsrc/lexfunc.o yaccsrc/SymbolTable.o yaccsrc/addDefaultSymbols.o yaccsrc/Script.o yaccsrc/ExecuteScriptStruct.o


LOCAL_OBJECTS = $(CURVE_OBJ) $(BALLS_OBJ) $(MISC_OBJ) $(DRAWFUNC_OBJ) $(IMAGE_OBJ) $(SRC_OBJ) $(YACCSRC_OBJ) $(MT_OBJ) $(DRAW_OBJ) $(DEBUG_OBJ) $(DITHER_OBJ)


if GUI
SUBDIRS = yaccsrc src image-formats drawfunc curve mt draw misc debug dither gtkgui
else
SUBDIRS = yaccsrc src image-formats drawfunc curve mt draw misc debug dither
endif

bin_PROGRAMS = surf
surf_SOURCES = 
EXTRA_DIST = background.pic Makefile.global aclocal.m4 surf.1 surf.xpm
man_MANS = surf.1
pkgdata_DATA = surf.xpm

if GUI
surf: $(LOCAL_OBJECTS) $(GTKGUI_OBJ)
	$(CXX) $(LOCAL_OBJECTS) $(GTKGUI_OBJ) -o surf $(LDFLAGS) -lXmu -lXext -lXt @X_PRE_LIBS@ @X_LIBS@ -lX11 @X_EXTRA_LIBS@ $(LIBS) -lfl @GTK_LIBS@
else
surf: $(LOCAL_OBJECTS)
	$(CXX) $(LOCAL_OBJECTS) -o surf $(LDFLAGS) $(LIBS) -lfl 
endif

clean-local::
	rm -f *~

dist-hook::
	absdistdir=`cd $(distdir) && pwd` && \
	(cd $(top_srcdir) && find examples docs | grep -v CVS | cpio -pad $$absdistdir)

#install-data-local:: surf.xpm
#	$(mkinstalldirs) $(pkgdatadir)
#	@(test -s surf.xpm && $(INSTALL) -m 644 surf.xpm $(pkgdatadir)) || echo "WARNING: background picture not installed"
#
#surf.xpm: surf $(srcdir)/background.pic
#	./surf --no-gui $(srcdir)/background.pic
