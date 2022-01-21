#include <curl/curl.h>
#include "tcurl.h"

int main() {
	tcurl_init();
	printf("Curl version info: %s\n", curl_version());

	Buffer * buf = tcurl_get("http://tcl.tk");

	buffer_tofile(buf, "tcl.tk.index.html", "wb");
	buffer_close(buf);
	
	printf("Save http://tcl.tk/ html page to tcl.tk.index.html");
	tcurl_quit();
	return 0;
}
