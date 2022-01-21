/*
 * tcl-apis.c --
 *
 *  This file provide the Api that can use in tcl script.
 */

#include <string.h>
#include <stdlib.h>

#include <tcl.h>

#include "tcurl.h"
#include "strops.h"

#define abs(x) (x<0)?-(x):(x)

static int urlgetCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	char * url;
	Tcl_Obj *result;
	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "need an argument for url");
		return TCL_ERROR;
	}

	url = Tcl_GetString(objv[1]);

	Buffer * buf = tcurl_get(url);
	if (buf == NULL) {
		result = Tcl_NewStringObj(tcurl_err(), tcurl_err_l());
		Tcl_SetObjResult(interp, result);
		return TCL_ERROR;
	}
	int length = buf->length;

	result = Tcl_NewStringObj(buf->buf, length);
	Tcl_SetObjResult(interp, result);

	buffer_close(buf);
	return TCL_OK;
}

static int setHeaderCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	char *title, *content;
	Tcl_Obj *result;

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "should be ::tc::setheader head content");
		return TCL_ERROR;
	}

	title = Tcl_GetString(objv[1]);
	content = Tcl_GetString(objv[2]);
	tcurl_add_header(title, content);

	result = Tcl_NewIntObj(1);
	return TCL_OK;
}

static int getRspHeaderCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	Buffer * rsp_header = tcurl_get_rsp_header();
	tc_str heads = str_triml(rsp_header->buf, rsp_header->length);
	Tcl_Obj *rstr = Tcl_NewStringObj(heads, strlen(heads));
	str_free(heads);
	Tcl_SetObjResult(interp, rstr);
	return TCL_OK;
}

static int getHeaderCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj  *CONST objv[]) {
	Tcl_Obj *rList = Tcl_NewObj();

	tcurl_slist * headers = tcurl_get_headers();

	if (headers == NULL) return TCL_ERROR;

	do {
		char * data = headers->data;
		tc_strs head = str_sep(data, ':');

		Tcl_Obj *title_obj = Tcl_NewStringObj(head[0], -1);
		// Add one to ignore the space before the content
		Tcl_Obj *content_obj = Tcl_NewStringObj(head[1] + 1, -1);

		strs_free(head, 2);

		Tcl_ListObjAppendElement(interp, rList, title_obj);
		Tcl_ListObjAppendElement(interp, rList, content_obj);

		headers = headers->next;
	} while (headers != NULL);

	Tcl_SetObjResult(interp, rList);
	return TCL_OK;
}

static tc_str str_trim_free(tc_str str) {
	tc_str r = str_trim(str);
	str_free(str);
	return r;
}

static int urlheadCmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[]) {
	Tcl_Obj * result;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "should be ::tc::setheader head content");
		return TCL_ERROR;
	}

	char * url = Tcl_GetString(objv[1]);
	Buffer * header = tcurl_head(url);
	if (header == NULL) {
		result = Tcl_NewStringObj(tcurl_err(), tcurl_err_l());
		Tcl_SetObjResult(interp, result);
		return TCL_ERROR;
	}

	// get header string
	int size = sizeof(char) * (header->length + 1);
	char * heads = (char *)malloc(size);
	heads[header->length] = '\0';
	memcpy(heads, header->buf, header->length);

	// trim the string, remove space after it
	tc_str no_space = str_trim(heads);
	// spare the space
	free(heads);
	int c = 0;
	tc_strs lines = str_split(no_space, '\n', &c);
	// spare the unused place
	str_free(no_space);
	// str_foreach(&lines, str_trim_free, c);

	Tcl_Obj * hd_obj, * ctt_obj;
	result = Tcl_NewObj();
	tc_strs two;
	// Get the result
	for (int i = c - 1; i >= 0; i--) {
		two = str_sep(lines[i], ':');
		if (two[1][0] == '\0') {
			// That means this line is like "HTTP/2 200", "HTTP/1.1 200 ...", for example.
			// So the last ones are useless or just for previous redirect header
			break;
		}
		tc_str ctx = str_trim(two[1]);

		// hd_obj = Tcl_NewStringObj(two[0], strlen(two[0]));
		// ctt_obj = Tcl_NewStringObj(ctx , strlen(ctx));
		ctt_obj = Tcl_NewStringObj(ctx , -1);
		hd_obj = Tcl_NewStringObj(two[0], -1);

		Tcl_ListObjAppendElement(interp, result, hd_obj);
		Tcl_ListObjAppendElement(interp, result, ctt_obj);

		// spare the space
		str_free(ctx);
		strs_free(two, 2);
	}
	strs_free(lines, c);
	Tcl_SetObjResult(interp, result);
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

	Tcl_CreateObjCommand(interp, "::tc::urlhead", urlheadCmd,
			(ClientData) NULL, (Tcl_CmdDeleteProc *)NULL);

	Tcl_CreateObjCommand(interp, "::tc::setHeader", setHeaderCmd,
			(ClientData) NULL, (Tcl_CmdDeleteProc *)NULL);

	Tcl_CreateObjCommand(interp, "::tc::getHeader", getHeaderCmd,
			(ClientData) NULL, (Tcl_CmdDeleteProc *)NULL);

	Tcl_CreateObjCommand(interp, "::tc::getRspHeader", getRspHeaderCmd,
			(ClientData) NULL, (Tcl_CmdDeleteProc *)NULL);
	return TCL_OK;
}
