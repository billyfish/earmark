#include <limits.h>
#include <math.h>
#include <stdio.h>


#include <exec/lists.h>

#include <workbench/startup.h>
#include <workbench/workbench.h>

#include <libraries/asl.h>

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <mui/NListview_mcc.h>
#include <mui/Aboutbox_mcc.h>


#include "editor_gadget.h"
#include "viewer_gadget.h"

#include "debugging_utils.h"
#include "SDI_hook.h"

//#include "memwatch.h"

#define GUI_DEBUG


#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((uint32) (a)<<24 | (uint32) (b)<<16 | (uint32) (c)<<8 | (uint32) (d))
#endif

enum
{
	MENU_ID_QUIT,
	MENU_ID_ABOUT,
	MENU_ID_UDATE,
	MENU_ID_LOAD,
	MENU_ID_SAVE
};

/***************************************/
/********* STATIC PROTOTYPES ***********/
/***************************************/


static void FreeGUIObjects (APTR app_p);


static APTR CreateGUIObjects (struct MUI_CustomClass *editor_class_p, struct MUI_CustomClass *viewer_class_p);

static void RunMD (APTR app_p);

static uint32 Convert (void);

static uint32 AslHook (int32 type, APTR obj, struct FileRequester *req_p);


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
static APTR s_window_p = NULL;

STRPTR s_file_pattern_s = NULL;

/***************************************/
/*********** API FUNCTIONS ************/
/***************************************/

BOOL CreateMUIInterface (void)
{
	BOOL success_flag = FALSE;
	struct MUI_CustomClass *editor_class_p = InitMarkdownEditorClass ();

	if (editor_class_p)
		{
			struct MUI_CustomClass *viewer_class_p = InitMarkdownViewerClass ();

			if (viewer_class_p)
				{
					APTR app_p = CreateGUIObjects (editor_class_p, viewer_class_p);

					if (app_p)
						{
							CONST STRPTR md_reg_s = "(#?.md)";
							const size_t md_reg_length = strlen (md_reg_s);

							s_file_pattern_s = (STRPTR) IExecAllocVecTags ((md_reg_length + 1) << 1, TAG_DONE);

							if (s_file_pattern_s)
								{
									RunMD (app_p);
									success_flag = TRUE;

									IExec -> FreeVec (s_file_pattern_s);
								}

							FreeGUIObjects (app_p);
						}		/* if (CreateGUIObjects (screen_p, app_port_p, hook_p)) */
					else
						{
							printf ("Failed to create the user interface\n");
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
							DB (KPRINTF ("%s %ld - dropped %s\n", __FILE__, __LINE__, filename_s));
						}
				}
		}

	return 0;
}
MakeStaticHook(DroppedFileHook, DroppedFile);



