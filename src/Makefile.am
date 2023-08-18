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

