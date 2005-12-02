/* 
+----------------------------------------------------------------------+
| (c) Copyright IBM Corporation 2005.                                  | 
| All Rights Reserved.                                                 |
+----------------------------------------------------------------------+ 
|                                                                      | 
| Licensed under the Apache License, Version 2.0 (the "License"); you  | 
| may not use this file except in compliance with the License. You may | 
| obtain a copy of the License at                                      | 
| http://www.apache.org/licenses/LICENSE-2.0                           |
|                                                                      | 
| Unless required by applicable law or agreed to in writing, software  | 
| distributed under the License is distributed on an "AS IS" BASIS,    | 
| WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      | 
| implied. See the License for the specific language governing         | 
| permissions and limitations under the License.                       | 
+----------------------------------------------------------------------+ 
| Author: Caroline Maynard                                             | 
+----------------------------------------------------------------------+ 

*/
static char rcs_id[] = "$Id$";

#ifdef PHP_WIN32
#include <iostream>
#include <math.h>
#include "zend_config.w32.h"
#endif

#include <sstream>

#include "php.h"
#include "zend_exceptions.h"

#include "php_sdo_int.h"

#define CLASS_NAME "SDO_Model_Type"

/* {{{ sdo_model_type_object
 * The instance data for this class - extends the standard zend_object
 */
typedef struct {
	zend_object		 zo;			/* The standard zend_object */
	const Type		*typep;			/* The sdo4cpp type */
} sdo_model_type_object;
/* }}} */

static zend_object_handlers sdo_model_type_object_handlers;

/* {{{ sdo_model_type_get_instance
 */
static sdo_model_type_object *sdo_model_type_get_instance(zval *me TSRMLS_DC) 
{
	return (sdo_model_type_object *)zend_object_store_get_object(me TSRMLS_CC);
}
/* }}} */

/* {{{ sdo_model_type_object_free_storage
 */
static void sdo_model_type_object_free_storage(void *object TSRMLS_DC)
{
	sdo_model_type_object *my_object;

	my_object = (sdo_model_type_object *)object;
	zend_hash_destroy(my_object->zo.properties);
	FREE_HASHTABLE(my_object->zo.properties);
	my_object->typep = NULL;
	efree(my_object);

}
/* }}} */

/* {{{ sdo_model_type_object_create
 */
