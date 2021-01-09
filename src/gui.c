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
#include <mui/TheBar_mcc.h>

#include "gui.h"
#include "editor_gadget.h"
#include "settings_gadget.h"
#include "image_editor.h"


#include "debugging_utils.h"
#include "SDI_hook.h"

//#include "memwatch.h"
#include "prefs.h"

#include "string_utils.h"

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

static APTR CreateGUIObjects (struct MUI_CustomClass *editor_class_p, struct MUI_CustomClass *prefs_class_p, struct MUI_CustomClass *image_editor_class_p, MDPrefs *prefs_p);

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
			struct MUI_CustomClass *settings_class_p;

			DB (KPRINTF ("%s %ld - Inited Editor\n", __FILE__, __LINE__));

			settings_class_p = InitMarkdownSettingsClass ();

			if (settings_class_p)
				{
					struct MUI_CustomClass *image_editor_class_p;

					DB (KPRINTF ("%s %ld - Inited Settings\n", __FILE__, __LINE__));

					image_editor_class_p = InitImageEditorClass ();

					if (image_editor_class_p)
						{
							APTR app_p;

							DB (KPRINTF ("%s %ld - Inited Image Editor\n", __FILE__, __LINE__));

							app_p = CreateGUIObjects (editor_class_p, settings_class_p, image_editor_class_p, prefs_p);

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
									IDOS -> PutStr ("Failed to create the user interface\n");
								}

							FreeImageEditorClass (image_editor_class_p);
						}
					else
						{
							IDOS -> PutStr ("Failed to set up the image editor class\n");
						}

					FreeMarkdownSettingsClass (settings_class_p);
				}		/* if (settings_class_p) */
			else
				{
					IDOS -> PutStr ("Failed to set up the settings class\n");
				}


			FreeMarkdownEditorClass (editor_class_p);
		}		/* if (mol_viewer_class_p)*/
	else
		{
			IDOS -> PutStr ("Failed to set up the editor, pleae install TextEditor.mcc\n");
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



static APTR CreateGUIObjects (struct MUI_CustomClass *editor_class_p, struct MUI_CustomClass *settings_class_p, struct MUI_CustomClass *image_editor_class_p, MDPrefs *prefs_p)
{
	APTR app_p = NULL;
	APTR strip_p = NULL;
	Object *editor_scrollbar_p = NULL;
	Object *toolbar_p = NULL;
	Object *about_box_p = NULL;
	Object *image_editor_p = NULL;
	Object *image_editor_window_p = NULL;

	static const char * const used_classes [] =
		{
			"TextEditor.mcc",
			"TheBar.mcc",
			NULL
		};

	static const char *pages_ss []   = { "Editor", "Settings", NULL };

	enum
		{
			BID_OPEN,
			BID_SAVE,
			BID_CONVERT,
			BID_UNDO,
			BID_REDO,
			BID_FONT_BOLD,
			BID_FONT_ITALIC,
			BID_FONT_CODE,
			BID_FONT_STRIKETHROUGH,
			BID_INDENTED_CODE,
			BID_HORIZONTAL_RULE,
			BID_HYPERLINK,
			BID_IMAGE,
			BID_FOOTNOTE,
			BID_TABLE,
			BID_NUM_BUTTONS
		}
	ButtonID;

	const char *pics_ss [] =
		{
			"open",
			"save",
			"convert",
			"undo",
			"redo",
			"font_bold",
			"font_italic",
			"font_color",
			"font_cancel",
			"braces",
			"forcenewpage",
			"hyperlink",
			"image",
			"insertfootnote",
			"tableadd",
			NULL
		};

	const char *selected_pics_ss [] =
		{
			"open_s",
			"save_s",
			"convert_s",
			"undo_s",
			"redo_s",
			"font_bold_s",
			"font_italic_s",
			"font_color_s",
			"font_cancel_s",
			"braces_s",
			"forcenewpage_s",
			"hyperlink_s",
			"image_s",
			"insertfootnote_s",
			"tableadd_s",
			NULL
		};

	const char *ghosted_pics_ss [] =
		{
			"open_g",
			"save_g",
			"convert_g",
			"undo_g",
			"redo_g",
			"font_bold_g",
			"font_italic_g",
			"font_color_g",
			"font_cancel_g",
			"braces_g",
			"forcenewpage_g",
			"hyperlink_g",
			"image_g",
			"insertfootnote_g",
			"tableadd_g",
			NULL
		};

	struct MUIS_TheBar_Button buttons [] =
		{
			{ BID_OPEN, BID_OPEN,  "_Load", "Load a Markdown file.", 0, 0, NULL, NULL },
			{ BID_SAVE, BID_SAVE,  "_Save", "Save the file.", 0, 0, NULL, NULL },
			{ BID_CONVERT, BID_CONVERT,  "_Convert", "Convert to HTML and view.", 0, 0, NULL, NULL },
			{ MUIV_TheBar_ButtonSpacer, -1, NULL, NULL, 0, 0, NULL, NULL },
			{ BID_UNDO, BID_UNDO,  "_Undo", "Undo the latest changes", 0, 0, NULL, NULL },
			{ BID_REDO, BID_REDO,  "_Redo", "Redo the latest reverted changes", 0, 0, NULL, NULL },
			{ MUIV_TheBar_BarSpacer, -1, NULL, NULL, 0, 0, NULL, NULL },
			{ BID_FONT_BOLD, BID_FONT_BOLD, "_Bold", "Make the selected text bold.", 0, 0, NULL, NULL },
			{ BID_FONT_ITALIC, BID_FONT_ITALIC, "_Italic", "Make the selected text italic.", 0, 0, NULL, NULL },
			{ BID_FONT_STRIKETHROUGH,  BID_FONT_STRIKETHROUGH, "Stri_kethrough", "Strike through the selected text.", 0, 0, NULL, NULL },
			{ BID_FONT_CODE,  BID_FONT_CODE, "Co_de", "Make the selected text code.", 0, 0, NULL, NULL },
			{ MUIV_TheBar_BarSpacer, -1, NULL, NULL, 0, 0, NULL, NULL },
			{ BID_HORIZONTAL_RULE, BID_HORIZONTAL_RULE, "Horizontal Rule", "Insert a Horizontal Rule.", 0, 0, NULL, NULL },
			{ BID_INDENTED_CODE,  BID_INDENTED_CODE, "Fenced Code", "Make a fenced code block.", 0, 0, NULL, NULL },
			{ MUIV_TheBar_BarSpacer, -1, NULL, NULL, 0, 0, NULL, NULL },
			{ BID_HYPERLINK, BID_HYPERLINK, "_Hyperlink", "Insert a hyperlink.", 0, 0, NULL, NULL },
			{ BID_IMAGE, BID_IMAGE, "_Image", "Insert an image.", 0, 0, NULL, NULL },
//			{ BID_FOOTNOTE, BID_FOOTNOTE, "_Footnote", "Insert a footnote.", 0, 0, NULL, NULL },
			{ BID_TABLE, BID_TABLE, "_Table", "Insert a table.", 0, 0, NULL, NULL },
			{ MUIV_TheBar_End, -1, NULL, NULL, 0, 0, NULL, NULL }
		};


	DB (KPRINTF ("%s %ld - CreateGUIObjects starting\n", __FILE__, __LINE__));

	app_p = IMUIMaster -> MUI_NewObject (MUIC_Application,
		MUIA_Application_Title      , s_app_name_s,
		MUIA_Application_Version    , "$VER: AmiMarkdown 0.5",
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


		SubWindow, image_editor_window_p = IMUIMaster -> MUI_NewObject (MUIC_Window,
			MUIA_Window_Title, "Insert Image",
 			MUIA_ShortHelp, (uint32) "Edit Image Details",

			WindowContents, IMUIMaster -> MUI_NewObject (MUIC_Group,
				MUIA_Group_Horiz, FALSE,

				MUIA_Group_Child, image_editor_p = IIntuition -> NewObject (image_editor_class_p -> mcc_Class, NULL,
					ImageButtonFrame,
					MUIA_FillArea, FALSE,
					MUIA_ShortHelp, (uint32) "Image Editor",
				TAG_DONE),
			TAG_DONE),

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
						/*
						MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
							MUIA_Group_Horiz, TRUE,

							MUIA_Group_Child, load_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Load"),
							MUIA_Group_Child, save_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Save"),
							MUIA_Group_Child, update_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Update"),
							MUIA_Group_Child, bold_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Bold"),
							MUIA_Group_Child, italic_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Italic"),
							MUIA_Group_Child, code_button_p = IMUIMaster -> MUI_MakeObject (MUIO_Button, "_Code"),
						TAG_DONE),
						*/

						/* TheBar tool bar */

						MUIA_Group_Child, toolbar_p = IMUIMaster -> MUI_NewObject (MUIC_TheBar,
              MUIA_Group_Horiz,             TRUE,
              MUIA_TheBar_IgnoreAppearance, TRUE,
              MUIA_TheBar_ViewMode,         MUIV_TheBar_ViewMode_Gfx,
              MUIA_TheBar_Buttons,          buttons,
              MUIA_TheBar_PicsDrawer,       "tbimages:",
              MUIA_TheBar_Pics,             pics_ss,
              MUIA_TheBar_SelPics,          selected_pics_ss,
              MUIA_TheBar_DisPics,          ghosted_pics_ss,
						TAG_DONE),


						/* Editor  */
						MUIA_Group_Child, IMUIMaster -> MUI_NewObject (MUIC_Group,
							MUIA_Group_Horiz, TRUE,

							MUIA_Group_Child, s_editor_p = IIntuition -> NewObject (editor_class_p -> mcc_Class, NULL,
								ImageButtonFrame,
								MUIA_FillArea, FALSE,
								MUIA_ShortHelp, (uint32) "The Markdown source code",
							TAG_DONE),

							MUIA_Group_Child, editor_scrollbar_p = IMUIMaster -> MUI_NewObject (MUIC_Scrollbar,
							TAG_DONE),

						TAG_DONE),		/* End Editor */

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

				TAG_DONE),		/* End Register Group */

			TAG_DONE),		/* End WindowContents */

		TAG_DONE),		/* End Window */

	TAG_DONE);


	if (app_p)
		{
			Object *menu_item_p;
			MDPrefs *prefs_p = NULL;

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

			IIntuition -> GetAttrs (s_settings_p, MSA_Prefs, &prefs_p);
			IIntuition -> SetAttrs (s_editor_p, MEA_Prefs, prefs_p, TAG_DONE);

			IIntuition -> SetAttrs (s_editor_p, MUIA_TextEditor_Slider, editor_scrollbar_p, TAG_DONE);

			IIntuition -> IDoMethod (about_box_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Self, 3, MUIM_Set, MUIA_Window_Open, FALSE);

			if (image_editor_window_p)
				{
					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_IMAGE, MUIM_Notify, MUIA_Pressed, FALSE, image_editor_window_p, 3, MUIM_Set, MUIA_Window_Open, TRUE);
					IIntuition -> IDoMethod (image_editor_window_p, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Self, 3, MUIM_Set, MUIA_Window_Open, FALSE);

					IIntuition -> SetAttrs (image_editor_p, IEA_Editor, s_editor_p, TAG_DONE);
				}

			if (toolbar_p)
				{
		      IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_OPEN, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Load);
		      IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_SAVE, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Save);
		      IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_CONVERT, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 1, MEM_MDEditor_Convert);

		      IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_UNDO, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 2, MUIM_TextEditor_ARexxCmd, "Undo");
		      IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_REDO, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 2, MUIM_TextEditor_ARexxCmd, "Redo");

					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_FONT_BOLD, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_SurroundSelection, MEV_MDEditor_Style_Bold);
					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_FONT_ITALIC, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_SurroundSelection, MEV_MDEditor_Style_Italic);
					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_FONT_CODE, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_SurroundSelection, MEV_MDEditor_Style_Code);
					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_FONT_STRIKETHROUGH, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_SurroundSelection, MEV_MDEditor_Style_Strikethrough);
					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_INDENTED_CODE, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_SurroundSelection, MEV_MDEditor_Style_IndentedCode);

					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_HORIZONTAL_RULE, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_InsertItem, MEV_MDEditor_HorizontalRule);
					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_TABLE, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_InsertItem, MEV_MDEditor_Table);
					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_IMAGE, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_InsertItem, MEV_MDEditor_Image);

					IIntuition -> IDoMethod (toolbar_p, MUIM_TheBar_DoOnButton, BID_HYPERLINK, MUIM_Notify, MUIA_Pressed, FALSE, s_editor_p, 3, MUIM_Set, MEA_InsertItem, MEV_MDEditor_Hyperlink);
				}


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
									CONST_STRPTR join_s = " - ";
									STRPTR title_s = NULL;


									* (content_s + size) = '\0';

									IIntuition -> IDoMethod (s_editor_p, MUIM_TextEditor_ClearText);
									IIntuition -> SetAttrs (s_editor_p, MUIA_TextEditor_Contents, content_s, TAG_DONE);

									title_s = ConcatenateVarargsStrings (s_app_name_s, join_s, filename_s, NULL);

									if (title_s)
										{
											IIntuition -> SetAttrs (s_window_p, MUIA_Window_Title, title_s, TAG_DONE);

											DB (KPRINTF ("%s %ld - LoadFile setting editor filename to %s (%lu)\n", __FILE__, __LINE__, filename_s, (uint32) filename_s));
											IIntuition -> SetAttrs (s_editor_p, MEA_Filename, filename_s, TAG_DONE);
										}

									success_flag = TRUE;
								}
						}
				}

			IDOS -> FClose (fh_p);
		}


	return success_flag;
}


