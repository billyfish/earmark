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

#include <stdio.h>


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
 * Read a line from a file with no buffer overflow issues.
 *
 * @param in_f The File to read from.
 * @param buffer_pp Pointer to a buffer string where the line will get written to.
 * After being used, *buffer_pp muet be freed.
 * @return TRUE if the line was read successfully, false if there was an error.
 * @ingroup utility_group
 */
BOOL GetLineFromFile (FILE *in_f, char **buffer_pp);


/**
 * Free a buffer that has been allocated with GetLineFromFile.
 *
 * @param buffer_p Pointer to a buffer to be freed.
 * @ingroup utility_group
 */
void FreeLineBuffer (char *buffer_p);


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
 * Parse the format string and sort out into a linked
 * list of tokens.
 *
 * @param format_p The format string to use.
 * @param delimiters_p A custom array of characters to use to separate the tokens
 * in format_p. If this <code>NULL</code> then whitespace will be used
 * to delimit the tokens.
 * @param treat_whitespace_as_delimiter_flag If TRUE whitespace will be treated as a delimiter.
 * @return A pointer to a StringLinkedList where each node
 * refers to a token or NULL upon error.
 * @ingroup utility_group
 */
LinkedList *ParseStringToStringLinkedList (const char * const format_p, const char * const delimiters_p, const BOOL treat_whitespace_as_delimiter_flag);



/**
 * Parse the format string and sort out into a linked
 * list of IntListNodes holding the tokens as numbers.
 *
 * @param format_p The format string to use.
 * @param delimiters_p A custom array of characters to use to separate the tokens
 * in format_p. If this <code>NULL</code> then whitespace will be used
 * to delimit the tokens.
 * @return A pointer to a IntLinkedList where each node
 * refers to a token or NULL upon error.
 * @ingroup utility_group
 */
LinkedList *ParseStringToIntLinkedList (const char * const format_p, const char * const delimiters_p);


/**
 * Parse the format string and sort out into a linked
 * list of IntListNodes holding the tokens as numbers.
 *
 * @param format_p The format string to use.
 * @param delimiters_p A custom array of characters to use to separate the tokens
 * in format_p. If this <code>NULL</code> then whitespace will be used
 * to delimit the tokens.
 * @return A pointer to a IntLinkedList where each node
 * refers to a token or NULL upon error.
 * @ingroup utility_group
 */
LinkedList *ParseStringToDataLinkedList (const char * const format_p, const char * const delimiters_p);


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
 * Get and make copies of two strings from a line of the format
 *
 * key string = value string
 *
 * @param line_p The string to get the values from.
 * @param key_pp Pointer to a new string where the key will be stored upon success.
 * @param value_pp Pointer to a new string where the value will be stored upon success.
 * @param comment_identifier If the line begins with this charcter then treat the whole
 * line as a comment and do not try to get the key and value.
 * @return TRUE if the key and values were successfully copied, FALSE otherwise.
 * @ingroup utility_group
 */
BOOL GetKeyValuePair (char *line_p, char **key_pp, char **value_pp, const char comment_identifier);


/**
 * Get a copy of the next word from a buffer.
 *
 * @param start_pp Address of the buffer to read from.
 * @param delimiters_p An array of characters that can be used as delimiters. If this is NULL
 * then the behvaiour is determined by check_for_whitespace_flag.
 * @param check_for_whitespace_flag
 * @param update_position_flag If this is set to <code>true</code>, then the address pointed to by start_pp will be updated to after
 * the token.
 * @return A copied version of the string or <code>NULL</code> if there was an error or no more tokens were available.
 * @see FreeToken
 * @ingroup utility_group
 */
char *GetNextToken (const char **start_pp, const char *delimiters_p, const BOOL check_for_whitespace_flag, const BOOL update_position_flag);

/**
 * Free a c-style string that has been returned by GetNextToken.
 *
 * @param token_s The token to free.
 * @see GetNextToken
 * @ingroup utility_group
 */
void FreeToken (char *token_s);


/**
 * Format a string containing a number by removing all of its
 * trailing zeroes, e.g.
 *
 *		7.0000 would become 7
 *		7.5000 would become 7.5
 *
 * @param numeric_string_p The string to formatted.
 * @ingroup utility_group
 */
void NullifyTrailingZeroes (char *numeric_string_p);


/**
 * Test whether a string is NULL or contains just whitespace.
 *
 * @param value_s The string to test.
 * @return <code>true</code> If the string is <code>NULL</code> or just whitespace, <code>false</code> otherwise.
 * @ingroup utility_group
 */
BOOL IsStringEmpty (const char *value_s);


/**
 * Do a case-insensitive comparison between two strings.
 * This is a case-insensitive version of the standard ANSI function strcmp.
 *
 * @param c0_s The first string.
 * @param c1_s The second string.
 * @return Less than zero if the first string is less than the second,
 * greater than zero if the first string is greater than the second and
 * zero if the strings match.
 * @ingroup utility_group
 */
int Stricmp (const char *c0_s, const char *c1_s);


/**
 * Do a case-insensitive comparison between the initial portions of two strings.
 * This is a case-insensitive version of the standard ANSI function strncmp.
 *
 * @param c0_s The first string.
 * @param c1_s The second string.
 * @param length The maximum number of characters in each string to compare.
 * @return Less than zero if the first string is less than the second,
 * greater than zero if the first string is greater than the second and
 * zero if the strings match.
 * @ingroup utility_group
 */
int Strnicmp (const char *c0_s, const char *c1_s, size_t length);


/**
 * Find a substring within a string. The search is case-insensitive.
 * This is a case-insensitive version of the standard ANSI function strstr.
 *
 * @param value_s The string to search within.
 * @param substring_s The string to search for
 * @return The substring within the larger string or <code>NULL</code> if it was
 * not found.
 * @ingroup utility_group
 */
const char *Stristr (const char *value_s, const char *substring_s);



/**
 * Find the last occurence of a substring within a string.
 *
 * @param haystack_s The string to search within.
 * @param needle_s The string to search for
 * @return The substring within the larger string or <code>NULL</code> if it was
 * not found.
 * @ingroup utility_group
 */
char *Strrstr (const char *haystack_s, const char *needle_s);



/**
 * Get the string representation of a uuid_t.
 *
 * @param id The uuid_t to get the string representation of.
 * @return A newly-allocated string of the id or <code>NULL</code> upon error.
 * @see FreeUUIDString.
 * @ingroup utility_group
 */
char *GetUUIDAsString (const uuid_t id);


/**
 * Convert a uuid_t into a given string buffer.
 *
 * @param id The uuid_t to get the string representation of.
 * @param uuid_s The buffer to write the representation to. This must be at least
 * UUID_STRING_BUFFER_SIZE bytes long.
 * @see UUID_STRING_BUFFER_SIZE
 * @ingroup utility_group
 */
void ConvertUUIDToString (const uuid_t id, char *uuid_s);



/**
 * Convert the string representation of a uuid_t to a uuid_t.
 *
 * @param id_s The buffer containing the representation.
 * @param id The uuid_t to set.
 * @return <code>true</code> if the uuid_t was set successfully, <code>false</code> otherwise.
 * @ingroup utility_group
 */
BOOL ConvertStringToUUID (const char *id_s, uuid_t id);


/**
 * Free a string representation of a uuid_t.
 *
 * @param uuid_s The string representation to free.
 * @see GetUUIDAsString.
 * @ingroup utility_group
 */
void FreeUUIDString (char *uuid_s);


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

