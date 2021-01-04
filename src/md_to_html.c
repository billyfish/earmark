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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "md4c-html.h"

#include <exec/types.h>

#include <proto/exec.h>


#include "memory_buffer.h"



static void AppendChunk (const MD_CHAR *text_s, MD_SIZE size, void *data_p);

/*
static void
usage(void)
{
    printf(
        "Usage: md2html [OPTION]... [FILE]\n"
        "Convert input FILE (or standard input) in Markdown format to HTML.\n"
        "\n"
        "General options:\n"
        "  -o  --output=FILE    Output file (default is standard output)\n"
        "  -f, --full-html      Generate full HTML document, including header\n"
        "  -x, --xhtml          Generate XHTML instead of HTML\n"
        "  -s, --stat           Measure time of input parsing\n"
        "  -h, --help           Display this help and exit\n"
        "  -v, --version        Display version and exit\n"
        "\n"
        "Markdown dialect options:\n"
        "(note these are equivalent to some combinations of the flags below)\n"
        "      --commonmark     CommonMark (this is default)\n"
        "      --github         Github Flavored Markdown\n"
        "\n"
        "Markdown extension options:\n"
        "      --fcollapse-whitespace\n"
        "                       Collapse non-trivial whitespace\n"
        "      --flatex-math    Enable LaTeX style mathematics spans\n"
        "      --fpermissive-atx-headers\n"
        "                       Allow ATX headers without delimiting space\n"
        "      --fpermissive-url-autolinks\n"
        "                       Allow URL autolinks without '<', '>'\n"
        "      --fpermissive-www-autolinks\n"
        "                       Allow WWW autolinks without any scheme (e.g. 'www.example.com')\n"
        "      --fpermissive-email-autolinks  \n"
        "                       Allow e-mail autolinks without '<', '>' and 'mailto:'\n"
        "      --fpermissive-autolinks\n"
        "                       Same as --fpermissive-url-autolinks --fpermissive-www-autolinks\n"
        "                       --fpermissive-email-autolinks\n"
        "      --fstrikethrough Enable strike-through spans\n"
        "      --ftables        Enable tables\n"
        "      --ftasklists     Enable task lists\n"
        "      --funderline     Enable underline spans\n"
        "      --fwiki-links    Enable wiki links\n"
        "\n"
        "Markdown suppression options:\n"
        "      --fno-html-blocks\n"
        "                       Disable raw HTML blocks\n"
        "      --fno-html-spans\n"
        "                       Disable raw HTML spans\n"
        "      --fno-html       Same as --fno-html-blocks --fno-html-spans\n"
        "      --fno-indented-code\n"
        "                       Disable indented code blocks\n"
        "\n"
        "HTML generator options:\n"
        "      --fverbatim-entities\n"
        "                       Do not translate entities\n"
        "\n"
    );
}
*/

BOOL ConvertText (const char *in_s, char **out_ss, uint16 parser_flags, uint16 renderer_flags, BOOL full_html_flag)
{
	MemBuffer output_buffer;
	BOOL success_flag = FALSE;
	const size_t in_length = strlen (in_s);
	
	
  /* Input size is good estimation of output size. Add some more reserve to
   * deal with the HTML header/footer and tags. */
  InitMemBuffer (&output_buffer, in_length + (in_length >> 3) + 64);

  /* Parse the document. This shall call our callbacks provided via the
   * md_renderer_t structure. */

  success_flag = md_html (in_s, in_length, AppendChunk, (void *) &output_buffer, parser_flags, renderer_flags);

	if (success_flag == 0)
		{
    	/* Write down the document in the HTML format. */
    	if (full_html_flag) 
    		{
   				const char *header_s = 
    				"<html>\n"
      			"<head>\n"
        		"<title></title>\n"
        		"<meta name=\"generator\" content=\"amimd\">\n"
        		"</head>\n"
        		"<body>\n";
      
      		const char *footer_s = 
      			"</body>\n"
      			"</html>\n";
      
      		const size_t header_length = strlen (header_s);
      		const size_t footer_length = strlen (footer_s);	      
      		char *output_s = (char *) IExec -> AllocVecTags (output_buffer.mb_used_size + header_length + footer_length + 1, TAG_DONE);


					printf ("header length %lu\n", header_length);
					printf ("footer length %lu\n", footer_length);
					printf ("output_buffer.mb_used_size %lu\n", output_buffer.mb_used_size);					
      
      		if (output_s)
      			{
      				char *c_p = output_s;
      				uint32 i;
      				uint32 size = output_buffer.mb_used_size + header_length + footer_length;
      				
      				IExec -> CopyMem (header_s, c_p, header_length); 
      				c_p += header_length;
      				
      				IExec -> CopyMem (output_buffer.mb_data_p, c_p, output_buffer.mb_used_size);
      				c_p += output_buffer.mb_used_size;
      				
      				IExec -> CopyMem (footer_s, c_p, footer_length);
      				* (c_p + footer_length) = '\0';
      				
      				for (i = 0; i <= size; ++ i)
      					{
      						printf ("res [%lu] = \'%c\'\n", i, * (output_s + i));
      					}
							
							*out_ss = output_s;
							      				
      				success_flag = TRUE; 
      			}      		
      
      		ClearMemBuffer (&output_buffer);
    		} 	
    	else 
    		{
    			if (MakeBufferValidString (&output_buffer))
    				{
		    			*out_ss = output_buffer.mb_data_p;
    					success_flag = TRUE;
    				}
				}
		}
	else
		{
			printf ("Failed to convert md_html");
  	}

	return success_flag;	
}


static void AppendChunk (const MD_CHAR *text_s, MD_SIZE size, void *data_p)
{
	AppendToMemBuffer ((MemBuffer *) data_p, text_s, size);
}
