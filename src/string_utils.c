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
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include <proto/exec.h>

#include "string_utils.h"
#include "byte_buffer.h"
#include "debugging_utils.h"



void FreeCopiedString (char *str_p)
{
	if (str_p)
		{
			IExec -> FreeVec (str_p);
		}
}


char *EasyCopyToNewString (const char * const src_s)
{
	return CopyToNewString (src_s, 0, FALSE);
}


/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error.
 */
char *CopyToNewString (const char * const src_p, const size_t l, BOOL trim)
{
	char *dest_p = NULL;

	if (src_p)
		{
			size_t len = strlen (src_p);

			if (l > 0)
				{
					if (len > l)
						{
							len = l;
						}
				}

			if (trim)
				{
					const char *start_p = src_p;
					const char *end_p = src_p + len - 1;

					while (isspace (*start_p))
						{
							++ start_p;
						}

					if (*start_p == '\0')
						{
							/* all white space */
							return NULL;
						}

					while ((start_p < end_p) && (isspace (*end_p)))
						{
							-- end_p;
						}

					if (start_p <= end_p)
						{
							ptrdiff_t d = end_p - start_p + 1;
							dest_p = (char *) IExec ->  AllocVecTags (d + 1, TAG_DONE);

							if (dest_p)
								{
									memcpy (dest_p, start_p, d);
									* (dest_p + d) = '\0';

									return dest_p;
								}

						}
				}
			else
				{
					dest_p = (char *) IExec -> AllocVecTags (len + 1, TAG_DONE);

					if (dest_p)
						{
							strncpy (dest_p, src_p, len);
							* (dest_p + len) = '\0';

							return dest_p;
						}
				}
		}

	return dest_p;
}



BOOL CloneValidString (const char *src_s, char **dest_ss)
{
	BOOL success_flag = FALSE;


	if (!IsStringEmpty (src_s))
		{
			char *dest_s = EasyCopyToNewString (src_s);

			if (dest_s)
				{
					*dest_ss = dest_s;
					success_flag = TRUE;
				}
		}
	else
		{
			*dest_ss = NULL;
			success_flag = TRUE;
		}

	return success_flag;
}




char *ConcatenateStrings (const char * const first_s, const char * const second_s)
{
	const size_t len1 = (first_s != NULL) ? strlen (first_s) : 0;
	const size_t len2 = (second_s != NULL) ? strlen (second_s) : 0;

	char *result_s = (char *) IExec -> AllocVecTags (sizeof (char) * (len1 + len2 + 1), TAG_DONE);

	if (result_s)
		{
			if (len1 > 0)
				{
					strncpy (result_s, first_s, len1);
				}

			if (len2 > 0)
				{
					strcpy (result_s + len1, second_s);
				}

			* (result_s + len1 + len2) = '\0';
		}

	return result_s;
}





void ReplaceCharacter (char *value_s, const char char_to_replace, const char replacement_char)
{
	char *ptr = strchr (value_s, char_to_replace);

	while (ptr)
		{
			*ptr = replacement_char;
			ptr = strchr (ptr + 1, char_to_replace);
		}
}


/*
 * Taken from
 *
 * https://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
 *
 * You must free the result if result is non-NULL.
 */
