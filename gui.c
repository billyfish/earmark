#include <limits.h>
#include <math.h>
#include <stdio.h>


#include <exec/lists.h>

#include <workbench/startup.h>
#include <workbench/workbench.h>

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
	MENU_ID_CONVERT,
};

/***************************************/
/********* STATIC PROTOTYPES ***********/
/***************************************/


static void FreeGUIObjects (APTR app_p);


static APTR CreateGUIObjects (struct MUI_CustomClass *editor_class_p, struct MUI_CustomClass *viewer_class_p);

static void RunMD (APTR app_p);

static uint32 Convert (void);


/***************************************/
/********** STATIC VARIABLES ***********/
/***************************************/

static struct NewMenu s_menus_p [] =
{
	{ NM_TITLE, (STRPTR) "Project", NULL, 0, 0, NULL },

 	{ NM_ITEM, (STRPTR) "About...", (STRPTR) "A", 0, 0, (APTR) MENU_ID_ABOUT },
	{ NM_ITEM,  NM_BARLABEL, NULL, 0, 0, NULL },
	{ NM_ITEM, (STRPTR) "Quit", (STRPTR) "Q",   0, 0, (APTR) MENU_ID_QUIT },

	{ NM_TITLE, (STRPTR) "Edit", NULL, 0, 0, NULL },

	{ NM_ITEM, (STRPTR) "Convert", NULL, 0, 0, (APTR) MENU_ID_CONVERT },

	{ NM_END, NULL, NULL, 0, 0, NULL }
};


static APTR s_viewer_p = NULL;
static APTR s_editor_p = NULL;
static APTR s_window_p = NULL;


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
							RunMD (app_p);
							success_flag = TRUE;

							FreeGUIObjects (app_p);
						}		/* if (CreateGUIObjects (screen_p, app_port_p, hook_p)) */
					else
						{
							printf ("failed to create app\n");
						}

					FreeMarkdownViewerClass (viewer_class_p);
				}		/* if (mol_info_class_p) */
			else
				{
					printf ("failed to init viewer class\n");
				}

			FreeMarkdownEditorClass (editor_class_p);
		}		/* if (mol_viewer_class_p)*/
	else
		{
			printf ("failed to init editor class\n");
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
	APTR text_p = NULL;
	Object *about_box_p = NULL;
	Object *load_button_p = NULL;
	Object *save_button_p = NULL;
	Object *update_button_p = NULL;
	
	static const char * const used_classes [] =
		{
			"HTMLview.mcc",
			"TextEditor.mcc",
			NULL
		};


	app_p = IMUIMaster->MUI_NewObject (MUIC_Application,
		MUIA_Application_Title      , "AmiMarkdown",
		MUIA_Application_Version    , "$VER: AmiMarkdown 0.1 (02.01.21)",
		MUIA_Application_Copyright  , "©2021, Simon Tyrrell",
		MUIA_Application_Author     , "Simon Tyrrell",
		MUIA_Application_Description, "Edit and view Markdown documents.",
		MUIA_Application_Base       , "AMIMD",
		MUIA_Application_UsedClasses, used_classes,

		SubWindow, about_box_p = IMUIMaster->MUI_NewObject (MUIC_Aboutbox,
			MUIA_Aboutbox_Build,   "0.1",
			MUIA_Aboutbox_Credits, "Try clicking the version string above...\n",
			// fallback to external image in case the program icon cannot be obtained
			//MUIA_Aboutbox_LogoFile, "PROGDIR:boing.png",
			// fallback to embedded image in case the external image cannot be loaded
			//MUIA_Aboutbox_LogoData, boing_bz2,
		TAG_DONE),

		SubWindow, s_window_p = IMUIMaster->MUI_NewObject (MUIC_Window,
			MUIA_Window_Title, "AmiMarkdown",
			MUIA_Window_ID   , MAKE_ID('A','M','M','D'),
			MUIA_Window_AppWindow, TRUE,
			MUIA_Window_Menustrip, strip_p = IMUIMaster->MUI_MakeObject (MUIO_MenustripNM, s_menus_p, 0),
 			MUIA_ShortHelp, (uint32) "Hello",
			WindowContents, IMUIMaster->MUI_NewObject (MUIC_Group,
				MUIA_Group_Horiz, FALSE,
				
				MUIA_Group_Child, IMUIMaster->MUI_NewObject (MUIC_Group,
					MUIA_Group_Horiz, TRUE,
					
					Child, load_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Load"),
					Child, save_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Save"),
					Child, update_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Update"),
				TAG_DONE),	

				MUIA_Group_Child, IMUIMaster->MUI_NewObject (MUIC_Group,
					MUIA_Group_Horiz, TRUE,
	
					MUIA_Group_Child, s_editor_p = IIntuition->NewObject (editor_class_p -> mcc_Class, NULL,
						ImageButtonFrame,
						MUIA_FillArea, FALSE,
						MUIA_ShortHelp, (uint32) "Editor",
					TAG_DONE),
	
					MUIA_Group_Child, s_viewer_p = IIntuition->NewObject (viewer_class_p -> mcc_Class, NULL,
						ImageButtonFrame,
						MUIA_FillArea, FALSE,
						MUIA_ShortHelp, (uint32) "Viewer",
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
			IIntuition->IDoMethod (s_window_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
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

			IIntuition -> IDoMethod (about_box_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Self, 3, MUIM_Set, MUIA_Window_Open, FALSE);


			IIntuition -> IDoMethod (load_button_p, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1,MEM_MDEditor_Load);

      IIntuition -> IDoMethod (update_button_p, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1,MEM_MDEditor_Convert);
     
		}		/* if (app_p) */


	return app_p;
}



BOOL LoadFile (STRPTR filename_s)
{
	BOOL success_flag = FALSE;	

	
	return success_flag;
}


BOOL SaveFile (STRPTR filename_s)
{
	BOOL success_flag = FALSE;	

	
	return success_flag;
}


static void FreeGUIObjects (APTR app_p)
{
	IMUIMaster->MUI_DisposeObject (app_p);
}


static void RunMD (APTR app_p)
{
	uint32 sigs;
	APTR window_p = s_window_p;


	if (window_p)
		{
			//SetMouseQueue (window_p, 255);
			IIntuition->SetAttrs (window_p, MUIA_Window_Open, TRUE, TAG_DONE);

			while (IIntuition->IDoMethod (app_p, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
				{
					if (sigs)
						{
							sigs = IExec->Wait (sigs | SIGBREAKF_CTRL_C);

							if (sigs & SIGBREAKF_CTRL_C)
								{
									break;
								}

						}		/* if (sigs) */

				}		/* while (DoMethod (app_p, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit) */

			IIntuition->SetAttrs (window_p, MUIA_Window_Open, FALSE, TAG_DONE);
		}
}