static APTR CreateGUIObjects (struct MUI_CustomClass *editor_class_p, struct MUI_CustomClass *viewer_class_p)
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

	app_p = IMUIMaster -> MUI_NewObject (MUIC_Application,
		MUIA_Application_Title      , "AmiMarkdown",
		MUIA_Application_Version    , "$VER: AmiMarkdown 0.1 (04.01.21)",
		MUIA_Application_Copyright  , "�2021, Simon Tyrrell",
		MUIA_Application_Author     , "Simon Tyrrell",
		MUIA_Application_Description, "Edit and view Markdown documents.",
		MUIA_Application_Base       , "AMIMD",
		MUIA_Application_UsedClasses, used_classes,

		SubWindow, about_box_p = IMUIMaster -> MUI_NewObject (MUIC_Aboutbox,
			MUIA_Aboutbox_Build,   "0.1",
			MUIA_Aboutbox_Credits, "Try clicking the version string above...\n",
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
				MUIA_Group_Horiz, FALSE,

				MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
					MUIA_Group_Horiz, TRUE,

					Child, load_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Load"),
					Child, save_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Save"),
					Child, update_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Update"),
				TAG_DONE),

				MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
					MUIA_Group_Horiz, TRUE,

					MUIA_Group_Child, s_editor_p = IIntuition -> NewObject (editor_class_p -> mcc_Class, NULL,
						ImageButtonFrame,
						MUIA_FillArea, FALSE,
						MUIA_ShortHelp, (uint32) "The Markdown source code",
					TAG_DONE),

					MUIA_Group_Child, editor_scrollbar_p = IMUIMaster -> MUI_NewObject (MUIC_Scrollbar,
					TAG_DONE),

					MUIA_Group_Child, s_viewer_p = IIntuition -> NewObject (viewer_class_p -> mcc_Class, NULL,
						ImageButtonFrame,
						MUIA_FillArea, FALSE,
						MUIA_ShortHelp, (uint32) "The generated HTML",
					TAG_DONE),
				TAG_DONE),

			TAG_DONE),

		TAG_DONE),
	TAG_DONE);


	if (app_p)
		{
			Object *menu_item_p;

			/*
			** Call the AppMsgHook when an icon is dropped on the molecule viewer
			*/
			IIntuition -> IDoMethod (s_window_p, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
				s_window_p, 3, MUIM_CallHook, &DroppedFileHook, MUIV_TriggerValue);


			/* set the window close gadget to work */
			IIntuition -> IDoMethod (s_window_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
				app_p, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


			/*
			** Call the AppMsgHook when an icon is dropped on the molecule viewer
			*/
			IIntuition -> IDoMethod (s_window_p, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
				s_window_p, 3, MUIM_CallHook, &DroppedFileHook, MUIV_TriggerValue);


			/* QUIT */
			menu_item_p = (Object *) IIntuition -> IDoMethod (strip_p, MUIM_FindUData, MENU_ID_QUIT);
			IIntuition -> IDoMethod (menu_item_p, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
				app_p, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);


			/* ABOUT */
			menu_item_p = (Object *) IIntuition -> IDoMethod (strip_p, MUIM_FindUData, MENU_ID_ABOUT);
			IIntuition -> IDoMethod (menu_item_p, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
				about_box_p, 3, MUIM_Set, MUIA_Window_Open, TRUE);


			/* CONVERT */
			menu_item_p = (Object *) IIntuition -> IDoMethod (strip_p, MUIM_FindUData, MENU_ID_CONVERT);
			IIntuition -> IDoMethod (menu_item_p, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
				s_editor_p, 1, MEM_MDEditor_Convert);

			IIntuition -> SetAttrs (s_editor_p, MEA_Viewer, s_viewer_p, TAG_DONE);

			IIntuition -> SetAttrs (s_editor_p, MUIA_TextEditor_Slider, editor_scrollbar_p, TAG_DONE);

			IIntuition -> IDoMethod (about_box_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Self, 3, MUIM_Set, MUIA_Window_Open, FALSE);

			IIntuition -> IDoMethod (load_button_p, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Load);

      IIntuition -> IDoMethod (update_button_p, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Convert);

		}		/* if (app_p) */


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
									* (content_s + size) = '\0';

									IIntuition -> DoMethod (s_editor_p, MUIM_TextEditor_ClearText);
									IIntuition -> SetAttrs (s_editor_p, MUIA_TextEditor_Contents, content_s, TAG_DONE);
									IIntuition -> SetAttrs (s_window_p, MUIA_Window_Title, filename_s, TAG_DONE);

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
			STRPTR text_s = (STRPTR) IIntuition -> IDoMethod (editor_p, MUIM_TextEditor_ExportText);

			if (text_s)
				{
					const uint32 size = strlen (text_s);

					if (IDOS -> FWrite (fh_p, content_s, size, 1) == 1)
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


	if (window_p)
		{
			//SetMouseQueue (window_p, 255);
			IIntuition -> SetAttrs (window_p, MUIA_Window_Open, TRUE, TAG_DONE);

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
	struct FileRequester *req_p = IAsl -> AllocFileRequest ();

	if (req_p)
		{
			CONST_STRPTR button_label_s = "Load";
			UINT32 flags = FILF_DOWILDFUNC | FILF_DOMSGFUNC;

			if (save_flag)
				{
					flags |= FILF_SAVE;
					button_label_s = "Save";
				}

			if (IAsl -> AslRequestTags (req_p,
				ASL_Dir, "SYS:Utilities",
				ASL_Window, window,
				ASL_TopEdge, 0,
				ASL_Height, 200,
				ASL_Hail, title_s,
				ASL_FuncFlags, flags,
				ASL_HookFunc, AslHook,
				ASL_OKText, button_label_s,
				TAG_END))
					{
						IDOS_>Printf("PATH=%s FILE=%s\n", req_p -> rf_Dir, req_p -> rf_File);
						IDOS->Printf("To combine the path and filename, copy the path\n");
						IDOS->Printf("to a buffer, add the filename with Dos AddPart().\n");
						#define FNAME_MAX (2048)
						char buffer_s [FNAME_MAX];

						if (IUtility -> Strlcpy (buffer_s, req_p -> rf_Dir, FNAME_MAX) < FNAME_MAX)
							{
								if (IDOS -> AddPart (buffer_s, req_p -> rf_File, FNAME_MAX))
									{
										const size_t l = strlen (buffer_s);

										filename_s = (STRPTR) IExec -> AllocVecTags (l + 1, TAG_DONE);

										if (filename_s)
											{
												if (IUtility -> Strlcpy (filename_s, buffer_s, l + 1) > l)
													{
														IExec -> FreeVec (filename_s);
														filename_s = NULL;
													}

											}
									}
							}
					}

			IAsl -> FreeFileRequest (req_p);
		}

	return filename_s;
}



static uint32 AslHook (int32 type, APTR obj, struct FileRequester *req_p)
{
	uint32 ret = 0;

  switch (type)
  	{
      case FILF_DOMSGFUNC:
      	/* We got a message meant for the window */
        IDOS->Printf("You activated the window\n");
        ret = (uint32) obj;
        break;

			case FILF_DOWILDFUNC:
				{
					/* We got an AnchorPath structure, should
	      	** the requester display this file? */

	          /* MatchPattern() is a dos.library function that
	          ** compares a matching pattern (parsed by the
	          ** ParsePattern() DOS function) to a string and
	          ** returns true if they match. */
	          BOOL b = IDOS -> MatchPattern (s_file_pattern_s, ((struct AnchorPath *) obj) -> ap_Info.fib_FileName);

	          /* we have to negate MatchPattern()'s return value
	          ** because the file requester expects a zero for
	          ** a match not a TRUE value */
	          ret = (uint32) !b;
	          break;
				}
    }

	return ret;
}
