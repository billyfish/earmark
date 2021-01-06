#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>


#include <exec/lists.h>

#include <workbench/startup.h>
#include <workbench/workbench.h>

#include <libraries/asl.h>

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <mui/TextEditor_mcc.h>
#include <mui/Aboutbox_mcc.h>

#include "gui.h"
#include "editor_gadget.h"
#include "viewer_gadget.h"
#include "settings_gadget.h"

#include "debugging_utils.h"
#include "SDI_hook.h"

//#include "memwatch.h"
#include "prefs.h"


#define GUI_DEBUG


#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((uint32) (a)<<24 | (uint32) (b)<<16 | (uint32) (c)<<8 | (uint32) (d))
#endif

enum
{
	MENU_ID_QUIT,
	MENU_ID_ABOUT,
	MENU_ID_UPDATE,
	MENU_ID_LOAD,
	MENU_ID_SAVE
};

/***************************************/
/********* STATIC PROTOTYPES ***********/
/***************************************/


static void FreeGUIObjects (APTR app_p);

static APTR CreateGUIObjects (struct MUI_CustomClass *editor_class_p, struct MUI_CustomClass *viewer_class_p, struct MUI_CustomClass *prefs_class_p, MDPrefs *prefs_p);

static void RunMD (APTR app_p);

static uint32 Convert (void);


/***************************************/
/********** STATIC VARIABLES ***********/
/***************************************/

static struct NewMenu s_menus_p [] =
{
	{ NM_TITLE, (STRPTR) "Project", NULL, 0, 0, NULL },

	{ NM_ITEM, (STRPTR) "Load ..", (STRPTR) "O", 0, 0, (APTR) MENU_ID_LOAD},
	{ NM_ITEM, (STRPTR) "Save ..", (STRPTR) "O", 0, 0, (APTR) MENU_ID_SAVE},

	{ NM_ITEM,  NM_BARLABEL, NULL, 0, 0, NULL },

	{ NM_ITEM, (STRPTR) "Update", NULL, 0, 0, (APTR) MENU_ID_UPDATE },

	{ NM_ITEM,  NM_BARLABEL, NULL, 0, 0, NULL },

 	{ NM_ITEM, (STRPTR) "About...", (STRPTR) "A", 0, 0, (APTR) MENU_ID_ABOUT },
	{ NM_ITEM, (STRPTR) "Quit", (STRPTR) "Q",   0, 0, (APTR) MENU_ID_QUIT },

	{ NM_END, NULL, NULL, 0, 0, NULL }
};


static APTR s_viewer_p = NULL;
static APTR s_editor_p = NULL;
static APTR s_settings_p = NULL;
static APTR s_window_p = NULL;

static STRPTR s_file_pattern_s = NULL;

static CONST_STRPTR s_app_name_s = "AmiMarkdown";


/***************************************/
/*********** API FUNCTIONS ************/
/***************************************/

