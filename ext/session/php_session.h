/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SESSION_H
#define PHP_SESSION_H

#include "ext/standard/php_var.h"
#include "ext/random/php_random.h"

#define PHP_SESSION_API 20161017

#include "php_version.h"
#define PHP_SESSION_VERSION PHP_VERSION

/* save handler macros */
#define PS_OPEN_ARGS     void **mod_data, const char *save_path, const char *session_name
#define PS_CLOSE_ARGS    void **mod_data
#define PS_READ_ARGS     void **mod_data, zend_string *key, zend_string **val, zend_long maxlifetime
#define PS_WRITE_ARGS    void **mod_data, zend_string *key, zend_string *val, zend_long maxlifetime
#define PS_DESTROY_ARGS  void **mod_data, zend_string *key
#define PS_GC_ARGS       void **mod_data, zend_long maxlifetime, zend_long *nrdels
#define PS_CREATE_SID_ARGS void **mod_data
#define PS_VALIDATE_SID_ARGS void **mod_data, zend_string *key
#define PS_UPDATE_TIMESTAMP_ARGS void **mod_data, zend_string *key, zend_string *val, zend_long maxlifetime

typedef struct ps_module_struct {
	const char *s_name;
	zend_result (*s_open)(PS_OPEN_ARGS);
	zend_result (*s_close)(PS_CLOSE_ARGS);
	zend_result (*s_read)(PS_READ_ARGS);
	zend_result (*s_write)(PS_WRITE_ARGS);
	zend_result (*s_destroy)(PS_DESTROY_ARGS);
	zend_long (*s_gc)(PS_GC_ARGS);
	zend_string *(*s_create_sid)(PS_CREATE_SID_ARGS);
	zend_result (*s_validate_sid)(PS_VALIDATE_SID_ARGS);
	zend_result (*s_update_timestamp)(PS_UPDATE_TIMESTAMP_ARGS);
} ps_module;

#define PS_GET_MOD_DATA() *mod_data
#define PS_SET_MOD_DATA(a) *mod_data = (a)

#define PS_OPEN_FUNC(x) 	zend_result ps_open_##x(PS_OPEN_ARGS)
#define PS_CLOSE_FUNC(x) 	zend_result ps_close_##x(PS_CLOSE_ARGS)
#define PS_READ_FUNC(x) 	zend_result ps_read_##x(PS_READ_ARGS)
#define PS_WRITE_FUNC(x) 	zend_result ps_write_##x(PS_WRITE_ARGS)
#define PS_DESTROY_FUNC(x) 	zend_result ps_delete_##x(PS_DESTROY_ARGS)
#define PS_GC_FUNC(x) 		zend_long ps_gc_##x(PS_GC_ARGS)
#define PS_CREATE_SID_FUNC(x)	zend_string *ps_create_sid_##x(PS_CREATE_SID_ARGS)
#define PS_VALIDATE_SID_FUNC(x)	zend_result ps_validate_sid_##x(PS_VALIDATE_SID_ARGS)
#define PS_UPDATE_TIMESTAMP_FUNC(x) 	zend_result ps_update_timestamp_##x(PS_UPDATE_TIMESTAMP_ARGS)

/* Legacy save handler module definitions */
#define PS_FUNCS(x) \
	PS_OPEN_FUNC(x); \
	PS_CLOSE_FUNC(x); \
	PS_READ_FUNC(x); \
	PS_WRITE_FUNC(x); \
	PS_DESTROY_FUNC(x); \
	PS_GC_FUNC(x);	\
	PS_CREATE_SID_FUNC(x)

#define PS_MOD(x) \
	#x, ps_open_##x, ps_close_##x, ps_read_##x, ps_write_##x, \
	 ps_delete_##x, ps_gc_##x, php_session_create_id, \
	 php_session_validate_sid, php_session_update_timestamp

/* Legacy SID creation enabled save handler module definitions */
#define PS_FUNCS_SID(x) \
	PS_OPEN_FUNC(x); \
	PS_CLOSE_FUNC(x); \
	PS_READ_FUNC(x); \
	PS_WRITE_FUNC(x); \
	PS_DESTROY_FUNC(x); \
	PS_GC_FUNC(x); \
	PS_CREATE_SID_FUNC(x); \
	PS_VALIDATE_SID_FUNC(x); \
	PS_UPDATE_TIMESTAMP_FUNC(x);

#define PS_MOD_SID(x) \
	#x, ps_open_##x, ps_close_##x, ps_read_##x, ps_write_##x, \
	 ps_delete_##x, ps_gc_##x, ps_create_sid_##x, \
	 php_session_validate_sid, php_session_update_timestamp

/* Update timestamp enabled save handler module definitions
   New save handlers should use this API */
