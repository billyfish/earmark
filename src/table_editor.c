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



#include <clib/alib_protos.h>

#include <exec/memory.h>
#include <exec/types.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <libraries/mui.h>

#include <mui/TextEditor_mcc.h>

#include "debugging_utils.h"

#include "table_editor.h"

#include "byte_buffer.h"


typedef struct TableEditorData
{
	/** The Markdown Editor gadget */
	Object *ted_text_editor_p;
	uint32 ted_num_rows;
	uint32 ted_num_columns;
	STRPTR ted_column_alignments_s;
} TableEditorData;


static const char *S_MUIC_TableEditor  = "TableEditor";


/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/

static uint32 TableEditorDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 TableEditor_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 TableEditor_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 TableEditor_Dispose (Class *class_p, Object *object_p, Msg msg_p);


static Object *GetTableEditorObject (Object *parent_p);

/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitTableEditorClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster -> MUI_CreateCustomClass (NULL, MUIC_Group, NULL, sizeof (TableEditorData), TableEditorDispatcher);
}


void FreeTableEditorClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster -> MUI_DeleteCustomClass (mui_class_p);
}




/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 TableEditorDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res = 0;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - TableEditor Dispatcher: New\n", __FILE__, __LINE__));
				res = TableEditor_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - TableEditor Dispatcher: Set\n", __FILE__, __LINE__));
				res = TableEditor_Set (class_p, object_p, msg_p);
				break;

			case OM_DISPOSE:
				DB (KPRINTF ("%s %ld - TableEditor Dispatcher: Dispose\n", __FILE__, __LINE__));
				res = TableEditor_Dispose (class_p, object_p, msg_p);
				break;

			case TEM_Insert:
				{
					TableEditorData *data_p = INST_DATA (class_p, object_p);

					DB (KPRINTF ("%s %ld - TableEditor Dispatcher: TEM_Insert\n", __FILE__, __LINE__));

					if (data_p -> ted_text_editor_p)
						{
							if ((data_p -> ted_path_s) && (data_p -> ted_alt_s))
								{
									ByteBuffer *buffer_p = AllocateByteBuffer (1024);

									if (buffer_p)
										{

											if (GenerateEmptyRow (buffer_p, num_cols))
												{
													CONST STRPTR alignments_s = NULL;

													if (GenerateAlignmentRow (buffer_p, num_cols, alignments_s))
														{
															BOOL success_flag = TRUE;
															uint32 i = 0;

															while (success_flag && (i < num_rows))
																{
																	success_flag = GenerateEmptyRow (buffer_p, num_cols);
																}

															if (success_flag)
																{
																	const char *table_s = GetByteBufferData (buffer_p);
																	res = IIntuition -> IDoMethod (data_p -> ted_text_editor_p, MUIM_TextEditor_InsertText, table_s, MUIV_TextEditor_InsertText_Cursor);
																}
														}
												}

												FreeByteBuffer (buffer_p);
										}		/* ( if (buffer_p) */

								}
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


static Object *GetTableEditorObject (Object *parent_p)
{
	Object *num_rows_p = NULL;
	Object *num_cols_p = NULL;
	Object *ok_p = NULL;
	Object *cancel_p = NULL;

	Object *child_object_p = IMUIMaster -> MUI_NewObject (MUIC_Group,
		MUIA_Group_Horiz, FALSE,

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Group_Columns, 2,

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Number of rows:", TAG_DONE),
			MUIA_Group_Child, num_rows_p = IMUIMaster -> MUI_NewObject (MUIC_Numericbutton,
	      MUIA_Numeric_Min, 2,
	      MUIA_Numeric_Value, 2,
			TAG_DONE),

			MUIA_Group_Child, IMUIMaster -> MUI_MakeObject (MUIO_Label, "Number of columns:", TAG_DONE),
			MUIA_Group_Child, num_columns_p = IMUIMaster -> MUI_NewObject (MUIC_Numericbutton,
	      MUIA_Numeric_Min, 2,
	      MUIA_Numeric_Value, 2,
			TAG_DONE),

		TAG_DONE),

		MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
			MUIA_Group_Horiz, TRUE,

			MUIA_Group_Child, ok_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "Ok"),
			MUIA_Group_Child, cancel_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "Cancel"),

		TAG_DONE),

	TAG_DONE);


	if (child_object_p)
		{
			DB (KPRINTF ("%s %ld - TableEditor_New: Adding child obj\n", __FILE__, __LINE__));

			IIntuition -> IDoMethod (parent_p, OM_ADDMEMBER, child_object_p);

			IIntuition -> SetAttrs (num_rows_p, MUIA_ShortHelp, "Path to the image.", TAG_DONE);
			IIntuition -> SetAttrs (num_cols_p, MUIA_ShortHelp, "The alternative text for the image", TAG_DONE);

			IIntuition -> IDoMethod (num_rows_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, TEA_Path, MUIV_TriggerValue);
			IIntuition -> IDoMethod (num_cols_p, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, parent_p, 3, MUIM_Set, TEA_Alt, MUIV_TriggerValue);

			IIntuition -> IDoMethod (ok_p, MUIM_Notify, MUIA_Pressed, FALSE, parent_p, 1, TEM_Insert);

		}


	return child_object_p;
}


