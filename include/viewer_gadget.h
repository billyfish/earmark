/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: viewer_gadget.h
**
** Date: 04-01-2021 11:50:48
**
************************************************************

*/


#ifndef MD_VIEWER_GADGET_H
#define MD_VIEWER_GADGET_H

#include <libraries/mui.h>


struct MUI_CustomClass *InitMarkdownViewerClass (void);


void FreeMarkdownViewerClass (struct MUI_CustomClass *mui_class_p);


#endif
