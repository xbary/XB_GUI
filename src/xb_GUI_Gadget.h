#ifndef _XB_GUIGADGET_H
#define _XB_GUIGADGET_H

#include <xb_board.h>
#ifdef XB_GUI
#include "xb_GUI.h"

typedef enum { tpmgNone, tpmgAll, tpmgOnlyLastCurrent, tpmgOnlyCurrent } TTypePaintMenuGadget;

class TGADGETMenu
{
public:
	TGADGETMenu(TTaskDef *AOwnerTaskDef,int8_t AIDMenu,bool Amodal, int16_t Ax, int16_t Ay);
	~TGADGETMenu();

	TGADGETMenu *Next;
	TGADGETMenu *Prev;

	
	TTaskDef *OwnerTaskDef;
	int8_t IDMenu;
	TWindowClass *WindowClass;
	TWindowClass *ModalWindowClass;
	int8_t IDWindow;
	bool Modal;
	
	bool DoMenuInit();
	bool MenuIsInit;
	
	uint8_t GetItemsMenuCount();
	bool GetItemMenuString(uint8_t Aindx, String &Astr, TTextAlignment *Atextalign=NULL);
	bool GetCaptionMenuString(String &Astr);
	void ChangeCurrentItemMenu(uint8_t Anewcurrentitem);
	void ClickItemMenu(uint8_t Aclickitem);
	void ClickRightItemMenu(uint8_t Aclickitem);
	void ClickLeftItemMenu(uint8_t Aclickitem);
	bool DeleteItemMenu(uint8_t Adelitem);
	bool SendEscapeMessageToOwnerTask();

	
	uint8_t ItemCount;
	uint8_t WidthItems;
	Tx X;
	Ty Y;
	uint8_t LastItem;
	uint8_t CurrentItem;
	
	TTypePaintMenuGadget TypePaintMenuGadget;
};

typedef union 
{
	int64_t *Int64;
	uint64_t *uInt64;
	int32_t *Int32;
	uint32_t *uInt32;
	int16_t *Int16;
	uint16_t *uInt16;
	int8_t *Int8;
	uint8_t *uInt8;
	char *DynArrayChar;
	String *DynString;
	IPAddress* IP;
} TInputVar;

typedef enum { tpidgAll } TTypePaintInputDialogGadget;

class TGADGETInputDialog
{
public:
	TGADGETInputDialog(TTaskDef *AOwnerTaskDef, int8_t AIDMenu, bool Amodal);
	~TGADGETInputDialog();
	void EnterVAR();
	void SendEnterMessageToOwnerTask();
	void SendEscapeMessageToOwnerTask();
	
	TGADGETInputDialog *Next;
	TGADGETInputDialog *Prev;

	TTaskDef *OwnerTaskDef;
	int8_t IDInputDialog;
	TWindowClass *WindowClass;
	int8_t IDWindow;

	TTypeInputVar TypeInputVar;
	TInputVar InputVar;
	uint8_t MaxLengthVar;

	union 
	{
		Tuint32MinMax uint32MinMax;
		Tuint16MinMax uint16MinMax;
		Tuint8MinMax uint8MinMax;
	} MinMax;
	
	String EditVar;
	int8_t CursorPosInputVar;
	bool MoveEditCursor(int8_t Astep);
	void PaintVar();
	bool EditInsertChar(char Achar);

	bool GetCaptionInputDialogString(String &Astr);
	bool GetDescriptionInputDialogString(String &Astr);
};




void GUIGADGET_Setup(void);
uint32_t GUIGADGET_Loop(void);
bool GUIGADGET_Message(TMessageBoard *Ams);

TGADGETInputDialog *GUIGADGET_FindInputDialogByIDWindow(int8_t AIDWindow);
TGADGETInputDialog *GUIGADGET_FindInputDialogByWindowClass(TWindowClass *Aw);
TGADGETInputDialog *GUIGADGET_FindInputDialogByOwnerTaskDef(TTaskDef *ATaskDef, int8_t AIDInputDialog);
TGADGETInputDialog *GUIGADGET_CreateInputDialog(TTaskDef *AOwnerTaskDef, int8_t AIDInputDialog, bool Amodal=false);
void GUIGADGET_DestroyInputDialog(TGADGETInputDialog **Amenu);

