/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: AmiMarkdown
**
** File: viewer_gadget.c
**
** Date: 04-01-2021 12:34:17
**
************************************************************

*/

#include "viewer_gadget.h"
#include "debugging_utils.h"

#include <stdio.h>
#include <string.h>

#include <proto/muimaster.h>
#include <proto/oo.h>
#include <proto/dos.h>

#include <OO/ooNetwork.h>

#include "string_utils.h"


typedef struct MarkdownViewerData
{
	STRPTR mvd_filename_s;
	struct Hook *mvd_load_hook_p;
} MarkdownViewerData;


typedef struct 
{
	char *hi_local_path_s;
	char *hi_url_s;
	FILE *hi_image_f;
	APTR hi_data_p;
} HTMLImage;



/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/


static uint32 MarkdownViewerDispatcher (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownViewer_New (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownViewer_Set (Class *class_p, Object *object_p, Msg msg_p);


static void SetFile (MarkdownViewerData *md_p, STRPTR filename_s);


static HTMLImage *GetLocalImage (STRPTR filename_s);


static HTMLImage *GetRemoteImage (STRPTR url_s);


static void FreeImage (HTMLImage *image_p);


static STRPTR DownloadImage (STRPTR url_s);

/**************************************************/
/**************** PUBLIC FUNCTIONS ****************/
/**************************************************/


struct MUI_CustomClass *InitMarkdownViewerClass (void)
{
	/* Create the new custom class with a call to MUI_CreateCustomClass(). */
	/* Caution: This function returns not a struct IClass, but a		   */
	/* struct MUI_CustomClass which contains a struct IClass to be		 */
	/* used with NewObject() calls.										*/
	/* Note well: MUI creates the dispatcher hook for you, you may		 */
	/* *not* use its h_Data field! If you need custom data, use the		*/
	/* cl_UserData of the IClass structure!								*/

	return IMUIMaster->MUI_CreateCustomClass (NULL, MUIC_HTMLview, NULL, sizeof (MarkdownViewerData), MarkdownViewerDispatcher);
}


void FreeMarkdownViewerClass (struct MUI_CustomClass *mui_class_p)
{
	IMUIMaster->MUI_DeleteCustomClass (mui_class_p);
}



/* The procedure which will be called by the load task. Simple, right?
** Please note: The built-in LoadFunc in HTMLview is smarter (uses
** asyncio.library if present, for example), so this one serves only a
** teaching purpose. */
uint32 LoadHtmlImage (struct Hook *hook_p, Object *obj_p, struct HTMLview_LoadMsg *lmsg_p)
{
	uint32 ret = 0;
	
	switch (lmsg_p -> lm_Type)
	{
		case HTMLview_Open: /* Just open the file */
			{
				char *filename_s = lmsg_p -> lm_Params.lm_Open.URL;

				printf ("loading \"%s\"\n", filename_s);

				if (filename_s)
					{
						HTMLImage *image_p = NULL;
							
						if (strncmp (filename_s, "file://", 7) == 0)
							{
								filename_s += 7;
								image_p = GetLocalImage (filename_s);
							} 	
						else if ((strncmp (filename_s, "http://", 7) == 0) || (strncmp (filename_s, "https://", 8) == 0))
							{
								image_p = GetRemoteImage (filename_s);
							}			
							
						lmsg_p -> lm_Userdata = (APTR) image_p;
						ret = (uint32) (lmsg_p -> lm_Userdata);								
					}
			}
			break;
			
		case HTMLview_Close: /* And close it again */
			{
				HTMLImage *image_p = (HTMLImage *) lmsg_p -> lm_Userdata;
					
				if (image_p)
					{
						FreeImage (image_p);
						lmsg_p -> lm_Userdata = NULL;													
					} 	
			}
			break;
			
		case HTMLview_Read: /* Read from the file */
			{
				HTMLImage *image_p = (HTMLImage *) lmsg_p -> lm_Userdata; 				
				ret = (uint32) fread (lmsg_p -> lm_Params.lm_Read.Buffer, 1, lmsg_p -> lm_Params.lm_Read.Size, image_p -> hi_image_f);
			}
			break;
			
		default:
			printf ("unknown %lu\n", lmsg_p -> lm_Type);
			break;

	}
	
	return ret;
}


/**********************************/
/******* STATIC FUNCTIONS ********/
/**********************************/

static uint32 MarkdownViewerDispatcher (Class *class_p,  Object *object_p, Msg msg_p)
{
	uint32 res;

	switch (msg_p -> MethodID)
		{
			case OM_NEW:
				DB (KPRINTF ("%s %ld - MarkdownViewerDispatcher: New\n", __FILE__, __LINE__));
				res = MarkdownViewer_New (class_p, object_p, msg_p);
				break;

			case OM_SET:
			case OM_UPDATE:
				DB (KPRINTF ("%s %ld - MarkdownViewerDispatcher: Set\n", __FILE__, __LINE__));
				res = MarkdownViewer_Set (class_p, object_p, msg_p);
				break;

			default:
				//DB (KPRINTF ("%s %ld - MoleculeInfoDispatcher: %x\n", __FILE__, __LINE__, msg_p -> MethodID));
				res = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
				break;
		}

	return res;
}
           

static uint32 MarkdownViewer_New (Class *class_p, Object *object_p, Msg msg_p)
{
	Object *md_viewer_p = (Object *) IIntuition -> IDoSuperMethodA (class_p, object_p, msg_p);


	if (md_viewer_p)
		{
			MarkdownViewerData *md_p = INST_DATA (class_p, md_viewer_p);
			md_p -> mvd_filename_s = NULL;			

			DB (KPRINTF ("%s %ld - MarkdownViewer_New: Adding info obj\n", __FILE__, __LINE__));
		}
	else
		{
			/* invoke OM_DISPOSE on *our* class! */
			IIntuition->ICoerceMethod (class_p, md_viewer_p, OM_DISPOSE);
			md_viewer_p = NULL;
		}


	return ((uint32) md_viewer_p);
}


static uint32 MarkdownViewer_Set (Class *class_p, Object *object_p, Msg msg_p)
{
	uint32 retval = IIntuition->IDoSuperMethodA (class_p, object_p, msg_p);
	MarkdownViewerData *md_p = INST_DATA (class_p, object_p);

	if (md_p -> mvd_filename_s)
		{
			DB (KPRINTF ("%s %ld - MarkdownViewer_Set: setting file to \"%s\"\n", __FILE__, __LINE__, md_p -> mvd_filename_s));

			IIntuition -> IDoMethod (object_p, MUIM_HTMLview_GotoURL, md_p -> mvd_filename_s, NULL);
		}
	else
		{
			DB (KPRINTF ("%s %ld - MarkdownViewer_Set: no file\n", __FILE__, __LINE__ ));
		}
		
	return retval;
}



static void SetFile (MarkdownViewerData *md_p, STRPTR filename_s)
{
	md_p -> mvd_filename_s = filename_s;

	if (filename_s)
		{
			APTR list_p;
			LONG l1, l2;
			STRPTR buffer_s = NULL;

		}		/* if (mol_p) */

}





static HTMLImage *GetLocalImage (STRPTR filename_s)
{
	STRPTR copied_filename_s = EasyCopyToNewString (filename_s);
	
	if (copied_filename_s)
		{
			FILE *in_f = fopen (filename_s, "r");
			
			if (in_f)
				{
					HTMLImage *hi_p = (HTMLImage *) IExec -> AllocVecTags (sizeof (HTMLImage));
					
					if (hi_p)
						{
							hi_p -> hi_local_path_s = copied_filename_s;
							hi_p -> hi_url_s = NULL;
							hi_p -> hi_image_f = in_f;
							
							return hi_p;	
						}
					
					
					fclose (in_f);					
				}
					
			FreeCopiedString (copied_filename_s);	
		}
	
	return NULL;
} 


static HTMLImage *GetRemoteImage (STRPTR url_s)
{
	STRPTR copied_url_s = EasyCopyToNewString (url_s); 

	if (copied_url_s)
		{
			STRPTR downloaded_image_s = DownloadImage (url_s);
	
			if (downloaded_image_s)
				{			
					FILE *in_f = fopen (downloaded_image_s, "r");
					
					printf ("dowloaded image \"%s\"\n", url_s);
					
					if (in_f)
						{
							HTMLImage *hi_p = (HTMLImage *) IExec -> AllocVecTags (sizeof (HTMLImage));
							
							printf ("opened image \"%s\"\n", url_s);
							
							if (hi_p)
								{
									hi_p -> hi_local_path_s = downloaded_image_s;
									hi_p -> hi_url_s = copied_url_s;
									hi_p -> hi_image_f = in_f;
									
									
									printf ("returning image for \"%s\"\n", copied_url_s);
									
									return hi_p;	
								}
							
							
							fclose (in_f);					
						}
							
					FreeCopiedString (downloaded_image_s);	
				}		/* if (downloaded_image_s) */
		
			FreeCopiedString (copied_url_s);	
		}
	
	return NULL;	
}


static void FreeImage (HTMLImage *image_p)
{
	if (image_p -> hi_image_f)
		{
			fclose (image_p -> hi_image_f);	
		}
	
	if (image_p -> hi_url_s)
		{
			FreeCopiedString (image_p -> hi_url_s);	
		}

	if (image_p -> hi_local_path_s)
		{
			FreeCopiedString (image_p -> hi_local_path_s);	
		}
	
	IExec -> FreeVec (image_p);
}



static BOOL GetOrCreateDirectory (STRPTR path_s)
{
	BOOL success_flag = FALSE;
	struct ExamineData *data_p = IDOS -> ExamineObjectTags (EX_StringNameInput, path_s, TAG_DONE);
	
	if (data_p)
		{
			if (EXD_IS_DIRECTORY (data_p))
				{
					success_flag = TRUE;	
				}
				
			IDOS -> FreeDosObject (DOS_EXAMINEDATA, data_p);
		}
	else
		{
			BPTR lock_p = IDOS -> CreateDirTree (path_s);
		
			if (lock_p)
				{
					IDOS -> UnLock (lock_p);
					success_flag = TRUE;	
				}
		}
	
	return success_flag;
}


static STRPTR DownloadImage (STRPTR url_s)
{
	STRPTR cached_filename_s = NULL;
	STRPTR temp_file_name_s = NULL;


	if (strncmp (url_s, "http://", 7) == 0)
		{
			temp_file_name_s = EasyCopyToNewString (url_s + 7);
		}
	else if (strncmp (url_s, "https://", 8) == 0)
		{
			temp_file_name_s = EasyCopyToNewString (url_s + 8);			
		}
	
	if (temp_file_name_s)
		{
			BPTR lock_p = ZERO;
			CONST CONST_STRPTR TEMP_DIR_S = "T:";
			STRPTR dir_s = ConcatenateStrings (TEMP_DIR_S, temp_file_name_s);
									
			if (dir_s)
				{	
					STRPTR last_slash_s = strrchr (dir_s, '/');
		
					if (last_slash_s)
						{			
							*last_slash_s = '\0';	
						}
	
				
					if (GetOrCreateDirectory (dir_s))
						{
							STRPTR command_s = NULL;
							
							printf ("GetOrCreateDirectory \"%s\" succeeded\n", dir_s);

							command_s = ConcatenateVarargsStrings  ("copy t:earmark_1 to ", TEMP_DIR_S, temp_file_name_s, NULL); // ("curl -s ", url_s, " -o ", temp_file_name_s, NULL);	
				
							printf ("temp \"%s\", url \"%s\"\n", temp_file_name_s, url_s);
					
							if (command_s)
								{
									int32 ret = -1;
									
									printf ("About to run \"%s\" \n", command_s);			
						
									ret = IDOS -> SystemTags (command_s,
										NP_Name, "Earmark Image Downloader",
										SYS_Asynch, FALSE,
										TAG_DONE);
								
									printf ("\"%s\" returned %ld\n", command_s, ret);			
								
									if ((ret >= 0) && (ret <= 20))
										{
											printf ("Downloaded image \"%s\" successfully\n", url_s);
											
											cached_filename_s = ConcatenateVarargsStrings (TEMP_DIR_S, temp_file_name_s, NULL);
											
											if (!cached_filename_s)
												{
													printf ("Failed to copy local filename for  image \"%s\" successfully\n", url_s);														
												}
																	
										}
								
									FreeCopiedString (command_s);	
								}			
						}
					else
						{
							printf ("GetOrCreateDirectory \"%s\" failed\n", dir_s);
						}
						
					FreeCopiedString (dir_s);
				}
				
			
			
			FreeCopiedString (temp_file_name_s);					
		}
		
	return cached_filename_s;
}


static STRPTR DownloadImageByOO (STRPTR url_s)
{
	STRPTR cached_filename_s = NULL;
	
	NETWORKOBJ *net_p = (NETWORKOBJ *) IOO -> NewNetworkObject ();

	if (net_p != NULL)
		{
			int32 port = 80;
			BOOL secure_flag = FALSE;
			CONST CONST_STRPTR https_s = "https://";
			const size_t https_len = strlen (https_s);
		
		
			printf ("attempting to download \"%s\"\n", url_s);
			
			if (strncmp (url_s, https_s, https_len) == 0)
				{
					port = 443;
					secure_flag = TRUE;
					printf ("secure url\n");
				}

			if (net_p -> CreateConnection (url_s, port, secure_flag, TRUE))
				{ 
					if (net_p -> GetConnection ())
						{
				      STRPTR req_s = NULL;
				      uint32 resp_code = 0;

				      printf("Trying to load %s:%lu\n", url_s, port);
				      req_s = net_p -> CreateHTTPRequest (url_s, port);
				      net_p -> SendHTTPRequest (req_s);
				      resp_code = net_p -> GetHTTPResponseCode ();
				      printf("Response code=%lu\n",resp_code);
		
							/* Handle any redirects */
							while (((resp_code == 301) || (resp_code == 302) || (resp_code == 307)) && net_p -> netAltURL)
					      {
					      	net_p -> DisposeConnection ();
		
									resp_code = 0;
		
				
									if (strncmp (net_p -> netAltURL, https_s, https_len) == 0)
										{
											port = 443;
											secure_flag = TRUE;
										}
									else
										{
											port = 80;
											secure_flag = FALSE;
										}
					      
					      	if (net_p -> CreateConnection (net_p -> netAltURL, port, secure_flag, TRUE))
										{
											if (net_p -> GetConnection ())
							        	{
													printf("Redirecting to %s:%lu\n",net_p -> netAltURL, port);
													req_s = net_p -> CreateHTTPRequest (net_p -> netAltURL, port);
													net_p -> SendHTTPRequest (req_s);
													resp_code = net_p -> GetHTTPResponseCode ();
													printf("Response code=%lu\n",resp_code);
												}
									 	}
								}

							if ((resp_code >= 200) && (resp_code <= 202))
								{
									STRPTR resp_s = net_p -> GetResponseBody ();
									size_t resp_len = strlen (resp_s);
									
									printf ("len %lu\n", resp_len);
									 
									if (resp_s != NULL)
										{
											CONST_STRPTR temp_file_name_s = "T:earmark_1";
											BPTR fh_p = IDOS -> FOpen (temp_file_name_s, MODE_NEWFILE, 0);

											printf("Got repsonse\n");
											
											if (fh_p)
												{
													if (IDOS -> FWrite (fh_p, resp_s, resp_len, 1) == 1)
														{
															printf ("saved to \"%s\"\n", temp_file_name_s); 	
														}
													else
														{
															printf ("error saving to \"%s\"\n", temp_file_name_s); 
														}

												
													IDOS -> FClose (fh_p);	
												}
											else
												{
													printf ("error opening \"%s\"\n", temp_file_name_s); 
												}
											

		
											net_p->Free (resp_s);
										}
									else
										{ 
											printf("No response\n"); 
										}
								}
							else
								{
									if (net_p->netErrorMsg) 
										{	 
											printf ("Error: %s\n",net_p->netErrorMsg); 
										}
								}
						}
					else 
						{
							printf("SimpleHTTP ERROR: Connection failed for url \"%s\" on port %ld with secure_flag %d\n", url_s, port, secure_flag);
						}

    //			net_p->DisposeConnection();
				}
			else 
				{
					printf("SimpleHTTP ERROR: Connection (socket) not created\n");
				}
		
			IOO->DisposeNetworkObject(net_p);
		}

	return cached_filename_s;
}