#define PS_FUNCS_UPDATE_TIMESTAMP(x) \
	PS_OPEN_FUNC(x); \
	PS_CLOSE_FUNC(x); \
	PS_READ_FUNC(x); \
	PS_WRITE_FUNC(x); \
	PS_DESTROY_FUNC(x); \
	PS_GC_FUNC(x); \
	PS_CREATE_SID_FUNC(x); \
	PS_VALIDATE_SID_FUNC(x); \
	PS_UPDATE_TIMESTAMP_FUNC(x);

#define PS_MOD_UPDATE_TIMESTAMP(x) \
	#x, ps_open_##x, ps_close_##x, ps_read_##x, ps_write_##x, \
	 ps_delete_##x, ps_gc_##x, ps_create_sid_##x, \
	 ps_validate_sid_##x, ps_update_timestamp_##x


typedef enum {
	php_session_disabled,
	php_session_none,
	php_session_active
} php_session_status;

typedef struct _php_session_rfc1867_progress {
	zval      sid;
	smart_str key;

	zend_long      update_step;
	zend_long      next_update;
	double    next_update_time;
	bool cancel_upload;
	bool apply_trans_sid;
	size_t    content_length;

	zval      data;                 /* the array exported to session data */
	zval      files;                /* data["files"] array */
	zval	 *post_bytes_processed; /* data["bytes_processed"] */
	zval	 *current_file_bytes_processed;
	zval      current_file;         /* array of currently uploading file */
} php_session_rfc1867_progress;

typedef struct _php_ps_globals {
	zend_string *save_path;
	zend_string *session_name;
	zend_string *id;
	char *extern_referer_chk;
	zend_string *cache_limiter;
	zend_long cookie_lifetime;
	zend_string *cookie_path;
	zend_string *cookie_domain;
	zend_string *cookie_samesite;
	bool  cookie_secure;
	bool  cookie_httponly;
	const ps_module *mod;
	const ps_module *default_mod;
	void *mod_data;
	php_session_status session_status;
	zend_string *session_started_filename;
	uint32_t session_started_lineno;
	int module_number;
	php_random_status_state_pcgoneseq128xslrr64 random_state;
	php_random_algo_with_state random;
	zend_long gc_probability;
	zend_long gc_divisor;
	zend_long gc_maxlifetime;
	zend_long cache_expire;
	struct {
		zval ps_open;
		zval ps_close;
		zval ps_read;
		zval ps_write;
		zval ps_destroy;
		zval ps_gc;
		zval ps_create_sid;
		zval ps_validate_sid;
		zval ps_update_timestamp;
	} mod_user_names;
	zend_string *mod_user_class_name;
	bool mod_user_implemented;
	bool mod_user_is_open;
	bool auto_start;
	bool use_cookies;
	bool use_only_cookies;
	bool use_trans_sid; /* contains the INI value of whether to use trans-sid */
	bool send_cookie;
	bool define_sid;

	const struct ps_serializer_struct *serializer;
	zval http_session_vars;

	zend_long sid_length;
	zend_long sid_bits_per_character;

	php_session_rfc1867_progress *rfc1867_progress;
	zend_string *rfc1867_prefix;  /* session.upload_progress.prefix */
	zend_string *rfc1867_name;    /* session.upload_progress.name */
	zend_long rfc1867_freq;         /* session.upload_progress.freq */
	double rfc1867_min_freq;   /* session.upload_progress.min_freq */
	bool rfc1867_enabled; /* session.upload_progress.enabled */
	bool rfc1867_cleanup; /* session.upload_progress.cleanup */

	bool use_strict_mode; /* whether or not PHP accepts unknown session ids */
	bool lazy_write; /* omit session write when it is possible */
	bool in_save_handler; /* state if session is in save handler or not */
	bool set_handler;     /* state if session module i setting handler or not */
	zend_string *session_vars; /* serialized original session data */
} php_ps_globals;

typedef php_ps_globals zend_ps_globals;

extern zend_module_entry session_module_entry;
#define phpext_session_ptr &session_module_entry

#ifdef ZTS
#define PS(v) ZEND_TSRMG(ps_globals_id, php_ps_globals *, v)
#ifdef COMPILE_DL_SESSION
ZEND_TSRMLS_CACHE_EXTERN()
#endif
#else
#define PS(v) (ps_globals.v)
#endif

#define PS_SERIALIZER_ENCODE_ARGS void
#define PS_SERIALIZER_DECODE_ARGS const char *val, size_t vallen

typedef struct ps_serializer_struct {
	const char *name;
	zend_string *(*encode)(PS_SERIALIZER_ENCODE_ARGS);
	zend_result (*decode)(PS_SERIALIZER_DECODE_ARGS);
} ps_serializer;

