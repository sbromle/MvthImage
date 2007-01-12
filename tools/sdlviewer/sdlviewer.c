/* 
** This file is part of MVTH (Machine Vision Test Harness).
**
** Copyright (c) 2004 Samuel P. Bromley <sam@sambromley.com>
** 
** MVTH is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** MVTH is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with MVTH; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/ 

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <SDL/SDL.h>
#include <assert.h>
#include "base/images_utils.h"
#include "readmmap.h"

int main(int argc, char *argv[])
{
	SDL_Surface *screen=NULL;
	SDL_Surface *backbuffer=NULL;
	/* color palette for grayscale images */
	SDL_Color grayscale_palette[256];
	SDL_Event event;
	const SDL_VideoInfo *vidinfo=NULL;
	int colordepth;
	char winname[1024];
	int quit_flag=0;
	int bpp;
	image_t *img=NULL;
	image_t oldimg={"",0,0,0,0,NULL};
	unsigned long ts=0;
	char *varname=NULL;
	char filename[1024];
	char iconname[1024];
	int swaprgb=0;
	unsigned int rmask,gmask,bmask,amask;
	int i;

	/* initialize the grayscale palette */
	for (i=0;i<256;i++)
	{
		grayscale_palette[i].r=i;
		grayscale_palette[i].g=i;
		grayscale_palette[i].b=i;
		grayscale_palette[i].unused=0;
	}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	if (argc<2)
	{
		fprintf(stdout,"%s <mvth variable name>\n",argv[0]);
		return EXIT_FAILURE;
	}
	else if (argc==2 && strcmp(argv[1],"-h")==0)
	{
		fprintf(stdout,"%s [-hs] <mvth variable name>\n",argv[0]);
		fprintf(stdout,"   -h  print this help");
		fprintf(stdout,"   -s  swap rgb");
		return EXIT_SUCCESS;
	}
	else if (argc==2) varname=strdup(argv[1]);
	else if (argc==3) varname=strdup(argv[2]);
	if (argc==3 && strcmp(argv[1],"-s")==0) swaprgb=1;

	if (swaprgb)
	{
		int tmp;
		tmp=bmask;
		bmask=rmask;
		rmask=tmp;
	}

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)<0)
	{
		fprintf(stderr,"Unable to initialize SDL: %s\n",SDL_GetError());
		return EXIT_FAILURE;
	}
	atexit(SDL_Quit);

	vidinfo=SDL_GetVideoInfo();

	colordepth=vidinfo->vfmt->BitsPerPixel;
	fprintf(stderr,"Got colordepth=%d\n",colordepth);

	screen=SDL_SetVideoMode(256,256,colordepth,SDL_SWSURFACE);
	if (screen==NULL)
	{
		fprintf(stderr,"Unable to 256x256 video: %s\n",SDL_GetError());
		exit(1);
	}
	backbuffer=SDL_CreateRGBSurface(SDL_SWSURFACE,
			256,256,4*8,rmask,gmask,bmask,0);

	bpp=screen->format->BytesPerPixel;
	fprintf(stderr,"Got Bpp=%d\n",bpp);

	char *env;
	char *user;
	/* a number was specified, and we should construct the path
	 * to the file */
	sprintf(winname,"Image \"%s\"",varname);
	sprintf(iconname,"\"%s\"",varname);

	env=getenv("USER");
	if (env==NULL)
	{
		fprintf(stderr,"Problem determining username.\n");
		return EXIT_FAILURE;
	}
	user=strdup(env);
	snprintf(filename,1024,"/tmp/mvth.mmap_%s/mmap_%s",user,varname);
	fprintf(stderr,"Using file \"%s\".\n",filename);
	free(user);
	free(varname);

	SDL_WM_SetCaption(winname,iconname);


	while (quit_flag==0)
	{
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit_flag=1;
					break;
			}
		}

		/* see if we have a new image */
		int ret=readmmap(filename,&ts,&img);

		if (ret==0)
		{
			if (oldimg.data!=NULL)
			{
				free(oldimg.data);
				oldimg.data=NULL;
			}
			copy_image_t(img,&oldimg);
			/********************
			 * Do we need to reconfigure the window and back buffer?
			 *********************/
			if (oldimg.w!=screen->w || oldimg.h!=screen->h)
			{
				SDL_FreeSurface(backbuffer);
				backbuffer=SDL_CreateRGBSurface(SDL_SWSURFACE,
						oldimg.w,oldimg.h,4*8,rmask,gmask,bmask,0);
				if (backbuffer==NULL)
				{
					fprintf(stderr,"Error allocating back buffer.\n");
					exit(1);
				}
				/* we need to reconfigure the window */
				screen=SDL_SetVideoMode(oldimg.w,oldimg.h,colordepth,SDL_SWSURFACE);
				if (screen==NULL)
				{
					fprintf(stderr,"Unable to %dx%d: %s\n",oldimg.w,oldimg.h,SDL_GetError());
					exit(1);
				}
				SDL_WM_SetCaption(winname,iconname);
			}

			if (oldimg.bands==3 || oldimg.bands==4) {
				int i;
				unsigned char *data=oldimg.data;
				unsigned char *pixels=backbuffer->pixels;
				for (i=0;i<oldimg.w*oldimg.h;i++) {
					*pixels++=*data++;
					*pixels++=*data++;
					*pixels++=*data++;
					*pixels++=0;
				}
				SDL_SetAlpha(backbuffer,0,255);
			} else {
				int i;
				unsigned char *data=oldimg.data;
				unsigned char *pixels=backbuffer->pixels;
				for (i=0;i<oldimg.w*oldimg.h;i++)
				{
					*pixels++=*data;
					*pixels++=*data;
					*pixels++=*data;
					*pixels++=0;
					data++;
				}
				SDL_SetColors(backbuffer,grayscale_palette,0,256);
				SDL_SetColors(screen,grayscale_palette,0,256);
			}

			/* draw the image */
			SDL_BlitSurface(backbuffer,NULL,screen,NULL);
			SDL_UpdateRect(screen,0,0,screen->w,screen->h);
		}
		SDL_Delay(20); /* max of 50fps */
	}

	SDL_Quit();
	fprintf(stderr,"\n");
	return EXIT_SUCCESS;
}