BOOL SaveFile (STRPTR filename_s, CONST CONST_STRPTR text_s)
{
	BOOL success_flag = FALSE;

	BPTR fh_p = IDOS -> FOpen (filename_s, MODE_NEWFILE, 0);

	if (fh_p)
		{
			const uint32 size = strlen (text_s);

			if (IDOS -> FWrite (fh_p, text_s, size, 1) == 1)
				{
					success_flag = TRUE;
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
			CONST_STRPTR title_s = save_flag ? "Save Markdown File" : "Load Markdown File";
			struct Window *window_p = NULL;

		//	printf ("req win %lu\n", s_window_p);

			if (s_window_p)
				{
			 		if (IIntuition -> GetAttr (MUIA_Window_Window, s_window_p, (uint32 *) &window_p) == 0)
			 			{
				 			IDOS -> PutStr ("Failed to get Window\n");
			 			}
				}

			if (window_p)
				{
					if (IAsl -> AslRequestTags (req_p,
						//ASLFR_InitialDrawer, "RAM:",
						ASLFR_RejectIcons, TRUE,
						ASLFR_Window, window_p,
						ASLFR_TitleText, title_s,
						ASLFR_DoSaveMode, save_flag,
						ASLFR_DoPatterns, TRUE,
						ASLFR_InitialPattern, s_file_pattern_s,
						TAG_END))
						{
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
					IDOS -> PutStr ("Failed to open File Requester\n");
				}

			IAsl -> FreeAslRequest (req_p);
		}


	//printf ("filename: %s\n", filename_s ? filename_s : "NULL");
	return filename_s;
}
