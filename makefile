CC = gcc
CFLAGS = -g
CURL_LIBS = `curl-config --cflags --libs`
SRC = ./src
INC = ./include

all: $(SRC)/main.c buffer.o tcurl.o
	$(CC) $^ -o curl.out $(CFLAGS) -I$(INC) $(CURL_LIBS)

tcl-share: buffer.o tcurl.o strops.o $(SRC)/tcl-apis.c
	$(CC) -shared $^ -o libTCurl.so \
		-I$(INC) $(CFLAGS) $(CURL_LIBS)
	@echo "pkg_mkIndex . *.so" > mkIndex.tcl
	@tclsh mkIndex.tcl
	@rm mkIndex.tcl

tcurl.o: $(SRC)/tcurl.c $(INC)/tcurl.h
	$(CC) -c $(SRC)/tcurl.c -I$(INC) -o $@ $(CFLAGS) -fPIC

buffer.o: $(SRC)/buffer.c $(INC)/buffer.h
	$(CC) -c $(SRC)/buffer.c -I$(INC) -o $@ $(CFLAGS)

strops.o: $(SRC)/strops.c $(INC)/strops.h
	$(CC) -c $(SRC)/strops.c -I$(INC) -o $@ $(CFLAGS) -fPIC

test-strops: strops.o test/test-strops.c
	$(CC) strops.o test/test-strops.c -o test-strops -I$(INC)

clean:
	@# three lines are for:
	@# build cache
	@# test cache
	@# executables
	rm -rf *.o *.so \
		*.html \
		test-* curl *.out
