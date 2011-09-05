#ifndef VARSTATE_H
#define VARSTATE_H

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

#ifdef __cplusplus
extern "C" {
#endif 
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <tcl.h>

/* generic state management structure. Maps var names to blobs.
 * Created once per interpreter */
typedef struct StateManager_s *StateManager_t;

extern int varExists0(StateManager_t statePtr,char *name);

extern int varExistsTcl(Tcl_Interp *interp,
		StateManager_t statePtr,
		Tcl_Obj *CONST name);

extern int varNames(Tcl_Interp *interp, StateManager_t statePtr);
/* generate a uniqe variable name */
extern int varUniqName(Tcl_Interp *interp, StateManager_t statePtr, char *name);

/* enumeration for registration modes */
typedef enum {REG_VAR_DELETE_OLD,
	REG_VAR_IGNORE_OLD} REG_VAR_MODE;

/* register data with state manager under name */
extern int
	registerVar(Tcl_Interp *interp, StateManager_t statePtr, ClientData data, char *name, REG_VAR_MODE mode);

extern int getVarFromObj(ClientData clientData, Tcl_Interp *interp, Tcl_Obj *CONST name,
		void **iPtrPtr);
extern int getVarFromObjKey(const char *state_key, Tcl_Interp *interp, Tcl_Obj *CONST name,
		void **iPtrPtr);

/* function to initialize state for a variable type */
extern int InitializeStateManager(Tcl_Interp *interp, const char *key,
		const char *cmd_name,
		int (*unknownCmd)(ClientData,Tcl_Interp*,int,Tcl_Obj *CONST objv[]),
		void (*deleteProc)(void *ptr));

#ifdef __cplusplus
}
#endif 
#endif
