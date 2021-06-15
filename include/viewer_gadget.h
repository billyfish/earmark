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

#include <mui/HTMLview_mcc.h>


struct MUI_CustomClass *InitMarkdownViewerClass (void);


void FreeMarkdownViewerClass (struct MUI_CustomClass *mui_class_p);

uint32 LoadHtmlImage (struct Hook *hook_p, Object *obj_p, struct HTMLview_LoadMsg *lmsg);
 
#endif
