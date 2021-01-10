/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: hyperlink_editor.h
**
** Date: 08-01-2021 17:06:11
**
************************************************************

*/



#ifndef HYPERLINK_EDITOR_H
#define HYPERLINK_EDITOR_H

#include <libraries/mui.h>


#include "keyfile_tagbase.h"


/*************************/
/******* ATTRIBUTES ******/
/*************************/

#define HE_Dummy (TAGBASE_BILLY | 0x00000500)

#define HEA_Editor (HE_Dummy + 1)

#define HEA_Text (HE_Dummy + 2)

#define HEA_Url (HE_Dummy + 3)


/*************************/
/******** METHODS ********/
/*************************/

#define HEM_Insert   (HE_Dummy + 1)

/*************************/
/***** API FUNCTIONS *****/
/*************************/


struct MUI_CustomClass *InitHyperlinkEditorClass (void);


void FreeHyperlinkEditorClass (struct MUI_CustomClass *mui_class_p);


#endif
