#!/usr/bin/env wish

package provide viewimagesnit 1.0
package require Tk 8.5
package require snit;

puts stdout {viewimage Copyright (C) 2012 Sam Bromley}
puts stdout {This software comes with ABSOLUTELY NO WARRANTY.}
puts stdout {This is free software, and you are welcome to}
puts stdout {redistribute it under certain conditions.}
puts stdout {For details, see the GNU Lesser Public License V.3 <http://www.gnu.org/licenses>.}

snit::type ImageViewer {
	variable tl;
	variable can;
	variable myframe;
	variable myphoto;
	option -blitcmd -default "blitimage";
	option -image -default {};
	option -text -default {} -configuremethod _update_label;

	constructor {args} {
		set tl {};
		set can {};
		set myframe {};
		set myphoto {};
		$self configurelist $args;
	}

	method _update_label {option value} {
		if {"$option"!="-text"} {
			return {}
		}
		if {[winfo exists $myframe]} {
			$mframe configure -text $value;
		}
	}

	method toplevelName {} {
		return $tl;
	}

	method setTopLevel {tlname} {
		set tl $tlname;
	}

	method canvasName {} {
		return $can;
	}

	method toplevelNameFromImg {img} {
		return .tl_$img
	}
	
	method canvasNameFromImg {img} {
		return [toplevelNameFromImg $img].c
	}

	method _xscrollNameFromImg {img} {
		return [toplevelNameFromImg $img].x
	}

	method _yscrollNameFromImg {img} {
		return [toplevelNameFromImg $img].y
	}

	method imageNameFromImg {img} {
		return "photo_$img";
	}

	method build_frame {fname {img ""}} {
		set f [labelframe $fname -text $options(-text)];
		if {[string length $img]==0} {
			set img $options(-image);
		} else {
			set options(-image) $img;
		}
		if {[winfo exists $can]} {
			destroy $can;
		}
		set parent [winfo parent $fname];
		if {$parent == "."} {
			set parent "";
		}
		set can "$parent.c_$img";
		set x "$parent.x_$img";
		set y "$parent.y_$img";
		canvas $can;
		scrollbar $x -ori hori -command [list $can xview];
		scrollbar $y -ori vert -command [list $can yview];
		$can config -xscrollcommand [list $x set] -yscrollcommand [list $y set];
		set myphoto [image create photo [$self imageNameFromImg $img]]
		$can create image 0 0 -image $myphoto -anchor nw -tag img
		grid $can $y -in $f -sticky news
		grid $x -in $f -sticky ew
		grid rowconfig $f 0 -weight 1
		grid columnconfig $f 0 -weight 1
		$can config -scrollregion [$can bbox all]
		$can config -cursor trek
		return $f;
	}

	method viewimage {{img ""} {toplevel_name ""} {windowType toplevel}} {
		if {[string length $img]==0} {
			set img $options(-image);
		} else {
			set options(-image) $img;
		}
		if {[string length $toplevel_name]==0} {
			set toplevel_name [$self toplevelNameFromImg $img];
		}
		set tl [$windowType $toplevel_name];
		set f [$self build_frame $tl.topframe $img];
		pack $f -in $tl -side top -anchor nw -expand 1 -fill both;
		return $tl;
	}

	method xblitimage {{img ""} {photo ""} {blitcmd ""}} {
		if {[string length $img]==0} {
			set img $options(-image);
		}
		if {[string length $photo]==0} {
			set photo $myphoto;
		}
		if {[string length $blitcmd]==0} {
			set blitcmd $options(-blitcmd);
		}
		$blitcmd $img $photo;
		if {[winfo exists $can]} {
			$can config -scrollregion [$can bbox all]
		}
	}
}
