/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: md2html.h
**
** Date: 04-01-2021 16:44:14
**
************************************************************

*/


#ifndef MD_TO_HTML_H
#define MD_TO_HTML_H

#include <exec/types.h>

BOOL ConvertText (const char *in_s, char **out_ss, uint16 parser_flags, uint16 renderer_flags, BOOL full_html_flag);

#endif