bool GUIGADGET_IsMainMenu(TTaskDef* Ataskdef);
bool GUIGADGET_OpenMainMenu(TTaskDef *Ataskdef, Tx Axpos=WINDOW_POS_X_CENTER, Ty Aypos=WINDOW_POS_Y_CENTER);
bool GUIGADGET_CloseMainMenu(TTaskDef *Ataskdef);
void GUIGADGET_RepaintMenu(TGADGETMenu *Am);
TGADGETMenu *GUIGADGET_FindMenuByIDWindow(int8_t AIDWindow);
TGADGETMenu *GUIGADGET_FindMenuByWindowClass(TWindowClass *Aw);
TGADGETMenu *GUIGADGET_FindMenuByOwnerTaskDef(TTaskDef *ATaskDef,uint8_t AIDMenu);
TGADGETMenu *GUIGADGET_CreateMenu(TTaskDef *ATaskDef, int8_t AIDMenu, bool Amodal=false,  int16_t Ax=-1, int16_t Ay=-1);
void GUIGADGET_DestroyMenu(TGADGETMenu **Amenu);
void GUIGADGET_CloseMenu(TGADGETMenu **Amenu);

extern TTaskDef XB_GUIGADGET_DefTask;

//-------------------------------------------------------------------------------------------------------------
#define MENU_AUTOCOUNT 255

#define BEGIN_MENU(Aidmenu,Acaption,Ax,Ay,Awidth,Aitemcount,Acurrentselect,AEscapeClose) \
if (Am->Data.MenuData.IDMenu==Aidmenu) \
{ \
	uint8_t ItemIndex = 0; \
	uint8_t CurrentItemIndex = 0; \
	uint8_t CountItems = 0; \
	if (Am->Data.MenuData.TypeMenuAction == tmaGET_INIT_MENU) \
	{ \
		Am->Data.MenuData.ActionData.MenuInitData.ItemCount = Aitemcount;  \
		Am->Data.MenuData.ActionData.MenuInitData.CurrentSelect = Acurrentselect; \
		Am->Data.MenuData.ActionData.MenuInitData.Width = Awidth; \
		Am->Data.MenuData.ActionData.MenuInitData.X = Ax; \
		Am->Data.MenuData.ActionData.MenuInitData.Y = Ay; \
		Am->Data.MenuData.ActionData.MenuInitData.EscapeClose = AEscapeClose; \
		return true; \
	} \
	else if (Am->Data.MenuData.TypeMenuAction == tmaGET_CAPTION_MENU_STRING) \
	{ \
		*(Am->Data.MenuData.ActionData.MenuCaptionData.PointerString) = String(Acaption); \
		return true; \
	} \

#define ESCAPE_MENU() \
if (Am->Data.MenuData.TypeMenuAction == tmaESCAPE_MENU) 

#define END_MENU() \
if (Am->Data.MenuData.ActionData.MenuItemData.ItemIndex == 255) \
{ \
	Am->Data.MenuData.ActionData.MenuItemData.ItemIndex = ItemIndex; \
} \
};

#define SEPARATOR_MENUITEM() \
	if (Am->Data.MenuData.TypeMenuAction == tmaGET_ITEM_MENU_STRING) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuItemData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) \
		{ \
			*(Am->Data.MenuData.ActionData.MenuItemData.PointerString) = "-"; \
			return true; \
		} \
	} ItemIndex++; CountItems++; \
	
#define CONFIGURATION_MENUITEMS() \
			BEGIN_MENUITEM("Load configuration", taLeft) \
			{ \
				CLICK_MENUITEM() \
				{ \
					board.Log("Load configuration..", true, true); \
					board.LoadConfiguration(); \
					board.Log(".OK"); \
				} \
			} \
			END_MENUITEM() \
			BEGIN_MENUITEM("Save configuration", taLeft) \
			{ \
				CLICK_MENUITEM() \
				{ \
					board.Log("Save configuration..", true, true); \
					board.SaveConfiguration(); \
					board.Log(".OK"); \
				} \
			} \
			END_MENUITEM() 


#define BEGIN_MENUITEM_CHECKED(Aname,Atextalignment,Aboolvalue) \
	{ \
	if (Am->Data.MenuData.TypeMenuAction == tmaGET_ITEM_MENU_STRING) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuItemData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) \
		{ \
			*(Am->Data.MenuData.ActionData.MenuItemData.PointerString) = String("[" + String(Aboolvalue == true ? "*" : " ") + "] "+ String(Aname)); \
			Am->Data.MenuData.ActionData.MenuItemData.TextAlignment = Atextalignment; \
			return true; \
		} \
	} 