BOOL SearchAndReplaceInString (const char *src_s, char **dest_ss, const char *to_replace_s, const char *with_s)
{
	BOOL success_flag = TRUE;
	char *result_s = NULL; // the return string
	int len_rep;  // length of rep (the string to remove)
	int len_with; // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep

	// sanity checks and initialization
	if (!IsStringEmpty (src_s))
		{
			if (to_replace_s)
				{
					size_t to_replace_length = strlen (to_replace_s);
					size_t with_length;
					char *tmp_p;
					char *insert_p;    // the next insert point
					size_t count;    // number of replacements

					if (with_s)
						{
							with_length = strlen (with_s);
						}
					else
						{
							with_s = "";
							with_length = 0;
						}

					// count the number of replacements needed
					insert_p = (char *) src_s;
					for (count = 0; (tmp_p = strstr (insert_p, to_replace_s)) != NULL; ++ count)
						{
							insert_p = tmp_p + to_replace_length;
						}

					DB (KPRINTF ("%s %ld - SearchAndReplaceInString  \"%s\" , \"%s\", %lu matches\n",	__FILE__, __LINE__, src_s, to_replace_s, count));


					result_s = (char *) IExec -> AllocVecTags (strlen (src_s) + ((with_length - to_replace_length) * count) + 1, TAG_DONE);

					if (result_s)
						{
							tmp_p = result_s;

							// first time through the loop, all the variable are set correctly
							// from here on,
							//    tmp points to the end of the result string
							//    insert_p points to the next occurrence of rep in src_s
							//    src_s points to the remainder of src_s after "end of rep"
							while (count --)
								{
									insert_p = strstr (src_s, to_replace_s);
									len_front = insert_p - src_s;
									tmp_p = strncpy (tmp_p, src_s, len_front) + len_front;
									tmp_p = strcpy (tmp_p, with_s) + with_length;
									src_s += len_front + to_replace_length; // move to next "end of rep"
								}
							strcpy (tmp_p, src_s);

							*dest_ss = result_s;
						}
					else
						{
							success_flag = FALSE;
						}


				}		/* if (!IsStringEmpty (to_replace_s)) */
			else
				{
					DB (KPRINTF ("%s %ld - SearchAndReplaceInString empty replace\n",	__FILE__, __LINE__));
				}
		}		/* if (!IsStringEmpty (src_s)) */
	else
		{
			DB (KPRINTF ("%s %ld - SearchAndReplaceInString empty src\n",	__FILE__, __LINE__));
		}
		
	return success_flag;
}




BOOL IsStringEmpty (const char *value_s)
{
	if (value_s)
		{
			while (*value_s)
				{
					if (isspace (*value_s))
						{
							++ value_s;
						}
					else
						{
							return FALSE;
						}
				}
		}

	return TRUE;
}





BOOL ReplaceStringValue (char **dest_ss, const char *value_s)
{
	BOOL success_flag = FALSE;
	char *new_value_s = CopyToNewString (value_s, 0, FALSE);

	if (new_value_s)
		{
			if (*dest_ss)
				{
					FreeCopiedString (*dest_ss);
				}

			*dest_ss = new_value_s;
			success_flag = TRUE;
		}

	return success_flag;
}



void ReplaceChars (char *value_s, char old_data, char new_data)
{
	char *value_p = value_s;

	while (*value_p != '\0')
		{
			if (*value_p == old_data)
				{
					*value_p = new_data;
				}

			++ value_p;
		}
}




char *ConcatenateVarargsStrings (const char *value_s, ...)
{
	char *result_s = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			va_list args;

			va_start (args, value_s);

			if (AppendVarArgsToByteBuffer (buffer_p, value_s, args))
				{
					result_s = DetachByteBufferData (buffer_p);
				}		/* if (AppendVarArgsToByteBuffer (buffer_p, value_s, args)) */

			va_end (args);
		}		/* if (buffer_p) */

	return result_s;
}





BOOL DoesStringContainWhitespace (const char *value_s)
{
	while (*value_s != '\0')
		{
			if (isspace (*value_s))
				{
					return TRUE;
				}
			else
				{
					++ value_s;
				}
		}

	return FALSE;
}


BOOL DoesStringEndWith (const char *value_s, const char *ending_s)
{
	BOOL matching_ends_flag = FALSE;
	const size_t value_length = strlen (value_s);
	const size_t ending_length = strlen (ending_s);

	if (value_length >= ending_length)
		{
			const size_t offset = value_length - ending_length;

			value_s += offset;

			if (strcmp (value_s, ending_s) == 0)
				{
					matching_ends_flag = TRUE;
				}
		}

	return matching_ends_flag;
}


char *GetStringAsLowerCase (const char *value_s)
{
	char *lower_s = NULL;

	if (value_s)
		{
			lower_s = EasyCopyToNewString (value_s);

			if (lower_s)
				{
					char *c_p = lower_s;

					while (*c_p != '\0')
						{
							*c_p = tolower (*c_p);
							++ c_p;
						}
				}
		}

	return lower_s;

}
