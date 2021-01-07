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
#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <stdarg.h>
#include <stddef.h>

#include <exec/types.h>

/**
 * A datatype to allow an automatically growing buffer for appending
 * data to.
 * @ingroup utility_group
 */
typedef struct ByteBuffer
{
	/** @privatesection */
	char *bb_data_p;
	size_t bb_size;
	size_t bb_current_index;
} ByteBuffer;


/** @publicsection */


/**
 * Allocate a ByteBuffer.
 *
 * @param initial_size The initial size of the data buffer that this ByteBuffer will have.
 * @return The newly-allocated ByteBuffer or <code>NULL</code> on error.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API ByteBuffer *AllocateByteBuffer (size_t initial_size);


/**
 * Free a ByteBuffer.
 *
 * @param buffer_p The ByteBuffer to free.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API void FreeByteBuffer (ByteBuffer *buffer_p);


/**
 * Resize a ByteBuffer.
 * The content of the data buffer will be preserved.
 *
 * @param buffer_p The ByteBuffer to resize.
 * @param new_size The new size of the ByteBuffer's data.
 * @return <code>true</code> if the resize was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API BOOL ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size);


/**
 * Increase the size of a ByteBuffer's data buffer.
 * The content of the data buffer will be preserved.
 *
 * @param buffer_p The ByteBuffer to resize.
 * @param increment The amount to increase the ByteBuffer's data buffer.
 * @return <code>true</code> if the resize was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API BOOL ExtendByteBuffer (ByteBuffer *buffer_p, size_t increment);


/**
 * Append some data to a ByteBuffer's data buffer.
 *
 * @param buffer_p The ByteBuffer whose data buffer the new data will be appended to.
 * @param data_p The data to append.
 * @param data_length The length, in bytes, of the data to append.
 * @return <code>true</code> if the append was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API BOOL AppendToByteBuffer (ByteBuffer *buffer_p, const void *data_p, const size_t data_length);


/**
 * Append a string to a ByteBuffer's data buffer.
 *
 * @param buffer_p The ByteBuffer whose data buffer the string will be appended to.
 * @param value_s The <code>NULL</code> terminated string to append.
 * @return <code>true</code> if the append was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API BOOL AppendStringToByteBuffer (ByteBuffer *buffer_p, const char * const value_s);


/**
 * Append a varargs array of strings to a ByteBuffer's data buffer.
 *
 * @param buffer_p The ByteBuffer whose data buffer the string will be appended to.
 * @param value_s The varags-style array of <code>NULL</code> terminated strings to append. The final entry
 * in this varags-array must be a <code>NULL</code>.
 * @return <code>true</code> if the append was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 * @see AppendVarArgsToByteBuffer
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API BOOL AppendStringsToByteBuffer (ByteBuffer *buffer_p, const char *value_s, ...);


/**
 * Append a va_list of strings to a ByteBuffer's data buffer.
 *
 * @param buffer_p The ByteBuffer whose data buffer the string will be appended to.
 * @param value_s The varargs-style array of <code>NULL</code> terminated strings to append. The final entry
 * in this varargs-array must be a <code>NULL</code>.
 * @param args The varargs list of arguments used by value_s.
 * @return <code>true</code> if the append was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API BOOL AppendVarArgsToByteBuffer (ByteBuffer *buffer_p, const char *value_s, va_list args);


/**
 * Clear any data stored in a ByteBuffer.
 *
 *
 * @param buffer_p The ByteBuffer who will have all of its data emptied.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API void ResetByteBuffer (ByteBuffer *buffer_p);


/**
 * Get the remaining space in a ByteBuffer's data buffer.
 *
 * @param buffer_p The ByteBuffer to get the remaining space for.
 * @return The space remaining by the ByteBuffer in bytes.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API size_t GetRemainingSpaceInByteBuffer (const ByteBuffer * const buffer_p);


/**
 * Get the currently used size of a ByteBuffer's data buffer.
 *
 * @param buffer_p The ByteBuffer to get the space used for.
 * @return The space used by the ByteBuffer in bytes.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API size_t GetByteBufferSize (const ByteBuffer * const buffer_p);


/**
 * Get the data stored in a ByteBuffer.
 *
 * @param buffer_p The ByteBuffer to get the data from.
 * @return The data as valid c-style string.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API const char *GetByteBufferData (const ByteBuffer * const buffer_p);



/**
 * Get the data stored in a ByteBuffer and free a ByteBuffer
 *
 * @param buffer_p The ByteBuffer to get the data from which will be deallocated
 * after this call.
 * @return The data as valid c-style string.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API char *DetachByteBufferData (ByteBuffer * const buffer_p);



/**
 * Remove data from the end of a byte buffer
 *
 * @param buffer_p The ByteBuffer to remove the data from
 * @param size The number of bytes to remove. If this is greater than the size of the
 * ByteBuffer, then the ByteBUffer will be reset.
 * @memberof ByteBuffer
 */
GRASSROOTS_UTIL_API void RemoveFromByteBuffer (ByteBuffer *buffer_p, size_t size);


/**
 * Replace each instance of a character within a ByteBuffer with another
 *
 * @param buffer_p The ByteBuffer to replace the character in.
 * @param old_data The character to be replaced.
 * @param new_data The replacement character.
 * @memberof ByteBuffer
 * @see ReplaceChars
 */
GRASSROOTS_UTIL_API void ReplaceCharsInByteBuffer (ByteBuffer *buffer_p, char old_data, char new_data);



#endif		/* #ifndef BYTE_BUFFER_H */
