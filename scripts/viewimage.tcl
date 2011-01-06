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

proc panedwindowNameFromImg {img} {
	return [toplevelNameFromImg $img].pw
}

proc imageNameFromImg {img} {
	return my#image_$img
}


proc __build_world_view_frame {w img} {
	if {![info exists ::viewimage::vparams]} {
		set ::viewimage::vparams($img,Eaxis,min) 0;
		set ::viewimage::vparams($img,Eaxis,width) 1;
		set ::viewimage::vparams($img,Naxis,min) 0;
		set ::viewimage::vparams($img,Naxis,width) 1;
		set ::viewimage::vparams($img,Aaxis,min) 0;
		set ::viewimage::vparams($img,Aaxis,width) 1;
	}
	set wvlf [labelframe $w -text "World View"];
	set col 0;
	set row 1;
	foreach n {{Top SW corner} width} {
		set l [label ${wvlf}.l$n -text $n -justify right];
		grid $l -column $col -row $row -sticky news;
		incr row;
	}
	set col 1;
	set row 0;
	foreach dir {E N A} {
		set l [label ${wvlf}.l$dir -text $dir];
		grid $l -row 0 -column $col -sticky n;
		set row 1;
		# FIXME: viewimage spinbox should not call out to vprofile command;
		# FIXME: I'll have to fix this using traces on the vprofile side;
		# FIXME: Leaving it here for now during development;
		set s [spinbox ${wvlf}.s${dir}min \
			-from -99999999 -to 9999999 -increment 0.05 \
			-format %.2f -width 12 \
			-command ::vprofile::update_view_if_live -justify right];
		$s configure -textvariable ::viewimage::vparams($img,${dir}axis,min);
		grid $s -row $row -column $col -sticky news;
		incr row;
		set s [spinbox ${wvlf}.s${dir}width \
			-from 0.05 -to 1000000 -increment 0.05 \
			-format %.2f -width 7 \
			-command ::vprofile::update_view_if_live -justify right];
		$s configure -textvariable ::viewimage::vparams($img,${dir}axis,width);
		grid $s -row $row -column $col -sticky news;
		incr col;
	}
	return $wvlf;
}

proc viewimage {img} {
	set t [toplevel [toplevelNameFromImg $img]]
	set pw [panedwindow [panedwindowNameFromImg $img] -orient vertical -showhandle 1];
	set tf [frame $t.tf];
	set c [canvas [canvasNameFromImg $img]]
	set x [scrollbar [xscrollNameFromImg $img] -ori hori -command [list $c xview]]
	set y [scrollbar [yscrollNameFromImg $img] -ori vert -command [list $c yview]]
	$c config -xscrollcommand [list $x set] -yscrollcommand [list $y set]
	set i [image create photo [imageNameFromImg $img]]
	$c create image 0 0 -image $i -anchor nw -tag img
	blitimage $img $i
	grid $c $y -in $tf -sticky news
	grid $x -in $tf -sticky ew
	grid rowconfig $tf 0 -weight 1
	grid columnconfig $tf 0 -weight 1
	$c config -scrollregion [$c bbox all]
	$c config -cursor trek
	label $t.testlabel -text "HI"
	set wvf [__build_world_view_frame $t.wvlf $img];
	$pw add $wvf -sticky nw;
	$pw add $tf -minsize 1i -padx 0 -pady 6 -sticky news;
	pack $pw -expand yes -fill both;
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


