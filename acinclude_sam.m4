#------------------------------------------------------------------------
# SC_LOAD_TCLCONFIG_WITH_INCLUDES --
#
# Like the standard SCL_LOAD_TCLCONFIG, that is:
#   	Load the tclConfig.sh file
# but in addition, set the TCL_INCLUDE_SPEC attribute as well.
#
# Arguments:
#	
#	Requires the following vars to be set:
#		TCL_BIN_DIR
#
# Results:
#
#	Subst the following vars:
#		TCL_BIN_DIR
#		TCL_SRC_DIR
#		TCL_LIB_FILE
#
#		and
#
#		TCL_INCLUDE_SPEC
#
#------------------------------------------------------------------------

AC_DEFUN([SC_LOAD_TCLCONFIG_WITH_INCLUDES], [
    AC_MSG_CHECKING([for existence of ${TCL_BIN_DIR}/tclConfig.sh])

    if test -f "${TCL_BIN_DIR}/tclConfig.sh" ; then
        AC_MSG_RESULT([loading])
	. "${TCL_BIN_DIR}/tclConfig.sh"
    else
        AC_MSG_RESULT([could not find ${TCL_BIN_DIR}/tclConfig.sh])
    fi

    # eval is required to do the TCL_DBGX substitution
    eval "TCL_LIB_FILE=\"${TCL_LIB_FILE}\""
    eval "TCL_STUB_LIB_FILE=\"${TCL_STUB_LIB_FILE}\""

    # If the TCL_BIN_DIR is the build directory (not the install directory),
    # then set the common variable name to the value of the build variables.
    # For example, the variable TCL_LIB_SPEC will be set to the value
    # of TCL_BUILD_LIB_SPEC. An extension should make use of TCL_LIB_SPEC
    # instead of TCL_BUILD_LIB_SPEC since it will work with both an
    # installed and uninstalled version of Tcl.
    if test -f "${TCL_BIN_DIR}/Makefile" ; then
        TCL_LIB_SPEC=${TCL_BUILD_LIB_SPEC}
        TCL_STUB_LIB_SPEC=${TCL_BUILD_STUB_LIB_SPEC}
        TCL_STUB_LIB_PATH=${TCL_BUILD_STUB_LIB_PATH}
    elif test "`uname -s`" = "Darwin"; then
	# If Tcl was built as a framework, attempt to use the libraries
	# from the framework at the given location so that linking works
	# against Tcl.framework installed in an arbitary location.
	case ${TCL_DEFS} in
	    *TCL_FRAMEWORK*)
		if test -f "${TCL_BIN_DIR}/${TCL_LIB_FILE}"; then
		    for i in "`cd ${TCL_BIN_DIR}; pwd`" \
			     "`cd ${TCL_BIN_DIR}/../..; pwd`"; do
			if test "`basename "$i"`" = "${TCL_LIB_FILE}.framework"; then
			    TCL_LIB_SPEC="-F`dirname "$i"` -framework ${TCL_LIB_FILE}"
			    break
			fi
		    done
		fi
		if test -f "${TCL_BIN_DIR}/${TCL_STUB_LIB_FILE}"; then
		    TCL_STUB_LIB_SPEC="-L${TCL_BIN_DIR} ${TCL_STUB_LIB_FLAG}"
		    TCL_STUB_LIB_PATH="${TCL_BIN_DIR}/${TCL_STUB_LIB_FILE}"
		fi
		;;
	esac
    fi

    # eval is required to do the TCL_DBGX substitution
    eval "TCL_LIB_FLAG=\"${TCL_LIB_FLAG}\""
    eval "TCL_LIB_SPEC=\"${TCL_LIB_SPEC}\""
    eval "TCL_INCLUDE_SPEC=\"${TCL_INCLUDE_SPEC}\""
    eval "TCL_STUB_LIB_FLAG=\"${TCL_STUB_LIB_FLAG}\""
    eval "TCL_STUB_LIB_SPEC=\"${TCL_STUB_LIB_SPEC}\""

    AC_SUBST(TCL_VERSION)
    AC_SUBST(TCL_PATCH_LEVEL)
    AC_SUBST(TCL_BIN_DIR)
    AC_SUBST(TCL_SRC_DIR)

    AC_SUBST(TCL_LIB_FILE)
    AC_SUBST(TCL_LIB_FLAG)
    AC_SUBST(TCL_LIB_SPEC)

		AC_SUBST(TCL_INCLUDE_SPEC)

    AC_SUBST(TCL_STUB_LIB_FILE)
    AC_SUBST(TCL_STUB_LIB_FLAG)
    AC_SUBST(TCL_STUB_LIB_SPEC)
])
