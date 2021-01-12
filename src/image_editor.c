/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: image_editor.c
**
** Date: 08-01-2021 16:33:40
**
************************************************************

*/



#include <clib/alib_protos.h>

#include <exec/memory.h>
#include <exec/types.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <libraries/mui.h>

#include <mui/TextEditor_mcc.h>
#include <mui/BetterString_mcc.h>

#include "debugging_utils.h"

#include "image_editor.h"

#include "string_utils.h"


typedef struct ImageEditorData
{
	/** The Markdown Editor gadget */
	Object *ied_text_editor_p;
	STRPTR ied_path_s;
	STRPTR ied_alt_s;
	STRPTR ied_title_s;
} ImageEditorData;


static const char *S_MUIC_ImageEditor  = "ImageEditor";


/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/

static uint32 ImageEditorDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 ImageEditor_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 ImageEditor_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 ImageEditor_Dispose (Class *class_p, Object *object_p, Msg msg_p);


static Object *GetImageEditorObject (Object *parent_p, ImageEditorData *data_p);


/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitImageEditorClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster -> MUI_CreateCustomClass (NULL, MUIC_Group, NULL, sizeof (ImageEditorData), ImageEditorDispatcher);
}


void FreeImageEditorClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster -> MUI_DeleteCustomClass (mui_class_p);
}


