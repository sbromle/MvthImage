#!/usr/bin/env tclsh

#
# This file is part of MVTH - the Machine Vision Test Harness.
#
# Store and retrieve the state of the plugin between reloads.
#
# Copyright (C) 2006 Samuel P. Bromley <sam@sambromley.com>
#
# This code is licensed under the terms and conditions of
# the GNU GPL. See the file COPYING in the top level directory of
# the MVTH archive for details.

proc save_mvthimage_state { dir } {
	set images [listimages]
	if {[llength $images]>0} {
		file mkdir $dir
		set fd [open [file join $dir images] "w"]
		foreach i $images {
			set filename [file join $dir ${i}.png]
			# write out an actual tcl command that will read
			# the image back in 
			puts $fd [list uplevel #0 openimage $i $filename]
			writeimage $i $filename
		}
		close $fd
	}
	return
}

proc restore_mvthimage_state { dir } {
	set filename [file join $dir images]
	source $filename
}

