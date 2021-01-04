/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: memory_buffer.h
**
** Date: 04-01-2021 15:40:04
**
************************************************************

*/


#ifndef MEMORY_BUFFER_H
#define MEMORY_BUFFER_H


#include <exec/types.h>

#include <stddef.h>

typedef struct MemBuffer 
{
	char *mb_data_p;
	size_t mb_total_size;
	size_t mb_used_size;
} MemBuffer;


BOOL InitMemBuffer (MemBuffer *buffer_p, size_t size);

void ClearMemBuffer (MemBuffer *buffer_p);

BOOL ExtendMemBuffer (MemBuffer *buffer_p, size_t new_size);

BOOL AppendToMemBuffer (MemBuffer *buffer_p, const char *data_p, size_t size);

BOOL MakeBufferValidString (MemBuffer *buffer_p);

#endif

