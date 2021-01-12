/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: prefs_gadget.c
**
** Date: 06-01-2021 12:53:26
**
************************************************************

*/

#include <stdio.h>

#include <clib/alib_protos.h>

#include <exec/memory.h>
#include <exec/types.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <libraries/mui.h>

#include "debugging_utils.h"

#include "settings_gadget.h"
#include "prefs.h"


typedef struct MarkdownSettingsData
{
	MDPrefs *msd_prefs_p;
} MarkdownSettingsData;


static const char *S_MUIC_MarkdownSettings  = "MarkdownSettings";


/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/


static uint32 MarkdownSettingsDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownSettings_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownSettings_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownSettings_Get (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownSettings_Dispose (Class *class_p, Object *object_p, Msg msg_p);


static Object *GetSettingsObject (MDPrefs *prefs_p, Object *parent_p);

/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitMarkdownSettingsClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster -> MUI_CreateCustomClass (NULL, MUIC_Group, NULL, sizeof (MarkdownSettingsData), MarkdownSettingsDispatcher);
}


void FreeMarkdownSettingsClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster -> MUI_DeleteCustomClass (mui_class_p);
}


/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 MarkdownSettingsDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - MarkdownSettings Dispatcher: New\n", __FILE__, __LINE__));
				res = MarkdownSettings_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - MarkdownSettings Dispatcher: Set\n", __FILE__, __LINE__));
				res = MarkdownSettings_Set (class_p, object_p, msg_p);
				break;

			case OM_DISPOSE:
				DB (KPRINTF ("%s %ld - MarkdownSettings Dispatcher: Dispose\n", __FILE__, __LINE__));
				res = MarkdownSettings_Dispose (class_p, object_p, msg_p);
				break;
				
			case OM_GET:
				DB (KPRINTF ("%s %ld - MarkdownSettings Dispatcher: Get\n", __FILE__, __LINE__));
				res = MarkdownSettings_Get (class_p, object_p, msg_p);				
				break;

			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}


