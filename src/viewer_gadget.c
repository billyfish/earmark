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
} HTMLImage;



/**********************************/
/******* STATIC PROTOTYPES ********/
/**********************************/


static uint32 MarkdownViewerDispatcher (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownViewer_New (Class *class_p, Object *object_p, Msg msg_p);


static uint32 MarkdownViewer_Set (Class *class_p, Object *object_p, Msg msg_p);


static void SetFile (MarkdownViewerData *md_p, STRPTR filename_s);

static uint32 LoadFile (struct Hook *hook_p, Object *obj_p, struct HTMLview_LoadMsg *lmsg);


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
				char *filename_s = lmsg -> lm_Params.lm_Open.URL;

				printf ("loading \"%s\"\n", filename);

				if (filename_s)
					{
						if (strncmp (filename_s, "file://", 7) == 0)
							{
								filename_s += 7;
								uint32 ret = lmsg_p -> lm_Userdata = fopen (filename_s ,"r"));
							} 	
						else if strncmp (filename_s, "http://", 7) == 0)
							{
								char *uefa 81!
								
								filename_s += 7;	
								ret = LoadRemoteImage (filename_s);
							}
						else if strncmp (filename_s, "https://", 8) == 0)
							{
								filename_s += 8;	
								ret = LoadRemoteImage (filename_s);								
							}						
				}
			}
			break;
			
		case HTMLview_Close: /* And close it again */
			{
				FILE *image_f = (FILE *) lmsg_p -> lm_Userdata; // Our file handle
					
				if (image_f)
					{
						fclose (image_f);
						lmsg_p -> lm_Userdata = NULL;													
					} 	
			}
			break;
			
		case HTMLview_Read: /* Read from the file */
			{
				FILE *image_f = (FILE *) lmsg_p -> lm_Userdata; // Our file handle				
				ret = fread (lmsg_p -> lm_Params.lm_Read.Buffer, 1 ,lmsg_p -> lm_Params.lm_Read.Size, image_f));
			}
			break;
			
		default:
			printf ("unknown %lu\n", lmsg->lm_Type);
			break;

	}
	return(0);
}


static HTMLImage *GetLocalImage (STRPTR filename_s)
{
	STRPTR copied_filename_s = EasyCopyToNewString (filename_s);
	
	if (copied_filename_s)
		{
			FILE *in_f = fopen (filename_s, "r");
			
			if (in_f)
				{
					HTMLImage *hi_p = (HtmlImage *) IExec -> AllocVecTags (sizeof (HtmlImage));
					
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
	HTMLImage *hi_p = NULL;	
	STRPTR downloaded_image_s = DownloadImage (url_s);
	
	if (downloaded_image_s)
		{
			
		
			FreeCopiedString (downloaded_image_s);	
		}		/* if (downloaded_image_s) */
	
	
	return hi_p;	
}


static void FreeHtmlImage (HtmlImage *image_p)
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


static STRPTR DownloadImage (STRPTR url_s)
{
	STRPTR cached_filename_s = NULL;
	
	OOBase = (struct Library *) IExec -> OpenLibrary ("oo.library",1);
 	if (OOBase!=NULL)
 		{
 	 		IOO = (struct OOIFace *) IExec -> GetInterface (OOBase, "main", 1, NULL);
  		
  		if (IOO!=NULL)
  			{
   				NETWORKOBJ *net= (NETWORKOBJ *)IOO->NewNetworkObject();

   				if (net!=NULL)
   					{
    					portnum=80;
					
					    if (net->CreateConnection (url_s, portnum, FALSE, TRUE))
    						{ 
						     if (net->GetConnection())
     								{
								      printf("Trying to load %s:%lu\n",Url,portnum);
								      httpreq=net->CreateHTTPRequest(Url,portnum);
								      net->SendHTTPRequest(httpreq);
								      respcode=(uint32)net->GetHTTPResponseCode();
								      IDOS->Printf("Response code=%lu\n",respcode);

     									while (((respcode==301) || (respcode==302) || (respcode==307)) && net->netAltURL)
									      {
									      	net->DisposeConnection();

									       respcode=0;
									       if (net->CreateConnection(net->netAltURL,portnum,FALSE,TRUE))
										       {
										        if (net->GetConnection())
											        {
																printf("Redirecting to %s:%lu\n",net->netAltURL,portnum);
																httpreq=net->CreateHTTPRequest(net->netAltURL,portnum);
																net->SendHTTPRequest(httpreq);
																respcode=(uint32)net->GetHTTPResponseCode();
																IDOS->Printf("Response code=%lu\n",respcode);
															}
													 }
												}

      								if ((respcode>=200) && (respcode<=202))
												{
													httpresp=net->GetHTTPResponse();
													if (httpresp!=NULL)
														{
															IDOS->Printf("%s\n",httpresp);

															net->Free(httpresp);
														}
       										else
       											{ 
       												IDOS->Printf("No response\n"); 
       											}
      									}
      								else
      									{
       										if (net->netErrorMsg) 
       											{	 
       												IDOS->Printf ("Error: %s\n",net->netErrorMsg); 
       											}
												}
     								}
     							else 
     								{
     									printf("SimpleHTTP ERROR: Connection failed\n");
										}
						
						    	net->DisposeConnection();
    						}
							else 
								{
									printf("SimpleHTTP ERROR: Connection (socket) not created\n");
								}
    				
    					IOO->DisposeNetworkObject(net);
   					}

   				IExec->DropInterface((struct Interface *)IOO);
  			}
  		else 
  			{
  				printf("SimpleHTTP ERROR: Can't open oo main interface\n");
  			}
		
		  IExec->CloseLibrary(OOBase);
 		}
	else 
		{
			printf("SimpleHTTP ERROR: Can't open oo.library v1\n");
		}
	
	return cached_filename_s;
}

