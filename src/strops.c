#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "strops.h"

#define abs(x) ((x)<0)?-(x):(x)


tc_str str_trim(tc_str str) {
	tc_str rstr;

	int length = strlen(str);

	int seek = 0;
	// isspace include in ctype.h
	while (isspace(str[seek])) {
		seek++;
		if (seek >= length - 1) {
			return "";
		}
	}

	int end = length - 1;
	while (isspace(str[end])) {
		end--;
	}
	end++;

	int size = sizeof(char) * (end - seek + 1);
	rstr = (tc_str)malloc(size);
	memset(rstr, '\0', size);
	strncpy(rstr, str+seek, end - seek);

	return rstr;
}

tc_str str_triml(tc_str str, int length) {
	tc_str tstr = (tc_str)malloc(sizeof(char) * (length + 1));
	tstr[length] = '\0';
	memcpy(tstr, str, length);
	tc_str rstr = str_trim(tstr);
	str_free(tstr);
	return rstr;
}

tc_strs str_split(tc_str _str, char s, int *count) {
	tc_strs rstrs = (tc_strs)malloc(sizeof(char*) * 1);
	tc_str str = _str;

	int nl = 0;
	int ni = 0;
	int slength = strlen(str);
	for (int i = 0; i < slength; i++) {
		if (_str[i] == s) {
			int size = sizeof(char) * (nl + 1);
			rstrs[ni] = (tc_str)malloc(size);
			rstrs[ni][nl] = '\0';
			memcpy(rstrs[ni], str, nl);
			str += nl + 1;
			nl = 0;
			ni++;
			rstrs = (tc_strs)realloc(rstrs, sizeof(char *) * (ni + 1));
		} else {
			nl++;
		}
	}
	int size = sizeof(char) * (nl + 1);
	rstrs[ni] = (tc_str)malloc(size);
	rstrs[ni][nl] = '\0';
	memcpy(rstrs[ni], str, nl);

	if (count != NULL) {
		*count = ni + 1;
	}

	return rstrs;
}

tc_strs str_sep(tc_str str, char sep) {
	int length, mlength;
	tc_strs rstrs = (tc_strs)malloc(sizeof(tc_str) * 2);

	tc_str mid = strchr(str, sep);
	if (mid == NULL) {
		length = strlen(str);
		rstrs[0] = (tc_str)malloc(sizeof(char) * (length + 1));
		rstrs[0][length] = '\0';
		memcpy(rstrs[0], str, length);

		rstrs[1] = (tc_str)malloc(sizeof(char));
		rstrs[1][0] = '\0';
		return rstrs;
	}
	length = abs(mid - str);
	mlength = strlen(mid);


	rstrs[0] = (tc_str)malloc(sizeof(char) * (length + 1));
	rstrs[0][length] = '\0';
	memcpy(rstrs[0], str, length);

	rstrs[1] = (tc_str)malloc(sizeof(char) * mlength);
	rstrs[1][mlength - 1] = '\0';
	memcpy(rstrs[1], mid+1, mlength);

	return rstrs;
}

void str_foreach(tc_strs * strs, tc_str(*operation)(tc_str), int length) {
	for (int i = 0; i < length; i++) {
		(*strs)[i] = operation((*strs)[i]);
	}
}

tc_str str_free(tc_str str) {
	if (str != NULL) {
		free(str);
	}
	return NULL;
}

void strs_free(tc_strs strs, int length) {
	str_foreach(&strs, str_free, length);
	free(strs);
}