#define PS_SERIALIZER_ENCODE_NAME(x) ps_srlzr_encode_##x
#define PS_SERIALIZER_DECODE_NAME(x) ps_srlzr_decode_##x

#define PS_SERIALIZER_ENCODE_FUNC(x) \
	zend_string *PS_SERIALIZER_ENCODE_NAME(x)(PS_SERIALIZER_ENCODE_ARGS)
#define PS_SERIALIZER_DECODE_FUNC(x) \
	zend_result PS_SERIALIZER_DECODE_NAME(x)(PS_SERIALIZER_DECODE_ARGS)

#define PS_SERIALIZER_FUNCS(x) \
	PS_SERIALIZER_ENCODE_FUNC(x); \
	PS_SERIALIZER_DECODE_FUNC(x)

#define PS_SERIALIZER_ENTRY(x) \
	{ #x, PS_SERIALIZER_ENCODE_NAME(x), PS_SERIALIZER_DECODE_NAME(x) }

/* default create id function */
PHPAPI zend_string *php_session_create_id(PS_CREATE_SID_ARGS);
/* Dummy PS module functions */
PHPAPI zend_result php_session_validate_sid(PS_VALIDATE_SID_ARGS);
PHPAPI zend_result php_session_update_timestamp(PS_UPDATE_TIMESTAMP_ARGS);

PHPAPI void session_adapt_url(const char *url, size_t url_len, char **new_url, size_t *new_len);

PHPAPI zend_result php_session_destroy(void);
PHPAPI void php_add_session_var(zend_string *name);
PHPAPI zval *php_set_session_var(zend_string *name, zval *state_val, php_unserialize_data_t *var_hash);
PHPAPI zval *php_get_session_var(zend_string *name);
PHPAPI zval* php_get_session_var_str(const char *name, size_t name_len);

PHPAPI zend_result php_session_register_module(const ps_module *);

PHPAPI zend_result php_session_register_serializer(const char *name,
	        zend_string *(*encode)(PS_SERIALIZER_ENCODE_ARGS),
	        zend_result (*decode)(PS_SERIALIZER_DECODE_ARGS));

PHPAPI zend_result php_session_start(void);
PHPAPI zend_result php_session_flush(int write);
PHPAPI php_session_status php_get_session_status(void);

PHPAPI const ps_module *_php_find_ps_module(const char *name);
PHPAPI const ps_serializer *_php_find_ps_serializer(const char *name);

PHPAPI zend_result php_session_valid_key(const char *key);
PHPAPI zend_result php_session_reset_id(void);

#define PS_ADD_VARL(name) do {										\
	php_add_session_var(name);							\
} while (0)

#define PS_ADD_VAR(name) PS_ADD_VARL(name)

#define PS_DEL_VARL(name) do {										\
	if (!Z_ISNULL(PS(http_session_vars))) {							\
		zend_hash_del(Z_ARRVAL(PS(http_session_vars)), name);		\
	}																\
} while (0)


#define PS_ENCODE_VARS 												\
	zend_string *key;												\
	zend_ulong num_key;													\
	zval *struc;

/* Do not use a return statement in `code` because that may leak memory.
 * Break out of the loop instead. */
#define PS_ENCODE_LOOP(code) do {									\
	zval _zv;														\
	/* protect against user interference */							\
	ZVAL_COPY(&_zv, Z_REFVAL(PS(http_session_vars)));				\
	HashTable *_ht = Z_ARRVAL(_zv);									\
	ZEND_HASH_FOREACH_KEY(_ht, num_key, key) {						\
		if (key == NULL) {											\
			php_error_docref(NULL, E_WARNING,						\
					"Skipping numeric key " ZEND_LONG_FMT, num_key);\
			continue;												\
		}															\
		if ((struc = php_get_session_var(key))) {					\
			code;		 											\
		} 															\
	} ZEND_HASH_FOREACH_END();										\
	zval_ptr_dtor(&_zv);											\
} while(0)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(ps)

void php_session_auto_start(void *data);

extern PHPAPI zend_class_entry *php_session_class_entry;

extern PHPAPI zend_class_entry *php_session_iface_entry;

extern PHPAPI zend_class_entry *php_session_id_iface_entry;

extern PHPAPI zend_class_entry *php_session_update_timestamp_iface_entry;

extern PHP_METHOD(SessionHandler, open);
extern PHP_METHOD(SessionHandler, close);
extern PHP_METHOD(SessionHandler, read);
extern PHP_METHOD(SessionHandler, write);
extern PHP_METHOD(SessionHandler, destroy);
extern PHP_METHOD(SessionHandler, gc);
extern PHP_METHOD(SessionHandler, create_sid);

#endif
