AM_CFLAGS = -I$(top_builddir) -I$(top_srcdir)/src -I$(top_srcdir)/src/UTILS \
-I$(top_srcdir)/include \
$(SDL_CFLAGS)


bin_PROGRAMS = sdlviewer

sdlviewer_SOURCES = readmmap.c \
readmmap.h \
sdlviewer.c \
../../src/base/images_utils.c \
../../src/base/images_utils.h

sdlviewer_LDADD = -lm $(SDL_LIBS)
