/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Maintain variable contexts on Tcl side.
 *
 * Copyright (C) 2003,2004,2011 Samuel P. Bromley <sam@sambromley.com>
 *
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MVTH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License Version 3,
 * as published by the Free Software Foundation.
 *
 * MVTH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * (see the file named "COPYING"), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <tcl.h>
#include "variable_state.h"

/* generic state management structure. Maps var names to blobs.
 * Created once per interpreter */
struct StateManager_s {
	Tcl_HashTable hash; /* list of variables by name */
	int uid;
	char *prefix;
	void (*deleteProc)(void *ptr);
	int (*unknownCmd)(ClientData, Tcl_Interp *,int,Tcl_Obj *CONST objv[]);
};

/* this is called when the command associated with a state is destroyed.
 * The hash table is walked, destroying all variables as
 * you go, and then the HashTable itself is freed */
void StateManagerDeleteProc(ClientData clientData) {
	Tcl_HashEntry *entryPtr;
	Tcl_HashSearch search;
	void *iPtr=NULL;
	StateManager_t state=(StateManager_t)clientData;
	if (state==NULL) return;
	if (state->deleteProc==NULL) return;

	entryPtr=Tcl_FirstHashEntry(&state->hash,&search);
	while (entryPtr!=NULL) {
		iPtr=Tcl_GetHashValue(entryPtr);
		state->deleteProc(iPtr);
		Tcl_DeleteHashEntry(entryPtr);
		/* get the first entry again, not the next one, since
		 * the previous first one is now deleted. */
		entryPtr=Tcl_FirstHashEntry(&state->hash,&search);
	}
	ckfree((char*)state);
	return;
}

int varExists0(StateManager_t statePtr,
		char *name)
{
	Tcl_HashEntry *entryPtr=NULL;
	if (name==NULL) return 0;
	entryPtr=Tcl_FindHashEntry(&statePtr->hash,name);
	if (entryPtr==NULL) return 0;
	return 1;
}

int varExistsTcl(Tcl_Interp *interp,
		StateManager_t statePtr,
		Tcl_Obj *CONST name)
{
	if (varExists0(statePtr,Tcl_GetString(name)))
	{
		Tcl_SetObjResult(interp,Tcl_NewIntObj(1));
		return TCL_OK;
	}
	Tcl_SetObjResult(interp,Tcl_NewIntObj(0));
	return TCL_OK;
}

int varNames(Tcl_Interp *interp, StateManager_t statePtr)
{
	Tcl_HashEntry *entryPtr;
	Tcl_HashSearch search;
	Tcl_Obj *listPtr;
	char *name;
	/* Walk the hash table and build a list of names */
	listPtr=Tcl_NewListObj(0,NULL);
	entryPtr=Tcl_FirstHashEntry(&statePtr->hash,&search);
	while (entryPtr!=NULL) {
		name=Tcl_GetHashKey(&statePtr->hash,entryPtr);
		if (Tcl_ListObjAppendElement(interp,listPtr,
					Tcl_NewStringObj(name,-1))!=TCL_OK) return TCL_ERROR;
		entryPtr=Tcl_NextHashEntry(&search);
	}
	Tcl_SetObjResult(interp,listPtr);
	return TCL_OK;
}

int varUniqName(Tcl_Interp *interp, StateManager_t statePtr, char *name)
{
	char tmp[1024]="";
	if (statePtr==NULL) return TCL_ERROR;
	while (1) {
		snprintf(tmp,sizeof(tmp),statePtr->prefix,statePtr->uid);
		if (!varExists0(statePtr,tmp)) {
			strcpy(name,tmp);
			return TCL_OK;
		}
		/* otherwise, increment the uid and try again */
		statePtr->uid++;
	};
	return TCL_ERROR;
}

int registerVar(Tcl_Interp *interp, StateManager_t statePtr,
		ClientData data, char *name,
		REG_VAR_MODE mode)
{
	int new;
	Tcl_HashEntry *entryPtr;
	entryPtr=Tcl_CreateHashEntry(&statePtr->hash,name,&new);
	if (new!=1) {
		ClientData old=Tcl_GetHashValue(entryPtr);
		if (old==data) {
			/* nothing to do! This data is already registered with that name. */
			return TCL_OK;
		} else if (mode==REG_VAR_DELETE_OLD) {
			statePtr->deleteProc(old);
		}
	}
	Tcl_SetHashValue(entryPtr,(ClientData)data);
	return TCL_OK;
}

int getVarFromObj(ClientData clientData, Tcl_Interp *interp, Tcl_Obj *CONST name,
		void **iPtrPtr)
{
	Tcl_HashEntry *entryPtr=NULL;
	StateManager_t statePtr=(StateManager_t)clientData;
	void *iPtr=NULL;
	if (clientData==NULL) {
		Tcl_AppendResult(interp,"clientData was NULL",NULL);
		return TCL_ERROR;
	}
	if (name==NULL) {
		Tcl_AppendResult(interp,"name was NULL",NULL);
		return TCL_ERROR;
	}
	if (iPtrPtr==NULL) {
		Tcl_AppendResult(interp,"iPtrPtr was NULL",NULL);
		return TCL_ERROR;
	}

	entryPtr=Tcl_FindHashEntry(&statePtr->hash,Tcl_GetString(name));
	if (entryPtr==NULL) {
		Tcl_AppendResult(interp,"Unknown var: ", Tcl_GetString(name),NULL);
		return TCL_ERROR;
	}
	iPtr=(void*)Tcl_GetHashValue(entryPtr);
	*iPtrPtr=iPtr;
	return TCL_OK;
}

