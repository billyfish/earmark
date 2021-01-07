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
#include <stdarg.h>

#include <proto/exec.h>

#include "byte_buffer.h"
#include "string_utils.h"


ByteBuffer *AllocateByteBuffer (size_t initial_size)
{
	char *data_p = (char *) IExec -> AllocVecTags (initial_size, AVT_ClearWithValue, 0, TAG_DONE);
	
	if (data_p)
		{
			ByteBuffer *buffer_p = (ByteBuffer *) IExec -> AllocVecTags (sizeof (ByteBuffer)), TAG_DONE;
			
			if (buffer_p)
				{
					buffer_p -> bb_data_p = data_p;
					buffer_p -> bb_size  = initial_size;
					buffer_p -> bb_current_index = 0;
					
					return buffer_p;
				}
			
			IExec -> FreeVec (data_p);
		}
		
	return NULL;
}


void FreeByteBuffer (ByteBuffer *buffer_p)
{
	if (buffer_p -> bb_data_p)
		{
			IExec -> FreeVec (buffer_p -> bb_data_p);
		}

	IExec -> FreeVec (buffer_p);
}


bool ExtendByteBuffer (ByteBuffer *buffer_p, size_t increment)
{
	return ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + increment);
}


bool ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size)
{
	bool success_flag = false;		
	char *new_data_p = (char *) IExec -> AllocVecTags (new_size, AVT_ClearWithValue, 0, TAG_DONE);
	
	if (new_data_p)
		{
			if (new_size > buffer_p -> bb_current_index)
				{
					IExec -> CopyMem (buffer_p -> bb_data_p, new_data_p, buffer_p -> bb_current_index);
				}
				
			IExec -> FreeVec (buffer_p -> bb_data_p);			

			buffer_p -> bb_data_p = new_data_p;
			buffer_p -> bb_size = new_size;
			
			success_flag = true;
		}
	
	return success_flag;
}


void RemoveFromByteBuffer (ByteBuffer *buffer_p, size_t size)
{
	if (buffer_p -> bb_current_index > size)
		{
			buffer_p -> bb_current_index -= size;
			* ((buffer_p -> bb_data_p) + (buffer_p -> bb_current_index)) = '\0';
		}
	else
		{
			ResetByteBuffer (buffer_p);
		}
}


bool AppendStringToByteBuffer (ByteBuffer *buffer_p, const char * const value_s)
{
	return AppendToByteBuffer (buffer_p, value_s, strlen (value_s));
}


bool AppendToByteBuffer (ByteBuffer *buffer_p, const void *data_p, const size_t data_length)
{
	const size_t space_remaining = GetRemainingSpaceInByteBuffer (buffer_p);
	bool success_flag = true;
	
	if (space_remaining <= data_length)
		{
			success_flag = ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + data_length - space_remaining + 1);
		}
		
	if (success_flag)
		{
			char *current_data_p = (buffer_p -> bb_data_p) + (buffer_p -> bb_current_index);
			
			IExec -> CopyMem (data_p, current_data_p, data_length);			
			buffer_p -> bb_current_index += data_length;
		}
		
	return success_flag;
}



bool AppendStringsToByteBuffer (ByteBuffer *buffer_p, const char *value_s, ...)
{
	bool success_flag;
	va_list args;

	va_start (args, value_s);

	success_flag = AppendVarArgsToByteBuffer (buffer_p, value_s, args);

	va_end (args);

	return success_flag;
}


bool AppendVarArgsToByteBuffer (ByteBuffer *buffer_p, const char *value_s, va_list args)
{
	bool success_flag = true;
	bool loop_flag = true;
	const char *arg_s = value_s;

	while (success_flag && loop_flag)
		{
			if (arg_s)
				{
					success_flag = AppendToByteBuffer (buffer_p, arg_s, strlen (arg_s));
					arg_s = va_arg (args, char *);
				}
			else
				{
					loop_flag = false;
				}
		}

	return success_flag;
}



void ResetByteBuffer (ByteBuffer *buffer_p)
{
	buffer_p -> bb_current_index = 0;
	memset (buffer_p -> bb_data_p, 0, buffer_p -> bb_size);
}


size_t GetRemainingSpaceInByteBuffer (const ByteBuffer * const buffer_p)
{
	return (buffer_p -> bb_size) - (buffer_p -> bb_current_index);
}


size_t GetByteBufferSize (const ByteBuffer * const buffer_p)
{
	return (buffer_p -> bb_current_index);
}


const char *GetByteBufferData (const ByteBuffer * const buffer_p)
{
	return buffer_p -> bb_data_p;
}



char *DetachByteBufferData (ByteBuffer * const buffer_p)
{
	char *value_s = buffer_p -> bb_data_p;

	buffer_p -> bb_data_p = NULL;
	FreeByteBuffer (buffer_p);

	return value_s;
}


void ReplaceCharsInByteBuffer (ByteBuffer *buffer_p, char old_data, char new_data)
{
	ReplaceChars (buffer_p -> bb_data_p, old_data, new_data);
}


