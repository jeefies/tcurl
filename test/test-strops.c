#include <stdio.h>

#include "strops.h"

tc_str trim(tc_str str) {
	return str_trim(str);
}

int main() {
	tc_str str = "        Some string here";
	printf(str_trim(str));

	tc_str str2 = "  split 1 ;\tsplit 2";
	int c = 0;
	tc_strs slist = str_split(str2, ';', &c);

	printf("count: %d\n", c);
	for (int i = 0; i < c; i++) {
		printf("%s\n", slist[i]);
	}

	str_foreach(&slist, str_trim, c);
	for (int i = 0; i < c; i++) {
		printf("%s\n", slist[i]);
	}
}