BOOL CreateMUIInterface (MDPrefs *prefs_p)
{
	BOOL success_flag = FALSE;
	struct MUI_CustomClass *editor_class_p = InitMarkdownEditorClass ();

	if (editor_class_p)
		{
			struct MUI_CustomClass *viewer_class_p;
			
			DB (KPRINTF ("%s %ld - Inited Editor\n", __FILE__, __LINE__));
						
			viewer_class_p = InitMarkdownViewerClass ();

			if (viewer_class_p)
				{
					struct MUI_CustomClass *settings_class_p;
					
					DB (KPRINTF ("%s %ld - Inited Editor\n", __FILE__, __LINE__));
								
					settings_class_p = InitMarkdownSettingsClass ();
		
					if (settings_class_p)
						{
							APTR app_p;
		
							DB (KPRINTF ("%s %ld - Inited Settings\n", __FILE__, __LINE__));
												
							app_p = CreateGUIObjects (editor_class_p, viewer_class_p, settings_class_p, prefs_p);
		
							if (app_p)
								{
									CONST CONST_STRPTR md_reg_s = "#?.md";
									const size_t md_reg_length = strlen (md_reg_s);
									const size_t size = (md_reg_length + 1) << 1;
									
									DB (KPRINTF ("%s %ld - Created GUI Objects\n", __FILE__, __LINE__));
					
									s_file_pattern_s = (STRPTR) IExec -> AllocVecTags (size, TAG_DONE);
		
									if (s_file_pattern_s)
										{
											DB (KPRINTF ("%s %ld - Created File Pattern\n", __FILE__, __LINE__));
									
											if (IDOS -> ParsePattern (md_reg_s, s_file_pattern_s, size) >= 0)
												{
													DB (KPRINTF ("%s %ld - Parsed File Pattern\n", __FILE__, __LINE__));									
												
													RunMD (app_p);
													success_flag = TRUE;
		
													/*
													** save the current weights of all Balance objects until the next reboot
													** if the weights are to be saved permanently the MUIV_Application_Save_ENVARC must be used instead
													*/
													IIntuition -> IDoMethod (app_p, MUIM_Application_Save, MUIV_Application_Save_ENV);
		
													IExec -> FreeVec (s_file_pattern_s);
												}
										}
		
									FreeGUIObjects (app_p);
								}		/* if (CreateGUIObjects (screen_p, app_port_p, hook_p)) */
							else
								{
									printf ("Failed to create the user interface\n");
								}		
								
							FreeMarkdownSettingsClass (settings_class_p);					
						}		/* if (settings_class_p) */
					else
						{
							printf ("Failed to set up the settings class\n");
						}
						
					FreeMarkdownViewerClass (viewer_class_p);
				}		/* if (mol_info_class_p) */
			else
				{
					printf ("Failed to set up the viewer, please install HTMLView.mcc.\n");
				}

			FreeMarkdownEditorClass (editor_class_p);
		}		/* if (mol_viewer_class_p)*/
	else
		{
			printf ("Failed to set up the editor, pleae install TextEditor.mcc\n");
		}

	return success_flag;
}

// The following hook is just an example hook where we use the object
// for printing out some minor text. Do you see how easy it is to use hooks and
// how great it is to use SDI_hook.h to automatically keep your sources compatible
// to all common AmigaOS platforms?
HOOKPROTONH(DroppedFile, uint32, APTR object_p, struct AppMessage **msg_pp)
{
	#define FNAME_MAX (2048)
	struct AppMessage *msg_p = *msg_pp;
	struct WBArg *arg_p = arg_p = msg_p -> am_ArgList;
	char filename_s [FNAME_MAX];
	int i;

	for (i = msg_p -> am_NumArgs; i > 0; -- i,++ arg_p)
		{
			/* Obtain the filename and load the file. */

			if (IDOS -> NameFromLock (arg_p -> wa_Lock, filename_s, FNAME_MAX))
				{
					if (IDOS -> AddPart (filename_s, arg_p -> wa_Name, FNAME_MAX))
						{
							LoadFile (filename_s);
						}
				}
		}

	return 0;
}
MakeStaticHook(DroppedFileHook, DroppedFile);



