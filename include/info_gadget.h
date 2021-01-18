/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: info_gadget.h
**
** Date: 18-01-2021 09:49:04
**
************************************************************

*/



#ifndef INFO_GADGET_H
#define INFO_GADGET_H

#include <libraries/mui.h>


#include "keyfile_tagbase.h"


/*************************/
/******* ATTRIBUTES ******/
/*************************/

#define IG_Dummy (TAGBASE_BILLY | 0x00000700)

#define IGA_Editor (IG_Dummy + 1)

#define IGA_Columns (IG_Dummy + 2)

#define IGA_Rows (IG_Dummy + 3)

#define IGA_Changed (IG_Dummy + 4)

/*************************/
/******** METHODS ********/
/*************************/

#define IGM_Insert   (IG_Dummy + 1)

/*************************/
/***** API FUNCTIONS *****/
/*************************/


struct MUI_CustomClass *InitInfoGadgetClass (void);


void FreeInfoGadgetClass (struct MUI_CustomClass *mui_class_p);


#endif
