dnl $Id$
dnl config.m4 for extension Hk

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(Hk, for Hk support,
Make sure that the comment is aligned:
[  --with-Hk             Include Hk support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(Hk, whether to enable Hk support,
Make sure that the comment is aligned:
[  --enable-Hk           Enable Hk support])

if test "$PHP_HK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-Hk -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/Hk.h"  # you most likely want to change this
  dnl if test -r $PHP_HK/$SEARCH_FOR; then # path given as parameter
  dnl   HK_DIR=$PHP_HK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for Hk files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       HK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$HK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the Hk distribution])
  dnl fi

  dnl # --with-Hk -> add include path
  dnl PHP_ADD_INCLUDE($HK_DIR/include)

  dnl # --with-Hk -> check for lib and symbol presence
  dnl LIBNAME=Hk # you may want to change this
  dnl LIBSYMBOL=Hk # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $HK_DIR/lib, HK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_HKLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong Hk lib version or lib not found])
  dnl ],[
  dnl   -L$HK_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(HK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(Hk, Hk.c, $ext_shared)
fi
if test -z "$PHP_DEBUG"; then 
	AC_ARG_ENABLE(debug,
	[	--enable-debug			compile with debugging symbols],[
		PHP_DEBUG=$enableval
	],[	PHP_DEBUG=no
	])
fi
