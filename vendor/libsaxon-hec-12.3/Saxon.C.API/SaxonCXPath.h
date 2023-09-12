#ifndef SAXONCXPATH_H
#define SAXONCXPATH_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Liimited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SaxonCProcessor.h"

//===============================================================================================//

EXTERN_SAXONC

/**
 * Create boxed Boolean value
 * @param val - boolean value
 */

int64_t booleanValue(sxnc_environment *environi, bool);

/**
 * Create an boxed Integer value
 * @param val - int value
 */

int64_t integerValue(sxnc_environment *environi, int i);

/**
 * Create an boxed Double value
 * @param val - double value
 */

int64_t doubleValue(sxnc_environment *environi, double d);

/**
 * Create an boxed Float value
 * @param val - float value
 */

int64_t floatValue(sxnc_environment *environi, float f);

/**
 * Create an boxed Long value
 * @param val - Long value
 */

int64_t longValue(sxnc_environment *environi, long l);

/**
 * Create an boxed String value
 * @param val - as char array value
 */

int64_t getJavaStringValue(sxnc_environment *environi, const char *str);

/**
 * A Constructor. Create a XdmValue based on the target type. Conversion is
 * applied if possible
 * @param type - specify target type of the value
 * @param val - Value to convert
 */

int64_t xdmValueAsObj(sxnc_environment *environi, const char *type,
                      const char *str);

/**
 * A Constructor. Create a XdmValue based on the target type. Conversion is
 * applied if possible
 * @param type - specify target type of the value
 * @param val - Value to convert
 */

sxnc_value *xdmValue(sxnc_environment *environi, const char *type,
                     const char *str);

int c_createXPathProcessor(sxnc_environment *environi,
                           sxnc_processor *processor, sxnc_xpath *xpath);

/**
 * Compile and evaluate an XPath expression, supplied as a character string,
 *with properties and parameters required by the XPath expression
 * @param cwd  - Current working directory
 * @param xpathStr  - A string containing the source text of the XPath
 *expression
 * @param params - Parameters and properties names required by the XPath
 *expression. This could contain the context node , source as string or file
 *name, etc
 * @param values -  The values for the parameters and properties required by the
 *XPath expression
 **/

sxnc_value *evaluate(sxnc_environment *environi, sxnc_xpath *proc, char *cwd,
                     char *xpathStr, sxnc_parameter *parameters,
                     sxnc_property *properties, int parLen, int propLen);

sxnc_value *evaluateSingle(sxnc_environment *environi, sxnc_xpath *proc,
                           char *cwd, char *xpathStr,
                           sxnc_parameter *parameters,
                           sxnc_property *properties, int parLen, int propLen);

/**
 * Evaluate the XPath expression, returning the effective boolean value of the
 *result.
 *
 * @param cwd  - Current working directory
 * @param xpathStr  - A string containing the source text of the XPath
 *expression
 * @param params - Parameters and properties names required by the XPath
 *expression. This could contain the context node , source as string or file
 *name, etc
 * @param values -  The values for the parameters and properties required by the
 *XPath expression
 *
 *
 **/

bool effectiveBooleanValue(sxnc_environment *environi, sxnc_xpath *proc,
                           char *cwd, char *xpathStr,
                           sxnc_parameter *parameters,
                           sxnc_property *properties, int parLen, int propLen);

/**
 * Determine whether the item is an atomic value or a node
 *
 * @return true if the item is an atomic value, false if it is a node
 */

bool isAtomicValue(sxnc_environment *environi, sxnc_value value);

/**
 * Get the number of items in the sequence
 *
 * @return the number of items in the value - always one
 */

int size(sxnc_environment *environi, sxnc_value val);

/**
 * Get the n'th item in the value, counting from zero.
 *
 * @param n the item that is required, counting the first item in the sequence
 * as item zero
 * @return the n'th item in the sequence making up the value, counting from zero
 */

sxnc_value *itemAt(sxnc_environment *environi, sxnc_value, int i);

int64_t getvalue(sxnc_environment *environi, sxnc_value);

/**
 * Get the string value of the item. For a node, this gets the string value
 * of the node. For an atomic value, it has the same effect as casting the value
 * to a string. In all cases the result is the same as applying the XPath
 *string() function.
 **/

const char *getStringValue(sxnc_environment *environi, sxnc_value value);

/**
 *Get the integer value of the item. if the XdmItem is not castable to an
 *integer then
 * @param sxnc_value - Value to convert to integer
 * @failure_value - If the Value is not an integer then we can specify the value
 *to return. Default is zero.
 *
 **/

int getIntegerValue(sxnc_environment *environi, sxnc_value value,
                    int failure_value);

bool getBooleanValue(sxnc_environment *environi, sxnc_value value);

long getLongValue(sxnc_environment *environi, sxnc_value value,
                  long failureVal);

float getFloatValue(sxnc_environment *environi, sxnc_value value,
                    float failureVal);

double getDoubleValue(sxnc_environment *environi, sxnc_value value,
                      double failureVal);

EXTERN_SAXONC_END

#endif
