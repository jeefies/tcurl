# TCurl
**A package provide an extension for tcl to use libcurl to get resource from internet**

## Build
__Requirements__: libcurl and tcl head files  
Type `make` does not build an extension for tcl.  
`make tcl-shared` instead.  

## Command
All command are in ::tc namespace.  
So if you want to use urlget, for instance, you should use command `::tc::urlget _URL_`
### urlget url
This is the simplest command that just return the content gotten.  
If there's any error occured, it would print the message out and return the message with prefix: `Get Error:`

### setheader _HEADER_ _CONTENT_
Before urlget, you can set the header just for next time request.  
After a urlget method called, all header information would be cleared.  

### getheader
Return a list like {HEADER1 CONTENT1 HEADER2 CONTENT2 ...}  
You can use `array set _variable_ [::tc::getheader]` to make this to a array.
