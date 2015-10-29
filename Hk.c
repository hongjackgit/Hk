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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_Hk.h"

void Hk_buffer_init(TSRMLS_C);
struct timeval tv;
struct timezone tz;
struct _Hk_log_data{
       int starttime_usec;
       int starttime_sec;
       int endtime_usec;
       int endtime_sec;
       long log_id;
       zend_execute_data *data;
       long lineno;
       char *filename;
       char *fun;
       zval *msg;
       long mempeak;
}Hk_log_data;
ZEND_DECLARE_MODULE_GLOBALS(Hk)
/* True global resources - no need for thread safety here */
static int le_Hk;
zend_class_entry * Hk_ce;
/* {{{ Hk_functions[]
 *
 * Every user visible function must have an entry in Hk_functions[].
 */
const zend_function_entry Hk_functions[] = {
	PHP_FE(confirm_Hk_compiled,	NULL)		/* For testing, remove later. */
        PHP_ME(Hk,setPath,NULL,ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
        PHP_ME(Hk,setBufferNum,NULL,ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
        PHP_ME(Hk,setInitBufferNum,NULL,ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
        PHP_ME(Hk,test,NULL,ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
        PHP_ME(Hk,debug,NULL,ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in Hk_functions[] */
};
/* }}} */

/* {{{ Hk_module_entry
 */
zend_module_entry Hk_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"Hk",
	Hk_functions,
	PHP_MINIT(Hk),
	PHP_MSHUTDOWN(Hk),
	PHP_RINIT(Hk),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(Hk),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(Hk),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HK
ZEND_GET_MODULE(Hk)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("Hk.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_Hk_globals, Hk_globals)
    STD_PHP_INI_ENTRY("Hk.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_Hk_globals, Hk_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_Hk_init_globals
 */
/* Uncomment this function if you have INI entries
*/
static void php_Hk_init_globals(zend_Hk_globals *Hk_globals)
{
	Hk_globals->global_value = 0;
	Hk_globals->global_bufferNum = 1;
	Hk_globals->global_initBufferNum = 1;
	Hk_globals->global_string = NULL;
	Hk_globals->global_logPath = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(Hk)
{
	/* If you have INI entries, uncomment these lines 
	ZEND_INIT_MODULE_GLOBALS
	REGISTER_INI_ENTRIES();
	*/
        ZEND_INIT_MODULE_GLOBALS(Hk,php_Hk_init_globals,NULL);
        REGISTER_STRINGL_CONSTANT(HK_DEBUG,HK_DEBUG,sizeof(HK_DEBUG)-1,CONST_CS|CONST_PERSISTENT);
        zend_class_entry Hk;
        INIT_CLASS_ENTRY(Hk,"Hk",Hk_functions);
        Hk_ce = zend_register_internal_class(&Hk TSRMLS_CC);
        zend_declare_property_null(Hk_ce,ZEND_STRL(HK_BUFFER),ZEND_ACC_STATIC TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(Hk)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(Hk)
{
        Hk_buffer_init(TSRMLS_C);
        Hk_log_id(TSRMLS_C);                                                                                                      
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(Hk)
{
	Hk_ex();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(Hk)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Hk support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_Hk_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_Hk_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "Hk", arg);
	RETURN_STRINGL(strg, len, 0);
}
PHP_METHOD(Hk,setPath)
{
    char *path = NULL;
    int path_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path,&path_len) == FAILURE){
        RETURN_FALSE; 
    }
/*  it is can do work also
    spprintf(&HK_G(global_logPath), 0, "%s",path);
*/
    zval *pzval;
    MAKE_STD_ZVAL(pzval);
    ZVAL_STRING(pzval,path,1);
    HK_G(global_logPath)=Z_STRVAL_P(pzval);
}
PHP_METHOD(Hk,debug)
{
    char *logPath;
    spprintf(&logPath,"0","%s%s",HK_G(global_logPath),"debug.log");
    Hk_dispatch(INTERNAL_FUNCTION_PARAM_PASSTHRU,logPath,1);
}
PHP_METHOD(Hk,setBufferNum)
{
    long num;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC ,"l",&num) == FAILURE){
        RETURN_NULL();
    }
    HK_G(global_bufferNum) = num;
}

PHP_METHOD(Hk,setInitBufferNum)
{
    long num;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC ,"l",&num) == FAILURE){
        RETURN_NULL();
    }
    HK_G(global_initBufferNum) = num;
}
int Hk_dispatch(INTERNAL_FUNCTION_PARAMETERS,char *level,int levelNo)
{
    char *message;
    int message_len,currentBufferNum;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&message,&message_len) == FAILURE){
	RETURN_FALSE;
    }
    currentBufferNum = HK_G(global_bufferNum);
    Hk_buffer_set(message,message_len,level,levelNo);
}
int Hk_buffer_set(char *inputMessage, int message_len,char *level,int levelNo)
{
    zval *childLevelZval,**data;
    zval *oldHkBuffer;
    zval *newHkBuffer;
    HashTable *ht;
    HashTable *hts; 
    HashPosition pos;
    zval **pValue;
    Hk_log_data.data = EG(current_execute_data);
    Hk_log_data.lineno  = Hk_log_data.data->opline->lineno;
    char *_date,*message;
    Hk_log_data.fun = Hk_log_data.data->function_state.function->common.function_name;
    Hk_log_data.filename = Hk_log_data.data->op_array->filename;
    gettimeofday(&tv,&tz);
    Hk_log_data.mempeak = zend_memory_peak_usage(0 TSRMLS_CC);
    _date = php_format_date("Y-m-d H:i:s", 1000, (long)time(NULL), (long)time(NULL) TSRMLS_CC);
    spprintf(&message, 0, "[logid=%ld][%s][line=%s + %ld function=%s][%s] \n",Hk_log_data.log_id,_date,Hk_log_data.filename,Hk_log_data.lineno,Hk_log_data.fun,inputMessage);
    int isOld = 0;
    MAKE_STD_ZVAL(childLevelZval);
    array_init(childLevelZval);
    MAKE_STD_ZVAL(newHkBuffer);
    array_init(newHkBuffer);
    oldHkBuffer = zend_read_static_property(Hk_ce,HK_BUFFER,sizeof(HK_BUFFER)-1,1 TSRMLS_CC);
    ht = Z_ARRVAL_P(oldHkBuffer);
    zend_hash_internal_pointer_reset(ht);
    while(zend_hash_get_current_data(ht, (void**)&data) == SUCCESS){
        char *key;
        ulong idx = 0;
        zend_hash_get_current_key(ht, &key, &idx, 0);
        if(strcmp(key,level) == 0 ) {
            if(Z_TYPE_PP(data) == IS_ARRAY){
           	zval *datas;
            	convert_to_array_ex(data);
            	MAKE_STD_ZVAL(datas);
            	ZVAL_STRING(datas,message,1);
            	int i = zend_hash_next_index_insert(Z_ARRVAL_PP(data),&datas,sizeof(zval *),NULL);
            	if(FAILURE == i){
                    printf("index_next is failure");
                }
            	isOld = 1;
            }
        }
        add_assoc_zval(newHkBuffer,level,*data);
        zend_hash_move_forward(ht);
    }
    if( isOld == 0 ) {
        add_next_index_string(childLevelZval,message,1);
        add_assoc_zval(newHkBuffer,level,childLevelZval);
    }
    zend_update_static_property(Hk_ce,HK_BUFFER,sizeof(HK_BUFFER)-1,newHkBuffer TSRMLS_CC);
    HK_G(global_bufferNum)++;
    if(HK_G(global_bufferNum) > HK_G(global_initBufferNum)){
        Hk_ex();
        Hk_buffer_init(TSRMLS_C);
    }
    return 0;
}
int Hk_log_real(char *logfile,char *message){
    php_stream *stream;
    int options= ENFORCE_SAFE_MODE | REPORT_ERRORS;
    stream = php_stream_open_wrapper(logfile, "a", IGNORE_URL_WIN | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL);
    if(!stream){
    	printf("stream is erro ");
    }
    php_stream_write(stream, message, strlen(message));
    php_stream_close(stream);
}
int Hk_ex()
{
    HashPosition pos;
    HashTable *ht;
    zval **data,*oldHkBuffer;
    oldHkBuffer = zend_read_static_property(Hk_ce,HK_BUFFER,sizeof(HK_BUFFER)-1,1 TSRMLS_CC);
    if(Z_TYPE_P(oldHkBuffer) == IS_NULL) return ;
    ht = Z_ARRVAL_P(oldHkBuffer);
    for(zend_hash_internal_pointer_reset_ex(ht,&pos);zend_hash_get_current_data_ex(ht,(void**) &data,&pos) == SUCCESS; zend_hash_move_forward_ex(ht,&pos)){                                                                                                 
    char *key;
    ulong idx = 0;
    zend_hash_get_current_key(ht, &key, &idx, 0);
    if(Z_TYPE_PP(data) == IS_ARRAY){
        HashPosition poss; HashTable *hts;
    	zval **datas;                                                                                                                              
    	hts = Z_ARRVAL_P(*data);
        for(zend_hash_internal_pointer_reset_ex(hts,&poss);zend_hash_get_current_data_ex(hts,(void**) &datas,&poss) == SUCCESS; zend_hash_move_forward_ex(hts,&poss)){                                                                                                 
        char *message; 
        message = Z_STRVAL_PP(datas);
        Hk_log_real(key,message);
        }
    }
    }
}
void Hk_buffer_init(TSRMLS_D)
{
    HK_G(global_bufferNum) = 1;
    zval *buffer_zval,*farZval;
    MAKE_STD_ZVAL(buffer_zval);
    array_init(buffer_zval);
    zend_update_static_property(Hk_ce,HK_BUFFER,sizeof(HK_BUFFER)-1,buffer_zval TSRMLS_CC);
    zval_ptr_dtor(&buffer_zval);
}
void Hk_buffer_clear()
{
    
}
PHP_METHOD(Hk,test)
{
    char *logPath;
    spprintf(&logPath,"0","%s%s",HK_G(global_logPath),"test.log");
    Hk_dispatch(INTERNAL_FUNCTION_PARAM_PASSTHRU,logPath,2);
}
void Hk_log_id(TSRMLS_D){                                                                                                      
    long log_id;                                                                                                                          
    gettimeofday(&tv,&tz);                                                                                                                
    Hk_log_data.log_id = tv.tv_sec;                                                                                                       
}   
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
