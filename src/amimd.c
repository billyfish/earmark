#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include <classes/window.h>

#include <dos/dos.h>

#include <exec/types.h>

#include <intuition/intuition.h>

#include <libraries/mui.h>

/* Prototypes for system functions. */
#include <proto/graphics.h>
#include <images/bitmap.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/asl.h>
#include <proto/requester.h>

#include <proto/amijansson.h>

#include <proto/oo.h>


#define ALLOCATE_GLOBALS

#include "debugging_utils.h"
#include "gui.h"
#include "prefs.h"
#include "string_utils.h"

//#include "memwatch.h"

/* Prototypes for our functions. */

int main (int argc, char *argv []);

static BOOL OpenLibs (void);
static void CloseLibs (void);

static BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version);

static void CloseLib (struct Library *library_p, struct Interface *interface_p);


/* Global variables. */

struct Library *IntuitionBase = NULL;
struct IntuitionIFace *IIntuition = NULL;

//struct Library *DOSBase = NULL;
//struct DOSIFace *IDOS = NULL;

struct Library *WorkbenchBase = NULL;
struct WorkbenchIFace *IWorkbench = NULL;

struct Library *UtilityBase = NULL;
struct UtilityIFace *IUtility = NULL;


struct Library *GfxBase = NULL;
struct GraphicsIFace *IGraphics = NULL;

struct Library *MUIMasterBase = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;

struct Library *AslBase = NULL;
struct AslIFace *IAsl = NULL;

struct Library *JanssonBase = NULL;
struct JanssonIFace *IJansson = NULL;

struct Library *RequesterBase = NULL;
struct RequesterIFace *IRequester = NULL;

struct ClassLibrary *BitMapBase = NULL;

struct Library *IconBase = NULL;
struct IconIFace *IIcon = NULL;

struct Library *OOBase = NULL;
struct OOIFace *IOO = NULL;


static BPTR s_current_dir_p = ZERO;

static const char USED min_stack[] = "$STACK:102400";

/***************************************************************/

static BOOL OpenLibs (void)
{
	if (OpenLib (&IntuitionBase, "intuition.library", 53L, (struct Interface **) &IIntuition, "main", 1))
		{
			if (OpenLib (&GfxBase, "graphics.library", 53L, (struct Interface **) &IGraphics, "main", 1))
				{
					if (OpenLib (&UtilityBase, "utility.library", 53L, (struct Interface **) &IUtility, "main", 1))
						{
							if (OpenLib (&DOSBase, "dos.library", 53L, (struct Interface **) &IDOS, "main", 1))
								{
									if (OpenLib (&AslBase, "asl.library", 53L, (struct Interface **) &IAsl, "main", 1))
										{
											if (OpenLib (&MUIMasterBase, "muimaster.library", 19L, (struct Interface **) &IMUIMaster, "main", 1))
												{
													if (OpenLib (&JanssonBase, "jansson.library", 2L, (struct Interface **) &IJansson, "main", 1))
														{													
															if (OpenLib (&RequesterBase, "requester.class", 53L, (struct Interface **) &IRequester, "main", 1))
																{											
																	if ((BitMapBase = IIntuition->OpenClass ("images/bitmap.image", 53, &BitMapClass)) != NULL)
																		{															
																			if (OpenLib (&IconBase, "icon.library", 53L, (struct Interface **) &IIcon, "main", 1))
																				{		
																					if (OpenLib (&OOBase, "oo.library", 1L, (struct Interface **) &IOO, "main", 1))
																						{
																							return TRUE;
																						}
																					CloseLib (OOBase, (struct Interface *) IOO);
																				}
																			IIntuition -> CloseClass (BitMapBase);
																		}
																	CloseLib (RequesterBase, (struct Interface *) IRequester);
																}
															CloseLib (JanssonBase, (struct Interface *) IJansson);
														}
													CloseLib (MUIMasterBase, (struct Interface *) IMUIMaster);
												}
											CloseLib (AslBase, (struct Interface *) IAsl);
										}
									CloseLib (DOSBase, (struct Interface *) IDOS);
								}
						 	CloseLib (UtilityBase, (struct Interface *) IUtility);
						}
				
					CloseLib (GfxBase, (struct Interface *) IGraphics);				
				}
					
			CloseLib (IntuitionBase, (struct Interface *) IIntuition);
		}
		
	return FALSE;
}


static void CloseLibs (void)
{
	CloseLib (OOBase, (struct Interface *) IOO);
	CloseLib (IconBase, (struct Interface *) IIcon);
	IIntuition -> CloseClass (BitMapBase);
	CloseLib (RequesterBase, (struct Interface *) IRequester);
	CloseLib (JanssonBase, (struct Interface *) IJansson);
	CloseLib (MUIMasterBase, (struct Interface *) IMUIMaster);
	CloseLib (AslBase, (struct Interface *) IAsl);
	CloseLib (DOSBase, (struct Interface *) IDOS);
	CloseLib (UtilityBase, (struct Interface *) IUtility);
	CloseLib (GfxBase, (struct Interface *) IGraphics);				
	CloseLib (IntuitionBase, (struct Interface *) IIntuition);
}

