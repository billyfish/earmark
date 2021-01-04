/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: memory_buffer.c
**
** Date: 04-01-2021 15:40:59
**
************************************************************

*/

#include "memory_buffer.h"

#include <proto/exec.h>


BOOL InitMemBuffer (MemBuffer *buffer_p, size_t size)
{
	BOOL res = FALSE;
	char *data_p = (char *) IExec->AllocVecTags (size, TAG_DONE);
	
	if (data_p)
		{
	    buffer_p -> mb_used_size = 0;
     	buffer_p -> mb_total_size = size;
   		buffer_p -> mb_data_p = data_p;	
   			
			res = TRUE;
		}
	
	return res;
}


void ClearMemBuffer (MemBuffer *buffer_p)
{
	if (buffer_p -> mb_data_p)
		{
			IExec -> FreeVec (buffer_p -> mb_data_p);
			buffer_p -> mb_data_p = NULL;
		}
}


BOOL ExtendMemBuffer (MemBuffer *buffer_p, size_t new_size)
{
	BOOL res = TRUE;
	uint32 alloc_size = IExec -> ReallocVec (buffer_p -> mb_data_p, new_size, 0); 

	if (alloc_size == new_size)
		{
			buffer_p -> mb_total_size = new_size;
		}
	else
		{	
			res = FALSE;
		}
 
  return res;
}


BOOL AppendToMemBuffer (MemBuffer *buffer_p, const char *data_p, size_t size)
{
	BOOL res = TRUE;
	
  if (buffer_p -> mb_total_size < (buffer_p -> mb_used_size) + size)
    {
      ExtendMemBuffer (buffer_p, buffer_p -> mb_total_size + (buffer_p -> mb_total_size / 2) + size);
		}
		
	if (res)
		{
	    IExec -> CopyMem (data_p, (buffer_p -> mb_data_p) + (buffer_p -> mb_used_size), size);
  	  buffer_p -> mb_used_size += size;
  	}
  	
  return res;
}


BOOL MakeBufferValidString (MemBuffer *buffer_p)
{
	const char *terminator_s = "\0";
	
	return AppendToMemBuffer (buffer_p, terminator_s, 1); 	
}

