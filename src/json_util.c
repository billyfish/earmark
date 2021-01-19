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
#include <string.h>

#include <proto/amijansson.h>
#include <proto/dos.h>

#include "json_util.h"
#include "string_utils.h"
#include "byte_buffer.h"
#include "debugging_utils.h"




static BOOL SetJSONValue (json_t *json_p, const char * const key_s, json_t *value_p);







const char *GetJSONString (const json_t *json_p, const char * const key_s)
{
	const char *value_s = NULL;
	json_t *value_p = IJansson -> json_object_get (json_p, key_s);

	if (value_p)
		{
			if (json_is_string (value_p))
				{
					value_s = IJansson -> json_string_value (value_p);
				}
		}
	
	return value_s;
}


BOOL SetJSONString (json_t *json_p, const char * const key_s, const char * const value_s)
{
	BOOL success_flag = FALSE;

	if (value_s)
		{
			success_flag = SetJSONValue (json_p, key_s, IJansson -> json_string (value_s));
		}
	else
		{
			success_flag = SetJSONNull (json_p, key_s);
		}

	return success_flag;
}



BOOL GetJSONInteger (const json_t *json_p, const char * const key_s, int32 *value_p)
{
	BOOL success_flag = FALSE;
	json_t *json_value_p = IJansson -> json_object_get (json_p, key_s);

	if (json_value_p)
		{
			success_flag = SetIntegerFromJSON (json_value_p, value_p);
		}

	return success_flag;
}


BOOL GetJSONUnsignedInteger (const json_t *json_p, const char * const key_s, uint32 *value_p)
{
	BOOL success_flag = FALSE;
	json_t *json_value_p = IJansson -> json_object_get (json_p, key_s);

	if (json_value_p)
		{
			if (json_is_integer (json_value_p))
				{
 					json_int_t i = IJansson -> json_integer_value (json_value_p);

					if (i >= 0)
						{
							*value_p = (uint32) i;
							success_flag = TRUE;
						}
				}
		}

	return success_flag;
}



BOOL SetJSONInteger (json_t *json_p, const char * const key_s, const int value)
{
	return SetJSONValue (json_p, key_s, IJansson -> json_integer (value));
}


BOOL SetJSONNull (json_t *json_p, const char * const key_s)
{
	BOOL success_flag = FALSE;
	json_t *null_p = IJansson -> json_null ();

	if (null_p)
		{
			if (IJansson -> json_object_set (json_p, key_s, null_p) == 0)
				{
					success_flag = TRUE;
				}
		}

	return success_flag;
}



BOOL GetJSONLong (const json_t *json_p, const char * const key_s, int64 *value_p)
{
	BOOL success_flag = FALSE;
	json_t *json_value_p = IJansson -> json_object_get (json_p, key_s);

	if (json_value_p)
		{
			if (json_value_p)
				{
					success_flag = SetLongFromJSON (json_value_p, value_p);
				}
		}

	return success_flag;
}


BOOL GetJSONReal (const json_t *json_p, const char * const key_s, double *value_p)
{
	BOOL success_flag = FALSE;
	json_t *json_value_p = IJansson -> json_object_get (json_p, key_s);

	if (json_value_p)
		{
			success_flag = SetRealFromJSON (json_value_p, value_p);
		}

	return success_flag;
}



BOOL SetJSONReal (json_t *json_p, const char * const key_s, const double value)
{
	return SetJSONValue (json_p, key_s, IJansson -> json_real (value));
}



BOOL GetJSONBoolean (const json_t *json_p, const char * const key_s, BOOL *value_p)
{
	BOOL success_flag = FALSE;
	json_t *json_value_p = IJansson -> json_object_get (json_p, key_s);

	if (json_value_p)
		{
			success_flag = SetBooleanFromJSON (json_value_p, value_p);
		}

	return success_flag;
}


BOOL SetJSONBoolean (json_t *json_p, const char * const key_s, const BOOL value)
{
	return SetJSONValue (json_p, key_s, value ? IJansson -> json_true () : IJansson -> json_false ());
}



