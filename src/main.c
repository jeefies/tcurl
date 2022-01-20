#include <curl/curl.h>
#include "tcurl.h"

int main() {
	scurl_init();
	printf("Curl version info: %s\n", curl_version());

	Buffer * buf = get("http://tcl.tk");

	buffer_tofile(buf, "tcl.tk.index.html", "wb");
	buffer_close(buf);
	
	printf("Save http://tcl.tk/ html page to tcl.tk.index.html");
	scurl_quit();
	return 0;
}
