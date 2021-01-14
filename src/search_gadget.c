/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: search_gadget.c
**
** Date: 13-01-2021 14:57:13
**
************************************************************

*/

#include <string.h>

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

#include "search_gadget.h"

#include "string_utils.h"
#include "gui.h"

typedef struct SearchGagdetData
{
	Object *sgd_text_editor_p;
	STRPTR sgd_text_s;
	uint32 sgd_search_from;
	BOOL sgd_search_backwards_flag;
	BOOL sgd_case_sensitive_flag;
	BOOL sgd_fresh_search_flag;
} SearchGadgetData;

/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/

enum
{
	FROM_CURSOR,
	FROM_TOP
};

static const char *S_FROM_SS [] = { "Cursor", "Top", NULL };

static uint32 SearchGadgetDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 SearchGadget_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 SearchGadget_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 SearchGadget_Dispose (Class *class_p, Object *object_p, Msg msg_p);


static Object *GetSearchGadgetObject (Object *parent_p, SearchGadgetData *data_p);


/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitSearchGadgetClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/


	return IMUIMaster -> MUI_CreateCustomClass (NULL, MUIC_Group, NULL, sizeof (SearchGadgetData), SearchGadgetDispatcher);
}


void FreeSearchGadgetClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster -> MUI_DeleteCustomClass (mui_class_p);
}


/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 SearchGadgetDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res = 0;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - SearchGadget Dispatcher: New\n", __FILE__, __LINE__));
				res = SearchGadget_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - SearchGadget Dispatcher: Set\n", __FILE__, __LINE__));
				res = SearchGadget_Set (class_p, object_p, msg_p);
				break;

			case OM_DISPOSE:
				DB (KPRINTF ("%s %ld - SearchGadget Dispatcher: Dispose\n", __FILE__, __LINE__));
				res = SearchGadget_Dispose (class_p, object_p, msg_p);
				break;

			case SGM_Search:
				{
					SearchGadgetData *data_p = INST_DATA (class_p, object_p);
					Object *window_obj_p = NULL;
					uint32 flags = data_p -> sgd_search_from;
					data_p -> sgd_fresh_search_flag = TRUE;

					DB (KPRINTF ("%s %ld - SearchGadget Dispatcher: SGM_Search\n", __FILE__, __LINE__));

					if (data_p -> sgd_text_s)
						{
							const size_t l = strlen (data_p -> sgd_text_s);

							/* TextEditor has a max search length of 120 */
							if (l < 120)
								{
									if (data_p -> sgd_case_sensitive_flag)
										{
											flags |= MUIF_TextEditor_Search_CaseSensitive;
										}

									if (data_p -> sgd_search_backwards_flag)
										{
											flags |= MUIF_TextEditor_Search_Backwards;
										}

									/*
										If starting from the top, continue from the cursur for
										the next search if appropriate
									*/

									DB (KPRINTF ("%s %ld - SearchGadget Dispatcher: searching for \"%s\" with flags %lu\n", __FILE__, __LINE__, data_p -> sgd_text_s, flags));

									if (IIntuition -> IDoMethod (data_p -> sgd_text_editor_p, MUIM_TextEditor_Search, data_p -> sgd_text_s, flags))
										{
											res = 1;
										}
									else
										{
											ShowInformation ("Search", "String not found", "_Ok");
										}
								}
							else
								{
									ShowWarning ("Search Error", "The search string must be a maximum of 120 characters.", "_Ok");
								}


						}		/* if (data_p -> sgd_text_s) */



				}
				break;


			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}


