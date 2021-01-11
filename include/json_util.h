/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
#ifndef JSON_UTIL_H
#define JSON_UTIL_H


#include <exec/types.h>

#include <stdio.h>


#include "jansson.h"






#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @return The corresponding value or <code>NULL</code> if the key does not exist in the
 * given json_t object.
 *
 * @ingroup utility_group
 */
const char *GetJSONString (const json_t *json_p, const char * const key_s);



BOOL SetJSONString (json_t *json_p, const char * const key_s, const char * const value_s);


/**
 * Get the newly-allocated value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @return A newly-allocated copy of the corresponding value or <code>NULL</code> if the key does not exist in the
 * given json_t object or failed to be copied. This value, if valid, should be freed with FreeCopiedString().
 *
 * @ingroup utility_group
 */
char *GetCopiedJSONString (const json_t *json_p, const char * const key_s);


/**
 * Get the integer value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to an int.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
BOOL GetJSONInteger (const json_t *json_p, const char * const key_s, int32 *value_p);


BOOL SetJSONInteger (json_t *json_p, const char * const key_s, const int value);

/**
 * Get the long value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to an int.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
BOOL GetJSONLong (const json_t *json_p, const char * const key_s, int64 *value_p);


/**
 * Get the double value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to a double.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
BOOL GetJSONReal (const json_t *json_p, const char * const key_s, double *value_p);


BOOL SetJSONReal ( json_t *json_p, const char * const key_s, const double value);


/**
 * Get the boolean value for a given key in a json_t object.
 *
 * @param json_p The json object to query.
 * @param key_s The key to search for.
 * @param value_p Where the corresponding value will be stored if the key exists and relates to a boolean.
 * @return <code>true</code> if the value was retrieved successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
BOOL GetJSONBoolean (const json_t *json_p, const char * const key_s, BOOL *value_p);


BOOL SetJSONBoolean (json_t *json_p, const char * const key_s, const BOOL value);


/**
 * Add a string key-value pair to a JSON object only if the value is not NULL
 *
 * @param parent_p The JSON object that the key-value pair will be added to.
 * @param key_s The key to use.
 * @param value_s The value to use.
 * @return <code>true</code> if value_s is not <code>NULL</code> and the key-value pair were added successfully
 * or if value_s is <code>NULL</code>. If value_s points to valid data and the key-value pair fails to get added,
 * <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
BOOL AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s);




/**
 * Is a json_t <code>NULL</code> or empty?
 *
 * @param json_p The json_t object to test.
 * @return <code>true</code> if the json_t is <code>NULL</code>, an empty array, an empty object or
 * an empty string, <code>false</code> otherwise.
 * @see IsStringEmpty
 *
 * @ingroup utility_group
 */
BOOL IsJSONEmpty (const json_t *json_p);


/**
 * Set the boolean value of a JSON object.
 *
 * @param json_p The JSON value to get the boolean value from.
 * @param value_p Pointer to where the boolean value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a boolean type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
BOOL SetBooleanFromJSON (const json_t *json_p, BOOL *value_p);


/**
 * Get the real value of a JSON object.
 *
 * @param json_p The JSON value to get the real value from.
 * @param value_p Pointer to where the real value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a real type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
BOOL SetRealFromJSON (const json_t *json_p, double *value_p);


/**
 * Get the integer value of a JSON object.
 *
 * @param json_p The JSON value to get the integer value from.
 * @param value_p Pointer to where the integer value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a integer type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
BOOL SetIntegerFromJSON (const json_t *json_p, int32 *value_p);


/**
 * Get the long value of a JSON object.
 *
 * @param json_p The JSON value to get the long value from.
 * @param value_p Pointer to where the long value will be stored if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved. If
 * the JSON value is not a long type <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
BOOL SetLongFromJSON (const json_t *json_p, int64 *value_p);


/**
 * Get the string value of a JSON object.
 *
 * @param json_p The JSON value to get the boolean value from.
 * @param value_ss Pointer to where the string value will be copied to if
 * it is successfully retrieved.
 * @return <code>true</code> if the value was successfully retrieved and copied. If
 * the JSON value is not a string type or the memory for copying the string value
 * could not be allocated then <code>false</code> will be returned.
 *
 * @ingroup utility_group
 */
BOOL SetStringFromJSON (const json_t *json_p, char **value_ss);



/**
 * Add a child key to a JSON object where the value is null.
 *
 * @param json_p The JSON object to add the vaue to.
 * @param key_s The key for the child object.
 * @return <code>true</code> if the value was successfully added, <code>false</code>
 * otherwise.
 * @ingroup utility_group
 */
BOOL SetJSONNull (json_t *json_p, const char * const key_s);




#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_UTIL_H */