/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 ImageEditorDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res = 0;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - ImageEditor Dispatcher: New\n", __FILE__, __LINE__));
				res = ImageEditor_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - ImageEditor Dispatcher: Set\n", __FILE__, __LINE__));
				res = ImageEditor_Set (class_p, object_p, msg_p);
				break;

			case OM_DISPOSE:
				DB (KPRINTF ("%s %ld - ImageEditor Dispatcher: Dispose\n", __FILE__, __LINE__));
				res = ImageEditor_Dispose (class_p, object_p, msg_p);
				break;

			case IEM_Insert:
				{
					ImageEditorData *data_p = INST_DATA (class_p, object_p);
					Object *window_obj_p = NULL;
					
					DB (KPRINTF ("%s %ld - ImageEditor Dispatcher: IEM_Insert\n", __FILE__, __LINE__));

					if (data_p -> ied_text_editor_p)
						{
							if ((data_p -> ied_path_s) && (data_p -> ied_alt_s))
								{
									STRPTR image_s = NULL;

									if (data_p -> ied_title_s)
										{
											image_s = ConcatenateVarargsStrings ("![", data_p -> ied_alt_s, "](", data_p -> ied_path_s, " \"", data_p -> ied_title_s, "\")", NULL);
										}
									else
										{
											image_s = ConcatenateVarargsStrings ("![", data_p -> ied_alt_s, "](", data_p -> ied_path_s, ")", NULL);
										}

									if (image_s)
										{
											res = IIntuition -> IDoMethod (data_p -> ied_text_editor_p, MUIM_TextEditor_InsertText, image_s, MUIV_TextEditor_InsertText_Cursor);


											/*
											data_p -> ied_alt_s = NULL;
											data_p -> ied_path_s = NULL;
											data_p -> ied_title_s = NULL;
											*/

											FreeCopiedString (image_s);
										}
								}
						}
						
					IIntuition -> GetAttrs (object_p, MUIA_WindowObject, &window_obj_p);
					
					if (window_obj_p)
						{ 	
							DB (KPRINTF ("%s %ld - ImageEditor Dispatcher: IEM_Insert  window %lu\n", __FILE__, __LINE__, (uint32) window_obj_p));
							IIntuition -> IDoMethod (window_obj_p, MUIM_Set, MUIA_Window_CloseRequest, TRUE, TAG_DONE);
						}
					else
						{
							DB (KPRINTF ("%s %ld - ImageEditor Dispatcher: IEM_Insert NO window\n", __FILE__, __LINE__));
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


static Object *GetImageEditorObject (Object *parent_p, ImageEditorData *data_p)
{
	Object *path_p = NULL;
	Object *alt_p = NULL;
	Object *title_p = NULL;
	Object *ok_p = NULL;
	Object *cancel_p = NULL;

	Object *child_object_p = IMUIMaster -> MUI_NewObject (MUIC_Group,
		MUIA_Group_Horiz, FALSE,

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Group_Columns, 2,

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Path:", TAG_DONE),
			MUIA_Group_Child, path_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
				MUIA_String_Contents, (uint32) data_p -> ied_path_s,
				MUIA_Frame, MUIV_Frame_String,
			TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Alt:", TAG_DONE),
			MUIA_Group_Child, alt_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
				MUIA_String_Contents, (uint32) data_p -> ied_alt_s,
				MUIA_Frame, MUIV_Frame_String,				
			TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Title:", TAG_DONE),
			MUIA_Group_Child, title_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
				MUIA_String_Contents, (uint32) data_p -> ied_title_s,
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
			DB (KPRINTF ("%s %ld - ImageEditor_New: Adding child obj\n", __FILE__, __LINE__));

			IIntuition -> IDoMethod (parent_p, OM_ADDMEMBER, child_object_p);

			IIntuition -> SetAttrs (path_p, MUIA_ShortHelp, "Path to the image.", TAG_DONE);
			IIntuition -> SetAttrs (alt_p, MUIA_ShortHelp, "The alternative text for the image", TAG_DONE);
			IIntuition -> SetAttrs (title_p, MUIA_ShortHelp, "An optional title for the image", TAG_DONE);

			IIntuition -> IDoMethod (path_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, IEA_Path, MUIV_TriggerValue);
			IIntuition -> IDoMethod (alt_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, IEA_Alt, MUIV_TriggerValue);
			IIntuition -> IDoMethod (title_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, IEA_Title, MUIV_TriggerValue);

			IIntuition -> IDoMethod (ok_p, MUIM_Notify, MUIA_Pressed, FALSE, parent_p, 1, IEM_Insert);
			IIntuition -> IDoMethod (cancel_p, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
		}


	return child_object_p;
}


static uint32 ImageEditor_New (Class *class_p, Object *object_p, Msg msg_p)
{
	Object *parent_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);

	if (parent_p)
		{
			ImageEditorData *data_p = INST_DATA (class_p, parent_p);
			Object *child_p;

			data_p -> ied_text_editor_p = NULL;
			data_p -> ied_path_s = NULL;
			data_p -> ied_alt_s = NULL;
			data_p -> ied_title_s = NULL;

			child_p = GetImageEditorObject (parent_p, data_p);

			if (child_p)
				{
					return (uint32) parent_p;
				}

			IIntuition->ICoerceMethod (class_p, parent_p, OM_DISPOSE);
		}		/* if (parent_p) */

	return 0;
}

static uint32 ImageEditor_Dispose (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	ImageEditorData *data_p = INST_DATA (class_p, object_p);

	if (data_p -> ied_path_s)
		{
		//	FreeCopiedString (data_p -> ied_path_s);
		}

	if (data_p -> ied_alt_s)
		{
		//	FreeCopiedString (data_p -> ied_alt_s);
		}

	if (data_p -> ied_title_s)
		{
		//	FreeCopiedString (data_p -> ied_title_s);
		}

	return retval;
}


static uint32 ImageEditor_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
	ImageEditorData *data_p = INST_DATA (class_p, object_p);
	struct opSet *ops_p = (struct opSet *) msg_p;
	struct TagItem *tags_p = ops_p -> ops_AttrList;
	struct TagItem *state_p = tags_p;
	struct TagItem *tag_p;

	DB (KPRINTF ("%s %ld - ImageEditor_Set enter\n", __FILE__, __LINE__));

	/* Step through the tag list while there are still items in the list */
 	while ((tag_p = IUtility -> NextTagItem (&state_p)) != NULL)
		{
			/* Cache the data for the current element */
			uint32 tag_data = tag_p -> ti_Data;

			DB (KPRINTF ("%s %ld - ImageEditor_Set ti_Tag: %8X ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
					case IEA_Editor:
						DB (KPRINTF ("%s %ld - ImageEditor_Set -> ied_text_editor_p to %lu", __FILE__, __LINE__, tag_data));
						data_p -> ied_text_editor_p = (Object *) tag_data;
						break;

					case IEA_Path:
						DB (KPRINTF ("%s %ld - ImageEditor_Set -> ied_path_s to %s", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> ied_path_s = (STRPTR) tag_data;
						break;

					case IEA_Alt:
						DB (KPRINTF ("%s %ld - ImageEditor_Set -> ied_alt_s to %s", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> ied_alt_s = (STRPTR) tag_data;
						break;

					case IEA_Title:
						DB (KPRINTF ("%s %ld - ImageEditor_Set -> ied_title_s to %s", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> ied_title_s = (STRPTR) tag_data;
						break;


					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}
