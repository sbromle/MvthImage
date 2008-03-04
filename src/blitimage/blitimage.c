#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <tk.h>
#include "base/images_types.h"
#include "base/mvthimagestate.h"

/* this command expects to be passed a photo image,
 * and will fill the image in with backbuffered image.
 */
int blitImage(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	Tk_PhotoHandle photo;
	Tk_PhotoImageBlock dst;
	image_t *img=NULL;
	MvthImage *mimg=NULL;
	const char *str;

	if (objc!=3)
	{
		Tcl_WrongNumArgs(interp,1,objv,"?img var? ?tk photo?");
		return TCL_ERROR;
	}
	/* get the image variable */
	if (getMvthImageFromObj(interp,objv[1],&mimg)!=TCL_OK)
		return TCL_ERROR;

	img=mimg->img;

	/* get the Tk Photo */
	str=Tcl_GetStringFromObj(objv[2],NULL);
	if (str==NULL) return TCL_ERROR;
	photo = Tk_FindPhoto(interp,str);
	if (photo==NULL)
	{
		Tcl_AppendResult(interp,"can't find photo\"",str,"\"",(char*)NULL);
		return TCL_ERROR;
	}

	/* ok, we now have a handle on the photo */
#if TCL_MINOR_VERSION <= 4
	Tk_PhotoSetSize(photo,img->w,img->h);
#else
	Tk_PhotoSetSize(interp,photo,img->w,img->h);
#endif
	Tk_PhotoGetImage(photo,&dst);
	/* Try to just overwrite the exiting data */
	if (dst.pixelPtr==NULL) {
		fprintf(stderr,"Allocating Tcl pixels.\n");
		dst.pixelSize=img->bands*sizeof(unsigned char);
		dst.pitch=img->w*dst.pixelSize;
		dst.width=img->w;
		dst.height=img->h;
		dst.offset[0]=1;
		dst.offset[1]=1;
		dst.offset[2]=1;
		dst.offset[3]=0;
		dst.pixelPtr=(unsigned char*)malloc(dst.height*dst.pitch);
	}
	int i,j,k;
	for (j=0;j<dst.height;j++)
	{
		for (i=0;i<dst.width;i++)
		{
			if (img->bands==1)
			{
				int v=(int)(255*img->data[j*img->w+i]);
				if (v<0) v=0;
				else if (v>255) v=255;
				for (k=0;k<3;k++)
					dst.pixelPtr[j*dst.pitch+i*dst.pixelSize+dst.offset[k]]=(unsigned char)v;
			}
			else {
				for (k=0;k<img->bands;k++)
				{
					int v=(int)(255*img->data[img->bands*(j*img->w+i)+k]);
					if (v<0) v=0;
					else if (v>255) v=255;
					dst.pixelPtr[j*dst.pitch+i*dst.pixelSize+dst.offset[k]]=(unsigned char)v;
				}
				for (k=img->bands;k<3;k++)
					dst.pixelPtr[j*dst.pitch+i*dst.pixelSize+dst.offset[k]]=0;
			}
			dst.pixelPtr[j*dst.pitch+i*dst.pixelSize+dst.offset[3]]=255;
		}
	}
#if TCL_MINOR_VERSION <= 4
	Tk_PhotoPutBlock(photo,&dst,0,0,img->w,img->h,TK_PHOTO_COMPOSITE_SET);
#else
	Tk_PhotoPutBlock(interp,photo,&dst,0,0,img->w,img->h,TK_PHOTO_COMPOSITE_SET);
#endif
	return TCL_OK;
}

