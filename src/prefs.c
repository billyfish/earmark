/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: prefs.c
**
** Date: 06-01-2021 10:59:40
**
************************************************************

*/


#include "prefs.h"

#include <proto/exec.h>
//#include <proto/amijansson.h>


//static json_t *ConvertPrefsToJSON (const MDPrefs *prefs_p);



BOOL SaveMDPrefs (const MDPrefs *prefs_p, CONST STRPTR path_s)
{
	BOOL success_flag = FALSE;
	
	return success_flag;	
}


MDPrefs *LoadMDPrefs (CONST STRPTR path_s)
{
	MDPrefs *prefs_p = NULL;
	
	return prefs_p;	
}


MDPrefs *AllocateMDPrefs (void)
{
	MDPrefs *prefs_p = (MDPrefs *) IExec -> AllocVecTags (sizeof (MDPrefs), TAG_DONE);
	
	if (prefs_p)
		{
			prefs_p -> mdp_dialect = DI_COMMON_MARK;					
			prefs_p -> mdp_tables = FALSE;
			prefs_p -> mdp_task_lists = TRUE;
			prefs_p -> mdp_collapse_whitespace = FALSE;
			prefs_p -> mdp_strike_though_spans = FALSE;
			prefs_p -> mdp_underline_spans = FALSE;
			prefs_p -> mdp_latex_maths = FALSE;
			prefs_p -> mdp_html_blocks = FALSE;
			prefs_p -> mdp_html_spans = FALSE;
			prefs_p -> mdp_indented_code_blocks = FALSE;
			prefs_p -> mdp_translate_html_entities = FALSE;	
		}
	
	return prefs_p;
}


void FreeMDPrefs (MDPrefs *prefs_p)
{
	IExec -> FreeVec (prefs_p);	
}

/*
static json_t *ConvertPrefsToJSON (const MDPrefs *prefs_p)
{
	
}
*/