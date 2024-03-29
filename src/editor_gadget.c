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
#include "info_gadget.h"

#include "md_to_html.h"

#include "md4c-html.h"

#include "gui.h"
#include "prefs.h"

#include "string_utils.h"

typedef struct MarkdownEditorData
{
	Object *med_viewer_p;
	Object *med_info_p;
	MDPrefs *med_prefs_p; 
	STRPTR med_filename_s;
	uint32 med_preview;
	BOOL med_updating_flag;
} MarkdownEditorData;



/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/


static uint32 MarkdownEditorDispatcher (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownEditor_New (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownEditor_Set (Class *class_p, Object *object_p, Msg msg_p);

static uint32 MarkdownEditor_Convert (Class *class_p, Object *editor_p);

static uint32 MarkdownEditor_Load (Class *class_p, Object *editor_p);

static uint32 MarkdownEditor_Save (Class *class_p, Object *editor_p, STRPTR filename_s);




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
	uint32 res = 0;

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
				{
					MarkdownEditorData *md_p = INST_DATA (class_p, object_p);		

					DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Convert updating = %s\n", __FILE__, __LINE__, md_p -> med_updating_flag ? "true" : "false"));

					if (md_p -> med_updating_flag == FALSE)
						{									
							DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Convert 1 - file \"%s\"\n", __FILE__, __LINE__, md_p -> med_filename_s ? md_p -> med_filename_s: "NULL"));
							
							md_p -> med_updating_flag = TRUE;
							
							/* Do we have a valid filename? */
							if (! (md_p -> med_filename_s))
								{
									MarkdownEditor_Save (class_p, object_p, md_p -> med_filename_s);
								}
							
							DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Convert 2 -  file \"%s\"\n", __FILE__, __LINE__, md_p -> med_filename_s ? md_p -> med_filename_s : "NULL"));					
							
							if (md_p -> med_filename_s)
								{
									res = MarkdownEditor_Convert (class_p, object_p);
								}
							else
								{
									ShowWarning ("Conversion problem", "You need to save the Markdown file before you can convert it", "_Ok");
								}
								
							/*
								If we're using the live preview, let the text editor know that we've reacted
								to this change and want to hear about any others.
							*/
							if (md_p -> med_preview == MEV_MDEditor_Preview_Internal_Live)
								{
									IIntuition -> SetAttrs (object_p, MUIA_TextEditor_ContentsChanged, FALSE, TAG_DONE);		
								}

							md_p -> med_updating_flag = FALSE;
						}
						
					DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: exiting Convert updating = %s\n", __FILE__, __LINE__, md_p -> med_updating_flag ? "true" : "false"));
				}
				break;

			case MEM_MDEditor_Load:
				DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Load\n", __FILE__, __LINE__));
				res = MarkdownEditor_Load (class_p, object_p);
				break;

			case MEM_MDEditor_Save:
				{
					MarkdownEditorData *md_p = INST_DATA (class_p, object_p);
					struct opGet *op_p = (struct opGet *) msg_p;
					uint32 *store_p = op_p -> opg_Storage;
					STRPTR filename_s = NULL;				
				
					switch (op_p -> opg_AttrID)
						{
							case MEV_MDEditor_UseExistingFilename:	
								filename_s = md_p -> med_filename_s;			
								break;	
						}
									
					DB (KPRINTF ("%s %ld - MarkdownEditorDispatcher: Save\n", __FILE__, __LINE__));
					res = MarkdownEditor_Save (class_p, object_p, filename_s);
				}
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
			md_p -> med_info_p = NULL;
			md_p -> med_preview = MEV_MDEditor_Preview_External;
			md_p -> med_updating_flag  = FALSE;
			 
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
						
						if (md_p -> med_viewer_p)
							{
								const BOOL show_flag = (md_p -> med_preview != MEV_MDEditor_Preview_External);	

								IIntuition -> SetAttrs (md_p -> med_viewer_p, MUIA_ShowMe, show_flag, TAG_DONE);
							}
						break;
						
					case MEA_InfoGadget:
						md_p -> med_info_p = (Object *) tag_data;
						break;
						

					case MEA_Prefs:
						md_p -> med_prefs_p = (MDPrefs *) tag_data;
						break;

					case MEA_Filename:
						{
							CONST_STRPTR filename_s = (CONST_STRPTR) tag_data;
							
							if (filename_s)
								{
									BOOL changed_filename_flag = FALSE;
									
									if (md_p -> med_filename_s)
										{
											/* Has the filename changed? */
											if (strcmp (filename_s, md_p -> med_filename_s) != 0)
												{													
													changed_filename_flag = TRUE;
												}
										}
									else
										{
											/* This appears to be the first save so store the filename */
											changed_filename_flag = TRUE;
										}
										
									if (changed_filename_flag)
										{
											STRPTR new_filename_s = EasyCopyToNewString (filename_s);
											
											if (new_filename_s)
												{
													if (md_p -> med_filename_s)
														{
															FreeCopiedString (md_p -> med_filename_s);	
														}
												
													md_p -> med_filename_s = new_filename_s;												
													UpdateWindowActiveFilename (md_p -> med_filename_s);	
												}
											else
												{
													ShowError ("Application Error", "Failed to set active filename for editor", "_Ok");
												}
											
										}		

								}		/* if (filename_s) */
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


 					case MEA_Previewer:
						{
							DB (KPRINTF ("%s %ld - ti_Tag: MEA_Previewer %lu\n", __FILE__, __LINE__, tag_data));							

							if (md_p -> med_viewer_p)
								{
									switch (tag_data)
										{	
											case MEV_MDEditor_Preview_External:
												{
													IIntuition -> SetAttrs (md_p -> med_viewer_p, MUIA_ShowMe, FALSE, TAG_DONE);

													if (md_p -> med_preview == MEV_MDEditor_Preview_Internal_Live)
														{
															DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_Previewer killing Notify", __FILE__, __LINE__));																														
															IIntuition -> IDoMethod (object_p, MUIM_KillNotifyObj, MUIA_TextEditor_ContentsChanged, object_p);																														
														}														

													md_p -> med_preview = MEV_MDEditor_Preview_External;
												}
												break;

											case MEV_MDEditor_Preview_Internal:
												{
													IIntuition -> SetAttrs (md_p -> med_viewer_p, MUIA_ShowMe, TRUE, TAG_DONE);

													if (md_p -> med_preview == MEV_MDEditor_Preview_Internal_Live)
														{
															DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_Previewer killing Notify", __FILE__, __LINE__));															
															IIntuition -> IDoMethod (object_p, MUIM_KillNotifyObj, MUIA_TextEditor_ContentsChanged, object_p);															
														}		
														
													md_p -> med_preview = tag_data;																							
												}
												break;
												
											case MEV_MDEditor_Preview_Internal_Live:
												{
													IIntuition -> SetAttrs (md_p -> med_viewer_p, MUIA_ShowMe, TRUE, TAG_DONE);

													if (md_p -> med_preview != MEV_MDEditor_Preview_Internal_Live)
														{
															DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_Previewer adding Notify", __FILE__, __LINE__));
															IIntuition -> IDoMethod (object_p, MUIM_Notify, MUIA_TextEditor_ContentsChanged, MUIV_EveryTime, object_p, 1, MEM_MDEditor_Convert);
														}	
														
													md_p -> med_preview = tag_data;
												}
												break;
												
												
											default:
												{
													DB (KPRINTF ("%s %ld - ti_Tag: MEA_Previewer UNKNOWN:  %lu\n", __FILE__, __LINE__, tag_data));							
												}	
												break;	
										}

								}					

						}
						break;


					/* Put a case statement here for each attribute that your
					 * function understands */
					case MEA_SurroundSelection:
						{
							const char *before_s = NULL;
							const char *after_s = NULL;
							STRPTR marked_text_s = NULL;
							BOOL trim_flag = TRUE;
							
							switch (tag_data)
								{
									case MEV_MDEditor_Style_Bold:
										before_s = after_s = "**";
										break;

									case MEV_MDEditor_Style_Italic:
										before_s = after_s = "*";
										break;

									case MEV_MDEditor_Style_Strikethrough:
										before_s = after_s = "~~";
										break;

									case MEV_MDEditor_Style_Code:
										before_s = after_s = "`";
										break;

									case MEV_MDEditor_Style_IndentedCode:
										before_s = "\n```\n";
										after_s = "\n```\n";
										trim_flag = FALSE;
										break;

									default:
										break;
								}

							marked_text_s = (STRPTR) IIntuition -> IDoMethod (object_p, MUIM_TextEditor_ExportBlock, 0 /*, MUIF_TextEditor_ExportBlock_TakeBlock, x1, y1, x2, y2 */);
							
							if (marked_text_s)
								{	
									STRPTR trimmed_s = trim_flag ?  CopyToNewString (marked_text_s, 0, TRUE) : marked_text_s;
								
									if (trimmed_s)
										{
											if (before_s)
												{
													if (after_s)
														{
															STRPTR replacement_s;
															
															DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_SurroundSelection marked text = \"%s\", before_s = \"%s\", after_s = \"%s\"\n",
																__FILE__, __LINE__, marked_text_s, before_s, after_s));
		
															replacement_s = ConcatenateVarargsStrings (before_s, trimmed_s, after_s, NULL);
		
															if (replacement_s)
																{
																	IIntuition -> IDoMethod (object_p, MUIM_TextEditor_Replace, replacement_s);
		
																	IExec -> FreeVec (replacement_s);
																}
		
		
														}
				    							else
														{
															DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_SurroundSelection failed to get after_s", __FILE__, __LINE__));
														}
												}
		    							else
												{
													DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_SurroundSelection failed to get before_s\n", __FILE__, __LINE__));
												}											
											
											if (trimmed_s != marked_text_s)
												{
													FreeCopiedString (trimmed_s);					
												}
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

					case MEA_InsertItem:
						{
							const char *item_s = NULL;

							switch (tag_data)
								{
									case MEV_MDEditor_HorizontalRule:
										item_s = "\n---\n";
										break;

									case MEV_MDEditor_Image:
										{
											DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEV_MDEditor_Image\n", __FILE__, __LINE__));		

										}
										break;
									
									case MEV_MDEditor_Table:
										break;
										
									case MEV_MDEditor_Hyperlink:
										break;

									default:
										break;
								}

							if (item_s)
								{
									IIntuition -> IDoMethod (object_p, MUIM_TextEditor_InsertText, item_s, MUIV_TextEditor_InsertText_Cursor);
								}
						}
						break;


					case MEA_PrefixMarkedLines:
						{
							CONST_STRPTR before_s = NULL;
							CONST_STRPTR initial_before_s = NULL;
							STRPTR marked_text_s = NULL;

							
							switch (tag_data)
								{
									case MEV_MDEditor_BlockQuote:
										initial_before_s = "> ";
										before_s = "\n> ";
										break;

									case MEV_MDEditor_Heading1:
										initial_before_s = "# ";
										before_s = "\n# ";
										break;

									case MEV_MDEditor_Heading2:
										initial_before_s = "## ";
										before_s = "\n## ";
										break;

									case MEV_MDEditor_Heading3:
										initial_before_s = "### ";
										before_s = "\n### ";
										break;

									case MEV_MDEditor_Heading4:
										initial_before_s = "#### ";
										before_s = "\n#### ";
										break;

									case MEV_MDEditor_Heading5:
										initial_before_s = "##### ";
										before_s = "\n##### ";
										break;

									case MEV_MDEditor_Heading6:
										initial_before_s = "###### ";
										before_s = "\n###### ";
										break;

									default:
										break;
								}

							DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_PrefixMarkedLines tag %lu before \"%s\"\n",	__FILE__, __LINE__, tag_data, before_s ? before_s : "NULL"));

							marked_text_s = (STRPTR) IIntuition -> IDoMethod (object_p, MUIM_TextEditor_ExportBlock, 0 /*, MUIF_TextEditor_ExportBlock_TakeBlock, x1, y1, x2, y2 */);
							
							if (marked_text_s && (strlen (marked_text_s) > 0))
								{									
									if (before_s)
										{
											STRPTR replacement_s = NULL;
											BOOL b = FALSE;
											
											DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_PrefixMarkedLines marked text = \"%s\", before_s = \"%s\"\n",
												__FILE__, __LINE__, marked_text_s, before_s));

//											replacement_s = ConcatenateVarargsStrings (before_s, marked_text_s, NULL);

											if (!SearchAndReplaceInString (marked_text_s, &replacement_s, "\n", before_s))
												{
													replacement_s = marked_text_s;
												}

											DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_PrefixMarkedLines tag %lu b %lu replacement_s \"%s\"\n",	__FILE__, __LINE__, tag_data, b, replacement_s ? replacement_s : "NULL"));
												
											if (replacement_s)
												{
													STRPTR full_s = ConcatenateStrings (initial_before_s, replacement_s);
													
													if (full_s)
														{														
															IIntuition -> IDoMethod (object_p, MUIM_TextEditor_Replace, full_s);
														
															FreeCopiedString (full_s);
														}

													IExec -> FreeVec (replacement_s);
												}
										}
    							else
										{
											DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_SurroundSelection failed to get before_s\n", __FILE__, __LINE__));
										}
																		
									IExec -> FreeVec (marked_text_s);
								}
							else
								{
									DB (KPRINTF ("%s %ld - ti_Tag: MarkdownEditor_Set MEA_PrefixMarkedLines no marked text\n", __FILE__, __LINE__));		
									IIntuition -> IDoMethod (object_p, MUIM_TextEditor_InsertText, before_s, MUIV_TextEditor_InsertText_Cursor);									
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
					//DB (KPRINTF ("%s %ld - MarkdownEditor_Convert: html:\n%s\n\n", __FILE__, __LINE__, html_s));
					
					if (md_p -> med_filename_s)
						{
 							const char *prefix_s = "URL:file=";
 							const size_t prefix_length = strlen (prefix_s);
							const char *suffix_s = ".html";
 							STRPTR html_filename_s = NULL;
							const size_t filename_length = strlen (md_p -> med_filename_s);
							const size_t suffix_length = strlen (suffix_s);
							
							/*
							 * if the filename already ends in ".html", don't add it again
							 */ 
							if ((filename_length >= suffix_length) && (strncmp ((md_p -> med_filename_s) + filename_length - suffix_length, suffix_s, suffix_length) == 0))
								{
									html_filename_s = ConcatenateVarargsStrings (prefix_s, md_p -> med_filename_s, NULL);
								}
							else
								{
									html_filename_s = ConcatenateVarargsStrings (prefix_s, md_p -> med_filename_s, suffix_s, NULL);
								}
							
							
							if (html_filename_s)
								{
									DB (KPRINTF ("%s %ld - MarkdownEditor_Convert: md_p -> med_filename_s \"%s\", html_filename_s \"%s\", after prefix \"%s\"\n", 
										__FILE__, __LINE__, md_p -> med_filename_s, html_filename_s, html_filename_s + prefix_length));
									
 									if (SaveFile (html_filename_s + prefix_length, html_s))
										{
											if ((md_p -> med_preview == MEV_MDEditor_Preview_Internal) || (md_p -> med_preview == MEV_MDEditor_Preview_Internal_Live))
												{
													char *url_s = ConcatenateStrings ("file://", html_filename_s + prefix_length); 	/* skip past the "URL:" bit */
													
													if (url_s)
														{
															//IIntuition -> SetAttrs (md_p -> med_viewer_p, MUIA_HTMLview_Contents, html_s, TAG_DONE);
															DB (KPRINTF ("%s %ld - Viewing \"%s\"\n", __FILE__, __LINE__, url_s));
															IIntuition -> IDoMethod (md_p -> med_viewer_p, MUIM_HTMLview_GotoURL, url_s, NULL);
															FreeCopiedString (url_s);
														}
												}
											else
												{
													/*
													** This example allows an application to determine if the URL: handler is
													** currently mounted and to test whether the launch-handler is working
													** without posting any requesters.
													*/
											    APTR old_win_p = IDOS -> SetProcWindow ((APTR) -1); 
				    							BPTR url_handle_p = IDOS -> Open ("URL:NIL:", MODE_OLDFILE);
				                  BOOL success_flag = FALSE;
				                  
				                  IDOS -> SetProcWindow (old_win_p);
			 
											    if (url_handle_p)  /* Non-zero return values indicates success. */
											    	{
											        IDOS -> Close (url_handle_p);  /* Must still close it */
											        
											        /* We now know that the launch handler is working, so open our file */
											        url_handle_p = IDOS -> Open (html_filename_s, MODE_OLDFILE);
			
				    									if (url_handle_p)  /* Check return value and Close() immediately. */
				    										{	
													        IDOS -> Close (url_handle_p);
													        success_flag = TRUE;
																}
														}
														
													if (!success_flag)
														{
															ShowWarning ("View Error", "Failed to open browser to view converted file", "_Ok");
														}												

												}		/* if (use_launch_handler_flag) */
										}
									else
										{
											ShowError ("Conversion Error", "Failed to save converted HTML file", "_Ok");
										}
										
									IExec -> FreeVec (html_filename_s);	
								}		/* if (html_filename_s) */	
						}

					IExec -> FreeVec (html_s);
				}	
			else
				{
					ShowError ("Conversion Error", "Failed to convert Markdown to HTML", "_Ok");
				}	
			
			IExec -> FreeVec (text_s);
		}

	return res;
}