static uint32 TableEditor_New (Class *class_p, Object *object_p, Msg msg_p)
{
	Object *parent_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);

	if (parent_p)
		{
			TableEditorData *data_p = INST_DATA (class_p, parent_p);
			Object *child_p;

			data_p -> ted_text_editor_p = NULL;
			data_p -> ted_num_rows = 2;
			data_p -> ted_num_columns = 2;

			child_p = GetTableEditorObject (parent_p);

			if (child_p)
				{
					return (uint32) parent_p;
				}

			IIntuition->ICoerceMethod (class_p, parent_p, OM_DISPOSE);
		}		/* if (parent_p) */

	return 0;
}

static uint32 TableEditor_Dispose (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	TableEditorData *data_p = INST_DATA (class_p, object_p);

	if (data_p -> ted_path_s)
		{
		//	FreeCopiedString (data_p -> ted_path_s);
		}

	if (data_p -> ted_alt_s)
		{
		//	FreeCopiedString (data_p -> ted_alt_s);
		}

	if (data_p -> ted_title_s)
		{
		//	FreeCopiedString (data_p -> ted_title_s);
		}

	return retval;
}


static uint32 TableEditor_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
	TableEditorData *data_p = INST_DATA (class_p, object_p);
	struct opSet *ops_p = (struct opSet *) msg_p;
	struct TagItem *tags_p = ops_p -> ops_AttrList;
	struct TagItem *state_p = tags_p;
	struct TagItem *tag_p;

	DB (KPRINTF ("%s %ld - TableEditor_Set enter\n", __FILE__, __LINE__));

	/* Step through the tag list while there are still items in the list */
 	while ((tag_p = IUtility -> NextTagItem (&state_p)) != NULL)
		{
			/* Cache the data for the current element */
			uint32 tag_data = tag_p -> ti_Data;

			DB (KPRINTF ("%s %ld - TableEditor_Set ti_Tag: %8X ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
					case TEA_Editor:
						DB (KPRINTF ("%s %ld - TableEditor_Set -> ted_text_editor_p to %lu", __FILE__, __LINE__, tag_data));
						data_p -> ted_text_editor_p = (Object *) tag_data;
						break;

					case TEA_Path:
						DB (KPRINTF ("%s %ld - TableEditor_Set -> ted_path_s to %s", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> ted_path_s = (STRPTR) tag_data;
						break;

					case TEA_Alt:
						DB (KPRINTF ("%s %ld - TableEditor_Set -> ted_alt_s to %s", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> ted_alt_s = (STRPTR) tag_data;
						break;

					case TEA_Title:
						DB (KPRINTF ("%s %ld - TableEditor_Set -> ted_title_s to %s", __FILE__, __LINE__, (STRPTR) tag_data));
						data_p -> ted_title_s = (STRPTR) tag_data;
						break;


					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}



static BOOL GenerateEmptyRow (ByteBuffer *buffer_p, const uint32 num_cols)
{
	BOOL success_flag = AppendStringToByteBuffer ("\n|");

	if (success_flag)
		{
			uint32 i = 0 ;
			uint32 j = 0 ;
			const char * const heading_s = "       |";

			while (success_flag && (j < num_cols))
				{
					success_flag = AppendStringToByteBuffer (buffer_p, heading_s);
					 ++ j;
				}

			if (success_flag)
				{
					success_flag = AppendStringToByteBuffer (buffer_p, "\n");
				}
		}

	return success_flag;
}


static BOOL GenerateAlignmentRow (ByteBuffer *buffer_p, const uint32 num_cols, const char *alignments_s)
{
	BOOL success_flag = FALSE;

	if ((alignments_s == NULL) || (strlen (alignments_s) == num_cols))
		{
			success_flag = AppendStringToByteBuffer ("|");

			if (success_flag)
				{
					const char *format_s = alignments_s;
					const char * const left_s = " :---  ";
					const char * const right_s = "  ---: ";
					const char * const centre_s = " :---: ";
					uint32 j = 0 ;

					while (success_flag && (j < num_cols))
						{
							const char *alignment_s = centre_s;

							if (format_s)
								{
									const char c = *alignments_s;

									if (c == 'l')
										{
											alignment_s = left_s;
										}
									else if (c == 'c')
										{
											alignment_s = centre_s;
										}
									else if (c == 'r')
										{
											alignment_s = right_s;
										}
									else
										{

										}

									++ format_s;
								}

							success_flag = AppendStringToByteBuffer (buffer_p, heading_s);

							++ j;
						}

					if (success_flag)
						{
							success_flag = AppendStringToByteBuffer (buffer_p, "\n");
						}
				}

		}		/* if ((alignments_s == NULL) || (strlen (alignments_s) == num_cols)) */

	return success_flag;
}
