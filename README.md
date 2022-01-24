# TCurl
**A package provide an extension for tcl to use libcurl to get resource from internet**

## Build
__Requirements__: libcurl and tcl head files  
Type `make` does not build an extension for tcl.  
`make tcl-shared` instead.  

## Command
All command are in ::tc namespace.  
So if you want to use urlget, for instance, you should use command `::tc::urlget _URL_`, for instance.

### ok
If not specific value is returned, all commands returns an errorCode from this function.  
It's a int in `TCURL_ERR_CURL`, `TCURL_ERR_MULTIPROCESS`, `TCURL_ERR_FILEOPEN`, `TCURL_ERR_THREAD` (in C code, in Tcl code, `TCURL_ERR_CURL` is named like `::tc::ERR_CURL`).  
Or it means *OK* if return a non-zero number.  
### errorCode
It's like what `ok` does.  

### error
Show the detail error information about the error occured.  

### urlget _URL_
This is the simplest command that just return the content gotten.  
If there's any error occured, it would print the message out and return the message with prefix: `Get Error:`

### setHeader _HEADER_ _CONTENT_
Before urlget, you can set the header just for next time request.  
After a urlget method called, all header information would be cleared.  

### getHeader
Return the Request Header Content you set by `setheader` method  
Return a list like {HEADER1 CONTENT1 HEADER2 CONTENT2 ...}  
You can use `array set VARIABLE [::tc::getheader]` to make this to a array.  

### getRspHeader
Return the Response Header Content got from the url.  
It's also a list like the `getheader` returned.  

### urlhead _URL_
Instead of receiving the content, urlhead just perform HEAD request.  
Only returns the head data list from thr url.  
Like what `getRspHeader` returns.

- - -
> The following commands do nothing with the commands above ^\_^.
### urlAget _URL_
With `A` prefix, no doubt it's a asynchronous method.  
It will return as Header Data received.  
So the returned content is like what `urlhead` returns.  

### urlAsize
Return the data size received from previous `urlAget` call.  

### urlAfinish
Return a number to show if previous urlAget call is finished.  
Return a constant in one of `::tc::OK`, `::tc::ERROR`, `::tc::NOT_FINISH`.  
So most like, a process printed might be like:
```tcl
while {[::tc::urlAfinish] == $::tc::NOT_FINISH} {
	set size [::tc::urlAsize]
	puts -nonewline "\rGet $size bytes, progress [format "%.2f" [expr {$size / double($length)}]]"
	after 3 ;# wait for a while
}
```

### setfn _FILENAME_
Set the file name that the data should save to.  
If not filename is given before a `urlAget` call, the filename would automatically set to TMP (a variable in environmet variables, or `/tmp` is the default) + the hash code of the URL (Hash use time33 algorithm).

### getfn
Return the name of the file the data would save to.  