static uint32 MarkdownEditor_Load (Class *class_p, Object *editor_p)
{
	uint32 res = 0;
	CONST CONST_STRPTR pattern_s = GetMarkdownFilePattern ();
	STRPTR filename_s = RequestFilename (FALSE, "Load Markdown file", pattern_s, NULL, NULL);

	if (filename_s)
		{
			DB (KPRINTF ("%s %ld - Trying to load \"%s\"\n", __FILE__, __LINE__, filename_s));

			if (LoadFile (filename_s))
				{					
					MarkdownEditorData *md_p = INST_DATA (class_p, editor_p);																	
				
					if (md_p -> med_info_p)
						{
							IIntuition -> SetAttrs (md_p -> med_info_p, IGA_Changed, FALSE, TAG_DONE);	
						}	
				}
					
			IExec -> FreeVec (filename_s);
		}

	return res;
}



static uint32 MarkdownEditor_Save (Class *class_p, Object *editor_p, STRPTR filename_s)
{
	uint32 res = 0;
	MarkdownEditorData *md_p = INST_DATA (class_p, editor_p);
	CONST CONST_STRPTR pattern_s = GetMarkdownFilePattern ();
	
	if (!filename_s)
		{
			filename_s = RequestFilename (TRUE, "Save Markdown file", pattern_s, NULL, md_p -> med_filename_s);
		}
		
	if (filename_s)
		{
			STRPTR text_s = (STRPTR) IIntuition -> IDoMethod (editor_p, MUIM_TextEditor_ExportText);

			if (text_s)
				{
					if (SaveFile (filename_s, text_s))
						{
							BOOL changed_filename_flag = FALSE;
							
							if (md_p -> med_filename_s)
								{
									/* Has the filename changed? */
									if (strcmp (filename_s, md_p -> med_filename_s) != 0)
										{
											IExec -> FreeVec (md_p -> med_filename_s);
											md_p -> med_filename_s = filename_s;
											
											changed_filename_flag = TRUE;
										}
									else
										{
											IExec -> FreeVec (filename_s);	
										}
								}
							else
								{
									/* This appears to be the first save so store the filename */
									md_p -> med_filename_s = filename_s;
									changed_filename_flag = TRUE;
								}
								
							if (changed_filename_flag)
								{
									UpdateWindowActiveFilename (md_p -> med_filename_s);
								}	
								
																
							if (md_p -> med_info_p)
								{
									IIntuition -> SetAttrs (md_p -> med_info_p, IGA_Changed, FALSE, TAG_DONE);	
								}
						}
				}
			
		}

	return res;
}

