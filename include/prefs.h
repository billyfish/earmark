/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: prefs.h
**
** Date: 06-01-2021 10:59:24
**
************************************************************

*/

#ifndef MD_PREFS_H
#define MD_PREFS_H

#include <exec/types.h>


typedef enum
{
	DI_COMMON_MARK,
	DI_GITHUB,
	DI_NUM_DILAECTS
} Dialect;


typedef struct MDPrefs
{
	Dialect mdp_dialect;
	uint32 mdp_tables;
	uint32 mdp_task_lists;
	uint32 mdp_collapse_whitespace;
	uint32 mdp_strike_though_spans;
	uint32 mdp_underline_spans;
	uint32 mdp_latex_maths;
	uint32 mdp_html_blocks;
	uint32 mdp_html_spans;
	uint32 mdp_indented_code_blocks;
	uint32 mdp_translate_html_entities;	
	
} MDPrefs;


BOOL SaveMDPrefs (const MDPrefs *prefs_p, CONST STRPTR path_s);

MDPrefs *LoadMDPrefs (CONST STRPTR path_s);

MDPrefs *AllocateMDPrefs (void);

void FreeMDPrefs (MDPrefs *prefs_p);



#endif