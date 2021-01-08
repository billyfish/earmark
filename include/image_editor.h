/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: image_editor.h
**
** Date: 08-01-2021 17:06:11
**
************************************************************

*/



#ifndef IMAGE_EDITOR_H
#define IMAGE_EDITOR_H

#include <libraries/mui.h>


#include "keyfile_tagbase.h"


/*************************/
/******* ATTRIBUTES ******/
/*************************/

#define IE_Dummy (TAGBASE_BILLY | 0x00000300)

#define IEA_Editor (IE_Dummy + 1)

#define IEA_Path (IE_Dummy + 2)

#define IEA_Alt (IE_Dummy + 3)

#define IEA_Title (IE_Dummy + 4)

/*************************/
/******** METHODS ********/
/*************************/

#define IEM_Insert   (IE_Dummy + 1)

/*************************/
/***** API FUNCTIONS *****/
/*************************/


struct MUI_CustomClass *InitImageEditorClass (void);


void FreeImageEditorClass (struct MUI_CustomClass *mui_class_p);


#endif


