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
#ifndef HAVE_PHP_UI_GRID
#define HAVE_PHP_UI_GRID
#include <ui.h>

#include "php.h"

#include <classes/control.h>
#include <classes/grid.h>

zend_object_handlers php_ui_grid_handlers;

zend_object* php_ui_grid_create(zend_class_entry *ce) {
	php_ui_grid_t *grid = 
		(php_ui_grid_t*) ecalloc(1, sizeof(php_ui_grid_t));

	zend_object_std_init(&grid->std, ce);

	object_properties_init(&grid->std, ce);

	grid->std.handlers = &php_ui_grid_handlers;

	return &grid->std;
}

ZEND_BEGIN_ARG_INFO_EX(php_ui_grid_construct_info, 0, 0, 0)
ZEND_END_ARG_INFO()

/* {{{ proto Grid Grid::__construct() */
PHP_METHOD(Grid, __construct) 
{
	php_ui_grid_t *grid = php_ui_grid_fetch(getThis());

	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	grid->g = uiNewGrid();
} /* }}} */

ZEND_BEGIN_ARG_INFO_EX(php_ui_grid_set_padded_info, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, text, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

/* {{{ proto void Grid::setPadded(bool padded) */
PHP_METHOD(Grid, setPadded) 
{
	php_ui_grid_t *grid = php_ui_grid_fetch(getThis());
	zend_bool padded = 0;
	
	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "b", &padded) != SUCCESS) {
		return;
	}

	uiGridSetPadded(grid->g, padded);
} /* }}} */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(php_ui_grid_is_padded_info, 0, 0, _IS_BOOL, NULL, 0)
ZEND_END_ARG_INFO()

/* {{{ proto string Grid::isPadded(void) */
PHP_METHOD(Grid, isPadded) 
{
	php_ui_grid_t *grid = php_ui_grid_fetch(getThis());

	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	RETURN_BOOL(uiGridPadded(grid->g));	
} /* }}} */

ZEND_BEGIN_ARG_INFO_EX(php_ui_grid_append_info, 0, 0, 8)
	ZEND_ARG_OBJ_INFO(0, control, UI\\Control, 0)
	ZEND_ARG_TYPE_INFO(0, left, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, top, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, xspan, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, yspan, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hexpand, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, halign, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, vexpand, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, valign, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* {{{ proto void Grid::append(Control control, int left, int top, int xspan, int yspan, bool hexpand, int halign, bool vexpand, int valign) */
PHP_METHOD(Grid, append)
{
	php_ui_grid_t *grid = php_ui_grid_fetch(getThis());
	zval *control = NULL;
	zend_long top = 0, left = 0, xspan = 0, yspan = 0, halign = 0, valign = 0;
	zend_bool hexpand = 0, vexpand = 0;
	php_ui_control_t *ctrl;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "Ollllblbl", &control, uiControl_ce, &left, &top, &xspan, &yspan, &hexpand, &halign, &vexpand, &valign) != SUCCESS) {
		return;
	}

	ctrl = php_ui_control_fetch(control);
	
	uiGridAppend(grid->g, ctrl->c, left, top, xspan, yspan, hexpand, halign, vexpand, valign);
} /* }}} */

/* {{{ */
const zend_function_entry php_ui_grid_methods[] = {
	PHP_ME(Grid, __construct, php_ui_grid_construct_info,   ZEND_ACC_PUBLIC)
	PHP_ME(Grid, setPadded,   php_ui_grid_set_padded_info,  ZEND_ACC_PUBLIC)
	PHP_ME(Grid, isPadded,    php_ui_grid_is_padded_info,   ZEND_ACC_PUBLIC)
	PHP_ME(Grid, append,      php_ui_grid_append_info,      ZEND_ACC_PUBLIC)
	PHP_FE_END
}; /* }}} */

/* {{{ */
PHP_MINIT_FUNCTION(UI_Grid) 
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "UI", "Grid", php_ui_grid_methods);

	uiGrid_ce = zend_register_internal_class_ex(&ce, uiControl_ce);
	uiGrid_ce->create_object = php_ui_grid_create;

	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("FILL"), uiAlignFill);
	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("START"), uiAlignStart);
	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("CENTER"), uiAlignCenter);
	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("END"), uiAlignEnd);
	
	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("LEADING"), uiAtLeading);
	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("TOP"), uiAtTop);
	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("TRAILING"), uiAtTrailing);
	zend_declare_class_constant_long(uiGrid_ce, ZEND_STRL("BOTTOM"), uiAtBottom);

	memcpy(&php_ui_grid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	
	php_ui_grid_handlers.offset = XtOffsetOf(php_ui_grid_t, std);

	return SUCCESS;
} /* }}} */
#endif
