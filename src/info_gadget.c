/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: info_gadget.c
**
** Date: 18-01-2021 09:49:25
**
************************************************************

*/


/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: table_editor.c
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

#include "info_gadget.h"
#include "gui.h"

#include "byte_buffer.h"
#include "string_utils.h"


typedef struct InfoGadgetData
{
	/** The Markdown Editor gadget */
	Object *igd_text_editor_p;
	uint32 igd_num_rows;
	uint32 igd_num_columns;
 	BOOL igd_changed;
} InfoGadgetData;

Object *s_row_p = NULL;
Object *s_column_p = NULL;
Object *s_changed_p = NULL;

/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/

static uint32 InfoGadgetDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 InfoGadget_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 InfoGadget_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 InfoGadget_Dispose (Class *class_p, Object *object_p, Msg msg_p);


static Object *GetInfoGadgetObject (Object *parent_p, InfoGadgetData *data_p);


/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitInfoGadgetClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster -> MUI_CreateCustomClass (NULL, MUIC_Group, NULL, sizeof (InfoGadgetData), InfoGadgetDispatcher);
}


void FreeInfoGadgetClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster -> MUI_DeleteCustomClass (mui_class_p);
}




/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 InfoGadgetDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res = 0;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - InfoGadget Dispatcher: New\n", __FILE__, __LINE__));
				res = InfoGadget_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - InfoGadget Dispatcher: Set\n", __FILE__, __LINE__));
				res = InfoGadget_Set (class_p, object_p, msg_p);
				break;

			case OM_DISPOSE:
				DB (KPRINTF ("%s %ld - InfoGadget Dispatcher: Dispose\n", __FILE__, __LINE__));
				res = InfoGadget_Dispose (class_p, object_p, msg_p);
				break;


			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}


static Object *GetInfoGadgetObject (Object *parent_p, InfoGadgetData *data_p)
{
	Object *num_rows_p = NULL;
	Object *num_columns_p = NULL;
	Object *changed_p = NULL;

	Object *child_object_p = IMUIMaster -> MUI_NewObject (MUIC_Group,
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Row:", TAG_DONE),				
		MUIA_Group_Child, s_row_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
			MUIA_String_Accept, "",	
      MUIA_String_Integer, data_p -> igd_num_rows,
		TAG_DONE),

		MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Column:", TAG_DONE),
		MUIA_Group_Child, s_column_p = IMUIMaster -> MUI_NewObject (MUIC_BetterString,
			MUIA_String_Accept, "",	
      MUIA_String_Integer, data_p -> igd_num_columns,
		TAG_DONE),

		MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Changed?:", TAG_DONE),
		MUIA_Group_Child, s_changed_p = IMUIMaster -> MUI_MakeObject (MUIO_Checkmark,
		TAG_DONE),

	TAG_DONE);


	if (child_object_p)
		{
			DB (KPRINTF ("%s %ld - InfoGadget_New: Adding child obj\n", __FILE__, __LINE__));

			IIntuition -> IDoMethod (parent_p, OM_ADDMEMBER, child_object_p);

			IIntuition -> SetAttrs (s_row_p, MUIA_ShortHelp, "Current row of cursor.", TAG_DONE);
			IIntuition -> SetAttrs (s_column_p, MUIA_ShortHelp, "Current column of cursor ", TAG_DONE);
			IIntuition -> SetAttrs (s_changed_p, MUIA_ShortHelp, "Has the text been changed since the last save?", MUIA_Disabled, TRUE, TAG_DONE);
			
	//		IIntuition -> IDoMethod (row_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_NoNotifySet, IGA_Rows, MUIV_TriggerValue);
	//		IIntuition -> IDoMethod (column_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_NoNotifySet, IGA_Columns, MUIV_TriggerValue);

		}


	return child_object_p;
}


static uint32 InfoGadget_New (Class *class_p, Object *object_p, Msg msg_p)
{
	Object *parent_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);

	if (parent_p)
		{
			InfoGadgetData *data_p = INST_DATA (class_p, parent_p);
			Object *child_p;

			data_p -> igd_text_editor_p = NULL;
			data_p -> igd_num_rows = 1;
			data_p -> igd_num_columns = 1;
			data_p -> igd_changed = FALSE;

			child_p = GetInfoGadgetObject (parent_p, data_p);

			if (child_p)
				{
					return (uint32) parent_p;
				}

			IIntuition->ICoerceMethod (class_p, parent_p, OM_DISPOSE);
		}		/* if (parent_p) */

	return 0;
}

