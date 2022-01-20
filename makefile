CC = gcc
CFLAGS = `curl-config --cflags --libs`
SRC = ./src
INC = ./include

all: $(SRC)/main.c buffer.o tcurl.o
	$(CC) $^ -o curl.out $(CFLAGS) -I$(INC)

tcl-share: buffer.o tcurl.o $(SRC)/tcl-apis.c
	$(CC) -shared $^ -o libTCurl.so \
		-I$(INC) $(CFLAGS) -g
	@echo "pkg_mkIndex . *.so" > mkIndex.tcl
	@tclsh mkIndex.tcl
	@rm mkIndex.tcl

tcurl.o: $(SRC)/tcurl.c $(INC)/tcurl.h
	$(CC) -c $(SRC)/tcurl.c -I$(INC) -o $@ -fPIC

buffer.o: $(SRC)/buffer.c $(INC)/buffer.h
	$(CC) -c $(SRC)/buffer.c -I$(INC) -o $@

clean:
	rm -rf *.o *.so \
		*.html \
		curl
