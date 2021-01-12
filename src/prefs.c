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


#include <string.h>

#include "prefs.h"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/amijansson.h>


#include "json_util.h"

#include "gui.h"
#include "debugging_utils.h"

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


static CONST CONST_STRPTR S_PREFS_S = "prefs";


static json_t *ConvertPrefsToJSON (const MDPrefs *prefs_p);
static BOOL SetPrefsFromJSON (MDPrefs *prefs_p, const json_t *prefs_json_p);



BOOL SaveMDPrefs (const MDPrefs *prefs_p, CONST STRPTR path_s)
{
	BOOL success_flag = FALSE;

	json_t *settings_json_p = IJansson -> json_object ();
	
	if (settings_json_p)
		{
			json_t *prefs_json_p = ConvertPrefsToJSON (prefs_p);
			
			if (prefs_json_p)
				{
					if (IJansson -> json_object_set_new (settings_json_p, S_PREFS_S, prefs_json_p) == 0)
						{
							STRPTR settings_s = IJansson -> json_dumps (settings_json_p, JSON_INDENT (2));
							
							if (settings_s)
								{
									success_flag = SaveFile (path_s, settings_s);
								
									free (settings_s);	
								}
							else
								{
									
								}
							
							
						}
					else
						{
							IJansson -> json_decref (prefs_json_p);	
						}
					
				}		/* if (prefs_json_p) */
			
			IJansson -> json_decref (settings_json_p);
		}		/* if (settings_json_p) */

	
	return success_flag;	
}


BOOL LoadMDPrefs (MDPrefs *prefs_p, CONST STRPTR path_s)
{
	BOOL success_flag = FALSE;
	json_error_t err;
	
	json_t *settings_json_p = IJansson -> json_load_file (path_s, 0, &err);
	
	if (settings_json_p)
		{
			json_t *prefs_json_p = IJansson -> json_object_get (settings_json_p, S_PREFS_S);
			
			DB (KPRINTF ("%s %ld - LoadMDPrefs: filename \"%s\" loaded\n", __FILE__, __LINE__, path_s));		
			
			if (prefs_json_p)
				{	
					DB (KPRINTF ("%s %ld - LoadMDPrefs: filename \"%s\" prefs found\n", __FILE__, __LINE__, path_s));		
			
					success_flag = SetPrefsFromJSON (prefs_p, prefs_json_p);
				}

			if (!success_flag)
				{
					IDOS -> Printf ("Failed to parse settings from \"%s\"\n", path_s);		
				}
								
			IJansson -> json_decref (settings_json_p);
		}
	else
		{
			IDOS -> Printf ("Failed to load settings from \"%s\"\n", path_s);	
		}
	
	return success_flag;	
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


void PrintPrefs (const MDPrefs *const prefs_p)
{
	IDOS -> Printf ("%s: %lu\n", S_DIALECT_S, prefs_p -> mdp_dialect);
	IDOS -> Printf ("%s: %lu\n", S_TABLES_S, prefs_p -> mdp_tables);
	IDOS -> Printf ("%s: %lu\n", S_TASK_LISTS_S, prefs_p -> mdp_task_lists);
	IDOS -> Printf ("%s: %lu\n", S_COLLAPSE_WHITESPACE_S, prefs_p -> mdp_collapse_whitespace);
	IDOS -> Printf ("%s: %lu\n", S_STRIKETHROUGH_SPANS_S, prefs_p -> mdp_strike_though_spans);
	IDOS -> Printf ("%s: %lu\n", S_UNDERLINE_SPANS_S, prefs_p -> mdp_underline_spans);
	IDOS -> Printf ("%s: %lu\n", S_LATEX_MATHS_S, prefs_p -> mdp_latex_maths);
	IDOS -> Printf ("%s: %lu\n", S_HTML_BLOCKS_S, prefs_p -> mdp_html_blocks);
	IDOS -> Printf ("%s: %lu\n", S_HTML_SPANS_S, prefs_p -> mdp_html_spans);
	IDOS -> Printf ("%s: %lu\n", S_INDENTED_CODE_BLOCK_S, prefs_p -> mdp_indented_code_blocks);
	IDOS -> Printf ("%s: %lu\n", S_TRANSLATE_HTML_ENTITIES_S, prefs_p -> mdp_translate_html_entities);		
}


static BOOL SetPrefsFromJSON (MDPrefs *prefs_p, const json_t *prefs_json_p)
{
	const char *value_s = GetJSONString (prefs_json_p, S_DIALECT_S);
	BOOL b;
	
	if (value_s)
		{
			if (strcmp (value_s, S_DIALECT_COMMON_MARK_S) == 0)
				{
					prefs_p -> mdp_dialect = DI_COMMON_MARK;					
				}
			else if (strcmp (value_s, S_DIALECT_GITHUB_S) == 0)
				{
					prefs_p -> mdp_dialect = DI_GITHUB;
				}
			
		}		/* if (value_s) */

	
	if (GetJSONBoolean (prefs_json_p, S_TABLES_S, &b))
		{
			prefs_p -> mdp_tables = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_TASK_LISTS_S, &b))
		{
			prefs_p -> mdp_task_lists = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_COLLAPSE_WHITESPACE_S, &b))
		{
			prefs_p -> mdp_collapse_whitespace = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_STRIKETHROUGH_SPANS_S, &b))
		{
			prefs_p -> mdp_strike_though_spans = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_UNDERLINE_SPANS_S, &b))
		{
			prefs_p -> mdp_underline_spans = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_LATEX_MATHS_S, &b))
		{
			prefs_p -> mdp_latex_maths = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_HTML_BLOCKS_S, &b))
		{
			prefs_p -> mdp_html_blocks = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_HTML_SPANS_S, &b))
		{
			prefs_p -> mdp_html_spans = b ? TRUE : FALSE;
		}
	
	if (GetJSONBoolean (prefs_json_p, S_INDENTED_CODE_BLOCK_S, &b))
		{
			prefs_p -> mdp_indented_code_blocks = b ? TRUE : FALSE;
		}

	if (GetJSONBoolean (prefs_json_p, S_TRANSLATE_HTML_ENTITIES_S, &b))
		{
			prefs_p -> mdp_translate_html_entities = b ? TRUE : FALSE;
		}
				
	return TRUE;
}
