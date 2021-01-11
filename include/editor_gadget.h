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

#define MEA_Prefs (ME_Dummy + 2)

#define MEA_Filename (ME_Dummy + 3)

#define MEA_SurroundSelection (ME_Dummy + 4)

#define MEA_InsertItem (ME_Dummy + 5)

#define MEA_PrefixMarkedLines (ME_Dummy + 6)

/*************************/
/******** METHODS ********/
/*************************/

#define MEM_MDEditor_Convert   (ME_Dummy + 1)


#define MEM_MDEditor_Load   (ME_Dummy + 2)


#define MEM_MDEditor_Save   (ME_Dummy + 3)


#define MEM_MDEditor_ReplaceText (ME_Dummy + 4)



/*************************/
/********* VALUES ********/
/*************************/


#define MEV_MDEditor_Style_Bold   (ME_Dummy + 1)

#define MEV_MDEditor_Style_Italic   (ME_Dummy + 2)

#define MEV_MDEditor_Style_Strikethrough   (ME_Dummy + 3)

#define MEV_MDEditor_Style_Code   (ME_Dummy + 4)

#define MEV_MDEditor_Style_IndentedCode   (ME_Dummy + 5)

#define MEV_MDEditor_HorizontalRule   (ME_Dummy + 6)

#define MEV_MDEditor_Table   (ME_Dummy + 7)

#define MEV_MDEditor_Hyperlink   (ME_Dummy + 8)

#define MEV_MDEditor_Image   (ME_Dummy + 9)

#define MEV_MDEditor_BlockQuote   (ME_Dummy + 10)

/*************************/
/***** API FUNCTIONS *****/
/*************************/

struct MUI_CustomClass *InitMarkdownEditorClass (void);


void FreeMarkdownEditorClass (struct MUI_CustomClass *mui_class_p);

#endif
