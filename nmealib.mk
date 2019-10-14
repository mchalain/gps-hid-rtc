lib-y+=nmea

builddir:=nmealib/lib
nmea_SOURCES+=nmealib/src/context.c
nmea_SOURCES+=nmealib/src/generate.c
nmea_SOURCES+=nmealib/src/generator.c
nmea_SOURCES+=nmealib/src/gmath.c
nmea_SOURCES+=nmealib/src/info.c
nmea_SOURCES+=nmealib/src/parse.c
nmea_SOURCES+=nmealib/src/parser.c
nmea_SOURCES+=nmealib/src/sentence.c
nmea_SOURCES+=nmealib/src/time.c
nmea_SOURCES+=nmealib/src/tok.c
nmea_CFLAGS+=-Inmealib/include

