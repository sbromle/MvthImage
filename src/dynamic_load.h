#ifndef DYNAMIC_LOAD_H
#define DYNAMIC_LOAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#ifndef MVTHIMAGELIB
#error "MVTHIMAGELIB not defined!"
/* this should be defined in Makefile.am */
#endif

extern void *load_symbol(char *libname,char *symbname, void *handle);
extern void release_handle(void *handle);

#endif