static APTR CreateGUIObjects (struct MUI_CustomClass *editor_class_p, struct MUI_CustomClass *viewer_class_p, struct MUI_CustomClass *settings_class_p, MDPrefs *prefs_p)
{
	APTR app_p = NULL;
	APTR strip_p = NULL;
	Object *about_box_p = NULL;
	Object *load_button_p = NULL;
	Object *save_button_p = NULL;
	Object *update_button_p = NULL;
	Object *editor_scrollbar_p = NULL;

		
	static const char * const used_classes [] =
		{
			"HTMLview.mcc",
			"TextEditor.mcc",
			NULL
		};

	static const char *pages_ss []   = { "Editor","Settings", NULL };
	static const char *dialects_ss []   = { "CommonMark","Github", NULL };
	
	
	DB (KPRINTF ("%s %ld - CreateGUIObjects starting\n", __FILE__, __LINE__));
	  		
	app_p = IMUIMaster -> MUI_NewObject (MUIC_Application,
		MUIA_Application_Title      , s_app_name_s,
		MUIA_Application_Version    , "$VER: AmiMarkdown 0.1",
		MUIA_Application_Copyright  , "(c) 2021, Simon Tyrrell",
		MUIA_Application_Author     , "Simon Tyrrell",
		MUIA_Application_Description, "Edit and view Markdown documents.",
		MUIA_Application_Base       , "AMIMD",
		MUIA_Application_UsedClasses, used_classes,

		SubWindow, about_box_p = IMUIMaster -> MUI_NewObject (MUIC_Aboutbox,
			MUIA_Aboutbox_Build,  __DATE__,
			MUIA_Aboutbox_Credits, "This uses sections of code from md4c at http://github.com/mity/md4c by Martin Mit�s.\n\n Click the version string above to get the build date...\n",
			// fallback to external image in case the program icon cannot be obtained
			//MUIA_Aboutbox_LogoFile, "PROGDIR:boing.png",
			// fallback to embedded image in case the external image cannot be loaded
			//MUIA_Aboutbox_LogoData, boing_bz2,
		TAG_DONE),

		SubWindow, s_window_p = IMUIMaster -> MUI_NewObject (MUIC_Window,
			MUIA_Window_Title, "AmiMarkdown",
			MUIA_Window_ID, MAKE_ID('A','M','M','D'),
			MUIA_Window_AppWindow, TRUE,
			MUIA_Window_Menustrip, strip_p = IMUIMaster -> MUI_MakeObject (MUIO_MenustripNM, s_menus_p, 0),
 			MUIA_ShortHelp, (uint32) "A Markdown editor and viewer",
 		
 			
			WindowContents, IMUIMaster -> MUI_NewObject (MUIC_Group,
				MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Register,
					MUIA_Register_Titles, pages_ss,
					MUIA_Register_Frame, TRUE,
					
					/* main tools */
					MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
						MUIA_Group_Horiz, FALSE,
		
						/* tool bar */
						MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
							MUIA_Group_Horiz, TRUE,
		
							MUIA_Group_Child, load_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Load"),
							MUIA_Group_Child, save_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Save"),
							MUIA_Group_Child, update_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Update"),
						TAG_DONE),
		
						/* Editor and viewer */
						MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
							MUIA_Group_Horiz, TRUE,

							
							MUIA_Group_Child, s_editor_p = IIntuition -> NewObject (editor_class_p -> mcc_Class, NULL,
								ImageButtonFrame,
								MUIA_FillArea, FALSE,
								MUIA_ShortHelp, (uint32) "The Markdown source code",
							TAG_DONE),
		
							MUIA_Group_Child, editor_scrollbar_p = IMUIMaster -> MUI_NewObject (MUIC_Scrollbar,
							TAG_DONE),
								
							MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Balance, 
								MUIA_CycleChain, 1, 
								MUIA_ObjectID, MAKE_ID('B', 'A', 'L', 1), 
							TAG_DONE),
								
							MUIA_Group_Child, s_viewer_p = IIntuition -> NewObject (viewer_class_p -> mcc_Class, NULL,
								ImageButtonFrame,
								MUIA_FillArea, FALSE,
								MUIA_ShortHelp, (uint32) "The generated HTML",
							TAG_DONE),
						TAG_DONE),
		
					TAG_DONE),		/* End main tools */										

								
					/* settings */
					MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
						MUIA_Group_Horiz, FALSE,
						
						MUIA_Group_Child, s_settings_p = IIntuition -> NewObject (settings_class_p -> mcc_Class, NULL,
							ImageButtonFrame,
							MUIA_FillArea, FALSE,
							MUIA_ShortHelp, (uint32) "Markdown conversion settings",
						TAG_DONE),
						
					TAG_DONE),					
					
					/*
					MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
						MUIA_Group_Horiz, TRUE,
							
						MUIA_Group_Child, renderer_p = IMUIMaster -> MUI_NewObject (MUIC_Radio,
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
						
					TAG_DONE), */		/* end settings group */
						
				TAG_DONE),		/* End Register Group */

			TAG_DONE),		/* End WindowContents */

		TAG_DONE),		/* End Window */
	
	TAG_DONE);


	if (app_p)
		{
			Object *menu_item_p;

  		DB (KPRINTF ("%s %ld - Application created\n", __FILE__, __LINE__));

			/*
			** Call the AppMsgHook when an icon is dropped on the application
			*/
			IIntuition -> IDoMethod (s_window_p, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
				s_window_p, 3, MUIM_CallHook, &DroppedFileHook, MUIV_TriggerValue);


			/* set the window close gadget to work */
			IIntuition -> IDoMethod (s_window_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
				app_p, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


			/* QUIT */
			menu_item_p = (Object *) IIntuition -> IDoMethod (strip_p, MUIM_FindUData, MENU_ID_QUIT);
			IIntuition -> IDoMethod (menu_item_p, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
				app_p, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


			/* ABOUT */
			menu_item_p = (Object *) IIntuition -> IDoMethod (strip_p, MUIM_FindUData, MENU_ID_ABOUT);
			IIntuition -> IDoMethod (menu_item_p, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
				about_box_p, 3, MUIM_Set, MUIA_Window_Open, TRUE);


			/* CONVERT */
			menu_item_p = (Object *) IIntuition -> IDoMethod (strip_p, MUIM_FindUData, MENU_ID_UPDATE);
			IIntuition -> IDoMethod (menu_item_p, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
				s_editor_p, 1, MEM_MDEditor_Convert);

			IIntuition -> SetAttrs (s_editor_p, MEA_Viewer, s_viewer_p);

			IIntuition -> SetAttrs (s_editor_p, MUIA_TextEditor_Slider, editor_scrollbar_p);

			IIntuition -> IDoMethod (about_box_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Self, 3, MUIM_Set, MUIA_Window_Open, FALSE);

			IIntuition -> SetAttrs (load_button_p, MUIA_ShortHelp, "Load a Markdown file");
			IIntuition -> IDoMethod (load_button_p, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Load);

			IIntuition -> SetAttrs (save_button_p, MUIA_ShortHelp, "Save the editor content to a Markdown file");
			IIntuition -> IDoMethod (save_button_p, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Save);

			IIntuition -> SetAttrs (update_button_p, MUIA_ShortHelp, "Update the generated HTML for the Markdown");
      IIntuition -> IDoMethod (update_button_p, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Convert);
					
		}		/* if (app_p) */


	DB (KPRINTF ("%s %ld - CreateGUIObjects returning %lu\n created\n", __FILE__, __LINE__, app_p));

	return app_p;
}



BOOL LoadFile (STRPTR filename_s)
{
	BOOL success_flag = FALSE;
	BPTR fh_p = IDOS -> FOpen (filename_s, MODE_OLDFILE, 0);

	if (fh_p)
		{
			int64 size = IDOS -> GetFileSize (fh_p);

			if (size != -1LL)
				{
					STRPTR content_s = (STRPTR) IExec -> AllocVecTags (size + 1, TAG_DONE);

					if (content_s)
						{
							if (IDOS -> FRead (fh_p, content_s, size, 1) == 1)
								{
									const size_t filename_length = strlen (filename_s);
									const size_t app_name_length = strlen (s_app_name_s);
									CONST_STRPTR join_s = " - ";
									const size_t join_length = strlen (join_s);
									STRPTR title_s = NULL;
									
									
									* (content_s + size) = '\0';
										
									IIntuition -> IDoMethod (s_editor_p, MUIM_TextEditor_ClearText);
									IIntuition -> SetAttrs (s_editor_p, MUIA_TextEditor_Contents, content_s, TAG_DONE);
													
									title_s = IExec -> AllocVecTags (filename_length + app_name_length + join_length + 1, TAG_DONE);
									
									if (title_s)
										{
											STRPTR temp_p = title_s;
											
											IExec -> CopyMem (s_app_name_s, temp_p, app_name_length);
											temp_p += app_name_length;
									
											IExec -> CopyMem (join_s, temp_p, join_length);
											temp_p += join_length;
											
											IExec -> CopyMem (filename_s, temp_p, filename_length);
											temp_p += filename_length + 1;
											
											*temp_p = '\0';
											
											IIntuition -> SetAttrs (s_window_p, MUIA_Window_Title, title_s, TAG_DONE);		
										}
																		
									success_flag = TRUE;
								}
						}
				}

			IDOS -> FClose (fh_p);
		}


	return success_flag;
}


BOOL SaveFile (STRPTR filename_s)
{
	BOOL success_flag = FALSE;

	BPTR fh_p = IDOS -> FOpen (filename_s, MODE_NEWFILE, 0);

	if (fh_p)
		{
			STRPTR text_s = (STRPTR) IIntuition -> IDoMethod (s_editor_p, MUIM_TextEditor_ExportText);

			if (text_s)
				{
					const uint32 size = strlen (text_s);

					if (IDOS -> FWrite (fh_p, text_s, size, 1) == 1)
						{
							success_flag = TRUE;
						}
				}

			IDOS -> FClose (fh_p);
		}

	return success_flag;
}


static void FreeGUIObjects (APTR app_p)
{
	IMUIMaster -> MUI_DisposeObject (app_p);
}


static void RunMD (APTR app_p)
{
	uint32 sigs;
	APTR window_p = s_window_p;

	/*
	** restore the previously set weights of all Balance objects
	*/
	IIntuition -> IDoMethod (app_p, MUIM_Application_Load, MUIV_Application_Load_ENV);


	DB (KPRINTF ("%s %ld - Loaded ENV\n", __FILE__, __LINE__));

	if (window_p)
		{			
			IIntuition -> SetAttrs (window_p, MUIA_Window_Open, TRUE, TAG_DONE);

			DB (KPRINTF ("%s %ld - Opened Window\n", __FILE__, __LINE__));
									
			while (IIntuition -> IDoMethod (app_p, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
				{
					if (sigs)
						{
							sigs = IExec -> Wait (sigs | SIGBREAKF_CTRL_C);

							if (sigs & SIGBREAKF_CTRL_C)
								{
									break;
								}

						}		/* if (sigs) */

				}		/* while (DoMethod (app_p, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit) */

			IIntuition -> SetAttrs (window_p, MUIA_Window_Open, FALSE, TAG_DONE);
		}
}


STRPTR RequestFilename (const BOOL save_flag)
{
	STRPTR filename_s = NULL;
	struct FileRequester *req_p = (struct FileRequester *) IAsl -> AllocAslRequest (ASL_FileRequest, NULL);

	if (req_p)
		{
			CONST_STRPTR title_s = "Load Markdown File";
			struct Window *window_p = NULL;
			

		//	printf ("req win %lu\n", s_window_p);			
			
			if (s_window_p)
				{
			 		if (IIntuition -> GetAttr (MUIA_Window_Window, s_window_p, (uint32 *) &window_p) == 0)
			 			{
			 				printf ("Failed to get window\n");	
			 			}
				}
				
			if (window_p)
				{
								if (save_flag)
				{
					title_s = "Save Markdown File";
				}

			if (IAsl -> AslRequestTags (req_p,
				ASLFR_InitialDrawer, "RAM:",
				ASLFR_RejectIcons, TRUE,
				ASLFR_Window, window_p,
				ASLFR_TitleText, title_s,
				ASLFR_DoSaveMode, save_flag,
				ASLFR_DoPatterns, TRUE,
				ASLFR_InitialPattern, s_file_pattern_s,
				TAG_END))
					{;
						#define FNAME_MAX (2048)
						char buffer_s [FNAME_MAX];

						if (IUtility -> Strlcpy (buffer_s, req_p -> fr_Drawer, FNAME_MAX) < FNAME_MAX)
							{
								if (IDOS -> AddPart (buffer_s, req_p -> fr_File, FNAME_MAX))
									{
										const size_t l = strlen (buffer_s) + 1;
										
										//printf ("asl: %s %s\n", req_p -> fr_Drawer, req_p -> fr_File);
										
										filename_s = (STRPTR) IExec -> AllocVecTags (l, TAG_DONE);

										if (filename_s)
											{
												if (IUtility -> Strlcpy (filename_s, buffer_s, l) > l)
													{
														IExec -> FreeVec (filename_s);
														filename_s = NULL;
													}

											}
									}
							}
					}
				}
			else
				{
					printf ("no window\n");
					}
			


			IAsl -> FreeAslRequest (req_p);
		}


	//printf ("filename: %s\n", filename_s ? filename_s : "NULL");
	return filename_s;
}