BOOL AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s)
{
	BOOL success_flag = TRUE;

	if (value_s)
		{
			if (IJansson -> json_object_set_new (parent_p, key_s, IJansson -> json_string (value_s)) != 0)
				{
					success_flag = FALSE;
				}
		}

	return success_flag;
}



BOOL IsJSONEmpty (const json_t *json_p)
{
	BOOL empty_flag = FALSE;

	if (json_p)
		{
			switch (json_p -> type)
				{
					case JSON_ARRAY:
						{
							size_t i = IJansson -> json_array_size (json_p);
							empty_flag = (i == 0);
						}
						break;

					case JSON_OBJECT:
						{
							size_t i = IJansson -> json_object_size (json_p);
							empty_flag = (i == 0);
						}
						break;

					case JSON_STRING:
						{
							const char *value_s = IJansson -> json_string_value (json_p);
							empty_flag = IsStringEmpty (value_s);
						}
						break;

					default:
						break;
				}		/* switch (json_p -> type) */

		}		/* if (json_p) */
	else
		{
			empty_flag = TRUE;
		}

	return empty_flag;
}



BOOL SetBooleanFromJSON (const json_t *json_p, BOOL *value_p)
{
	BOOL success_flag = FALSE;

	if (json_is_boolean (json_p))
		{
			if (json_p == IJansson -> json_true ())
				{
					*value_p = TRUE;
					success_flag = TRUE;
				}
			else if (json_p == IJansson -> json_false ())
				{
					*value_p = FALSE;
					success_flag = TRUE;
				}
		}

	return success_flag;
}


BOOL SetRealFromJSON (const json_t *json_p, double *value_p)
{
	BOOL success_flag = FALSE;

	if (json_is_number (json_p))
		{
			*value_p = IJansson -> json_number_value (json_p);
			success_flag = TRUE;
		}
	else
		{
			DB (KPRINTF ("%s %ld - JSON value is of the wrong type, %d not real\n", __FILE__, __LINE__, json_p -> type));
		}

	return success_flag;
}



BOOL SetIntegerFromJSON (const json_t *json_p, int32 *value_p)
{
	BOOL success_flag = FALSE;

	if (json_is_integer (json_p))
		{
			*value_p = IJansson -> json_integer_value (json_p);
			success_flag = TRUE;
		}
	else
		{
			DB (KPRINTF ("%s %ld - JSON value is of the wrong type, %d not integer  json_p -> type\n", __FILE__, __LINE__, json_p -> type));
		}

	return success_flag;
}


BOOL SetLongFromJSON (const json_t *json_p, int64 *value_p)
{
	BOOL success_flag = FALSE;

	if (json_is_integer (json_p))
		{
			*value_p = IJansson -> json_integer_value (json_p);
			success_flag = TRUE;
		}
	else
		{
			DB (KPRINTF ("%s %ld - JSON value is of the wrong type, %d not integer\n", __FILE__, __LINE__, json_p -> type));
		}

	return success_flag;
}


BOOL SetStringFromJSON (const json_t *json_p, char **value_ss)
{
	BOOL success_flag = FALSE;

	if (json_is_string (json_p))
		{
			const char *json_value_s = IJansson -> json_string_value (json_p);

			if (json_value_s)
				{
					char *copied_value_s = CopyToNewString (json_value_s, 0, FALSE);

					if (copied_value_s)
						{
							*value_ss = copied_value_s;
							success_flag = TRUE;
						}
					else
						{
							DB (KPRINTF ("%s %ld - Failed to copy \"%s\"", __FILE__, __LINE__, json_value_s));
						}
				}
			else
				{
					*value_ss = NULL;
					success_flag = TRUE;
				}
		}
	else
		{
			DB (KPRINTF ("%s %ld - JSON value is of the wrong type, %d not string\n", __FILE__, __LINE__, json_p -> type));
		}

	return success_flag;
}


static BOOL SetJSONValue (json_t *json_p, const char * const key_s, json_t *value_p)
{
	if (value_p)
		{
			if (IJansson -> json_object_set_new (json_p, key_s, value_p) == 0)
				{
					return TRUE;
				}

			IJansson -> json_decref (value_p);
		}		/* if (value_p) */

	return FALSE;
}
