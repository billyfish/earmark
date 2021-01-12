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
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/asl.h>
#include <proto/requester.h>

#include <proto/amijansson.h>

#define ALLOCATE_GLOBALS

#include "debugging_utils.h"
#include "gui.h"
#include "prefs.h"

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

struct Library *MUIMasterBase = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;

struct Library *AslBase = NULL;
struct AslIFace *IAsl = NULL;

struct Library *JanssonBase = NULL;
struct JanssonIFace *IJansson = NULL;

struct Library *RequesterBase = NULL;
struct RequesterIFace *IRequester = NULL;

static const char USED min_stack[] = "$STACK:102400";

/***************************************************************/

static BOOL OpenLibs (void)
{
	if (OpenLib (&IntuitionBase, "intuition.library", 53L, (struct Interface **) &IIntuition, "main", 1))
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
															return TRUE;
														}
													CloseLib (RequesterBase, (struct Interface *) IRequester);
												}
											CloseLib (MUIMasterBase, (struct Interface *) IMUIMaster);
										}
									CloseLib (AslBase, (struct Interface *) IAsl);
								}
							CloseLib (DOSBase, (struct Interface *) IDOS);
						}
				 	CloseLib (UtilityBase, (struct Interface *) IUtility);
				}
			CloseLib (IntuitionBase, (struct Interface *) IIntuition);
		}
		
	return FALSE;
}


static void CloseLibs (void)
{
	CloseLib (JanssonBase, (struct Interface *) IJansson);
	CloseLib (MUIMasterBase, (struct Interface *) IMUIMaster);
	CloseLib (AslBase, (struct Interface *) IAsl);
	CloseLib (DOSBase, (struct Interface *) IDOS);
	CloseLib (UtilityBase, (struct Interface *) IUtility);
	CloseLib (IntuitionBase, (struct Interface *) IIntuition);
}

int main (int argc, char *argv [])
{
	int result = 0;

	if (OpenLibs ())
		{			
			MDPrefs *prefs_p = NULL;
			
			DB (KPRINTF ("%s %ld - Opened Libraries\n", __FILE__, __LINE__));

			CreateMUIInterface (prefs_p);

			CloseLibs ();
		}		/* if (OpenLibs ()) */
	else
		{
			printf ("Failed to open libs\n");
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
					printf ("Failed to open interface \"%s\" version %lu from \"%s\"\n", interface_name_s, interface_version, lib_name_s);
				}
			IExec->CloseLibrary (*library_pp);
		}
	else
		{
			printf ("Failed to open library \"%s\" version %lu\n", lib_name_s, lib_version);
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
