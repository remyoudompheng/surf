include $(top_srcdir)/Makefile.global

DITHER_OBJ = dither/dither_brute.o dither/dither_clustered.o dither/dither_dispersed.o dither/dither_floyd_steinberg.o dither/dither_jarvis_judis_ninke.o dither/dither_stucki.o dither/dither_dot_diffusion.o dither/dither_smooth_dot_diffusion.o dither/copy_rgb_to_float.o dither/copy_rgb_to_float_curve.o dither/dither_pixel_radius_adjust.o

DEBUG_OBJ = debug/debug.o

MISC_OBJ = misc/mystrdup.o misc/Misc.o

DRAW_OBJ = draw/NewClip.o draw/NewClipSphere.o draw/NewClipSphereCentral.o draw/NewClipSphereParallel.o draw/NewClipAlgebraic.o draw/NewClipAlgebraicCube.o draw/NewClipAlgebraicTetra.o draw/NewClipAlgebraicOcta.o draw/NewClipAlgebraicDode.o draw/NewClipAlgebraicIco.o draw/NewClipAlgebraicCyl.o draw/NewClipAlgebraicUser.o draw/NewSimpleClip.o draw/SurfaceCalc.o

CURVE_OBJ = curve/RBTree.o curve/resultant.o curve/RootFinder2d.o curve/RootFinder3d.o curve/DrawCurve.o curve/BigInteger.o

DRAWFUNC_OBJ =  drawfunc/Bezier.o drawfunc/Clip.o drawfunc/DrawfuncData.o drawfunc/DrawFunc.o drawfunc/Hornergroup.o drawfunc/Horner.o drawfunc/MappingMatrix.o drawfunc/MultiVariatePolynom.o drawfunc/Position.o drawfunc/Sturmchain.o drawfunc/UniVariatePolynom.o drawfunc/Vector.o drawfunc/HornergroupXY.o drawfunc/HornergroupXYZ.o drawfunc/RationalHornerXY.o

IMAGE_OBJ =  image-formats/ImageFormats.o image-formats/AvailableImageFormats.o image-formats/ByExtension.o image-formats/JPEG.o image-formats/TIFF.o image-formats/Postscript.o image-formats/Sun.o image-formats/EPS.o image-formats/XBitmap.o image-formats/PPM.o image-formats/PBM.o image-formats/XPM.o image-formats/PNG.o #image-formats/GTS.o image-formats/OOGL.o image-formats/Inventor.o

SRC_OBJ = src/RGBToNetscape.o src/compfn.o src/cube.o src/float_buffer.o src/init_parser.o src/main.o src/oct_quan.o src/RgbBuffer.o src/roots.o src/bit_buffer.o src/SurfaceDataStruct.o src/CanvasDataStruct.o src/WindowGeometry.o src/antialiasing_factor.o

YACCSRC_OBJ = yaccsrc/bezierarith.o yaccsrc/hornerarith.o yaccsrc/monomarith.o yaccsrc/mymemory.o yaccsrc/polyarith.o yaccsrc/polylex.o yaccsrc/polyroot.o yaccsrc/polyyacc.o yaccsrc/simple.o yaccsrc/polyx.o yaccsrc/lexfunc.o yaccsrc/SymbolTable.o yaccsrc/addDefaultSymbols.o yaccsrc/Script.o yaccsrc/ScriptVar.o

#TRIANGULATOR_OBJ = triangulator/Triangulator.o

LOCAL_OBJECTS = $(CURVE_OBJ) $(BALLS_OBJ) $(MISC_OBJ) $(DRAWFUNC_OBJ) $(IMAGE_OBJ) $(SRC_OBJ) $(YACCSRC_OBJ) $(DRAW_OBJ) $(DEBUG_OBJ) $(DITHER_OBJ) #$(TRIANGULATOR_OBJ)

SUBDIRS = yaccsrc src image-formats drawfunc curve draw misc debug dither docs # triangulator

bin_PROGRAMS = surf
surf_SOURCES =

surf: $(LOCAL_OBJECTS)
	$(CXX) $(LOCAL_OBJECTS) -o surf $(LDFLAGS) $(LIBS)

EXTRA_DIST = aclocal.m4 Makefile.global defaults.pic

dist-hook::
	absdistdir=`cd $(distdir) && pwd` && \
	(cd $(top_srcdir) && find examples | grep -v CVS | cpio -pad $$absdistdir)

