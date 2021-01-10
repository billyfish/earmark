/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: table_editor.h
**
** Date: 08-01-2021 17:06:11
**
************************************************************

*/



#ifndef TABLE_EDITOR_H
#define TABLE_EDITOR_H

#include <libraries/mui.h>


#include "keyfile_tagbase.h"


/*************************/
/******* ATTRIBUTES ******/
/*************************/

#define TE_Dummy (TAGBASE_BILLY | 0x00000400)

#define TEA_Editor (TE_Dummy + 1)

#define TEA_Columns (TE_Dummy + 2)

#define TEA_Rows (TE_Dummy + 3)

#define TEA_Alignments (TE_Dummy + 4)

/*************************/
/******** METHODS ********/
/*************************/

#define TEM_Insert   (TE_Dummy + 1)

/*************************/
/***** API FUNCTIONS *****/
/*************************/


struct MUI_CustomClass *InitTableEditorClass (void);


void FreeTableEditorClass (struct MUI_CustomClass *mui_class_p);


#endif
