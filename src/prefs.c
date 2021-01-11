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
#include <proto/amijansson.h>


#include "json_util.h"


static CONST CONST_STRPTR S_DIALECT_S = "dialect";
static CONST CONST_STRPTR S_DIALECT_COMMON_MARK_S = "common_mark";
static CONST CONST_STRPTR S_DIALECT_GITHUB_S = "github";

static CONST CONST_STRPTR S_TABLES_S = "tables";
static CONST CONST_STRPTR S_TASK_LISTS_S = "task_lists";
static CONST CONST_STRPTR S_COLLAPSE_WHITESPACE_S = "collapse_whitespace";
static CONST CONST_STRPTR S_STRIKETHROUGH_SPANS_S = "strikethrough_spans";
static CONST CONST_STRPTR S_UNDERLINE_SPANS_S = "underline_spans";
static CONST CONST_STRPTR S_LATEX_MATHS_S = "latex_maths";
static CONST CONST_STRPTR S_HTML_BLOCKS_S = "html_blocks";
static CONST CONST_STRPTR S_HTML_SPANS_S = "html_spans";
static CONST CONST_STRPTR S_INDENTED_CODE_BLOCK_S = "indented_code_blocks";
static CONST CONST_STRPTR S_TRANSLATE_HTML_ENTITIES_S = "translate_html_entities";


static json_t *ConvertPrefsToJSON (const MDPrefs *prefs_p);



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


static json_t *ConvertPrefsToJSON (const MDPrefs *prefs_p)
{
	json_t *prefs_json_p = IJansson -> json_object ();

	if (prefs_json_p)
		{
			if (SetJSONBoolean  (prefs_json_p, S_TABLES_S, prefs_p -> mdp_tables))
				{
					if (SetJSONBoolean  (prefs_json_p, S_TASK_LISTS_S, prefs_p -> mdp_task_lists))
						{
							if (SetJSONBoolean  (prefs_json_p, S_COLLAPSE_WHITESPACE_S, prefs_p -> mdp_collapse_whitespace))
								{
									if (SetJSONBoolean  (prefs_json_p, S_STRIKETHROUGH_SPANS_S, prefs_p -> mdp_strike_though_spans))
										{
											if (SetJSONBoolean  (prefs_json_p, S_UNDERLINE_SPANS_S, prefs_p -> mdp_underline_spans))
												{
													if (SetJSONBoolean  (prefs_json_p, S_LATEX_MATHS_S, prefs_p -> mdp_latex_maths))
														{
															if (SetJSONBoolean  (prefs_json_p, S_HTML_BLOCKS_S, prefs_p -> mdp_html_blocks))
																{
																	if (SetJSONBoolean  (prefs_json_p, S_HTML_SPANS_S, prefs_p -> mdp_html_spans))
																		{
																			if (SetJSONBoolean  (prefs_json_p, S_INDENTED_CODE_BLOCK_S, prefs_p -> mdp_indented_code_blocks))
																				{
																					if (SetJSONBoolean  (prefs_json_p, S_TRANSLATE_HTML_ENTITIES_S, prefs_p -> mdp_translate_html_entities))
																						{
																							CONST_STRPTR dialect_s = S_DIALECT_COMMON_MARK_S;
																							
																							if (prefs_p -> mdp_dialect == DI_GITHUB)
																								{
																									dialect_s = S_DIALECT_GITHUB_S;	
																								}
																							
																							if (SetJSONString (prefs_json_p, S_DIALECT_S, dialect_s))
																								{																					
																									return prefs_json_p;
																								}
																						}	
																				}	
																		}						
																}	
														}	
												}	
										}	
								}	
						}					
				}
			
			
			
			IJansson -> json_decref (prefs_json_p);			
		}		/* if (prefs_json_p) */	

	return NULL;
}