static Object *GetSearchGadgetObject (Object *parent_p, SearchGadgetData *data_p)
{
	Object *text_p = NULL;
	Object *from_p = NULL;
	Object *case_p = NULL;
	Object *direction_p = NULL;
	Object *ok_p = NULL;
	Object *cancel_p = NULL;

	Object *child_object_p = IMUIMaster -> MUI_NewObject (MUIC_Group,

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Group_Horiz, FALSE,

			MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
				MUIA_Group_Columns, 2,

				MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Text:", TAG_DONE),
				MUIA_Group_Child, text_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
					MUIA_Frame, MUIV_Frame_String,
				TAG_DONE),

				MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "From:", TAG_DONE),
				MUIA_Group_Child, from_p = IMUIMaster -> MUI_NewObject (MUIC_Cycle,
					MUIA_Cycle_Entries, S_FROM_SS,
				TAG_DONE),
			TAG_DONE),

		TAG_DONE),

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Group_Horiz, TRUE,

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Case sensitive?:", TAG_DONE),
			MUIA_Group_Child, case_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark,
			TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Search Backwards?:", TAG_DONE),
			MUIA_Group_Child, direction_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark,
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
			DB (KPRINTF ("%s %ld - SearchGadget_New: Adding child obj\n", __FILE__, __LINE__));

			IIntuition -> IDoMethod (parent_p, OM_ADDMEMBER, child_object_p);

			IIntuition -> SetAttrs (text_p, MUIA_ShortHelp, "The text to search for.", TAG_DONE);
			IIntuition -> SetAttrs (from_p, MUIA_ShortHelp, "Search from the current cursor position or from the top.", TAG_DONE);
			IIntuition -> SetAttrs (case_p, MUIA_ShortHelp, "Do a case-senstive search.", TAG_DONE);
			IIntuition -> SetAttrs (direction_p, MUIA_ShortHelp, "Search backwards", TAG_DONE);

			IIntuition -> IDoMethod (text_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, SGA_Text, MUIV_TriggerValue);
			IIntuition -> IDoMethod (from_p, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, parent_p, 3, MUIM_Set, SGA_FromPosition, MUIV_TriggerValue);
			IIntuition -> IDoMethod (case_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, SGA_CaseSensitive, MUIV_TriggerValue);
			IIntuition -> IDoMethod (direction_p, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, parent_p, 3, MUIM_Set, SGA_SearchBackwards, MUIV_TriggerValue);

			IIntuition -> IDoMethod (ok_p, MUIM_Notify, MUIA_Pressed, FALSE, parent_p, 1,SGM_Search);
			IIntuition -> IDoMethod (cancel_p, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
		}


	return child_object_p;
}


static uint32 SearchGadget_New (Class *class_p, Object *object_p, Msg msg_p)
{
	Object *parent_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);

	if (parent_p)
		{
			SearchGadgetData *data_p = INST_DATA (class_p, parent_p);
			Object *child_p;

			data_p -> sgd_text_editor_p = NULL;
			data_p -> sgd_text_s = NULL;
			data_p -> sgd_search_from = 0;
			data_p -> sgd_case_sensitive_flag = FALSE;
			data_p -> sgd_search_backwards_flag = FALSE;

			child_p = GetSearchGadgetObject (parent_p, data_p);

			if (child_p)
				{
					return (uint32) parent_p;
				}

			IIntuition->ICoerceMethod (class_p, parent_p, OM_DISPOSE);
		}		/* if (parent_p) */

	return 0;
}

static uint32 SearchGadget_Dispose (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	SearchGadgetData *data_p = INST_DATA (class_p, object_p);


	return retval;
}


static uint32 SearchGadget_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
	SearchGadgetData *data_p = INST_DATA (class_p, object_p);
	struct opSet *ops_p = (struct opSet *) msg_p;
	struct TagItem *tags_p = ops_p -> ops_AttrList;
	struct TagItem *state_p = tags_p;
	struct TagItem *tag_p;

	DB (KPRINTF ("%s %ld - SearchGadget_Set enter\n", __FILE__, __LINE__));

	/* Step through the tag list while there are still items in the list */
 	while ((tag_p = IUtility -> NextTagItem (&state_p)) != NULL)
		{
			/* Cache the data for the current element */
			uint32 tag_data = tag_p -> ti_Data;

			DB (KPRINTF ("%s %ld - SearchGadget_Set ti_Tag: %8X ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
					case SGA_TextEditor:
						DB (KPRINTF ("%s %ld - SearchGadget_Set -> ied_text_editor_p to %lu", __FILE__, __LINE__, tag_data));
						data_p -> sgd_text_editor_p = (Object *) tag_data;
						break;

					case SGA_Text:
						DB (KPRINTF ("%s %ld - SearchGadget_Set -> ied_path_s to %s", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> sgd_text_s = (STRPTR) tag_data;
						break;

					case SGA_FromPosition:
						{
							if (tag_data == FROM_CURSOR)
								{
									data_p -> sgd_search_from = 0;
								}
							else if (tag_data == FROM_TOP)
								{
									data_p -> sgd_search_from = MUIF_TextEditor_Search_FromTop;
								}
						}
						break;

					case SGA_CaseSensitive:
						{
							DB (KPRINTF ("%s %ld - SearchGadget_Set -> case senstive to %lu", __FILE__, __LINE__, tag_data));

							if (tag_data == TRUE)
								{
									data_p -> sgd_case_sensitive_flag = TRUE;
								}
							else if (tag_data == FALSE)
								{
									data_p -> sgd_case_sensitive_flag = FALSE;
								}
						}
						break;


					case SGA_SearchBackwards:
						{
							DB (KPRINTF ("%s %ld - SearchGadget_Set -> sgd_backwards to %lu", __FILE__, __LINE__, tag_data));

							if (tag_data == TRUE)
								{
									data_p -> sgd_search_backwards_flag = TRUE;
								}
							else if (tag_data == FALSE)
								{
									data_p -> sgd_search_backwards_flag = FALSE;
								}
						}
						break;



					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}