static uint32 InfoGadget_Dispose (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	InfoGadgetData *data_p = INST_DATA (class_p, object_p);


	return retval;
}


static uint32 InfoGadget_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
	InfoGadgetData *data_p = INST_DATA (class_p, object_p);
	struct opSet *ops_p = (struct opSet *) msg_p;
	struct TagItem *tags_p = ops_p -> ops_AttrList;
	struct TagItem *state_p = tags_p;
	struct TagItem *tag_p;

	DB (KPRINTF ("%s %ld - InfoGadget_Set enter\n", __FILE__, __LINE__));

	/* Step through the tag list while there are still items in the list */
 	while ((tag_p = IUtility -> NextTagItem (&state_p)) != NULL)
		{
			/* Cache the data for the current element */
			uint32 tag_data = tag_p -> ti_Data;

			DB (KPRINTF ("%s %ld - InfoGadget_Set ti_Tag: %8X ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
					case IGA_Editor:
						DB (KPRINTF ("%s %ld - InfoGadget_Set -> igd_text_editor_p to %lu\n", __FILE__, __LINE__, tag_data));
						data_p -> igd_text_editor_p = (Object *) tag_data;
						IIntuition -> IDoMethod (data_p -> igd_text_editor_p, MUIM_Notify, MUIA_TextEditor_ContentsChanged, MUIV_EveryTime, object_p, 3, MUIM_Set, IGA_Changed, MUIV_TriggerValue);
						IIntuition -> IDoMethod (data_p -> igd_text_editor_p, MUIM_Notify, MUIA_TextEditor_CursorY, MUIV_EveryTime, object_p, 3, MUIM_Set, IGA_Rows, MUIV_TriggerValue);
						IIntuition -> IDoMethod (data_p -> igd_text_editor_p, MUIM_Notify, MUIA_TextEditor_CursorX, MUIV_EveryTime, object_p, 3, MUIM_Set, IGA_Columns, MUIV_TriggerValue);
						break;

					case IGA_Columns:
						{
							char *value_s = NULL;
							
							DB (KPRINTF ("%s %ld - InfoGadget_Set -> igd_num_columns to %lu\n", __FILE__, __LINE__, tag_data));
							
							/* Start counting frmo 1 rather than 0 for user-friendliness so add 1 */
							value_s = ConvertUnsignedIntegerToString (tag_data + 1);
							
							if (value_s)
								{
									IIntuition -> SetAttrs (s_column_p, MUIA_String_Contents, value_s, TAG_DONE);	
								}
							

						}
						break;

					case IGA_Rows:
						{
							char *value_s = NULL;

							DB (KPRINTF ("%s %ld - InfoGadget_Set -> igd_num_rows to %lu\n", __FILE__, __LINE__, tag_data));							

							/* Start counting frmo 1 rather than 0 for user-friendliness so add 1 */
							value_s = ConvertUnsignedIntegerToString (tag_data + 1);
							
							if (value_s)
								{
									IIntuition -> SetAttrs (s_row_p, MUIA_String_Contents, value_s, TAG_DONE);	
								}
							
						}
						break;

					case IGA_Changed:
						DB (KPRINTF ("%s %ld - InfoGadget_Set -> changed to %lu\n", __FILE__, __LINE__, tag_data));
						
						if (tag_data == TRUE)
							{
								data_p -> igd_changed = TRUE;
								IIntuition -> SetAttrs (s_changed_p, MUIA_Selected, TRUE, TAG_DONE);
							}
						else if (tag_data == FALSE)
							{
								data_p -> igd_changed = FALSE;	
								IIntuition -> SetAttrs (s_changed_p, MUIA_Selected, FALSE, TAG_DONE);
							}
							
						//IIntuition -> SetAttrs (s_changed_p, MUIA_Selected, data_p -> igd_changed, TAG_DONE);
						break;

					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}

