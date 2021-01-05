/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: editor_gadget.h
**
** Date: 04-01-2021 11:20:14
**
************************************************************

*/

#ifndef MD_EDITOR_GADGET_H
#define MD_EDITOR_GADGET_H

#include <libraries/mui.h>

#include "keyfile_tagbase.h"


/*************************/
/******* ATTRIBUTES ******/
/*************************/

#define ME_Dummy (TAGBASE_BILLY | 0x00000100)

#define MEA_Viewer (ME_Dummy + 1)

/*************************/
/******** METHODS ********/
/*************************/

#define MEM_MDEditor_Convert   (ME_Dummy + 1)


#define MEM_MDEditor_Load   (ME_Dummy + 2)


#define MEM_MDEditor_Save   (ME_Dummy + 3)

/*************************/
/***** API FUNCTIONS *****/
/*************************/

struct MUI_CustomClass *InitMarkdownEditorClass (void);


void FreeMarkdownEditorClass (struct MUI_CustomClass *mui_class_p);

#endif
