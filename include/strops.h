#ifndef _STROPS_C_H_
#define _STROPS_C_H_
#include <string.h>

typedef char* tc_str;
typedef tc_str* tc_strs;

// To del the white space before and after the main content
tc_str str_trim(tc_str str);
tc_str str_triml(tc_str str, int length);

// To split the str into many substrs sep by char 's'
// Remember to free every of them
tc_strs str_split(tc_str str, char s, int *count);
// It would just return two strs, split by the first sep
tc_strs str_sep(tc_str str, char sep);
// It will always return NULL
tc_str str_free(tc_str str);
void strs_free(tc_strs strs, int length);

void str_foreach(tc_strs * strs, tc_str(*operation)(tc_str), int length);
#endif
