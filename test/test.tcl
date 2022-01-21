lappend auto_path [file normal .]
package require tcurl

::tc::setHeader {User-Agent} {Mozilla/5.0 (X11; Linux aarch64; rv:78.0) Gecko/20100101 Firefox/78.0}
array set heads [::tc::getHeader]
parray heads

set r [::tc::urlget tcl.tk]
puts "Get tcl.tk ok\n"
set f [open "index.html" wb]
puts -nonewline $f $r
close $f

puts "Rsp Headers:"
puts [::tc::getRspHeader]
puts ":Rsp Headers end"

# puts [::tc::urlhead https://mirrors.tuna.tsinghua.edu.cn/raspberry-pi-os-images/AstroPi/images/AstroPi-2021-09-13/kitOS.20210902.zip.sha1]
# puts [::tc::urlhead mirrors.tuna.tsinghua.edu.cn/raspberry-pi-os-images/AstroPi/images/AstroPi-2021-09-13/kitOS.20210902.zip.sha1]
array set heads [::tc::urlhead mirrors.tuna.tsinghua.edu.cn]
parray heads
