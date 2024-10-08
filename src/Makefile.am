SUBDIRS = ../prozubilib

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = nprozubi
nprozubi_SOURCES = \
									 main.c

nprozubi_CFLAGS = 
nprozubi_LDADD = \
								-ltiff -lm -lcurl -lsqlite3 -lnewt\
								 ../prozubilib/kdata2/cYandexDisk/*.o \
								 ../prozubilib/kdata2/*.o \
								 ../prozubilib/.libs/libprozubilib.a
#prozubilib/*.o

if WINDOWS
#build windows bundle
all-local: curl_dll
	mkdir -p nrozubi
	cp -r ../prozubilib/Templates nrozubi/.
	cp ../prozubilib/*.png nprozubi/.
	cp ../prozubilib/*.ttf nprozubi/.
	cp ../prozubilib/*.dll nprozubi/.
	cp ../prozubilib/kdata2/*.dll nprozubi/.
	cp ../prozubilib/kdata2/*cYdexDisk/.dll nprozubi/.
endif

if MACOS
#build macos bundle
endif

if LINUX
dist_pkgdata_DATA = ../prozubilib/*.sqlite 

templatesdir = $(pkgdatadir)/Templates
dist_templates_DATA = ../prozubilib/Templates/*

ttfdir = $(pkgdatadir)
dist_ttf_DATA = ../prozubilib/*.ttf

pngdir = $(pkgdatadir)
dist_png_DATA = ../prozubilib/*.png
endif
