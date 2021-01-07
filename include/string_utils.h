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
/**@file string_utils.h
 * @brief A set of string functions.
 *
 *  A variety of functions for manipulating strings.
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H



#include <exec/types.h>


/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param length_to_copy If this is greater than zero, copy this many characters from src_p
 * or its entirety, whichever is smaller. If this is zero or less then all of src_p is copied.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error. If
 * the string was successfully created, its deallocation must be done with FreeCopiedString()
 * when you are finished with it.
 * @ingroup utility_group
 */
char *CopyToNewString (const char * const src_p, const size_t length_to_copy, BOOL trim);


/**
 * Copy a string to a newly created string. This calls CopyToNewString() with length_to_copy set to
 * the length of the source string and trim set to <code>false</code>.
 *
 * @param src_s he string to copy.
 * @return A newly allocated copy of the source string or NULL if there was an error. If
 * the string was successfully created, its deallocation must be done with FreeCopiedString()
 * when you are finished with it.
 * @ingroup utility_group
 */
char *EasyCopyToNewString (const char * const src_s);


/**
 * Delete a string previously allocated using CopyToNewString.
 *
 * @param str_p The string to deallocate. This is checked for being <code>NULL</code> in which
 * case this function becomes a no-op routine.
 * @ingroup utility_group
 */
void FreeCopiedString (char *str_p);


/**
 * Clone a valid string to a destination string pointer.
 *
 * @param src_s The string to clone. If this is <code>NULL</code> then
 * the function will treat it as a valid clone.
 * @param dest_ss The destination where the cloned string will be stored
 * if successful. If src_s is <code>NULL</code>, then this will be set to <code>
 * NULL</code> too.
 * @return <code>true</code> if the string was successfully cloned or
 * it src_s is <code>NULL</code>, <code>false</code> otherwise.
 */
BOOL CloneValidString (const char *src_s, char **dest_ss);


/**
 * Get a newly created string that is the concatenation of two others.
 *
 * @param first_s  The first string to concatenate.
 * @param second_s  The second string to concatenate.
 * @return The newly created string or NULL if there was an error.
 * @ingroup utility_group
 */
char *ConcatenateStrings (const char * const first_s, const char * const second_s);


/**
 * Replace all instances of a character by another within a string
 *
 * @param value_s The string to be altered.
 * @param char_to_replace  The character to replace.
 * @param replacement_char The replacement character.
 * @ingroup utility_group
 */
void ReplaceCharacter (char *value_s, const char char_to_replace, const char replacement_char);



BOOL SearchAndReplaceInString (const char *src_s, char **dest_ss, const char *to_replace_s, const char *with_s);



/**
 * Test whether a string is NULL or contains just whitespace.
 *
 * @param value_s The string to test.
 * @return <code>true</code> If the string is <code>NULL</code> or just whitespace, <code>false</code> otherwise.
 * @ingroup utility_group
 */
BOOL IsStringEmpty (const char *value_s);


/**
 * Make a copy of a string.
 *
 * @param dest_ss Pointer to the string where the source string
 * will be copied to.
 * @param src_s The string to copy.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * If successful, any string that dest_ss currently points to will be freed
 * prior to pointing at the copy. If the function is unsuccessful then the value
 * pointed to by dest_ss will remain intact.
 * @ingroup utility_group
 */
BOOL ReplaceStringValue (char **dest_ss, const char * const src_s);


/**
 * Replace each instance of a character within a string with another
 *
 * @param value_s The string to replace the character in.
 * @param old_data The character to be replaced.
 * @param new_data The replacement character.
 * @ingroup utility_group
 */
void ReplaceChars (char *value_s, char old_data, char new_data);



/**
 * Concatenate a va_list of strings.
 *
 * @param value_s The varargs-style array of <code>NULL</code> terminated strings to append. The final entry
 * in this varargs-array must be a <code>NULL</code>.
 * @return The newly-allocated string that should be freed with FreeCopiedString() when
 * no longer needed or <code>NULL</code> upon failure.
 * @ingroup utility_group
 */
char *ConcatenateVarargsStrings (const char *value_s, ...);


/**
 * Test whether a given string contains any whitespace characters.
 *
 * @param value_s The string to check.
 * @return <code>true</code> if the string contains any whitespace characters, <code>false</code> otherwise.
 * @ingroup utility_group
 */
BOOL DoesStringContainWhitespace (const char *value_s);


BOOL DoesStringEndWith (const char *value_s, const char *ending_s);


char *GetStringAsLowerCase (const char *value_s);





#endif	/* #ifndef STRING_UTILS_H */

