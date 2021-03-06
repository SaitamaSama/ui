/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: krakjoe                                                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ui.h"

#include <ui.h>

#ifdef HAVE_UI_X_THREADS
#	include <X11/Xlib.h>
#endif

#include <classes/point.h>
#include <classes/size.h>

#include <classes/control.h>
#include <classes/window.h>
#include <classes/tab.h>
#include <classes/form.h>
#include <classes/grid.h>
#include <classes/box.h>
#include <classes/check.h>
#include <classes/button.h>
#include <classes/cbutton.h>
#include <classes/label.h>
#include <classes/entry.h>
#include <classes/multi.h>
#include <classes/group.h>
#include <classes/spin.h>
#include <classes/slider.h>
#include <classes/progress.h>
#include <classes/separator.h>
#include <classes/combo.h>
#include <classes/ecombo.h>
#include <classes/radio.h>
#include <classes/picker.h>
#include <classes/menu.h>
#include <classes/item.h>

#include <classes/area.h>
#include <classes/path.h>
#include <classes/context.h>
#include <classes/brush.h>
#include <classes/draw.h>
#include <classes/stroke.h>
#include <classes/matrix.h>

/* {{{ */
typedef struct _php_ui_event_t {
	uiControl *ctrl;
	zend_object std;
	zval handler;
} php_ui_event_t;

void php_ui_event_handler(void *u, void *_event) {
	php_ui_event_t *event = (php_ui_event_t*) _event;

	if (Z_TYPE(event->handler) != IS_UNDEF) {
		zval rv;
		zval arg;
		zend_fcall_info fci = empty_fcall_info;
		zend_fcall_info_cache fcc = empty_fcall_info_cache;

		if (zend_fcall_info_init(&event->handler, IS_CALLABLE_CHECK_SILENT, &fci, &fcc, NULL, NULL) != SUCCESS) {
			return;
		}

		ZVAL_OBJ(&arg, &event->std);
		ZVAL_UNDEF(&rv);

		zend_fcall_info_argn(&fci, 1, &arg);
		fci.retval = &rv;

		if (zend_call_function(&fci, &fcc) != SUCCESS) {
			return;
		}

		if (Z_TYPE(rv) != IS_UNDEF) {
			zval_ptr_dtor(&rv);
		}

		zend_fcall_info_args_clear(&fci, 1);
	}
} /* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ui)
{
	const char *initError = NULL;

	uiInitOptions options;

#ifdef HAVE_UI_X_THREADS
	XInitThreads();
#endif

	memset(&options, 0, sizeof(uiInitOptions));

	initError = uiInit(&options);

	if (initError) {
		zend_error(E_ERROR,
			"Cannot initialize libui: %s", initError);
		uiFreeInitError(initError);

		return FAILURE;
	}

	uiMainSteps();

	PHP_MINIT(UI_Point)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Size)(INIT_FUNC_ARGS_PASSTHRU);

	PHP_MINIT(UI_Control)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Window)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Form)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Grid)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Tab)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Box)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Check)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Button)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_ColorButton)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Label)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Entry)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Multi)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Group)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Spin)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Slider)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Progress)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Separator)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Combo)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_EditableCombo)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Radio)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Picker)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Menu)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_MenuItem)(INIT_FUNC_ARGS_PASSTHRU);

	PHP_MINIT(UI_Area)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_DrawPath)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_DrawContext)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_DrawBrush)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_DrawStroke)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_DrawMatrix)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(UI_Draw)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ui)
{
	uiQuit();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ui)
{
#if defined(COMPILE_DL_UI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ui)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ui)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ui support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ */
PHP_FUNCTION(main)
{
	uiMain();
} /* }}} */

/* {{{ */
PHP_FUNCTION(mainStep)
{
	zend_bool block = 0;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|b", &block) != SUCCESS) {
		return;
	}
	
	uiMainStep(block);
} /* }}} */

/* {{{ */
PHP_FUNCTION(mainSteps)
{
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}
	
	uiMainSteps();
} /* }}} */

/* {{{ nothing happening here ... ignore this ... */
int php_ui_serialize(zval *object, unsigned char **buffer, size_t *buflen, zend_serialize_data *data) {
	void *address = ((char*) Z_OBJ_P(object) - XtOffsetOf(php_ui_window_t, std));

#ifdef _WIN64
	(*buflen) = snprintf(NULL, 0, ":%I64u:", (unsigned __int64) address);
#else
	(*buflen) = snprintf(NULL, 0, ":%lu:", (long unsigned int) address);
#endif
	(*buffer) = emalloc((*buflen) + 1);
	sprintf(
#ifdef _WIN64
		(char*) (*buffer), ":%I64u:", (unsigned __int64) address);
#else
		(char*) (*buffer), ":%lu:", (long unsigned int) address);
#endif
	(*buffer)[(*buflen)] = 0;

	return SUCCESS;
}

typedef struct _php_ui_object_t {
	void *p;
	zend_object std;
} php_ui_object_t;

#define php_ui_object_from(o) ((php_ui_object_t*) ((char*) o - XtOffsetOf(php_ui_object_t, std)))
#define php_ui_object_fetch(o) php_ui_object_from(Z_OBJ_P(o))

int php_ui_unserialize(zval *object, zend_class_entry *ce, const unsigned char *buffer, size_t blen, zend_unserialize_data *data) {
	php_ui_object_t *address = NULL;
	php_ui_object_t *o;

#ifdef _WIN64
	if (!sscanf((const char*) buffer, ":%I64u:", (unsigned __int64*)&address)) {
#else
	if (!sscanf((const char*) buffer, ":%lu:", (long unsigned int*)&address)) {
#endif
		return FAILURE;
	}

	object_init_ex(object, ce);

	o = php_ui_object_fetch(object);
	o->p = address->p;

	return SUCCESS;
} /* }}} */

/* {{{ ui_functions[]
 */
const zend_function_entry ui_functions[] = {
	ZEND_NS_FE("UI", main, NULL)	
	ZEND_NS_FE("UI", mainStep, NULL)	
	ZEND_NS_FE("UI", mainSteps, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ ui_module_entry
 */
zend_module_entry ui_module_entry = {
	STANDARD_MODULE_HEADER,
	"ui",
	ui_functions,
	PHP_MINIT(ui),
	PHP_MSHUTDOWN(ui),
	PHP_RINIT(ui),
	PHP_RSHUTDOWN(ui),
	PHP_MINFO(ui),
	PHP_UI_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_UI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(ui)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
