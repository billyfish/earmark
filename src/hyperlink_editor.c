/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: hyperlink_editor.c
**
** Date: 08-01-2021 16:33:40
**
************************************************************

*/


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

#include <clib/alib_protos.h>

#include <exec/memory.h>
#include <exec/types.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <libraries/mui.h>

#include <mui/TextEditor_mcc.h>
#include <mui/BetterString_mcc.h>

#include "debugging_utils.h"

#include "hyperlink_editor.h"

#include "string_utils.h"


typedef struct HyperlinkEditorData
{
	/** The Markdown Editor gadget */
	Object *hed_text_editor_p;
 	STRPTR hed_text_s;
 	STRPTR hed_url_s;
} HyperlinkEditorData;



/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/

static uint32 HyperlinkEditorDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 HyperlinkEditor_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 HyperlinkEditor_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 HyperlinkEditor_Dispose (Class *class_p, Object *object_p, Msg msg_p);


static Object *GetHyperlinkEditorObject (Object *parent_p, HyperlinkEditorData *data_p);


/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitHyperlinkEditorClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster -> MUI_CreateCustomClass (NULL, MUIC_Group, NULL, sizeof (HyperlinkEditorData), HyperlinkEditorDispatcher);
}


void FreeHyperlinkEditorClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster -> MUI_DeleteCustomClass (mui_class_p);
}




/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 HyperlinkEditorDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res = 0;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - HyperlinkEditor Dispatcher: New\n", __FILE__, __LINE__));
				res = HyperlinkEditor_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - HyperlinkEditor Dispatcher: Set\n", __FILE__, __LINE__));
				res = HyperlinkEditor_Set (class_p, object_p, msg_p);
				break;

			case OM_DISPOSE:
				DB (KPRINTF ("%s %ld - HyperlinkEditor Dispatcher: Dispose\n", __FILE__, __LINE__));
				res = HyperlinkEditor_Dispose (class_p, object_p, msg_p);
				break;

			case HEM_Insert:
				{
					HyperlinkEditorData *data_p = INST_DATA (class_p, object_p);
					Object *window_obj_p = NULL;
					
					DB (KPRINTF ("%s %ld - HyperlinkEditor Dispatcher: TEM_Insert\n", __FILE__, __LINE__));

					if (data_p -> hed_text_editor_p)
						{
							if ((data_p -> hed_text_s) && (data_p -> hed_url_s))
								{
									STRPTR link_s = ConcatenateVarargsStrings ("[", data_p -> hed_text_s, "](", data_p -> hed_url_s, ")", NULL);

									if (link_s)
										{
											res = IIntuition -> IDoMethod (data_p -> hed_text_editor_p, MUIM_TextEditor_InsertText, link_s, MUIV_TextEditor_InsertText_Cursor);

											/*
											data_p -> ied_alt_s = NULL;
											data_p -> ied_path_s = NULL;
											data_p -> ied_title_s = NULL;
											*/

											FreeCopiedString (link_s);
										}
								}
						}
						
						
					IIntuition -> GetAttrs (object_p, MUIA_WindowObject, &window_obj_p);
					
					if (window_obj_p)
						{ 	
							DB (KPRINTF ("%s %ld - HyperlinkEditor Dispatcher: IEM_Insert  window %lu\n", __FILE__, __LINE__, (uint32) window_obj_p));
							IIntuition -> IDoMethod (window_obj_p, MUIM_Set, MUIA_Window_CloseRequest, TRUE, TAG_DONE);
						}
					else
						{
							DB (KPRINTF ("%s %ld - HyperlinkEditor Dispatcher: IEM_Insert NO window\n", __FILE__, __LINE__));
						}								
				}
				break;


			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}