static zend_object_value sdo_model_type_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	zval *tmp; /* this must be passed to hash_copy, but doesn't seem to be used */
	sdo_model_type_object *my_object;

	my_object = (sdo_model_type_object *)emalloc(sizeof(sdo_model_type_object));
	memset(my_object, 0, sizeof(sdo_model_type_object));
	my_object->zo.ce = ce;
	ALLOC_HASHTABLE(my_object->zo.properties);
	zend_hash_init(my_object->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(my_object->zo.properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref,
		(void *)&tmp, sizeof(zval *));
	retval.handle = zend_objects_store_put(my_object, NULL, sdo_model_type_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &sdo_model_type_object_handlers;

	return retval;
}
/* }}} */

/* {{{ sdo_model_type_new
 */
void sdo_model_type_new(zval *me, const Type *typep TSRMLS_DC)
{	
	sdo_model_type_object *my_object;

	Z_TYPE_P(me) = IS_OBJECT;	
	if (object_init_ex(me, sdo_model_typeimpl_class_entry) == FAILURE) {
		php_error(E_ERROR, "%s:%i: object_init failed", CLASS_NAME, __LINE__);
		ZVAL_NULL(me);
		return;
	}

	my_object = (sdo_model_type_object *)zend_object_store_get_object(me TSRMLS_CC);
	my_object->typep = typep;
	zend_update_property_string(sdo_model_typeimpl_class_entry, me, 
		"name", strlen("name"), (char *)typep->getName() TSRMLS_CC);
	zend_update_property_string(sdo_model_typeimpl_class_entry, me, 
		"namespaceURI", strlen("namespaceURI"), (char *)typep->getURI() TSRMLS_CC);
}
/* }}} */

/* {{{ sdo_model_type_compare_objects
 * gets called as a consequence of an == comparison
 */
static int sdo_model_type_compare_objects(zval *object1, zval *object2 TSRMLS_DC) 
{
	sdo_model_type_object	*my_object1, *my_object2;
		
	my_object1 = sdo_model_type_get_instance(object1 TSRMLS_CC);
	my_object2 = sdo_model_type_get_instance(object2 TSRMLS_CC);
	
	if (my_object1->typep == my_object2->typep)
		return SUCCESS;
	
	try {
		return my_object1->typep->equals(*my_object2->typep);
	} catch (SDORuntimeException e) {
		/* In this case we won't rethrow the exception - suffice it to say that the objects are not equal */
		return FAILURE;
	}
}
/* }}} */

/* {{{ sdo_model_type_string
*/
void sdo_model_type_string (ostringstream& print_buf, const Type *typep, const char *old_indent TSRMLS_DC) 
{
	char *indent = (char *)emalloc(strlen(old_indent) + 4 + 1);
	sprintf (indent, "%s    ", old_indent);

	PropertyList pl = typep->getProperties();
	
	print_buf << indent;
	
	if (typep->isOpenType()) {
		print_buf << "<open> ";
	}
	if (typep->isSequencedType()) {
		print_buf << "<sequenced> ";
	}
	if (typep->isDataObjectType()) {
		print_buf << "<dataObject> ";
	}
	print_buf << typep->getURI() << ":" << typep->getName();
	
	print_buf << "[" << pl.size() <<"]";

	const Type *base_typep = typep->getBaseType();
	if (base_typep) {
		print_buf << "<extends " << base_typep->getURI() << ":" << base_typep->getName() << "> ";
	}	

	if (pl.size() > 0) {
		print_buf << " {";
		for (int i = 0; i < pl.size(); i++) {
			sdo_model_property_string (print_buf, &pl[i], indent TSRMLS_CC);
		}
		print_buf << indent << "}";
	} else {
		print_buf << ";";
	}

	efree(indent);
}
/* }}} */

/* {{{ sdo_model_type_cast_object
*/ 
static int sdo_model_type_cast_object(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC) 
{
	sdo_model_type_object *my_object;
	ostringstream print_buf;
	zval free_obj;
	int rc = SUCCESS;
	
	if (should_free) {
		free_obj = *writeobj;
	}
	
	my_object = sdo_model_type_get_instance(readobj TSRMLS_CC);
	if (my_object == (sdo_model_type_object *)NULL) {
		ZVAL_NULL(writeobj);
		php_error(E_ERROR, "%s:%i: object is not in object store", CLASS_NAME, __LINE__);
		rc = FAILURE;
	} else {		
		try {		
			sdo_model_type_string (print_buf, my_object->typep, "\n" TSRMLS_CC);
			string print_string = print_buf.str()/*.substr(0, SDO_TOSTRING_MAX)*/;
			ZVAL_STRINGL(writeobj, (char *)print_string.c_str(), print_string.length(), 1);						
		} catch (SDORuntimeException e) {
			ZVAL_NULL(writeobj);
			sdo_throw_runtimeexception(&e TSRMLS_CC);
			rc = FAILURE;
		}
	}	
	
	switch(type) {
	case IS_STRING:
		convert_to_string(writeobj);
		break;
	case IS_BOOL:
		convert_to_boolean(writeobj);
		break;
	case IS_LONG:
		convert_to_long(writeobj);
		break;
	case IS_DOUBLE:
		convert_to_double(writeobj);
		break;
	default:
		rc = FAILURE;
	}
	
	if (should_free) {
		zval_dtor(&free_obj);
	}
	return rc;
}
/* }}} */

/* {{{ sdo_model_type_minit
 */
void sdo_model_type_minit(zend_class_entry *tmp_ce TSRMLS_DC)
{

	tmp_ce->create_object = sdo_model_type_object_create;
	
	sdo_model_typeimpl_class_entry = zend_register_internal_class(tmp_ce TSRMLS_CC);
	zend_declare_property_null (sdo_model_typeimpl_class_entry, 
		"name", strlen("name"), ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT TSRMLS_CC);
	zend_declare_property_null (sdo_model_typeimpl_class_entry, 
		"namespaceURI", strlen("namespaceURI"), ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT TSRMLS_CC);
	zend_class_implements(sdo_model_typeimpl_class_entry TSRMLS_CC, 1, sdo_model_type_class_entry);

	memcpy(&sdo_model_type_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	sdo_model_type_object_handlers.clone_obj = NULL;
	sdo_model_type_object_handlers.compare_objects = sdo_model_type_compare_objects;
		/*TODO There's a signature change for cast_object in PHP6. */
#if (PHP_MAJOR_VERSION < 6) 
	sdo_model_type_object_handlers.cast_object = sdo_model_type_cast_object;
#endif
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::__construct
 */
PHP_METHOD(SDO_Model_TypeImpl, __construct)
{
	php_error(E_ERROR, "%s:%i: private constructor was called", CLASS_NAME, __LINE__);
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::getName
 */
PHP_METHOD(SDO_Model_TypeImpl, getName) 
{
	sdo_model_type_object	*my_object;
		
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		RETVAL_STRING((char *)my_object->typep->getName(), 1);
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::getNamespaceURI
 */
PHP_METHOD(SDO_Model_TypeImpl, getNamespaceURI) 
{
	sdo_model_type_object	*my_object;
		
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		RETVAL_STRING((char *)my_object->typep->getURI(), 1);
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}

/* {{{ SDO_Model_TypeImpl::isInstance
 */
PHP_METHOD(SDO_Model_TypeImpl, isInstance) 
{
	sdo_model_type_object	*my_object;
	zval					*z_do;
	sdo_do_object			*target;
	int						 argc;
		
	if ((argc = ZEND_NUM_ARGS()) != 1) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(argc TSRMLS_CC, "O", &z_do, sdo_dataobject_class_entry) == FAILURE) {
		return;
	}

	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	if (my_object == (sdo_model_type_object *)NULL) {
		php_error(E_ERROR, "%s:%i: object is not in object store", CLASS_NAME, __LINE__);
		return;
	} 

	target = (sdo_do_object *)zend_object_store_get_object(z_do TSRMLS_CC);
	if (target == (sdo_do_object *)NULL) {
		php_error(E_ERROR, "%s:%i: object is not in object store", CLASS_NAME, __LINE__);
		return;
	} 

	try {
		RETVAL_BOOL(target->dop->getType().equals(*my_object->typep));
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::getProperties
 */
PHP_METHOD(SDO_Model_TypeImpl, getProperties) 
{
	sdo_model_type_object	*my_object;
	PropertyList			 pl;
	zval					*z_property;
		
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		pl = my_object->typep->getProperties();
		array_init(return_value);
		for (int i = 0; i < pl.size(); i++) {
			MAKE_STD_ZVAL(z_property);
			sdo_model_property_new(z_property, &pl[i] TSRMLS_CC);
		    add_next_index_zval(return_value, z_property);
		}
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::getProperty
 */
PHP_METHOD(SDO_Model_TypeImpl, getProperty) 
{
	int						 argc;
	sdo_model_type_object	*my_object;
	const Property			*propertyp;
	zval					*z_offset;
	const char				*xpath = NULL;
	int						 xpath_len;
	long					 property_index;
	
	if ((argc = ZEND_NUM_ARGS()) != 1) {
		WRONG_PARAM_COUNT;
	}
	
	if (zend_parse_parameters(argc TSRMLS_CC, "z", &z_offset) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_P(z_offset) == IS_STRING) {
		xpath = Z_STRVAL_P(z_offset);
		xpath_len = Z_STRLEN_P(z_offset);
	} else {
		if (Z_TYPE_P(z_offset) != IS_LONG) {
			convert_to_long(z_offset);
		}
		property_index = Z_LVAL_P(z_offset);
	}
	
	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		if (xpath)
			propertyp = &my_object->typep->getProperty(xpath);
		else 
			propertyp = &my_object->typep->getProperty(property_index);
		sdo_model_property_new (return_value, propertyp TSRMLS_CC);
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::isDataType
 */
PHP_METHOD(SDO_Model_TypeImpl, isDataType) 
{
	sdo_model_type_object	*my_object;
		
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		RETVAL_BOOL(my_object->typep->isDataType());
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::isSequencedType
 */
PHP_METHOD(SDO_Model_TypeImpl, isSequencedType) 
{
	sdo_model_type_object	*my_object;
		
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		RETVAL_BOOL(my_object->typep->isSequencedType());
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::isOpenType
 */
PHP_METHOD(SDO_Model_TypeImpl, isOpenType) 
{
	sdo_model_type_object	*my_object;
		
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		RETVAL_BOOL(my_object->typep->isOpenType());
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/* {{{ SDO_Model_TypeImpl::getBaseType
 */
PHP_METHOD(SDO_Model_TypeImpl, getBaseType) 
{
	sdo_model_type_object	*my_object;
	const Type				*base_typep;
	
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}
	
	my_object = sdo_model_type_get_instance(getThis() TSRMLS_CC);
	try {
		base_typep = my_object->typep->getBaseType();
		if (base_typep) {
			sdo_model_type_new (return_value, base_typep TSRMLS_CC);
		}
	} catch (SDORuntimeException e) {
		sdo_throw_runtimeexception(&e TSRMLS_CC);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */