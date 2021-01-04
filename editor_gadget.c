/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: editor_gadget.c
**
** Date: 03-01-2021 13:50:47
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

#include <mui/TextEditor_mcc.h>
#include <mui/HTMLview_mcc.h>

#include "debugging_utils.h"
//#include "memwatch.h"
#include "editor_gadget.h"

#include "md2html.h"

#include "md4c-html.h"

#include "gui.h"

typedef struct MarkdownEditorData
{
	Object *med_viewer_p;
	STRPTR med_filename_s;
} MarkdownEditorData;



/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/


static uint32 MarkdownEditorDispatcher (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownEditor_New (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownEditor_Set (Class *class_p, Object *object_p, Msg msg_p);


static void SetFile (MarkdownEditorData *md_p, STRPTR filename_s);

static uint32 ConvertMD (Class *class_p, Object *editor_p);

static uint32 LoadMD (Class *class_p, Object *editor_p);



static uint32 SaveMD (Class *class_p, Object *editor_p);

/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitMarkdownEditorClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster->MUI_CreateCustomClass (NULL, MUIC_TextEditor, NULL, sizeof (MarkdownEditorData), MarkdownEditorDispatcher);
}


void FreeMarkdownEditorClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster->MUI_DeleteCustomClass (mui_class_p);
}


/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 MarkdownEditorDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: New\n", __FILE__, __LINE__));
				res = MarkdownEditor_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Set\n", __FILE__, __LINE__));
				res = MarkdownEditor_Set (class_p, object_p, msg_p);
				break;

			case MEM_MDEditor_Convert:
				DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Convert\n", __FILE__, __LINE__));
				res = ConvertMD (class_p, object_p);
				break;

			case MEM_MDEditor_Load:
				DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: LoadMD\n", __FILE__, __LINE__));
				res = LoadMD (class_p, object_p);
				break;

			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}
           

static uint32 MarkdownEditor_New (Class *class_p, Object *object_p, Msg msg_p)
{


//	static struct Hook atom_select_hook  = { {NULL, NULL}, AtomSelectionChange , NULL, NULL };

	APTR mol_name_p = NULL;
	APTR num_atoms_p = NULL;
	APTR num_bonds_p = NULL;
	APTR atoms_listview_p = NULL;
	APTR bonds_listview_p = NULL;
	APTR text_p = NULL;
	APTR atoms_list_p = NULL;
	APTR bonds_list_p = NULL;

	Object *md_editor_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);


	if (md_editor_p)
		{
			MarkdownEditorData *md_p = INST_DATA (class_p, md_editor_p);

			md_p -> med_filename_s = NULL;
			md_p -> med_viewer_p = NULL;
			
			/*
			** Call when an atom is selected in the info - atoms list.
			*/
			//IIntuition->IDoMethod (atoms_list_p, MUIM_Notify, MUIA_NList_SelectChange, MUIV_EveryTime,
		//	mol_info_obj_p, 3, MUIM_CallHook, &atom_select_hook, MUIV_TriggerValue);


/*
			DoMethod (bonds_list_p, MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
				new_obj_p, 3, MUIM_Set, MIGA_Bonds_Selected, MUIV_TriggerValue);
*/
/*
			DoMethod (bonds_list_p, MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
				new_obj_p, 1, MIGM_MarkdownEditor_BondSelect);

*/

//			IIntuition->IDoMethod (atoms_list_p, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
	//			mol_info_obj_p, 3, MUIM_Set, MIGA_Atoms_Selected, MUIV_TriggerValue);

	//		IIntuition->IDoMethod (atoms_list_p, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
	//			mol_info_obj_p, 1, MIGM_MarkdownEditor_AtomSelect);


	//		IIntuition->IDoMethod (bonds_list_p, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
	//			mol_info_obj_p, 1, MIGM_MarkdownEditor_BondSelect);

			//IIntuition->IDoMethod(obj,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,obj,1,MUIM_DispIDlist_Change);


//			IIntuition->IDoMethod(obj,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,obj,1,MUIM_DispIDlist_Change);


			DB (KPRINTF ("%s %ld - MarkdownEditor_New: Adding info obj\n", __FILE__, __LINE__));
		}
	else
		{
			/* invoke OM_DISPOSE on *our* class! */
			IIntuition->ICoerceMethod (class_p, md_editor_p, OM_DISPOSE);
			md_editor_p = NULL;
		}


	DB (KPRINTF ("%s %ld - MarkdownEditor_New: new mol info gadget: 0x%.8x\n", __FILE__, __LINE__, mol_info_obj_p));

	return ((uint32) md_editor_p);
}


static uint32 MarkdownEditor_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	MarkdownEditorData *md_p = INST_DATA (class_p, object_p);
	struct opSet *ops_p = (struct opSet *) msg_p;
	struct TagItem *tags_p = ops_p -> ops_AttrList;
	struct TagItem *state_p = tags_p;
	struct TagItem *tag_p;

	/* Step through the tag list while there are still items in the list */
 	while ((tag_p = IUtility -> NextTagItem (&state_p)) != NULL)
		{
			/* Cache the data for the current element */
			uint32 tag_data = tag_p -> ti_Data;

			DB (KPRINTF ("%s %ld - ti_Tag: %lu ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
					case MEA_Viewer:
						md_p -> med_viewer_p = (Object *) tag_data;
						printf ("setting viewer to %lu\n", tag_data);
						break;

					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}



static void SetFile (MarkdownEditorData *md_p, STRPTR filename_s)
{
	md_p -> med_filename_s = filename_s;

	if (filename_s)
		{
			APTR list_p;
			LONG l1, l2;
			STRPTR buffer_s = NULL;

		}		/* if (mol_p) */

}



static uint32 ConvertMD (Class *class_p, Object *editor_p)
{
	uint32 res = 0;
	MarkdownEditorData *md_p = INST_DATA (class_p, editor_p);
	STRPTR text_s = (STRPTR) IIntuition -> IDoMethod (editor_p, MUIM_TextEditor_ExportText);
	
	if (text_s)
		{
			STRPTR html_s = NULL;
			BOOL res;
						
			res = ConvertText (text_s, &html_s, MD_HTML_FLAG_DEBUG | MD_HTML_FLAG_SKIP_UTF8_BOM, 0, TRUE);
			
			if (res)
				{				
					if (md_p -> med_viewer_p)
						{
							IIntuition -> SetAttrs (md_p -> med_viewer_p, MUIA_HTMLview_Contents, html_s, TAG_DONE);
						}
					
					IExec -> FreeVec (html_s);	
				}
			
			IExec -> FreeVec (text_s);
		}
	
	return res;
}


static uint32 LoadMD (Class *class_p, Object *editor_p)
{
	uint32 res = 0;
	STRPTR filename_s = NULL;
	
	if (filename_s)
		{
			LoadFile (filename_s);
		}
	
	return res;
}



static uint32 SaveMD (Class *class_p, Object *editor_p)
{
	uint32 res = 0;
	STRPTR filename_s = NULL;
	
	if (filename_s)
		{
			SaveFile (filename_s);
		}
	
	return res;
}



