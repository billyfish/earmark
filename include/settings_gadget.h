/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: prefs_gadget.h
**
** Date: 06-01-2021 12:33:08
**
************************************************************

*/


#ifndef PREFS_GADGET_H
#define PREFS_GADGET_H

#include <libraries/mui.h>



#include "keyfile_tagbase.h"


/*************************/
/******* ATTRIBUTES ******/
/*************************/

#define MP_Dummy (TAGBASE_BILLY | 0x00000200)

#define MPA_EnableTables (MP_Dummy + 1)

#define MPA_EnableTaskLists (MP_Dummy + 2)

#define MPA_CollapseWhitespace (MP_Dummy + 3)

#define MPA_StrikeThroughSpans (MP_Dummy + 4)

#define MPA_UnderlineSpans (MP_Dummy + 5)

#define MPA_LatexMaths (MP_Dummy + 6)

#define MPA_HTMLBlocks (MP_Dummy + 7)

#define MPA_HTMLSpans (MP_Dummy + 8)

#define MPA_IndentedCodeBlocks (MP_Dummy + 9)

#define MPA_TranslateEntities (MP_Dummy + 10)

#define MPA_Dialect (MP_Dummy + 11)


/*************************/
/******** METHODS ********/
/*************************/

/*************************/
/***** API FUNCTIONS *****/
/*************************/


struct MUI_CustomClass *InitMarkdownSettingsClass (void);


void FreeMarkdownSettingsClass (struct MUI_CustomClass *mui_class_p);


#endif

