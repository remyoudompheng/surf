include $(top_srcdir)/Makefile.global

noinst_LIBRARIES = libdummy.a
libdummy_a_SOURCES = dither_brute.cc dither_brute.h			\
	dither_clustered.cc dither_clustered.h				\
	dither_dispersed.cc dither_dispersed.h				\
	dither_floyd_steinberg.cc dither_floyd_steinberg.h		\
	dither_jarvis_judis_ninke.cc dither_jarvis_judis_ninke.h	\
	dither_stucki.cc dither_stucki.h				\
	dither_dot_diffusion.cc dither_dot_diffusion.h			\
	dither_smooth_dot_diffusion.cc dither_smooth_dot_diffusion.h	\
	copy_rgb_to_float.cc copy_rgb_to_float.h			\
	copy_rgb_to_float_curve.cc copy_rgb_to_float_curve.h		\
	dither_pixel_radius_adjust.cc dither_pixel_radius_adjust.h	\
	dither.h

libdummy.a:: $(libdummy_a_OBJECTS) $(libdummy_a_DEPENDENCIES)

echo_obj::
	@echo $(libdummy_a_OBJECTS)
