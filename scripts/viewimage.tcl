#!/usr/bin/env wish

#package provide viewimage 1.0
#package require Tk 8.4

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


proc viewimage {img} {
	set t [toplevel [toplevelNameFromImg $img]]
	set c [canvas [canvasNameFromImg $img]]
	set x [scrollbar [xscrollNameFromImg $img] -ori hori -command [list $c xview]]
	set y [scrollbar [yscrollNameFromImg $img] -ori vert -command [list $c yview]]
	$c config -xscrollcommand [list $x set] -yscrollcommand [list $y set]
	set i [image create photo [imageNameFromImg $img]]
	$c create image 0 0 -image $i -anchor nw -tag img
	blitimage $img $i
	grid $c $y -in $t -sticky news
	grid $x -in $t -sticky ew
	grid rowconfig $t 0 -weight 1
	grid columnconfig $t 0 -weight 1
	$c config -scrollregion [$c bbox all]
	$c config -cursor trek
	return $t;
}

proc xblitimage {args} {
	if {[llength $args]==1} {
		::blitimage $args [imageNameFromImg $args]
		set c [canvasNameFromImg $args]
		$c config -scrollregion [$c bbox all]
	} else {
		::blitimage [lindex $args 0] [lindex $args 1]
	}
}

namespace export xblitimage viewimage

}

namespace import ::viewimage::xblitimage ::viewimage::viewimage