#define BEGIN_MENUITEM(Aname,Atextalignment) \
	{ \
	if (Am->Data.MenuData.TypeMenuAction == tmaGET_ITEM_MENU_STRING) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuItemData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) \
		{ \
			*(Am->Data.MenuData.ActionData.MenuItemData.PointerString) = String(Aname); \
			Am->Data.MenuData.ActionData.MenuItemData.TextAlignment = Atextalignment; \
			return true; \
		} \
	} 

#define CLICK_MENUITEM() } \
	if (Am->Data.MenuData.TypeMenuAction == tmaCLICK_ITEM_MENU) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuClickData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) 

#define CLICKLEFT_MENUITEM() } \
	if (Am->Data.MenuData.TypeMenuAction == tmaCLICKLEFT_ITEM_MENU) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuClickLeftRightData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) 

#define CLICKRIGHT_MENUITEM() } \
	if (Am->Data.MenuData.TypeMenuAction == tmaCLICKRIGHT_ITEM_MENU) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuClickLeftRightData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) 


#define CLICK_MENUITEM_AND_CLOSE() } \
	if (Am->Data.MenuData.TypeMenuAction == tmaCLICK_ITEM_MENU) \
	{	 \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuClickData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) Am->Data.MenuData.ActionData.MenuClickData.Close = true; \
		if (CurrentItemIndex==ItemIndex) 

#define DELETE_MENUITEM() } \
	if (Am->Data.MenuData.TypeMenuAction == tmaDEL_ITEM_MENU) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuDelData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) 

