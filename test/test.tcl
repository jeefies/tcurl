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

puts "tcl.tk Rsp Headers:"
array set heads [::tc::getRspHeader]
parray heads
puts ":Rsp Headers end\n"

# puts [::tc::urlhead https://mirrors.tuna.tsinghua.edu.cn/raspberry-pi-os-images/AstroPi/images/AstroPi-2021-09-13/kitOS.20210902.zip.sha1]
# puts [::tc::urlhead mirrors.tuna.tsinghua.edu.cn/raspberry-pi-os-images/AstroPi/images/AstroPi-2021-09-13/kitOS.20210902.zip.sha1]
puts "mirrors.tuna.tsinghua.edu.cn Rsp Headers:"
array set heads [::tc::urlhead mirrors.tuna.tsinghua.edu.cn]
parray heads
puts ":Rsp Headers end"

puts "Recv larger file:"
array set heads [::tc::urlAget https://mirrors.tuna.tsinghua.edu.cn/raspberry-pi-os-images/imager/imager_1.5_amd64.AppImage]
set length $heads(content-length)
puts "Content-Length: $length"
puts "File save at [::tc::getfn]"
while {[::tc::urlAfinish] == $::tc::NOT_FINISH} {
	set size [::tc::urlAsize]
	puts -nonewline "\rGet $size, progress [format "%.2f" [expr {$size / double($length)}]]"
	after 3
}
