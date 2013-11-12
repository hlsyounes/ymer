## Process this file with automake to produce Makefile.in

# AUTOMAKE_OPTIONS = readme-alpha

SUBDIRS = gsl utils sys test err complex block ieee-utils specfunc rng randist poly histogram cdf

SUBLIBS = block/libgslblock.la complex/libgslcomplex.la err/libgslerr.la histogram/libgslhistogram.la ieee-utils/libgslieeeutils.la poly/libgslpoly.la randist/libgslrandist.la rng/libgslrng.la specfunc/libgslspecfunc.la sys/libgslsys.la test/libgsltest.la utils/libutils.la cdf/libgslcdf.la

pkginclude_HEADERS = gsl_math.h gsl_pow_int.h gsl_nan.h gsl_machine.h gsl_mode.h gsl_precision.h gsl_types.h gsl_version.h

bin_SCRIPTS = gsl-config

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA= gsl.pc

EXTRA_DIST = autogen.sh gsl-config.in gsl.pc.in configure.ac THANKS BUGS SUPPORT gsl.spec.in gsl.m4 test_gsl_histogram.sh

lib_LTLIBRARIES = libgsl.la
libgsl_la_SOURCES = version.c
libgsl_la_LIBADD = $(SUBLIBS)
libgsl_la_LDFLAGS = -version-info $(GSL_LT_VERSION)
noinst_HEADERS = templates_on.h templates_off.h

m4datadir = $(datadir)/aclocal
m4data_DATA = gsl.m4

bin_PROGRAMS = gsl-randist gsl-histogram

gsl_randist_SOURCES = gsl-randist.c
gsl_randist_LDADD = libgsl.la

gsl_histogram_SOURCES = gsl-histogram.c
gsl_histogram_LDADD = libgsl.la

check_SCRIPTS = test_gsl_histogram.sh
TESTS = test_gsl_histogram.sh

#bin_PROGRAMS = main dummy
#dummy_SOURCES = version.c
#dummy_LDADD = $(SUBLIBS)
#main_SOURCES = version.c env.c
#main_LDADD = libgsl.la

ACLOCAL_AMFLAGS = -I m4
