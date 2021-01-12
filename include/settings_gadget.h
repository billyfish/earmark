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

#define MS_Dummy (TAGBASE_BILLY | 0x00000200)

#define MSA_EnableTables (MS_Dummy + 1)

#define MSA_EnableTaskLists (MS_Dummy + 2)

#define MSA_CollapseWhitespace (MS_Dummy + 3)

#define MSA_StrikeThroughSpans (MS_Dummy + 4)

#define MSA_UnderlineSpans (MS_Dummy + 5)

#define MSA_LatexMaths (MS_Dummy + 6)

#define MSA_HTMLBlocks (MS_Dummy + 7)

#define MSA_HTMLSpans (MS_Dummy + 8)

#define MSA_IndentedCodeBlocks (MS_Dummy + 9)

#define MSA_TranslateEntities (MS_Dummy + 10)

#define MSA_Dialect (MS_Dummy + 11)


#define MSA_Prefs (MS_Dummy + 12)


/*************************/
/******** METHODS ********/
/*************************/

#define MSM_SaveSettings (MS_Dummy + 1)

#define MSM_LoadSettings (MS_Dummy + 2)

/*************************/
/***** API FUNCTIONS *****/
/*************************/


struct MUI_CustomClass *InitMarkdownSettingsClass (void);


void FreeMarkdownSettingsClass (struct MUI_CustomClass *mui_class_p);


#endif