static Object *GetSettingsObject (MDPrefs *prefs_p, Object *parent_p)
{
	static const char *dialects_ss []   = { "CommonMark","Github", NULL };
		
	Object *dialect_p = NULL;
	Object *tables_cb_p = NULL;
	Object *task_lists_cb_p = NULL;
	Object *collapse_whitespace_cb_p = NULL;
	Object *strike_through_cb_p = NULL;	
	Object *underline_span_cb_p = NULL;
	Object *latex_math_span_cb_p = NULL;
	Object *raw_html_blocks_cb_p = NULL;	
	Object *raw_html_spans_cb_p = NULL;
	Object *indented_code_blocks_cb_p = NULL;	
	Object *translate_entities_cb_p = NULL;				

	
	Object *child_object_p = IMUIMaster -> MUI_NewObject (MUIC_Group,
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_VertCenter, MUIV_Group_VertCenter_Top,
			
		MUIA_Group_Child, dialect_p = IMUIMaster -> MUI_NewObject (MUIC_Radio,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, "Markdown Dialect",
			MUIA_Radio_Entries, dialects_ss,
		TAG_DONE),

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, "Markdown Extensions",
			MUIA_Group_Columns, 2,
						
			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Tables", TAG_DONE),
			MUIA_Group_Child, tables_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),
			
			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Task lists", TAG_DONE),
			MUIA_Group_Child, task_lists_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),
			
			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Collapse whitespace", TAG_DONE),
			MUIA_Group_Child, collapse_whitespace_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Strike-through spans", TAG_DONE),
			MUIA_Group_Child, strike_through_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Underline spans", TAG_DONE),														
			MUIA_Group_Child, underline_span_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "LaTeX maths", TAG_DONE),
			MUIA_Group_Child, latex_math_span_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),														
			
		TAG_DONE),

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, "HTML Options",
			MUIA_Group_Columns, 2,
						
			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Allow blocks", TAG_DONE),
			MUIA_Group_Child, raw_html_blocks_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),
			
			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Allow spans", TAG_DONE),
			MUIA_Group_Child, raw_html_spans_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),
			
			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Allow code blocks", TAG_DONE),
			MUIA_Group_Child, indented_code_blocks_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),											

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Translate Entities", TAG_DONE),
			MUIA_Group_Child, translate_entities_cb_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark, TAG_DONE),			
			
		TAG_DONE),
		
	TAG_DONE);		/* end settings group */
	
	if (child_object_p)
		{
			DB (KPRINTF ("%s %ld - MarkdownSettings_New: Adding child obj\n", __FILE__, __LINE__));

			IIntuition -> IDoMethod (parent_p, OM_ADDMEMBER, child_object_p);							

			IIntuition -> SetAttrs (dialect_p, MUIA_ShortHelp, "Choose which Markdown dialect to use", TAG_DONE);
			IIntuition -> IDoMethod (dialect_p, MUIM_Notify, MUIA_Radio_Active, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_Dialect, MUIV_TriggerValue);

			IIntuition -> SetAttrs (tables_cb_p, MUIA_ShortHelp, "Enable support for tables", TAG_DONE);
			IIntuition -> SetAttrs (tables_cb_p, MUIA_Selected, prefs_p -> mdp_tables, TAG_DONE); 
			IIntuition -> IDoMethod (tables_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_EnableTables, MUIV_TriggerValue);
				
			IIntuition -> SetAttrs (task_lists_cb_p, MUIA_ShortHelp, "Enable support for task lists", TAG_DONE);			
			IIntuition -> SetAttrs (task_lists_cb_p, MUIA_Selected, prefs_p -> mdp_task_lists, TAG_DONE); 
			IIntuition -> IDoMethod (task_lists_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_EnableTaskLists, MUIV_TriggerValue);
												
			IIntuition -> SetAttrs (collapse_whitespace_cb_p, MUIA_ShortHelp, "Collapse non-trivial whitespace", TAG_DONE);			
			IIntuition -> SetAttrs (collapse_whitespace_cb_p, MUIA_Selected, prefs_p -> mdp_collapse_whitespace, TAG_DONE); 
			IIntuition -> IDoMethod (collapse_whitespace_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_CollapseWhitespace, MUIV_TriggerValue);
															
			IIntuition -> SetAttrs (strike_through_cb_p, MUIA_ShortHelp, "Enable strike-through span elements", TAG_DONE);			
			IIntuition -> SetAttrs (strike_through_cb_p, MUIA_Selected, prefs_p -> mdp_strike_though_spans, TAG_DONE); 
			IIntuition -> IDoMethod (strike_through_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_StrikeThroughSpans, MUIV_TriggerValue);
									
			IIntuition -> SetAttrs (underline_span_cb_p, MUIA_ShortHelp, "Enable underline span elements", TAG_DONE);			
			IIntuition -> SetAttrs (underline_span_cb_p, MUIA_Selected, prefs_p -> mdp_underline_spans, TAG_DONE); 
			IIntuition -> IDoMethod (underline_span_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_UnderlineSpans, MUIV_TriggerValue);
									
			IIntuition -> SetAttrs (latex_math_span_cb_p, MUIA_ShortHelp, "Enable LaTeX-style mathematics span elements", TAG_DONE);			
			IIntuition -> SetAttrs (latex_math_span_cb_p, MUIA_Selected, prefs_p -> mdp_latex_maths, TAG_DONE); 
			IIntuition -> IDoMethod (latex_math_span_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_LatexMaths, MUIV_TriggerValue);
			
			IIntuition -> SetAttrs (raw_html_blocks_cb_p, MUIA_ShortHelp, "Allow HTML block elements in the markdown source", TAG_DONE);
			IIntuition -> SetAttrs (raw_html_blocks_cb_p, MUIA_Selected, prefs_p -> mdp_html_blocks, TAG_DONE); 
			IIntuition -> IDoMethod (raw_html_blocks_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_HTMLBlocks, MUIV_TriggerValue);
						
			IIntuition -> SetAttrs (raw_html_spans_cb_p, MUIA_ShortHelp, "Allow HTML span elements in the markdown source", TAG_DONE);			
			IIntuition -> SetAttrs (raw_html_spans_cb_p, MUIA_Selected, prefs_p -> mdp_html_spans, TAG_DONE); 
			IIntuition -> IDoMethod (raw_html_spans_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_HTMLSpans, MUIV_TriggerValue);
						
			IIntuition -> SetAttrs (indented_code_blocks_cb_p, MUIA_ShortHelp, "Allow indented code blocks in the markdown source", TAG_DONE);			
			IIntuition -> SetAttrs (indented_code_blocks_cb_p, MUIA_Selected, prefs_p -> mdp_indented_code_blocks, TAG_DONE); 
			IIntuition -> IDoMethod (indented_code_blocks_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_IndentedCodeBlocks, MUIV_TriggerValue);
						
			IIntuition -> SetAttrs (translate_entities_cb_p, MUIA_ShortHelp, "Translate HTML entities", TAG_DONE);	
			IIntuition -> SetAttrs (translate_entities_cb_p, MUIA_Selected, prefs_p -> mdp_translate_html_entities, TAG_DONE); 			
			IIntuition -> IDoMethod (translate_entities_cb_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, MSA_TranslateEntities, MUIV_TriggerValue);			
		}					
					
					
	return child_object_p;
}
	