#define DELETE_MENUITEM_AND_REINIT() } \
	if (Am->Data.MenuData.TypeMenuAction == tmaDEL_ITEM_MENU) \
	{ \
		CurrentItemIndex = Am->Data.MenuData.ActionData.MenuDelData.ItemIndex; \
		if (CurrentItemIndex==ItemIndex) Am->Data.MenuData.ActionData.MenuDelData.ReInit = true; \
		if (CurrentItemIndex==ItemIndex) 


#define END_MENUITEM() } ItemIndex++; CountItems++;

#define OPEN_MAINMENU() \
if (Am->Data.MenuData.TypeMenuAction == tmaIS_MAINMENU) \
{ \
Am->Data.MenuData.ActionData.MenuIsMainData.is = true; \
return true; \
} \
Tx X; \
Ty Y; \
if (Am->Data.MenuData.TypeMenuAction == tmaOPEN_MAINMENU) \
{ \
X=Am->Data.MenuData.ActionData.MenuOpenMainData.X; \
Y=Am->Data.MenuData.ActionData.MenuOpenMainData.Y; \
} \
if (Am->Data.MenuData.TypeMenuAction == tmaOPEN_MAINMENU) 



//------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define BEGIN_DIALOG(Aid,Acaption,Adescription,ATypeInputVar,AMaxLength,ADataPointer) if (Am->Data.InputDialogData.IDInputDialog==Aid)  \
{ \
if (Am->Data.InputDialogData.TypeInputDialogAction==ida_INIT_INPUTDIALOG) \
{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.TypeInputVar = ATypeInputVar; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MaxLength = AMaxLength; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.DataPointer = ADataPointer; \
} \
else if (Am->Data.InputDialogData.TypeInputDialogAction==ida_GET_CAPTION_STRING) \
{ \
	*(Am->Data.InputDialogData.ActionData.InputDialogCaptionData.PointerString) = Acaption; \
} \
else if (Am->Data.InputDialogData.TypeInputDialogAction == ida_GET_DESCRIPTION_STRING) \
{ \
	*(Am->Data.InputDialogData.ActionData.InputDialogDescriptionData.PointerString) = Adescription; \
} \
else \
{

#define BEGIN_DIALOG_MINMAX(Aid,Acaption,Adescription,ATypeInputVar,AMaxLength,ADataPointer,Amin,Amax) if (Am->Data.InputDialogData.IDInputDialog==Aid)  \
{ \
if (Am->Data.InputDialogData.TypeInputDialogAction==ida_INIT_INPUTDIALOG) \
{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.TypeInputVar = ATypeInputVar; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MaxLength = AMaxLength; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.DataPointer = ADataPointer; \
	if (ATypeInputVar==tivUInt8) \
	{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint8MinMax.Min = Amin; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint8MinMax.Max = Amax; \
	} else if (ATypeInputVar == tivUInt16) \
	{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint16MinMax.Min = Amin; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint16MinMax.Max = Amax; \
	} \
	else if (ATypeInputVar == tivUInt32) \
	{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint32MinMax.Min = Amin; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint32MinMax.Max = Amax; \
	} \
} \
else if (Am->Data.InputDialogData.TypeInputDialogAction == ida_GET_CAPTION_STRING) \
{ \
* (Am->Data.InputDialogData.ActionData.InputDialogCaptionData.PointerString) = Acaption; \
} \
else if (Am->Data.InputDialogData.TypeInputDialogAction == ida_GET_DESCRIPTION_STRING) \
{ \
* (Am->Data.InputDialogData.ActionData.InputDialogDescriptionData.PointerString) = Adescription; \
} \
else \
{ 

#define EVENT_ENTER() if (Am->Data.InputDialogData.TypeInputDialogAction==ida_ENTER_DIALOG)

#define EVENT_ESCAPE() if (Am->Data.InputDialogData.TypeInputDialogAction==ida_ESCAPE_DIALOG)

#define END_DIALOG() }}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define BEGIN_INPUTDIALOGINIT(iddialog) if ((Am->Data.InputDialogData.TypeInputDialogAction==ida_INIT_INPUTDIALOG) && (Am->Data.InputDialogData.IDInputDialog==iddialog)) \
{ 

#define DEF_INPUTDIALOGINIT(ATypeInputVar,AMaxLength,ADataPointer) \
{ \
Am->Data.InputDialogData.ActionData.InputDialogInitData.TypeInputVar = ATypeInputVar; \
Am->Data.InputDialogData.ActionData.InputDialogInitData.MaxLength = AMaxLength; \
Am->Data.InputDialogData.ActionData.InputDialogInitData.DataPointer = ADataPointer; \
}

#define	DEF_INPUTDIALOGINIT_MINMAX_UINT32(Amin,Amax) \
{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint32MinMax.Min = Amin; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint32MinMax.Max = Amax; \
} 

#define	DEF_INPUTDIALOGINIT_MINMAX_UINT16(Amin,Amax) \
{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint16MinMax.Min = Amin; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint16MinMax.Max = Amax; \
} 

#define	DEF_INPUTDIALOGINIT_MINMAX_UINT8(Amin,Amax) \
{ \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint8MinMax.Min = Amin; \
	Am->Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint8MinMax.Max = Amax; \
} 

#define END_INPUTDIALOGINIT() \
}

#define BEGIN_INPUTDIALOGCAPTION(idinputdialog) if ((Am->Data.InputDialogData.TypeInputDialogAction==ida_GET_CAPTION_STRING) && (Am->Data.InputDialogData.IDInputDialog==idinputdialog)) {
#define DEF_INPUTDIALOGCAPTION(caption) *(Am->Data.InputDialogData.ActionData.InputDialogCaptionData.PointerString) = caption;
#define END_INPUTDIALOGCAPTION() }

#define BEGIN_INPUTDIALOGDESCRIPTION(idinputdialog) if ((Am->Data.InputDialogData.TypeInputDialogAction==ida_GET_DESCRIPTION_STRING) && (Am->Data.InputDialogData.IDInputDialog==idinputdialog)) {
#define DEF_INPUTDIALOGDESCRIPTION(description) *(Am->Data.InputDialogData.ActionData.InputDialogDescriptionData.PointerString) = description;
#define END_INPUTDIALOGDESCRIPTION() }

#define BEGIN_INPUTDIALOG_ENTER(idinputdialog) if ((Am->Data.InputDialogData.TypeInputDialogAction==ida_ENTER_DIALOG) && (Am->Data.InputDialogData.IDInputDialog==idinputdialog)) {
#define END_INPUTDIALOG_ENTER() }

#define BEGIN_INPUTDIALOG_ESCAPE(idinputdialog) if ((Am->Data.InputDialogData.TypeInputDialogAction==ida_ESCAPE_DIALOG) && (Am->Data.InputDialogData.IDInputDialog==idinputdialog)) {
#define END_INPUTDIALOG_ESCAPE() }

#endif	
#endif