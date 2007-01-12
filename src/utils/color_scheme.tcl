# A little tcl utility that lets you play with
# different data color schemes.
# Copyright (c) Sam Bromley 2006

# Set getColor to return the result of your color scheme function 
proc getColor {i} {
	#return [color_wheel $i]
	return [blue_yellow $i]
}

# a helper routine to form an hsv triple as a Tk_Color #FFFFFF format color
proc format_hsv {args} {
	set rgb [hsv_to_rgb $args]
	set r [format "%.0f" [lindex $rgb 0]]
	set g [format "%.0f" [lindex $rgb 1]]
	set b [format "%.0f" [lindex $rgb 2]]
	#puts "format_hsv got $args"
	#puts "format_hsv rgb =  $r $g $b"
	return [format {#%02x%02x%02x} $r $g $b]
}

# painters wheel color scheme.
proc color_wheel {i} {
	set h [expr {($i*360.0/256.0)}]
	set s 1.0
	set v 1.0
	return [format_hsv $h $s $v]
}

# blue to yellow color scheme 
proc blue_yellow {i} {
	# hue, blue to yellow shading 
	set dh [expr {240-60}]
	set h [expr {240-$i*$dh/256.0}]
	if {0} {
		set h 60; # default is yellow
		if {$i<128} {set h 240.0}; # blue
	}
	# saturation in the middle to smooth the transition
	set gap 50
	set s 1.0
	if {$i>$gap && $i<128} {
		set s [expr {(128-$i+$gap)/128.0}]
	} elseif {$i>=128 && $i<[expr {256-$gap}]} {
		set s [expr {($i+$gap-128)/128.0}]
	}
	# value
	set v [expr {0.2*abs($i-128)/128+0.8}]
	return [format_hsv $h $s $v]
}

# Initially create the canvas rectangles given the canvas
proc makeColorBar {c} {
	for {set i 0} {$i<256} {incr i} {
	  set dx [expr {$w/256.0}]
	  set x0 [expr {$i*$dx}]
	  set y0 0
	  set x1 [expr {$x0+$dx-1}]
		set y1 [expr {$h-1}]
		#set fill "#[format {%02x%02x%02x} $i $i $i]"
		set color [getColor $i]
		$c create rect [list $x0 $y0 $x1 $y1] -fill $color -outline $color -width 0 -tag $i
	}
}

# redraw the color bar
proc drawColorBar {c} {
	for {set i 0} {$i<256} {incr i} {
		set color [getColor $i]
		#$c coords $i [list $x0 $y0 [expr {$x1-1}] $y1]
		$c itemconfigure $i -outline $color
	}
}

# helper routine to convert hsv values to rgb.
proc hsv_to_rgb {args} {
	if {[llength $args]==3} {
		set h [lindex $args 0]
		set s [lindex $args 1]
		set v [lindex $args 2]
	} else {
		set h [lindex $args 0 0]
		set s [lindex $args 0 1]
		set v [lindex $args 0 2]
	}
	#puts "hsv_to_rgb got $args $h $s $v"

	if {$s==0} {
		# then it is just grey
		set r [expr {floor($v*255)}]
		return [list $r $r $r]
	}

	# otherwise convert h to sector
	set h [expr {$h/60.0}]
	#set h 1.0
	set i [expr {floor($h)}]
	set f [expr {$h-$i}]
	set p [expr {$v*(1-$s)}]
	set q [expr {$v*(1-$s*$f)}]
	set t [expr {$v*(1-$s*(1-$f))}]

	if {$i==0} {
		set r [expr {$v*255}]
		set g [expr {$t*255}]
		set b [expr {$p*255}]
	} elseif {$i==1} {
		set r [expr {$q*255}]
		set g [expr {$v*255}]
		set b [expr {$p*255}]
	} elseif {$i==2} {
		set r [expr {$p*255}]
		set g [expr {$v*255}]
		set b [expr {$t*255}]
	} elseif {$i==3} {
		set r [expr {$p*255}]
		set g [expr {$q*255}]
		set b [expr {$v*255}]
	} elseif {$i==4} {
		set r [expr {$t*255}]
		set g [expr {$p*255}]
		set b [expr {$v*255}]
	} else {
		set r [expr {$v*255}]
		set g [expr {$p*255}]
		set b [expr {$q*255}]
	}

	set r [expr {floor($r)}]
	set g [expr {floor($g)}]
	set b [expr {floor($b)}]

	return [list $r $g $b]
}