static uint32 MarkdownSettings_New (Class *class_p, Object *object_p, Msg msg_p)
{
	MDPrefs *prefs_p = AllocateMDPrefs ();			

	if (prefs_p)
		{			
			Object *parent_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);

			DB (KPRINTF ("%s %ld - MarkdownSettings_New: Allocated prefs at %lu\n", __FILE__, __LINE__, prefs_p));
	
			if (parent_p)
				{
					MarkdownSettingsData *data_p = INST_DATA (class_p, parent_p);
					Object *child_p;
					
					data_p -> msd_prefs_p = prefs_p;
					
					child_p = GetSettingsObject (prefs_p, parent_p);
					
					if (child_p)
						{							
							return (uint32) parent_p;
						}	
						
						
					IIntuition->ICoerceMethod (class_p, parent_p, OM_DISPOSE);
				}		/* if (parent_p) */
			else
				{
					FreeMDPrefs (prefs_p);
				}
				
		}		/* if (prefs_p) */

	return 0;
}

static uint32 MarkdownSettings_Dispose (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	MarkdownSettingsData *md_p = INST_DATA (class_p, object_p);

	FreeMDPrefs (md_p -> msd_prefs_p);
}


static uint32 MarkdownSettings_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	MarkdownSettingsData *md_p = INST_DATA (class_p, object_p);
	MDPrefs *settings_p = md_p -> msd_prefs_p;
	struct opSet *ops_p = (struct opSet *) msg_p;
	struct TagItem *tags_p = ops_p -> ops_AttrList;
	struct TagItem *state_p = tags_p;
	struct TagItem *tag_p;

	DB (KPRINTF ("%s %ld - MarkdownSettings_Set enter\n", __FILE__, __LINE__));

	/* Step through the tag list while there are still items in the list */
 	while ((tag_p = IUtility -> NextTagItem (&state_p)) != NULL)
		{
			/* Cache the data for the current element */
			uint32 tag_data = tag_p -> ti_Data;

			DB (KPRINTF ("%s %ld - MarkdownSettings_Set ti_Tag: %8X ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
				 
					case MSA_Dialect:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_dialect to %lu", __FILE__, __LINE__, tag_data));							
						break;
					 
					case MSA_EnableTables:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_tables to %lu", __FILE__, __LINE__, tag_data));
						settings_p -> mdp_tables = tag_data;
						break;

					case MSA_EnableTaskLists:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_task_listss to %lu", __FILE__, __LINE__, tag_data));
						settings_p -> mdp_task_lists = tag_data;
						break;

					case MSA_CollapseWhitespace:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_collapse_whitespace to %lu", __FILE__, __LINE__, tag_data));						
						settings_p -> mdp_collapse_whitespace = tag_data;
						break;

					case MSA_StrikeThroughSpans:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_strike_though_spans to %lu", __FILE__, __LINE__, tag_data));
						settings_p -> mdp_strike_though_spans = tag_data;
						break;

					case MSA_UnderlineSpans:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_strike_though_spans to %lu", __FILE__, __LINE__, tag_data));						
						settings_p -> mdp_underline_spans = tag_data;
						break;

					case MSA_LatexMaths:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_underline_spans to %lu", __FILE__, __LINE__, tag_data));						
						settings_p -> mdp_latex_maths = tag_data;
						break;
						
					case MSA_HTMLBlocks:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_html_blocks to %lu", __FILE__, __LINE__, tag_data));						
						settings_p -> mdp_html_blocks = tag_data;
						break;

					case MSA_HTMLSpans:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_html_spans to %lu", __FILE__, __LINE__, tag_data));						
						settings_p -> mdp_html_spans = tag_data;
						break;
						
					case MSA_IndentedCodeBlocks:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_indented_code_blocks to %lu", __FILE__, __LINE__, tag_data));						
						settings_p -> mdp_indented_code_blocks = tag_data;
						break;

					case MSA_TranslateEntities:
						DB (KPRINTF ("%s %ld - MarkdownSettings_Set -> mdp_translate_html_entities to %lu", __FILE__, __LINE__, tag_data));						
						settings_p -> mdp_translate_html_entities = tag_data;
						break;
			
			
					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}



static uint32 MarkdownSettings_Get (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval;
	MarkdownSettingsData *md_p = INST_DATA (class_p, object_p);
	MDPrefs *settings_p = md_p -> msd_prefs_p;
	struct opGet *op_p = (struct opGet *) msg_p;
	uint32 *store_p = op_p -> opg_Storage;

	DB (KPRINTF ("%s %ld - MarkdownSettings_Get enter\n", __FILE__, __LINE__));

	switch (op_p -> opg_AttrID)
		{
			case MSA_Prefs:
				DB (KPRINTF ("%s %ld - MarkdownSettings_Get -> MSA_Prefs to %lu", __FILE__, __LINE__, settings_p));
				retval = *store_p = (uint32) settings_p;							
				break;	
	
			default:
				retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return retval;
}