int getVarFromObjKey(const char *state_key, Tcl_Interp *interp, Tcl_Obj *CONST name,
		void **iPtrPtr)
{
	StateManager_t statePtr=(StateManager_t)Tcl_GetAssocData(interp,state_key,NULL);
	if (statePtr==NULL) {
		Tcl_AppendResult(interp,"No state stored by key `",state_key,"'\n",NULL);
		return TCL_ERROR;
	}
	return getVarFromObj(statePtr,interp,name,iPtrPtr);
}


/* StateManagerCmd --
 * This implements the StateManager command, which has these subcommands:
 * 	names ?pattern?
 * 	exists ?name?
 * 	delete name
 *
 * Results:
 *  A standard Tcl command result.
 */
int StateManagerCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	StateManager_t statePtr=(StateManager_t)clientData;
	void *iPtr=NULL;
	Tcl_HashEntry *entryPtr=NULL;
	Tcl_Obj *valueObjPtr=NULL;

	/* the subCmd array defines the allowed values for
	 * the subcommand.
	 */
	CONST char *subCmds[] = {
		"delete","exists","names",NULL};
	enum StateCmdIx {
		DeleteIx, ExistsIx, NamesIx};
	int index;

	if (objc==1 || objc>=6) {
		Tcl_WrongNumArgs(interp,1,objv,"option ?arg ...?");
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp,objv[1],subCmds,"option",0,&index)!=TCL_OK)
		return statePtr->unknownCmd(clientData,interp,objc,objv);

	switch (index) {
		case ExistsIx:
			if (objc!=3) goto err;
			return varExistsTcl(interp,statePtr,objv[2]);
			break;
		case NamesIx:
			if (objc>2) goto err;
			return varNames(interp,statePtr);
			break;
		case DeleteIx:
			if (objc!=3) goto err;
			entryPtr=Tcl_FindHashEntry(&statePtr->hash,Tcl_GetString(objv[2]));
			if (entryPtr==NULL) {
				Tcl_AppendResult(interp,"Unknown var: ",
						Tcl_GetString(objv[2]),NULL);
				return TCL_ERROR;
			}
			iPtr=Tcl_GetHashValue(entryPtr);
			statePtr->deleteProc(iPtr);
			Tcl_DeleteHashEntry(entryPtr);
			return TCL_OK;
			break;
		default:
			return statePtr->unknownCmd(statePtr,interp,objc,objv);
			break;
	}
	return TCL_OK;
err:
	Tcl_WrongNumArgs(interp,1,objv,"option ?arg ...?");
	return TCL_ERROR;
}

/* function to initialize state for a variable type */
int InitializeStateManager(Tcl_Interp *interp, const char *key,
		const char *cmd_name,
		int (*unknownCmd)(ClientData,Tcl_Interp*,int,Tcl_Obj *CONST objv[]),
		void (*deleteProc)(void *ptr))
{
	StateManager_t state=NULL;
	if (NULL!=Tcl_GetAssocData(interp,key,NULL)) return TCL_OK;
	/* otherwise, we need to create a new context and associate it with
	 * the Tcl interpreter.
	 */
	state=(StateManager_t)ckalloc(sizeof(struct StateManager_s));
	Tcl_InitHashTable(&state->hash,TCL_STRING_KEYS);
	Tcl_SetAssocData(interp,key,NULL,(ClientData)state);
	state->uid=0;
	state->deleteProc=deleteProc;
	state->unknownCmd=unknownCmd;
	Tcl_CreateObjCommand(interp,cmd_name, StateManagerCmd, (ClientData)state,StateManagerDeleteProc);
	int len=strlen(cmd_name);
	state->prefix=(char*)ckalloc(len+6);
	sprintf(state->prefix,"%s%s",cmd_name,"#%04d");
	return TCL_OK;
}

/** Example usage:
*
* 	int MvthImageState_Init(Tcl_Interp *interp) {
* 		InitializeStateManager(interp,MVTHIMAGESTATEKEY,"mi",
* 		   MvthImageCmd,MvthImageDelete);
* 		return TCL_OK;
* 	}
* 
* So here we pass InitializeStateManager:
* + A string defined in MVTHIMAGESTATEKEY
*   that will be the string we use to re-attach to this stateManager from
*   a Tcl Interpreter.
* + A command name "mi", which will be the name of a new command that allows
*   the user to query the new state manager.
* + MvthImageCmd which is a pointer to a command that will handle
*   calls to the new "mi" command that do not match the default subcommands.
* + MvthImageDelete which is a pointer to a function that can delete
*   the specific data structures stored in the hash table.
*/
