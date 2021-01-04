/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: viewer_gadget.c
**
** Date: 04-01-2021 12:34:17
**
************************************************************

*/

#include "viewer_gadget.h"
#include "debugging_utils.h"

#include <mui/HTMLview_mcc.h>

#include <proto/muimaster.h>


typedef struct MarkdownViewerData
{
	STRPTR mvd_filename_s;
} MarkdownViewerData;




/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/


static uint32 MarkdownViewerDispatcher (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownViewer_New (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownViewer_Set (Class *class_p, Object *object_p, Msg msg_p);


static void SetFile (MarkdownViewerData *md_p, STRPTR filename_s);



/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitMarkdownViewerClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster->MUI_CreateCustomClass (NULL, MUIC_HTMLview, NULL, sizeof (MarkdownViewerData), MarkdownViewerDispatcher);
}


void FreeMarkdownViewerClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster->MUI_DeleteCustomClass (mui_class_p);
}





/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 MarkdownViewerDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - MarkdownViewerDispatcher: New\n", __FILE__, __LINE__));
				res = MarkdownViewer_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - MarkdownViewerDispatcher: Set\n", __FILE__, __LINE__));
				res = MarkdownViewer_Set (class_p, object_p, msg_p);
				break;

			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}
           

static uint32 MarkdownViewer_New (Class *class_p, Object *object_p, Msg msg_p)
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

	Object *md_viewer_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);


	if (md_viewer_p)
		{
			MarkdownViewerData *md_p = INST_DATA (class_p, md_viewer_p);

			md_p -> mvd_filename_s = NULL;

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
				new_obj_p, 1, MIGM_MarkdownViewer_BondSelect);

*/

//			IIntuition->IDoMethod (atoms_list_p, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
	//			mol_info_obj_p, 3, MUIM_Set, MIGA_Atoms_Selected, MUIV_TriggerValue);

	//		IIntuition->IDoMethod (atoms_list_p, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
	//			mol_info_obj_p, 1, MIGM_MarkdownViewer_AtomSelect);


	//		IIntuition->IDoMethod (bonds_list_p, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime,
	//			mol_info_obj_p, 1, MIGM_MarkdownViewer_BondSelect);

			//IIntuition->IDoMethod(obj,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,obj,1,MUIM_DispIDlist_Change);


//			IIntuition->IDoMethod(obj,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,obj,1,MUIM_DispIDlist_Change);


			DB (KPRINTF ("%s %ld - MarkdownViewer_New: Adding info obj\n", __FILE__, __LINE__));
		}
	else
		{
			/* invoke OM_DISPOSE on *our* class! */
			IIntuition->ICoerceMethod (class_p, md_viewer_p, OM_DISPOSE);
			md_viewer_p = NULL;
		}


	DB (KPRINTF ("%s %ld - MarkdownViewer_New: new mol info gadget: 0x%.8x\n", __FILE__, __LINE__, mol_info_obj_p));

	return ((uint32) md_viewer_p);
}


static uint32 MarkdownViewer_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	MarkdownViewerData *md_p = INST_DATA (class_p, object_p);



	return retval;
}



static void SetFile (MarkdownViewerData *md_p, STRPTR filename_s)
{
	md_p -> mvd_filename_s = filename_s;

	if (filename_s)
		{
			APTR list_p;
			LONG l1, l2;
			STRPTR buffer_s = NULL;

		}		/* if (mol_p) */

}


