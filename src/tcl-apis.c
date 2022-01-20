/*
 * tcl-apis.c --
 *
 *  This file provide the Api that can use in tcl script.
 */

#include <tcl.h>
#include <string.h>
#include "tcurl.h"

int abs(int x){return (x < 0)?-x:x;}

static int urlgetCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	char * url;
	Tcl_Obj *result;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "need an argument for url");
		return TCL_ERROR;
	}

	url = Tcl_GetString(objv[1]);

	Buffer * buf = get(url);
	if (buf == NULL) {
		result = Tcl_NewStringObj(scurl_err(), scurl_err_l());
		Tcl_SetObjResult(interp, result);
		return TCL_ERROR;
	}
	int length = buf->length;

	result = Tcl_NewStringObj(buf->buf, length);
	Tcl_SetObjResult(interp, result);

	buffer_close(buf);
	return TCL_OK;
}

static int setheaderCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	char *title, *content;
	Tcl_Obj *result;

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "should be ::tc::setheader head content");
	}

	title = Tcl_GetString(objv[1]);
	content = Tcl_GetString(objv[2]);
	scurl_add_header(title, content);

	result = Tcl_NewIntObj(1);
	return TCL_OK;
}

static int getheaderCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj  *CONST objv[]) {
	printf("Get header!\n");
	Tcl_Obj *rList = Tcl_NewObj();
	Tcl_SetObjResult(interp, rList);

	scurl_slist * headers = scurl_get_headers();

	if (headers == NULL) return TCL_ERROR;

	do {
		char *data = headers->data;
		char *sep = strchr(data, ':');
		char title_l  = abs(data - sep);

		Tcl_Obj *title_obj = Tcl_NewStringObj(data, title_l);
		// if only +1 that would ignore the ':', but +2 to ignore ':' and the following space
		Tcl_Obj *content_obj = Tcl_NewStringObj(sep + 2, strlen(sep) - 2);
		Tcl_ListObjAppendElement(interp, rList, title_obj);
		Tcl_ListObjAppendElement(interp, rList, content_obj);

		headers = headers->next;
	} while (headers != NULL);

	return TCL_OK;
}

// It could not be TCurl_Init
int Tcurl_Init(Tcl_Interp *interp) {
	// declare the name and the version of the package
	Tcl_PkgProvide(interp, "tcurl", "1.0");

	// All Tcurl command is in tc namespace
	Tcl_CreateNamespace(interp, "tc", (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

	// Every command would be create here
	// Tcl_CreateObjCommand(interp, cmdName, proc, clientData, deleteProc)
	// proc's name is always cmdName plus "Cmd" at last
	Tcl_CreateObjCommand(interp, "::tc::urlget", urlgetCmd,
			(ClientData) NULL, (Tcl_CmdDeleteProc *)NULL);

	Tcl_CreateObjCommand(interp, "::tc::setheader", setheaderCmd,
			(ClientData) NULL, (Tcl_CmdDeleteProc *)NULL);

	Tcl_CreateObjCommand(interp, "::tc::getheader", getheaderCmd,
			(ClientData) NULL, (Tcl_CmdDeleteProc *)NULL);
	return TCL_OK;
}
