/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HK_H
#define PHP_HK_H

extern zend_module_entry Hk_module_entry;
#define phpext_Hk_ptr &Hk_module_entry

#ifdef PHP_WIN32
#	define PHP_HK_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_HK_API __attribute__ ((visibility("default")))
#else
#	define PHP_HK_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define HK_DEBUG "debug"
#define HK_BUFFER "hk_buffer"

PHP_MINIT_FUNCTION(Hk);
PHP_MSHUTDOWN_FUNCTION(Hk);
PHP_RINIT_FUNCTION(Hk);
PHP_RSHUTDOWN_FUNCTION(Hk);
PHP_MINFO_FUNCTION(Hk);

PHP_FUNCTION(confirm_Hk_compiled);	/* For testing, remove later. */
PHP_METHOD(Hk,setPath);	/* For testing, remove later. */
PHP_METHOD(Hk,setBufferNum);	/* For testing, remove later. */
PHP_METHOD(Hk,setInitBufferNum);	/* For testing, remove later. */
PHP_METHOD(Hk,debug);	/* For testing, remove later. */
PHP_METHOD(Hk,test);	/* For testing, remove later. */


ZEND_BEGIN_MODULE_GLOBALS(Hk)
	long  global_value,global_bufferNum,global_initBufferNum;
	char *global_string,*global_logPath;
ZEND_END_MODULE_GLOBALS(Hk)

/* In every utility function you add that needs to use variables 
   in php_Hk_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as HK_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define HK_G(v) TSRMG(Hk_globals_id, zend_Hk_globals *, v)
#else
#define HK_G(v) (Hk_globals.v)
#endif

#endif	/* PHP_HK_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