static Object *GetHyperlinkEditorObject (Object *parent_p, HyperlinkEditorData *data_p)
{
	Object *text_p = NULL;
	Object *url_p = NULL;
	Object *ok_p = NULL;
	Object *cancel_p = NULL;

	Object *child_object_p = IMUIMaster -> MUI_NewObject (MUIC_Group,
		MUIA_Group_Horiz, FALSE,

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Group_Columns, 2,


			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Text:", TAG_DONE),
			MUIA_Group_Child, text_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
				MUIA_String_Contents, (uint32) data_p -> hed_text_s,
				MUIA_Frame, MUIV_Frame_String,
			TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "URL:", TAG_DONE),
			MUIA_Group_Child, url_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
				MUIA_String_Contents, (uint32) data_p -> hed_url_s,
				MUIA_Frame, MUIV_Frame_String,				
			TAG_DONE),

		TAG_DONE),

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Group_Horiz, TRUE,
			MUIA_Group_Child, ok_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "\33I[5:TBimages:ok] Ok"),
			MUIA_Group_Child, cancel_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "\33I[5:TBimages:cancel] Cancel"),
		TAG_DONE),

	TAG_DONE);


	if (child_object_p)
		{
			DB (KPRINTF ("%s %ld - HyperlinkEditor_New: Adding child obj\n", __FILE__, __LINE__));

			IIntuition -> IDoMethod (parent_p, OM_ADDMEMBER, child_object_p);

			IIntuition -> SetAttrs (text_p, MUIA_ShortHelp, "Number of rows in the table.", TAG_DONE);
			IIntuition -> SetAttrs (url_p, MUIA_ShortHelp, "The address to link to", TAG_DONE);
			
			IIntuition -> IDoMethod (text_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, HEA_Text, MUIV_TriggerValue);
			IIntuition -> IDoMethod (url_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, HEA_Url, MUIV_TriggerValue);

			IIntuition -> IDoMethod (ok_p, MUIM_Notify, MUIA_Pressed, FALSE, parent_p, 1, HEM_Insert);
			IIntuition -> IDoMethod (cancel_p, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
		}


	return child_object_p;
}


static uint32 HyperlinkEditor_New (Class *class_p, Object *object_p, Msg msg_p)
{
	Object *parent_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);

	if (parent_p)
		{
			HyperlinkEditorData *data_p = INST_DATA (class_p, parent_p);
			Object *child_p;

			data_p -> hed_text_editor_p = NULL;
			data_p -> hed_text_s = NULL;
			data_p -> hed_url_s = NULL;
			
			child_p = GetHyperlinkEditorObject (parent_p, data_p);

			if (child_p)
				{
					return (uint32) parent_p;
				}

			IIntuition->ICoerceMethod (class_p, parent_p, OM_DISPOSE);
		}		/* if (parent_p) */

	return 0;
}

static uint32 HyperlinkEditor_Dispose (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	HyperlinkEditorData *data_p = INST_DATA (class_p, object_p);


	return retval;
}


static uint32 HyperlinkEditor_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
	HyperlinkEditorData *data_p = INST_DATA (class_p, object_p);
	struct opSet *ops_p = (struct opSet *) msg_p;
	struct TagItem *tags_p = ops_p -> ops_AttrList;
	struct TagItem *state_p = tags_p;
	struct TagItem *tag_p;

	DB (KPRINTF ("%s %ld - HyperlinkEditor_Set enter\n", __FILE__, __LINE__));

	/* Step through the tag list while there are still items in the list */
 	while ((tag_p = IUtility -> NextTagItem (&state_p)) != NULL)
		{
			/* Cache the data for the current element */
			uint32 tag_data = tag_p -> ti_Data;

			DB (KPRINTF ("%s %ld - HyperlinkEditor_Set ti_Tag: %8X ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
					case HEA_Editor:
						DB (KPRINTF ("%s %ld - HyperlinkEditor_Set -> hed_text_editor_p to %lu\n", __FILE__, __LINE__, tag_data));
						data_p -> hed_text_editor_p = (Object *) tag_data;
						break;

					case HEA_Text:
						DB (KPRINTF ("%s %ld - HyperlinkEditor_Set -> hed_text_s to %s\n", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> hed_text_s = (STRPTR) tag_data;
						break;

					case HEA_Url:
						DB (KPRINTF ("%s %ld - HyperlinkEditor_Set -> hed_url_s to %s\n", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> hed_url_s = (STRPTR) tag_data;
						break;


					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}


