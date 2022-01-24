#include <curl/curl.h>
#include "tcurl.h"
#include <unistd.h>

int main() {
	tcurl_init();
	printf("Curl version info: %s\n", curl_version());

	// Buffer * buf = tcurl_get("http://tcl.tk");

	// buffer_tofile(buf, "tcl.tk.index.html", "wb");
	// buffer_close(buf);
	// 
	// printf("Save http://tcl.tk/ html page to tcl.tk.index.html");
	
	Buffer * header = tcurl_aget("http://tcl.tk");
	if (header == NULL) {
		printf("Some Error occured: %s\n", tcurl_err());
	}
	printf("File save at %s\n", tcurl_getfn());

	while (tcurl_afinish() == TCURL_NOTFINISH) {
		sleep(1);
		printf("size: %d\n", tcurl_get_size());
	}
	printf("OK: errCode = %d\n", tcurl_err_code());

	tcurl_quit();
	return 0;
}
