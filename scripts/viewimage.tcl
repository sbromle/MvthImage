#!/usr/bin/env wish

package provide viewimage 1.0
package require Tk 8.4


puts stdout {viewimage Copyright (C) 2009 Sam Bromley}
puts stdout {This software comes with ABSOLUTELY NO WARRANTY.}
puts stdout {This is free software, and you are welcome to}
puts stdout {redistribute it under certain conditions.}
puts stdout {For details, see the GNU Lesser Public License V.3 <http://www.gnu.org/licenses>.}

namespace eval ::viewimage {

proc toplevelNameFromImg {img} {
	return .tl_$img
}

proc canvasNameFromImg {img} {
	return [toplevelNameFromImg $img].c
}

proc xscrollNameFromImg {img} {
	return [toplevelNameFromImg $img].x
}

proc yscrollNameFromImg {img} {
	return [toplevelNameFromImg $img].y
}

proc imageNameFromImg {img} {
	return my#image_$img
}

proc viewimage_frame {fname img {blitcmd "blitimage"} {labeltext ""}} {
	set f [labelframe $fname];
	if {[string length $labeltext]>0} {
		$f configure -text $labeltext;
	}
	puts "canvasNameFromImg = [canvasNameFromImg $img]"
	set c [canvas [canvasNameFromImg $img]]
	set x [scrollbar [xscrollNameFromImg $img] -ori hori -command [list $c xview]]
	set y [scrollbar [yscrollNameFromImg $img] -ori vert -command [list $c yview]]
	$c config -xscrollcommand [list $x set] -yscrollcommand [list $y set]
	set i [image create photo [imageNameFromImg $img]]
	$c create image 0 0 -image $i -anchor nw -tag img
	$blitcmd $img $i
	grid $c $y -in $f -sticky news
	grid $x -in $f -sticky ew
	grid rowconfig $f 0 -weight 1
	grid columnconfig $f 0 -weight 1
	$c config -scrollregion [$c bbox all]
	$c config -cursor trek
	return $f;
}

proc viewimage {img {blitcmd "blitimage"} {toplevel_name {}}} {
	if {[string length $toplevel_name]==0} {
		set toplevel_name [toplevelNameFromImg $img];
	}
	set t [toplevel $toplevel_name];
	set f [viewimage_frame $t.topframe $img $blitcmd ""];
	pack $f -in $t -side top -anchor nw -expand 1 -fill both;
	return $t;
}

proc xblitimage {img {photo {}} {blitcmd ::blitimage}} {
	if {[string length $photo]==0} {
		set photo [imageNameFromImg $img];
	}
	$blitcmd $img $photo;
	set c [canvasNameFromImg $img]
	if {[winfo exists $c]} {
		$c config -scrollregion [$c bbox all]
	}
}

namespace export xblitimage viewimage

}

namespace import ::viewimage::xblitimage ::viewimage::viewimage


