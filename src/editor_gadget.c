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
#include <string.h>

#include <clib/alib_protos.h>

#include <exec/memory.h>
#include <exec/types.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <mui/TextEditor_mcc.h>
#include <mui/HTMLview_mcc.h>

#include "debugging_utils.h"
//#include "memwatch.h"
#include "editor_gadget.h"

#include "md_to_html.h"

#include "md4c-html.h"

#include "gui.h"
#include "prefs.h"

typedef struct MarkdownEditorData
{
	Object *med_viewer_p;
	MDPrefs *med_prefs_p; 
	STRPTR med_filename_s;
} MarkdownEditorData;



/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/


static uint32 MarkdownEditorDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownEditor_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownEditor_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownEditor_Convert (Class *class_p, Object *editor_p);

static uint32 MarkdownEditor_Load (Class *class_p, Object *editor_p);

static uint32 MarkdownEditor_Save (Class *class_p, Object *editor_p);




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

	return IMUIMaster -> MUI_CreateCustomClass (NULL, MUIC_TextEditor, NULL, sizeof (MarkdownEditorData), MarkdownEditorDispatcher);
}


void FreeMarkdownEditorClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster -> MUI_DeleteCustomClass (mui_class_p);
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
				res = MarkdownEditor_Convert (class_p, object_p);
				break;

			case MEM_MDEditor_Load:
				DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Load\n", __FILE__, __LINE__));
				res = MarkdownEditor_Load (class_p, object_p);
				break;

			case MEM_MDEditor_Save:
				DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Save\n", __FILE__, __LINE__));
				res = MarkdownEditor_Save (class_p, object_p);
				break;

			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}


static uint32 MarkdownEditor_New (Class *class_p, Object *object_p, Msg msg_p)
{
	Object *md_editor_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);


	if (md_editor_p)
		{
			MarkdownEditorData *md_p = INST_DATA (class_p, md_editor_p);

			md_p -> med_filename_s = NULL;
			md_p -> med_viewer_p = NULL;
			md_p -> med_prefs_p = NULL;


			DB (KPRINTF ("%s %ld - MarkdownEditor_New: Adding info obj\n", __FILE__, __LINE__));
		}
	else
		{
			/* invoke OM_DISPOSE on *our* class! */
			IIntuition->ICoerceMethod (class_p, md_editor_p, OM_DISPOSE);
			md_editor_p = NULL;
		}

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

			DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set %lu ti_Data: %lu\n", __FILE__, __LINE__, tag_p -> ti_Tag, tag_data));

			/* Handle each attribute that we understand */
			switch (tag_p -> ti_Tag)
				{
					/* Put a case statement here for each attribute that your
					 * function understands */
					case MEA_Viewer:
						md_p -> med_viewer_p = (Object *) tag_data;
						break;
						
					case MEA_Prefs:
						md_p -> med_prefs_p = (MDPrefs *) tag_data;
						break;

					case MEA_Filename:
						{
							CONST_STRPTR filename_s = (CONST_STRPTR) tag_data;
							
							if (filename_s)
								{
									const size_t l = strlen (filename_s);
									
									STRPTR copied_filename_s = (STRPTR) IExec -> AllocVecTags (l + 1, TAG_DONE);
									
									if (copied_filename_s)
										{
											IExec -> CopyMem (filename_s, copied_filename_s, l);
											
											* (copied_filename_s + l) = '\0';
											
											if (md_p -> med_filename_s)
												{
													IExec -> FreeVec (md_p -> med_filename_s);	
												}
												
											md_p -> med_filename_s = copied_filename_s;
										}
								}
							else
								{
									if (md_p -> med_filename_s)
										{											
											IExec -> FreeVec (md_p -> med_filename_s);											
											md_p -> med_filename_s;
										}					
								}	
						}
						break;

					/* Put a case statement here for each attribute that your
					 * function understands */
					case MEA_SurroundSelection:
						{
							CONST_STRPTR surround_s = (CONST_STRPTR) tag_data;	
							STRPTR marked_text_s = (STRPTR) IIntuition -> IDoMethod (object_p, MUIM_TextEditor_ExportBlock, 0 /*, MUIF_TextEditor_ExportBlock_TakeBlock, x1, y1, x2, y2 */);
							
							if (marked_text_s)
								{									
									DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_SurroundSelection marked text = \"%s\"\n", __FILE__, __LINE__, marked_text_s));				
									const size_t surround_length = strlen (surround_s);
									const size_t marked_text_length = strlen (marked_text_s);
									STRPTR replacement_s = (STRPTR) IExec -> AllocVecTags ((surround_length << 1) + marked_text_length + 1, TAG_DONE);
									
									if (replacement_s)
										{
											STRPTR cursor_p = replacement_s;
											
											IExec -> CopyMem (surround_s, cursor_p, surround_length);
											cursor_p += surround_length;
											
											IExec -> CopyMem (marked_text_s, cursor_p, marked_text_length);
											cursor_p += marked_text_length;
											
											IExec -> CopyMem (surround_s, cursor_p, surround_length);
											cursor_p += surround_length;
											
											*cursor_p = '\0';											
											
											IIntuition -> IDoMethod (object_p, MUIM_TextEditor_Replace, replacement_s);
											
											IExec -> FreeVec (replacement_s);	
										} 											
																		
									IExec -> FreeVec (marked_text_s);
								}
							else
								{
									DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_SurroundSelection failed to get marked text\n", __FILE__, __LINE__));											
								}
		

							
							//IIntuition -> IDoMethod (object_p, MUIM_TextEditor_InsertText, surround_s, MUIV_TextEditor_InsertText_Cursor);
						}
						break;

					/* We don't understand this attribute */
					default:
						break;

				}		/* switch (tag_p -> ti_Tag) */

		}		/* while (tag_p = NextTagItem (&state_p)) */


	return retval;
}


