/*
 * MD4C: Markdown parser for C
 * (http://github.com/mity/md4c)
 *
 * Copyright (c) 2016-2020 Martin Mitas
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "md4c-html.h"

#include <exec/types.h>

#include <proto/dos.h>
#include <proto/exec.h>


#include "byte_buffer.h"
#include "string_utils.h"


static void AppendChunk (const MD_CHAR *text_s, MD_SIZE size, void *data_p);


BOOL ConvertText (const char *in_s, char **out_ss, uint16 parser_flags, uint16 renderer_flags, BOOL full_html_flag)
{
	BOOL success_flag = FALSE;
	const size_t in_length = strlen (in_s);
	ByteBuffer *output_buffer_p = AllocateByteBuffer (in_length << 1);

	if (output_buffer_p)
		{
			/* Parse the document. This shall call our callbacks provided via the
			 * md_renderer_t structure. */

			success_flag = md_html (in_s, in_length, AppendChunk, output_buffer_p, parser_flags, renderer_flags);

			if (success_flag == 0)
				{
					STRPTR html_s = DetachByteBufferData (output_buffer_p);

					/* Write down the document in the HTML format. */
					if (full_html_flag) 
						{
			 				const char * const header_s = 
								"<html>\n"
				  			"<head>\n"
				    		"<title></title>\n"
								"<meta name=\"generator\" content=\"md4c from AmiMarkdown\">\n"
				    		"</head>\n"
				    		"<body>\n";
				  
				  		const char * const footer_s = 
				  			"</body>\n"
				  			"</html>\n";
				  
				  		char *output_s = ConcatenateVarargsStrings (header_s, html_s, footer_s, NULL);

							/*
							printf ("header length %lu\n", header_length);
							printf ("footer length %lu\n", footer_length);
							printf ("output_buffer.mb_used_size %lu\n", output_buffer.mb_used_size);					
				  		*/
				  		
				  		if (output_s)
				  			{				
									*out_ss = output_s;
											  				
				  				success_flag = TRUE; 
				  			}      		
				  
				  		FreeCopiedString (html_s);
						} 	
					else 
						{
							*out_ss = html_s;
							success_flag = TRUE;
						}


				}		/* if (success_flag == 0) */		
			else
				{
					FreeByteBuffer (output_buffer_p);
				}

		} /* if (output_buffer_p) */


	return success_flag;	
}


static void AppendChunk (const MD_CHAR *text_s, MD_SIZE size, void *data_p)
{
	AppendToByteBuffer ((ByteBuffer *) data_p, text_s, size);
}