int main (int argc, char *argv [])
{
	int result = 0;

	if (OpenLibs ())
		{			
			BPTR original_dir_lock_p = ZERO;
			MDPrefs *prefs_p = NULL;
			STRPTR md_filename_s = NULL;
			STRPTR md_settings_s = NULL;
			typedef enum 
			{
				AE_SETTINGS,
				AE_FILENAME,
				AS_NUM_ARGS	
			} ArgEntry;
			int32 args_array [] = { 0, 0 };
			
			if (argc > 0)
				{
					/* Started from Shell */
					struct RDArgs *rd_args_p;
					CONST_STRPTR template_s = "SETTINGS/K,FILE";
					rd_args_p = IDOS -> ReadArgs (template_s, args_array, NULL);
					
					if (rd_args_p)
						{
							if (args_array [AE_SETTINGS])
								{
									md_settings_s = EasyCopyToNewString ((STRPTR) args_array [AE_SETTINGS]);
								}
								
							if (args_array [AE_FILENAME])
								{
									md_filename_s = EasyCopyToNewString ((STRPTR) args_array [AE_FILENAME]);
								}
							
							IDOS -> FreeArgs (rd_args_p);	
						}
				}
			else
				{
					/* Started from Workbench */
					struct WBStartup *wbs_p = (struct WBStartup *) argv;
					BPTR lock_p = wbs_p -> sm_ArgList [0].wa_Lock;
							
					if (lock_p != ZERO)
						{
							BPTR old_dir_p = IDOS -> SetCurrentDir (lock_p);
	
							if (old_dir_p != ZERO)
								{
									struct DiskObject *info_p = IIcon -> GetDiskObjectNew (wbs_p -> sm_ArgList [0].wa_Name);
									
									if (info_p)
										{
											STRPTR value_s  = IIcon -> FindToolType (info_p -> do_ToolTypes, "SETTINGS");
											
											if (value_s)
												{
													md_settings_s = EasyCopyToNewString (value_s);
												}
											
											IIcon -> FreeDiskObject (info_p);	
										}
									
									IDOS -> SetCurrentDir (old_dir_p);
								}
								
						}		/* if (lock_p != ZERO) */

				}

			DB (KPRINTF ("%s %ld - Settings: \"%s\"\n", __FILE__, __LINE__, md_settings_s ? md_settings_s : "NULL"));		
			DB (KPRINTF ("%s %ld - File: \"%s\"\n", __FILE__, __LINE__, md_filename_s ? md_filename_s : "NULL"));

			DB (KPRINTF ("%s %ld - Opened Libraries\n", __FILE__, __LINE__));

			if (md_settings_s)
				{
					prefs_p = CreateMDPrefsFromFile (md_settings_s);

					if (!prefs_p)
						{
							STRPTR error_s = ConcatenateStrings ("Failed to load settings file: ", md_settings_s);
							
							if (error_s)
								{
									ShowError ("Load Error", error_s, "_Ok");	
									FreeCopiedString (error_s);
								}
							else
								{
									ShowError ("Load Error", md_settings_s, "_Ok");																											
								}
						}
				}

			/*
			original_dir_lock_p = IDOS -> GetCurrentDir ();
			
			if (original_dir_lock_p != ZERO)
				{
					BPTR current_dir_lock_p = ZERO;
								
					CreateMUIInterface (prefs_p, md_filename_s);

					current_dir_lock_p = IDOS -> GetCurrentDir ();
			
					if (current_dir_lock_p != ZERO)
						{
							if (current_dir_lock_p != original_dir_lock_p)
								{
									IDOS -> SetCurrentDir (original_dir_lock_p);
								}
						}
				}
			*/

		CreateMUIInterface (prefs_p, md_filename_s);


			if (md_settings_s)
				{
					FreeCopiedString (md_settings_s);	
				}
				
			if (md_filename_s)
				{
					FreeCopiedString (md_filename_s);	
				}

			CloseLibs ();
		}		/* if (OpenLibs ()) */
	else
		{
			IDOS -> Printf ("Failed to open libs\n");
		}

	return result;
}



static BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version)
{
	if ((*library_pp = IExec->OpenLibrary (lib_name_s, lib_version)) != NULL)
		{
			if ((*interface_pp = IExec->GetInterface (*library_pp, interface_name_s, interface_version, NULL)) != NULL)
				{
					return TRUE;
				}
			else
				{
					IDOS -> Printf ("Failed to open interface \"%s\" version %lu from \"%s\"\n", interface_name_s, interface_version, lib_name_s);
				}
			IExec->CloseLibrary (*library_pp);
		}
	else
		{
			IDOS -> Printf ("Failed to open library \"%s\" version %lu\n", lib_name_s, lib_version);
		}

	return FALSE;
}


static void CloseLib (struct Library *library_p, struct Interface *interface_p)
{
	if (interface_p)
		{
			IExec->DropInterface (interface_p);
		}

	if (library_p)
		{
			IExec->CloseLibrary (library_p);
		}
}
