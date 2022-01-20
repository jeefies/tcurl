lappend auto_path [file normal .]
package require tcurl

::tc::setheader {User-Agent} {Mozilla/5.0 (X11; Linux aarch64; rv:78.0) Gecko/20100101 Firefox/78.0}
array set heads [::tc::getheader]
parray heads

set r [::tc::urlget tcl.tk]
puts "Get tcl.tk ok\n"
set f [open "index.html" wb]
puts -nonewline $f $r
close $f
