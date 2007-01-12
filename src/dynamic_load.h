#ifndef DYNAMIC_LOAD_H
#define DYNAMIC_LOAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define BTCOMMONLIB "libBTCommon.so"
#define MVTHIMAGELIB "libMvthImage.so"

extern void *load_symbol(char *libname,char *symbname, void *handle);
extern void release_handle(void *handle);

#endif
