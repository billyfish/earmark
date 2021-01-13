/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: search_gadget.h
**
** Date: 13-01-2021 14:58:27
**
************************************************************

*/


#ifndef MD_SEARCH_GADGET_H
#define MD_SEARCH_GADGET_H

#include <libraries/mui.h>


#include "keyfile_tagbase.h"

/*************************/
/******* ATTRIBUTES ******/
/*************************/

#define SG_Dummy (TAGBASE_BILLY | 0x00000600)

#define SGA_Text (SG_Dummy + 1)

#define SGA_FromPosition (SG_Dummy + 2)

#define SGA_CaseSensitive (SG_Dummy + 3)

#define SGA_TextEditor (SG_Dummy + 4)

#define SGA_SearchBackwards (SG_Dummy + 5)


/*************************/
/******** METHODS ********/
/*************************/

#define SGM_Search (SG_Dummy + 1)


/*************************/
/***** API FUNCTIONS *****/
/*************************/

struct MUI_CustomClass *InitSearchGadgetClass (void);


void FreeSearchGadgetClass (struct MUI_CustomClass *mui_class_p);


#endif
