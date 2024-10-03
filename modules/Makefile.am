AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  MvtxQaNtuplizer.h

lib_LTLIBRARIES = \
  libMvtxQaNtuplizer.la

libMvtxQaNtuplizer_la_SOURCES = \
  MvtxQaNtuplizer.cc

libMvtxQaNtuplizer_la_LIBADD = \
  -lphool \
  -lffaobjects \
  -lffarawobjects \
  -ltrack_io \
  -lg4detectors \
  -lintt \
  -lmvtx \
  -lmicromegas \
  -lglobalvertex \
  -ltrackbase_historic_io \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libMvtxQaNtuplizer.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
