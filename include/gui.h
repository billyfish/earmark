/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: gui.h
**
** Date: 04-01-2021 13:17:18
**
************************************************************

*/


#ifndef GUI_H
#define GUI_H

#include <exec/types.h>

#include <intuition/classes.h>

#include "prefs.h"

#ifdef ALLOCATE_GLOBALS
	#define PREFIX
#else
	#define PREFIX extern
#endif


PREFIX Class *BitMapClass;

BOOL CreateMUIInterface (MDPrefs *prefs_p, CONST CONST_STRPTR markdown_file_s);

BOOL LoadFile (CONST CONST_STRPTR filename_s);

BOOL SaveFile (CONST CONST_STRPTR filename_s, CONST CONST_STRPTR text_s);

STRPTR RequestFilename (const BOOL save_flag, CONST CONST_STRPTR title_s, CONST CONST_STRPTR file_pattern_s, CONST CONST_STRPTR initial_file_s);

void UpdateWindowActiveFilename (CONST CONST_STRPTR filename_s);

CONST CONST_STRPTR GetMarkdownFilePattern (void);

int32 ShowWarning (CONST CONST_STRPTR title_s, CONST CONST_STRPTR text_s, CONST CONST_STRPTR buttons_s);

int32 ShowError (CONST CONST_STRPTR title_s, CONST CONST_STRPTR text_s, CONST CONST_STRPTR buttons_s);

int32 ShowInformation (CONST CONST_STRPTR title_s, CONST CONST_STRPTR text_s, CONST CONST_STRPTR buttons_s);

#endif