static uint32 MarkdownEditor_Convert (Class *class_p, Object *editor_p)
{
	uint32 res = 0;
	MarkdownEditorData *md_p = INST_DATA (class_p, editor_p);
	MDPrefs *prefs_p = md_p -> med_prefs_p;
	STRPTR text_s = (STRPTR) IIntuition -> IDoMethod (editor_p, MUIM_TextEditor_ExportText);

	DB (KPRINTF ("%s %ld - MarkdownEditor_Convert:  prefs at %lu\n", __FILE__, __LINE__, prefs_p));

	if (text_s)
		{
			STRPTR html_s = NULL;
			BOOL res;
			uint16 parser_flags = 0;
			uint16 renderer_flags = MD_HTML_FLAG_DEBUG | MD_HTML_FLAG_SKIP_UTF8_BOM;
			
			if (prefs_p)
				{
					switch (prefs_p -> mdp_dialect)
						{
							case DI_COMMON_MARK:
								parser_flags = MD_DIALECT_COMMONMARK;
								break;
							
							case DI_GITHUB:
								parser_flags = MD_DIALECT_GITHUB;
								break;
							
							default:
								break;
						}
					
					
					if (prefs_p -> mdp_tables)
						{
							parser_flags |= MD_FLAG_TABLES;	
						}
					
					if (prefs_p -> mdp_task_lists)
						{
							parser_flags |= MD_FLAG_TASKLISTS;	
						}
												
					if (prefs_p -> mdp_collapse_whitespace)
						{
							parser_flags |= MD_FLAG_COLLAPSEWHITESPACE;	
						}
					
					if (prefs_p -> mdp_strike_though_spans)
						{
							parser_flags |= MD_FLAG_STRIKETHROUGH;	
						}
								
					if (prefs_p -> mdp_underline_spans)
						{
							parser_flags |= MD_FLAG_UNDERLINE;	
						}
					
					if (prefs_p -> mdp_latex_maths)
						{
							parser_flags |= MD_FLAG_LATEXMATHSPANS;	
						}
								
					if (! (prefs_p -> mdp_html_blocks))
						{
							parser_flags |= MD_FLAG_NOHTMLBLOCKS;	
						}
					
					if (! (prefs_p -> mdp_html_spans))
						{
							parser_flags |= MD_FLAG_NOHTMLSPANS;	
						}
								
					if (! (prefs_p -> mdp_indented_code_blocks))
						{
							parser_flags |= MD_FLAG_NOINDENTEDCODEBLOCKS;	
						}
					
					if (prefs_p -> mdp_translate_html_entities)
						{
							renderer_flags |= MD_HTML_FLAG_VERBATIM_ENTITIES;	
						}
																																						
						
					
				}		/* if (prefs_p) */
				
			res = ConvertText (text_s, &html_s, parser_flags, renderer_flags, TRUE);

			if (res)
				{ 
					DB (KPRINTF ("%s %ld - MarkdownEditor_Convert: filename \"%s\"\n", __FILE__, __LINE__, md_p -> med_filename_s ? md_p -> med_filename_s : "NULL"));
					DB (KPRINTF ("%s %ld - MarkdownEditor_Convert: html:\n%s\n\n", __FILE__, __LINE__, html_s));
					
					if (md_p -> med_filename_s)
						{
 							const char *prefix_s = "URL:file=";
							const char *suffix_s = ".html";
							const size_t prefix_length = strlen (prefix_s);
							const size_t suffix_length = strlen (suffix_s);
							size_t filename_length = strlen (md_p -> med_filename_s);
 							char *html_filename_s = (char *) IExec -> AllocVecTags (prefix_length + suffix_length + filename_length + 1, TAG_DONE);
							
							if (html_filename_s)
								{
									STRPTR cursor_p = html_filename_s;
									
									IExec -> CopyMem (prefix_s, cursor_p, prefix_length);
									cursor_p += prefix_length;
									
									IExec -> CopyMem (md_p -> med_filename_s, cursor_p, filename_length);
									cursor_p += filename_length;
									
									IExec -> CopyMem (suffix_s, cursor_p, suffix_length);
									cursor_p += suffix_length;
									
									*cursor_p = '\0';
									
									if (SaveFile (html_filename_s + prefix_length, html_s))
										{
											/*
											** This example allows an application to determine if the URL: handler is
											** currently mounted and to test whether the launch-handler is working
											** without posting any requesters.
											*/
									    APTR old_win_p = IDOS -> SetProcWindow ((APTR) -1); 
		    							BPTR url_handle_p = IDOS -> Open ("URL:NIL:", MODE_OLDFILE);
		                  
		                  IDOS -> SetProcWindow (old_win_p);
	 
									    if (url_handle_p)  /* Non-zero return values indicates success. */
									    	{
									        IDOS -> Close (url_handle_p);  /* Must still close it */
									        
									        /* We now know that the launch handler is working, so open our file */
									        url_handle_p = IDOS -> Open (html_filename_s, MODE_OLDFILE);
	
		    									if (url_handle_p)  /* Check return value and Close() immediately. */
		    										{	
											        IDOS -> Close (url_handle_p);
									        	}
									        else
									        	{
									        		IDOS -> PutStr ("URL: is not functioning, LaunchHandler is required.\n");
									        	}
									        	
									    	}
									    else
									    	{
													IDOS -> PutStr ("URL: is not functioning, LaunchHandler is required.\n");
									    	}
										}
									
									IExec -> FreeVec (html_filename_s);	
								}		/* if (html_filename_s) */	
						}
					
					if (md_p -> med_viewer_p)
						{
							//IIntuition -> SetAttrs (md_p -> med_viewer_p, MUIA_HTMLview_Contents, html_s, TAG_DONE);
						}

					IExec -> FreeVec (html_s);
				}

			IExec -> FreeVec (text_s);
		}

	return res;
}


static uint32 MarkdownEditor_Load (Class *class_p, Object *editor_p)
{
	uint32 res = 0;
	STRPTR filename_s = RequestFilename (FALSE);

	if (filename_s)
		{
			LoadFile (filename_s);
			IExec -> FreeVec (filename_s);
		}

	return res;
}



static uint32 MarkdownEditor_Save (Class *class_p, Object *editor_p)
{
	uint32 res = 0;
	STRPTR filename_s = RequestFilename (TRUE);

	if (filename_s)
		{
			STRPTR text_s = (STRPTR) IIntuition -> IDoMethod (editor_p, MUIM_TextEditor_ExportText);

			if (text_s)
				{
					SaveFile (filename_s, text_s);
				}
				
			IExec -> FreeVec (filename_s);
		}

	return res;
}

