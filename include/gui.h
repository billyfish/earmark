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

#include "prefs.h"

BOOL CreateMUIInterface (MDPrefs *prefs_p);

BOOL LoadFile (STRPTR filename_s);

BOOL SaveFile (STRPTR filename_s, CONST CONST_STRPTR text_s);

STRPTR RequestFilename (const BOOL save_flag, CONST CONST_STRPTR title_s, CONST CONST_STRPTR file_pattern_s);

void UpdateWindowActiveFilename (CONST CONST_STRPTR filename_s);

CONST CONST_STRPTR GetMarkdownFilePattern (void);

int32 ShowRequester (CONST CONST_STRPTR title_s, CONST CONST_STRPTR text_s, CONST CONST_STRPTR buttons_s);

#endif
