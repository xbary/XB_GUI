#include <xb_board.h>
#ifdef XB_GUI
#include "xb_GUI.h"
#include "xb_GUI_Gadget.h"


TGADGETMenu* GUI_menuhandle0_main;

DEFLIST_VAR(TWindowClass, WindowsList)
TWindowClass *CurrentWindowRepaint = NULL;
TWindowClass *WindowSetActivate = NULL;
bool DoClearDesktop = false;
bool GUI_Visible = false;


uint32_t GUI_DebugIndx = 0;

typedef enum {
	wrsOFF,
	wrsStartRepaint,
	wrsStopRepaint,
	wrsCheckRepaint,
	wrsNext,
	wrsRepaintWindowStep1,
	wrsRepaintWindowStep2,
	wrsRepaintWindowStep3,
	wrsRepaintWindowData,
	wrsRepaintWindowBorder,
	wrsClearDesktop

} TWindowRepaintStep;

TWindowRepaintStep  WindowRepaintStep = wrsOFF;

typedef enum { coAll=0, coAllWindowsPos=1, coWindowPos=2 } TConfigurationOption;

void GUI_LoadConfiguration(TConfigurationOption Aco = coAll, TWindowClass* Awc = NULL)
{
#ifdef XB_PREFERENCES
	switch ((uint8_t)Aco)
	{
	case coAll:
	{
		if (board.PREFERENCES_BeginSection("GUI"))
		{
			board.PREFERENCES_EndSection();
		}

	}
	case coAllWindowsPos:
	{
		TWindowClass* WC = WindowsList;
		while (WC != NULL)
		{
			GUI_LoadConfiguration(coWindowPos, WC);
			WC = WC->Next;
		}
		break;
	}
	case coWindowPos:
	{
		if (Awc != NULL)
		{
			String tn = "";
			if (board.SendMessage_GetTaskNameString(Awc->taskdef, tn))
			{
				tn += "G" + String(Awc->ID);

				if (board.PREFERENCES_BeginSection(tn))
				{
					//board.Log(String("GET "+tn).c_str(), true, true, tlWarn);
					Awc->X = board.PREFERENCES_GetINT16("X", Awc->X);
					Awc->Y = board.PREFERENCES_GetINT16("Y", Awc->Y);
					board.PREFERENCES_EndSection();
				}
			}
		}
		break;
	}
	default: break;
	}
#endif
}

void GUI_SaveConfiguration(TConfigurationOption Aco, TWindowClass* Awc = NULL)
{
#ifdef XB_PREFERENCES
	switch ((uint8_t)Aco)
	{
	case coAll:
	{
		if (board.PREFERENCES_BeginSection("GUI"))
		{
			//board.PREFERENCES_PutBool("ConInWin", xb_board_ConsoleInWindow);
			board.PREFERENCES_EndSection();
		}

	}
	case coAllWindowsPos:
	{
		TWindowClass* WC = WindowsList;
		while (WC != NULL)
		{
			GUI_SaveConfiguration(coWindowPos, WC);
			WC = WC->Next;
		}
		break;
	}
	case coWindowPos:
	{
		if (Awc != NULL)
		{
			String tn = "";
			if (board.SendMessage_GetTaskNameString(Awc->taskdef, tn))
			{
				tn += "G" + String(Awc->ID);
				if (board.PREFERENCES_BeginSection(tn))
				{
					//board.Log(String("SET " + tn).c_str(), true, true, tlWarn);
					board.PREFERENCES_PutINT16("X", Awc->X);
					board.PREFERENCES_PutINT16("Y", Awc->Y);
					board.PREFERENCES_EndSection();
				}
			}
		}
		break;
	}
	default: break;
	}
#endif
}

void GUI_ResetConfiguration()
{
#ifdef XB_PREFERENCES
	TWindowClass* WC = WindowsList;
	while (WC != NULL)
	{
		String tn = "";
		if (board.SendMessage_GetTaskNameString(WC->taskdef, tn))
		{
			tn = tn + "G" + String(WC->ID);
			if (board.PREFERENCES_BeginSection(tn))
			{
				//board.Log(String("RESET " + tn).c_str(), true, true, tlWarn);
				board.PREFERENCES_CLEAR();
				board.PREFERENCES_EndSection();
			}
		}
		WC = WC->Next;
	}
#endif
}



#pragma region WINDOWCLASS

TWindowClass::TWindowClass(TTaskDef *ATaskDef, int8_t AWindowID, bool Aactive, bool Aescapeclose, Tx Ax, Ty Ay)
{
#ifdef XB_GUI
	DoClose = false;
	TextWordWrap = false;
	LastActive = NULL;
	ModalWin = NULL;
	Active = false;
	IsInit = false;
	Visible = false;
	ADD_TO_LIST(WindowsList, TWindowClass);

	IsInit = false;
	taskdef = ATaskDef;
	ID = AWindowID;
	EscapeClose = Aescapeclose;
	
	BorderTextActiveColor = tfcCyan;
	BorderTextUnActiveColor = tfcWhite;
	TextColor = tfcWhite;
	TextBackgroundColor = tbcBlack;

	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_WINDOW;
	mb.Data.WindowData.WindowAction = waCreate;
	mb.Data.WindowData.ID = AWindowID;
	mb.Data.WindowData.ActionData.Create.X = Ax;
	mb.Data.WindowData.ActionData.Create.Y = Ay;
	mb.Data.WindowData.ActionData.Create.Width = 0;
	mb.Data.WindowData.ActionData.Create.Height = 0;

	if (board.DoMessage(&mb, true, NULL, ATaskDef))
	{
		if (((mb.Data.WindowData.ActionData.Create.X == 0) &&
			(mb.Data.WindowData.ActionData.Create.Y == 0) &&
			(mb.Data.WindowData.ActionData.Create.Width == 0) &&
			(mb.Data.WindowData.ActionData.Create.Height == 0)) ||
			(mb.Data.WindowData.ActionData.Create.Width < 3) ||
			(mb.Data.WindowData.ActionData.Create.Height < 3))
		{
			board.Log("Create window size error.",true,true,tlError);
		}
		else
		{
			if (mb.Data.WindowData.ActionData.Create.X == WINDOW_POS_X_DEF) mb.Data.WindowData.ActionData.Create.X = Ax;
			if (mb.Data.WindowData.ActionData.Create.Y == WINDOW_POS_Y_DEF) mb.Data.WindowData.ActionData.Create.Y = Ay;

			BorderSize.Left = 1;
			BorderSize.Right = 1;
			BorderSize.Bottom = 1;
			BorderSize.Top = 1;

			CursorXY.X = 0;
			CursorXY.Y = 0;
			ClearChar = ' ';
			isBeginDraw = 0;
			PaintOnBorder = 0;
			ReverseChar = false;
			BoldChar = false;
			IsInit = true;
			Visible = true;

			X = mb.Data.WindowData.ActionData.Create.X;
			Y = mb.Data.WindowData.ActionData.Create.Y;
			GUI_LoadConfiguration(coWindowPos, this);
			mb.Data.WindowData.ActionData.Create.X=X;
			mb.Data.WindowData.ActionData.Create.Y=Y;
			SetWindowPositionFrom(&mb.Data.WindowData);

			if (Aactive)
			{
				TWindowClass *wl = WindowsList;
				while (wl != NULL)
				{
					if (wl->Active)
					{
						LastActive = wl;
						wl->RepaintBorderCounter++;
						wl->Active = false;
						break;
					}
					wl = wl->Next;
				}
				Active = true;
			}

			RepaintCounter = 1;
			RepaintDataCounter = 1;
			board.TriggerInterrupt(&XB_GUI_DefTask);

			{
				uint16_t w = GUI_GetWidthDesktop() + 1;
				uint16_t h = GUI_GetHeightDesktop() + 1;
				if (ScreenText.DesktopWidth < w) ScreenText.DesktopWidth = w;
				if (ScreenText.DesktopHeight < h) ScreenText.DesktopHeight = h;
			}
		}
	}
	else
	{
		board.Log("Task not answer to create window.",true,true,tlError);
	}
#endif
}


void TWindowClass::SetWindowPositionFrom(TWindowData *Awd)
{
	if (Awd != NULL)
	{
		X = Awd->ActionData.Create.X;
		Y = Awd->ActionData.Create.Y;
		Width = Awd->ActionData.Create.Width;
		Height = Awd->ActionData.Create.Height;
		
	}

	if (X == WINDOW_POS_X_CENTER)
	{
		X = (ScreenText.ScreenWidth / 2) - (Width / 2);
	}
	if (Y == WINDOW_POS_Y_CENTER)
	{
		Y = (ScreenText.ScreenHeight / 2) - (Height / 2);
	}

	if (X == WINDOW_POS_LAST_RIGHT)
	{
		TWindowClass* wc = WindowsList;
		X = 0;
		while (wc != NULL)
		{
			if (wc != this)
			{
				if (X < wc->WindowRect.Right + 1)
				{
					X = wc->WindowRect.Right + 1;
				}
			}
			wc = wc->Next;
		}
	}
	if (Y == WINDOW_POS_LAST_BOTTOM)
	{
		TWindowClass* wc = WindowsList;
		Y = 0;
		while (wc != NULL)
		{
			if (wc != this)
			{
				if (Y < wc->WindowRect.Bottom + 1)
				{
					Y = wc->WindowRect.Bottom + 1;
				}
			}
			wc = wc->Next;
		}
	}

	if (X == WINDOW_POS_LAST_RIGHT_ACTIVE)
	{
		TWindowClass* wc = WindowsList;
		X = 0;
		while (wc != NULL)
		{
			if (wc != this)
			{
				if (wc->Active)
				{
					X = wc->WindowRect.Right+1;
					break;
				}
			}
			wc = wc->Next;
		}
	}
	if (Y == WINDOW_POS_LAST_BOTTOM_ACTIVE)
	{
		TWindowClass* wc = WindowsList;
		Y = 0;
		while (wc != NULL)
		{
			if (wc != this)
			{
				if (wc->Active)
				{
					Y = wc->WindowRect.Bottom + 1;
					break;
				}
			}
			wc = wc->Next;
		}
	}

	if (X < 0) X = 0;
	if (Y < 0) Y = 0;

	if (Width > ScreenText.ScreenWidth) Width = ScreenText.ScreenWidth;
	if (Height > ScreenText.ScreenHeight) Height = ScreenText.ScreenHeight;

	if (X + Width > ScreenText.ScreenWidth) X = ScreenText.ScreenWidth - Width;
	if (Y + Height > ScreenText.ScreenHeight) Y = ScreenText.ScreenHeight - Height;


	WindowRect.Left = X;
	WindowRect.Top = Y;
	WindowRect.Right = X + Width - 1;
	WindowRect.Bottom = Y + Height - 1;
}

TWindowClass::~TWindowClass()
{
	// Wyciêcie okna z listy
	DELETE_FROM_LIST(WindowsList);
}

void TWindowClass::Close()
{
	DoClose = true;
}

void TWindowClass::GetCaptionWindow(String &Astr)
{
#ifdef XB_GUI
	TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_WINDOW;
	mb.Data.WindowData.WindowAction = waGetCaptionWindow;
	mb.Data.WindowData.ID = ID;
	mb.Data.WindowData.ActionData.GetCaption.PointerString = &Astr;
	board.DoMessage(&mb, true, NULL, taskdef);
#endif
}

void TWindowClass::BeginDraw(bool Apaintonborder)
{

	if (Apaintonborder)
	{
		PaintOnBorder++;
	}

	if (isBeginDraw == 0)
	{
		isBeginDraw++;
		board.NoTxCounter++;
		ScreenText.SaveCursorPosition();
		ScreenText.HideCursor();
		ScreenText.SetCharMode_Normal();
		if (ReverseChar) ScreenText.SetCharMode_Reverse();
		if (BoldChar) ScreenText.SetCharMode_Bold();
		ScreenText.SetForegroundColor(TextColor);
		ScreenText.SetBackgroundColor(TextBackgroundColor);

		GoToXY(CursorXY.X, CursorXY.Y);

	}
	else
	{
		isBeginDraw++;
	}
}

void TWindowClass::EndDraw(bool Apaintonborder)
{
	if (isBeginDraw > 0)
	{
		isBeginDraw--;
		if (isBeginDraw == 0)
		{
			ScreenText.RestoreCursorPosition();
			ScreenText.ShowCursor();
			board.NoTxCounter--;
		}
	}

	if (Apaintonborder)
	{
		PaintOnBorder--;
	}

}

void TWindowClass::SetTextColor(TTextForegroundColor ATextColor)
{
	TextColor = ATextColor;
	BeginDraw();
	if (isBeginDraw > 1)
	{
		ScreenText.SetForegroundColor(TextColor);
	}
	EndDraw();

}

void TWindowClass::SetTextBackgoundColor(TTextBackgroundColor ATextBackgroundColor)
{
	TextBackgroundColor = ATextBackgroundColor;
	BeginDraw();
	if (isBeginDraw > 1)
	{
		ScreenText.SetBackgroundColor(TextBackgroundColor);
	}
	EndDraw();
}

void TWindowClass::Clear(char ch)
{
	BeginDraw();
	GoToXY(0, 0);

	Ty h = Height;
	Tx w = Width;
	if (ch == 0) ch = ClearChar;

	if (PaintOnBorder == 0)
	{
		w = w - (BorderSize.Left + BorderSize.Right);
		h = h - (BorderSize.Top + BorderSize.Bottom);
	}


	for (Ty y = 0; y < h; y++)
	{
		GoToXY(0, y);
		for (Tx x = 0; x < w; x++)
		{
			PutChar(ch);
		}
	}
	EndDraw();
}

void TWindowClass::ClearWindow()
{
	TTextForegroundColor LastTextColor = TextColor;
	TTextBackgroundColor LastTextBackgroundColor = TextBackgroundColor;
	bool LastReverseChar = ReverseChar;

	TextColor = tfcWhite;
	TextBackgroundColor = tbcBlack;
	ReverseChar = false;

	BeginDraw();
	Clear();
	EndDraw();

	ReverseChar = LastReverseChar;
	TextBackgroundColor = LastTextBackgroundColor;
	TextColor = LastTextColor;
}

void TWindowClass::GoToXY(Tx Ax, Ty Ay)
{
	Tx X;
	Ty Y;
	bool isdraw = (isBeginDraw == 0);

	CursorXY.X = Ax;
	CursorXY.Y = Ay;

	if (isdraw) BeginDraw();
	if (PaintOnBorder==0)
	{
		 X = WindowRect.Left + Ax + BorderSize.Left;
		 Y = WindowRect.Top + Ay + BorderSize.Top;
	}
	else
	{
		X = WindowRect.Left + Ax;
		Y = WindowRect.Top + Ay;
	}
	ScreenText.GotoXY(X, Y);
	if (isdraw) EndDraw();

}

bool TWindowClass::XYisShow(Tx Ax, Ty Ay)
{
	TWindowClass *w = this;

	Tx X;
	Ty Y;

	if (PaintOnBorder == 0)
	{
		X = WindowRect.Left + Ax + BorderSize.Left;
		Y = WindowRect.Top + Ay + BorderSize.Top;
	}
	else
	{
		X = WindowRect.Left + Ax;
		Y = WindowRect.Top + Ay;
	}



	if (PaintOnBorder == 0)
	{
		if (
			(X >= (w->WindowRect.Left + w->BorderSize.Left)) &&
			(X <= (w->WindowRect.Right - w->BorderSize.Right)) &&
			(Y >= (w->WindowRect.Top + w->BorderSize.Top)) &&
			(Y <= (w->WindowRect.Bottom - w->BorderSize.Bottom))
			)
		{
			w = w->Next;
			while (w != NULL)
			{
				if (w->Visible)
				{
					if ((X >= w->WindowRect.Left) && (X <= w->WindowRect.Right) && (Y >= w->WindowRect.Top) && (Y <= w->WindowRect.Bottom))
					{
						return false;
					}
				}
				w = w->Next;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		w = w->Next;
		while (w != NULL)
		{
			if (w->Visible)
			{
				if ((X >= w->WindowRect.Left) && (X <= w->WindowRect.Right) && (Y >= w->WindowRect.Top) && (Y <= w->WindowRect.Bottom))
				{
					return false;
				}
			}
			w = w->Next;
		}
		return true;
	}
}

void TWindowClass::PutChar(char Ach)
{
	static bool LastCharHide = true;
	BeginDraw();
	if (XYisShow(CursorXY.X, CursorXY.Y))
	{
		if (Ach == 0x0a)
		{
			CursorXY.X = 0;
			GoToXY(CursorXY.X, CursorXY.Y);
		}
		else if (Ach == 0x0d)
		{
			CursorXY.X = 0;
			CursorXY.Y++;
			GoToXY(CursorXY.X, CursorXY.Y);
		}
		else
		{
			if (LastCharHide) GoToXY(CursorXY.X, CursorXY.Y);
			CursorXY.X++;
			PutGui(Ach);
			if (TextWordWrap)
			{
				if (CursorXY.X >= Width-2) 
				{
					CursorXY.X = 0;
					CursorXY.Y++;
					GoToXY(CursorXY.X, CursorXY.Y);
				}					
			}
		}
		LastCharHide = false;
	}
	else
	{
		LastCharHide = true;
		if (Ach == 0x0a)
		{
			CursorXY.X = 0;
			
		}
		else if (Ach == 0x0d)
		{
			CursorXY.X = 0;
			CursorXY.Y++;
			
		}
		else
		{
			CursorXY.X++;
			if (TextWordWrap)
			{
				if (CursorXY.X >= Width-2) 
				{
					CursorXY.X = 0;
					CursorXY.Y++;
					GoToXY(CursorXY.X, CursorXY.Y);
				}					
			}
		}
	}
	EndDraw();
}

void TWindowClass::SetReverseChar()
{
	BeginDraw();
	ReverseChar = true;
	ScreenText.SetCharMode_Reverse();
	EndDraw();
}

void TWindowClass::SetBoldChar()
{
	BeginDraw();
	ReverseChar = true;
	BoldChar = true;
	ScreenText.SetCharMode_Bold();
	EndDraw();
}

void TWindowClass::SetNormalChar()
{
	ReverseChar = false;
	BoldChar = false;
	BeginDraw();
	ScreenText.SetCharMode_Normal();
	ScreenText.SetForegroundColor(TextColor);
	ScreenText.SetBackgroundColor(TextBackgroundColor);
	EndDraw();
}

void TWindowClass::PutStr(Tx Ax,Ty Ay,const char *Astr, Tx Awidth, char Achfill, TTextAlignment Atextalignment)
{
	BeginDraw();
	GoToXY(Ax, Ay);
	PutStr(Astr, Awidth, Achfill, Atextalignment);
	EndDraw();
}

void TWindowClass::PutStr(const char *Astr, Tx Awidth, char Achfill, TTextAlignment Atextalignment)
{
	BeginDraw();
	size_t l = StringLength((char *)Astr, 0);
		//strlen(Astr);

	if (Awidth == 0)
	{
		for (size_t i = 0; i < l; i++)
		{
			PutChar(Astr[i]);
		}
	}
	else
	{
		switch (Atextalignment)
		{
		case taLeft:
		{
			for (size_t i = 0; i < l; i++)
			{
				if (i >= Awidth) break;
				PutChar(Astr[i]);
			}

			if (l < Awidth)
			{
				l = Awidth - l;
				for (size_t i = 0; i < l; i++)
				{
					PutChar(Achfill);
				}
			}

			break;
		}
		case taCentre:
		{
			int xt;

			xt = (Awidth / 2) - (l / 2);

			for (int i = 0; i < xt; i++)
			{
				PutChar(Achfill);
			}

			for (size_t i = 0; i < l; i++)
			{
				if (xt >= 0)
				{
					PutChar(Astr[i]);
				}
				xt++;
				if (xt >= Awidth) break;
			}

			if (xt < Awidth)
			{
				xt = Awidth - xt;
				for (size_t i = 0; i < xt; i++)
				{
					PutChar(Achfill);
				}
			}


			break;
		}
		case taRight:
		{
			int xt;

			xt = Awidth - l;

			if (xt > 0)
			{
				for (int i = 0; i < xt; i++)
				{
					PutChar(Achfill);
				}
			}

			for (size_t i = 0; i < l; i++)
			{
				if (xt >= 0)
				{
					PutChar(Astr[i]);
				}
				xt++;
			}

			break;
		}

		default: break;
		}


	}

	EndDraw();
}

void TWindowClass::SetActive(void)
{
	TWindowClass *w = WindowsList;

	while (w != NULL)
	{
		if (w != this)
		{
			if (w->Active)
			{
				w->Active = false;
				w->RepaintBorderCounter++;
			}
		}
		w = w->Next;
	}

	Active = true;
	RepaintBorderCounter++;

	if (Next != NULL)
	{
		MOVE_STR_TO_ENDLIST(WindowsList, TWindowClass, this);
		RepaintCounter++;
		RepaintDataCounter++;
		board.TriggerInterrupt(&XB_GUI_DefTask);
	}

}

void TWindowClass::DoRepaint()
{
#ifdef XB_GUI	
	TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_WINDOW;
	mb.Data.WindowData.WindowAction = waRepaint;
	mb.Data.WindowData.ID = ID;
	board.DoMessage(&mb, true,NULL,taskdef);
#endif
}

void TWindowClass::Repaint()
{
	RepaintCounter++;
	board.TriggerInterrupt(&XB_GUI_DefTask);
}

void TWindowClass::RepaintData()
{
	RepaintDataCounter++;
	board.TriggerInterrupt(&XB_GUI_DefTask);
}

void TWindowClass::DoRepaintData()
{
#ifdef XB_GUI
	TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_WINDOW;
	mb.Data.WindowData.WindowAction = waRepaintData;
	mb.Data.WindowData.ID = ID;
	board.DoMessage(&mb, true, NULL, taskdef);
#endif
}

void TWindowClass::PaintBorder()
{
	TTextForegroundColor LastTextColor = TextColor;
	TTextBackgroundColor LastTextBackgroundColor = TextBackgroundColor;
	bool LastTextWordWrap = TextWordWrap;

	TextWordWrap = false;

	if (Active)
	{
		TextColor = BorderTextActiveColor;
	}
	else
	{
		TextColor = BorderTextUnActiveColor;
	}
	TextBackgroundColor = tbcBlack;

	BeginDraw(true);
	GoToXY(0, 0);
	{ // Rysowanie caption
		
		String str = "";
		GetCaptionWindow(str);

		SetReverseChar();
		PutChar('/');
		PutStr(str.c_str(), Width-2, ' ', taCentre);
		PutChar('\\');
		SetNormalChar();
	}
	int h = Height - 1;
	int w = Width - 1;
	for (int y = 1; y < h; y++)
	{
		GoToXY(0, y);
		PutChar('|');
		GoToXY(w, y);
		PutChar('|');
	}
	GoToXY(0, h);
	PutChar('\\');

	for (int x = 1; x < w; x++)
	{
		PutChar('_');
	}
	PutChar('/');
	SetNormalChar();
	EndDraw(true);

	TextWordWrap = LastTextWordWrap;
	TextBackgroundColor = LastTextBackgroundColor;
	TextColor = LastTextColor;

}

void TWindowClass::SetWindowSize(Tx Awidth, Ty Aheight)
{
	bool aclear = false;
	if (Awidth < Width) aclear = true;
	if (Aheight < Height) aclear = true;
	Width = Awidth;
	Height = Aheight;
	WindowRect.Right = WindowRect.Left + Width-1;
	WindowRect.Bottom = WindowRect.Top + Height-1;
	if (aclear)
	{
		GUI_ClearDesktop();
	}
	GUI_RepaintAllWindows();
}

void TWindowClass::SetWindowPos(Tx Ax, Ty Ay)
{
	TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_WINDOW;
	mb.Data.WindowData.WindowAction = waCreate;
	mb.Data.WindowData.ID = ID;
	mb.Data.WindowData.ActionData.Create.X = Ax;
	mb.Data.WindowData.ActionData.Create.Y = Ay;
	mb.Data.WindowData.ActionData.Create.Width = Width;
	mb.Data.WindowData.ActionData.Create.Height = Height;
	SetWindowPositionFrom(&mb.Data.WindowData);
	GUI_ClearDesktop();
	GUI_RepaintAllWindows();
}

#pragma endregion

int8_t GUI_GetUniqueIDWindow(TTaskDef *ATaskDef)
{
	TWindowClass *w;
	for (int8_t i = 0; i < 128; i++)
	{
		w = GUI_FindWindowByTaskDef(ATaskDef, i);
		if (w == NULL)
		{
			return i;
		}
	}
	return -1;
}

TWindowClass *GUI_FindWindowByTaskDef(TTaskDef *ATaskDef, int8_t AWindowID)
{
	TWindowClass *w = WindowsList;
	while (w != NULL)
	{
		if (w->taskdef == ATaskDef)
		{
			if (AWindowID == -1)
			{
				return w;
			}

			if (w->ID == AWindowID)
			{
				return w;
			}

		}
		w = w->Next;

	}
	return NULL;
}

TWindowClass *GUI_FindWindowByActive()
{
	TWindowClass *w = WindowsList;
	while (w != NULL)
	{
		if (w->Active)
		{
			return w;
			
		}
		w = w->Next;
	}
	return NULL;
}

TWindowClass *GUI_WindowCreate(TTaskDef *ATaskDef, int8_t AWindowID, bool Aactive, TWindowClass *Amodalwin,bool Aescapeclose,Tx Ax,Ty Ay)
{
	TWindowClass *w = GUI_FindWindowByTaskDef(ATaskDef, AWindowID);
	if (w == NULL)
	{
		w = new TWindowClass(ATaskDef, AWindowID,Aactive,Aescapeclose,Ax,Ay);
		if (Amodalwin != NULL)  Amodalwin->ModalWin = w;
	}
	else
	{
		w->Repaint();
		w->RepaintData();
		
	}
	return w;

}

void GUI_WindowDestroy(TWindowClass **Awindowclass)
{
	if (Awindowclass != NULL)
	{
		if (*Awindowclass != NULL)
		{
			(*Awindowclass)->DoClose = false;
			// oznajmienie zadania ¿e jego okno zosta³o skasowane
#ifdef XB_GUI
			TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
			mb.IDMessage = IM_WINDOW;
			mb.Data.WindowData.WindowAction = waDestroy;
			mb.Data.WindowData.ID = (*Awindowclass)->ID;
			board.DoMessage(&mb, true, NULL, (*Awindowclass)->taskdef);
#endif

			
			// NULLowanie wskaŸnika ostatnio aktywnego okna
			TWindowClass *wl = WindowsList;
			while (wl != NULL)
			{
				if (wl->ModalWin == *Awindowclass)
				{
					wl->ModalWin = NULL;
				}

				if (wl->LastActive == *Awindowclass)
				{
					wl->LastActive = NULL;
				}
				wl = wl->Next;
			}

			// sprawdzenie czy aktywne, jeœli by³o poprzednio aktywne to aktywuj
			if ((*Awindowclass)->Active)
			{
				(*Awindowclass)->Active = false;
				if ((*Awindowclass)->LastActive != NULL)
				{
					WindowSetActivate = (*Awindowclass)->LastActive;
				}
			}
			
			if (*Awindowclass!=NULL)
			delete(*Awindowclass);
			board.SendMessage_FREEPTR(*Awindowclass);
			*Awindowclass = NULL;
			

			{
				ScreenText.DesktopWidth = 0;
				ScreenText.DesktopHeight = 0;
				uint16_t w = GUI_GetWidthDesktop() + 1;
				uint16_t h = GUI_GetHeightDesktop() + 1;
				if (ScreenText.DesktopWidth < w) ScreenText.DesktopWidth = w;
				if (ScreenText.DesktopHeight < h) ScreenText.DesktopHeight = h;
			}


			if (WindowsList == NULL)
			{
				GUI_ClearScreen();
				
			}
			else
			{
				GUI_ClearDesktop();
				GUI_RepaintAllWindows();				
			}

		}
	}
}

void GUI_RepaintAllWindows()
{
	TWindowClass *wl = WindowsList;
	while (wl != NULL)
	{
		wl->RepaintCounter++;
		wl->RepaintDataCounter++;
		wl = wl->Next;
	}
	board.TriggerInterrupt(&XB_GUI_DefTask);
}

void GUI_Show(void)
{
	WindowRepaintStep = wrsStartRepaint;
	ScreenText.Clear();
	GUI_RepaintAllWindows();
	ScreenText.GotoXY(0, ScreenText.DesktopHeight);
	GUI_Visible = true;
}

void GUI_Hide(void)
{
	WindowRepaintStep = wrsStopRepaint;
	ScreenText.Clear();
	GUI_RepaintAllWindows();
	GUI_Visible = false;
}

void GUI_ClearScreen(void)
{
	ScreenText.GotoXY(0, 0);
	ScreenText.Clear();
	GUI_RepaintAllWindows();
}

void GUI_PutChar(char Ach,TPoint &Axy)
{
	static bool LastCharHide = true;
	
	if (GUI_XYisShow(Axy.X, Axy.Y))
	{
		if (Ach == 0x0a)
		{
			Axy.X = 0;
			ScreenText.GotoXY(Axy.X, Axy.Y);
		}
		else if (Ach == 0x0d)
		{
			Axy.X = 0;
			Axy.Y++;
			ScreenText.GotoXY(Axy.X, Axy.Y);
		}
		else
		{
			if (LastCharHide) ScreenText.GotoXY(Axy.X, Axy.Y);
			Axy.X++;
			PutGui(Ach);
		}
		LastCharHide = false;
	}
	else
	{
		LastCharHide = true;
		if (Ach == 0x0a)
		{
			Axy.X = 0;

		}
		else if (Ach == 0x0d)
		{
			Axy.X = 0;
			Axy.Y++;

		}
		else
		{
			Axy.X++;

		}
	}
	
}

bool GUI_XYisShow(Tx Ax, Ty Ay)
{
	TWindowClass *w = WindowsList;
	if (w == NULL) return true;
	while (w != NULL)
	{
		if (w->Visible)
		{
			if ((Ax >= w->WindowRect.Left) && (Ax <= w->WindowRect.Right) && (Ay >= w->WindowRect.Top) && (Ay <= w->WindowRect.Bottom))
			{
				return false;
			}
		}
		w = w->Next;
	}
	return true;
}

int GUI_GetHeightDesktop(void)
{
	TWindowClass *w = WindowsList;
	int y = 0;
	while (w != NULL)
	{
		if (w->IsInit)
		{
			if (w->WindowRect.Bottom > y) y = w->WindowRect.Bottom;
		}
		w = w->Next;
	}
	return y;
}

int GUI_GetWidthDesktop(void)
{
	TWindowClass *w = WindowsList;
	int x = 0;
	while (w != NULL)
	{
		if (w->IsInit)
		{
			if (w->WindowRect.Right > x) x = w->WindowRect.Right;
		}
		w = w->Next;
	}
	return x;
}

void GUI_ClearDesktop(char Ach)
{
	DoClearDesktop = true;
	if (WindowRepaintStep == wrsCheckRepaint)
		WindowRepaintStep = wrsClearDesktop;
}

uint32_t GUI_DoWindowsRepaintStep(void)
{
	DEF_WAITMS_VAR(txcountcheck);
	static uint32_t LastTXCounter = 0;

	if (CurrentWindowRepaint != NULL)
	{
		if (CurrentWindowRepaint->DoClose)
		{
			GUI_WindowDestroy((TWindowClass**)&CurrentWindowRepaint);
			CurrentWindowRepaint = WindowsList;
			return 0;
		}

	}

	switch (WindowRepaintStep)
	{
	case wrsOFF:
	{
		if (CurrentWindowRepaint != NULL)
		{
			CurrentWindowRepaint = CurrentWindowRepaint->Next;
		}
		else
		{
			CurrentWindowRepaint = WindowsList;
		}
		break;
	}
	case wrsStartRepaint:
	{
		board.TXCounter = 0;

		CurrentWindowRepaint = WindowsList;
		if (CurrentWindowRepaint != NULL)
		{
			WindowRepaintStep = wrsCheckRepaint;
		}

		break;
	}
	case wrsStopRepaint:
	{
		ScreenText.Clear();
		ScreenText.GotoXY(0, 0);

		TWindowClass* w = WindowsList;
		while (w != NULL)
		{
			w->Close();
			w = w->Next;
		}

		WindowRepaintStep = wrsOFF;
		break;
	}
	case wrsNext:
	{
		if (LastTXCounter != board.TXCounter)
		{
			RESET_WAITMS(txcountcheck);
			LastTXCounter = board.TXCounter;
		}

		BEGIN_WAITMS(txcountcheck, 1500)
		{
			if (LastTXCounter > 0)
			{
				board.TXCounter = 0;
				LastTXCounter = 0;
				GUI_RepaintAllWindows();
			}
		}
		END_WAITMS(txcountcheck)

			if (CurrentWindowRepaint != NULL)
			{
				CurrentWindowRepaint = CurrentWindowRepaint->Next;
				while (CurrentWindowRepaint != NULL)
				{
					if (CurrentWindowRepaint->IsInit)
					{
						WindowRepaintStep = wrsCheckRepaint;
						break;
					}

					CurrentWindowRepaint = CurrentWindowRepaint->Next;
				}
			}
			else
			{
				CurrentWindowRepaint = WindowsList;
				if (CurrentWindowRepaint != NULL)
				{
					if (CurrentWindowRepaint->IsInit)
					{
						WindowRepaintStep = wrsCheckRepaint;
					}
					else
					{
						WindowRepaintStep = wrsNext;
					}
				}
				else
				{
					WindowRepaintStep = wrsStartRepaint;
				}
			}
		break;
	}
	case wrsClearDesktop:
	{
		if (DoClearDesktop)
		{
			board.NoTxCounter++;

			DoClearDesktop = false;

			if (WindowsList != NULL)
			{
				ScreenText.SaveCursorPosition();
				ScreenText.HideCursor();

				ScreenText.GotoXY(0, 0);
				TPoint XY;
				XY.X = 0;
				XY.Y = 0;


				for (int y = 0; y < ScreenText.ScreenHeight; y++)
				{
					for (int x = 0; x < ScreenText.ScreenWidth; x++)
					{
						GUI_PutChar(' ', XY);
					}
					GUI_PutChar(0x0d, XY);
				}
				ScreenText.ShowCursor();
				ScreenText.RestoreCursorPosition();
			}
			else
			{
				ScreenText.Clear();
			}
			board.NoTxCounter--;
		}
		WindowRepaintStep = wrsCheckRepaint;
		break;
	}
	case wrsCheckRepaint:
	{
		if (CurrentWindowRepaint == NULL)
		{
			WindowRepaintStep = wrsNext;
			break;
		}

		if (WindowSetActivate != NULL)
		{
			WindowSetActivate->SetActive();
			WindowSetActivate = NULL;
			break;
		}

		if (CurrentWindowRepaint->RepaintBorderCounter > 0)
		{
			WindowRepaintStep = wrsRepaintWindowBorder;
		}
		else if (CurrentWindowRepaint->RepaintCounter > 0)
		{
			WindowRepaintStep = wrsRepaintWindowStep1;
		}
		else if (CurrentWindowRepaint->RepaintDataCounter > 0)
		{
			WindowRepaintStep = wrsRepaintWindowData;
		}
		else if (DoClearDesktop)
		{
			WindowRepaintStep = wrsClearDesktop;
		}
		else
		{
			WindowRepaintStep = wrsNext;
		}

		break;
	}
	case wrsRepaintWindowStep1: // Czyszczenie okna
	{
		if (CurrentWindowRepaint == NULL)
		{
			WindowRepaintStep = wrsNext;
			break;
		}

		board.NoTxCounter++;

		CurrentWindowRepaint->ClearWindow();
		WindowRepaintStep = wrsRepaintWindowStep2;

		board.NoTxCounter--;
		break;
	}

	case wrsRepaintWindowStep2: // Rysowanie Ramy
	{
		if (CurrentWindowRepaint == NULL)
		{
			WindowRepaintStep = wrsNext;
			break;
		}

		board.NoTxCounter++;

		CurrentWindowRepaint->PaintBorder();
		//if (CurrentWindowRepaint->RepaintBorderCounter>0) 
		CurrentWindowRepaint->RepaintBorderCounter = 0;
		WindowRepaintStep = wrsRepaintWindowStep3;

		board.NoTxCounter--;
		break;
	}
	case wrsRepaintWindowStep3: // Wywo³anie odrysowania elementów GUI
	{
		if (CurrentWindowRepaint == NULL)
		{
			WindowRepaintStep = wrsNext;
			break;
		}

		board.NoTxCounter++;

		CurrentWindowRepaint->BeginDraw();
		CurrentWindowRepaint->DoRepaint();
		CurrentWindowRepaint->EndDraw();
		CurrentWindowRepaint->RepaintCounter = 0;
		WindowRepaintStep = wrsRepaintWindowData;

		board.NoTxCounter--;
		break;
	}
	case wrsRepaintWindowData: // Wywo³anie odrysowania zmiennych danych GUI
	{
		if (CurrentWindowRepaint == NULL)
		{
			WindowRepaintStep = wrsNext;
			break;
		}
		board.NoTxCounter++;

		CurrentWindowRepaint->BeginDraw();
		CurrentWindowRepaint->SetNormalChar();
		CurrentWindowRepaint->DoRepaintData();
		CurrentWindowRepaint->EndDraw();
		CurrentWindowRepaint->RepaintDataCounter = 0;
		WindowRepaintStep = wrsNext;
		//WindowRepaintStep = wrsCheckRepaint;

		board.NoTxCounter--;
		break;
	}
	case wrsRepaintWindowBorder: // Wywo³anie odrysowania samej ramki
	{
		if (CurrentWindowRepaint == NULL)
		{
			WindowRepaintStep = wrsNext;
			break;
		}
		board.NoTxCounter++;

		CurrentWindowRepaint->PaintBorder();

		CurrentWindowRepaint->RepaintBorderCounter = 0;
		//WindowRepaintStep = wrsClearDesktop;
		WindowRepaintStep = wrsCheckRepaint;
		board.NoTxCounter--;
		break;
	}


	default: WindowRepaintStep = wrsOFF; break;
	};
	return 0;
}

// --------------------------------------------------------------------------------------------------------------
void GUI_Setup(void)
{
	ScreenText.SetCharMode_Normal();
	ScreenText.SetBackgroundColor(tbcDefault);
	ScreenText.SetForegroundColor(tfcDefault);

	board.AddTask(&XB_GUIGADGET_DefTask);
}

void GUI_DoInterrupt(void)
{
	GUI_DoWindowsRepaintStep();
}

uint32_t GUI_DoLoop(void)
{
	return GUI_DoWindowsRepaintStep();
}

bool GUI_DoMessage(TMessageBoard *Am)
{
	bool res = false;
	
	switch (Am->IDMessage)
	{
	case IM_HANDLEPTR:
	{
		HANDLEPTR(GUI_menuhandle0_main);
		HANDLEPTR(CurrentWindowRepaint);
		HANDLEPTR(WindowSetActivate);
		res = true;
		break;
	}
	case IM_LOAD_CONFIGURATION:
	{
		GUI_LoadConfiguration();
		res = true;
		break;
	}
	case IM_SAVE_CONFIGURATION:
	{
		GUI_SaveConfiguration(coAll);
		res = true;
		break;
	}
	case IM_RESET_CONFIGURATION:
	{
		GUI_ResetConfiguration();
		res = true;
		break;
	}
	case IM_MENU:
	{
		OPEN_MAINMENU()
		{
			GUI_menuhandle0_main = GUIGADGET_CreateMenu(&XB_GUI_DefTask, 0,false,X,Y);
		}


		BEGIN_MENU(0, "GUI", WINDOW_POS_X_DEF, WINDOW_POS_Y_DEF, 32, MENU_AUTOCOUNT, 0, true)
		{
			BEGIN_MENUITEM("Snapshot all windows pos.", taLeft)
			{
				CLICK_MENUITEM()
				{
					board.Log("Save All Windows Position...", true, true);
					GUI_SaveConfiguration(coAllWindowsPos);
					board.Log("OK");
				}
			}
			END_MENUITEM()
			SEPARATOR_MENUITEM()
			CONFIGURATION_MENUITEMS()
		}
		END_MENU()


		res = true;
		break;
	}
	case IM_KEYBOARD:
	{
		switch (Am->Data.KeyboardData.KeyFunction)
		{
		case KF_F5:
			{
				GUI_ClearDesktop();
				GUI_RepaintAllWindows();
				break;
			}
		case KF_ESC:
			{
		
				TWindowClass *w = GUI_FindWindowByActive();
				if (w != NULL)
				{
					if (w->EscapeClose)				
					{
						w->Close();
						//GUI_WindowDestroy(&w);
						//GUI_ClearDesktop();
						//GUI_RepaintAllWindows();
						res=true;
					}
				}
				break;
			}
		case KF_TABNEXT:
			{
				TWindowClass *w = GUI_FindWindowByActive();

				while (w != NULL)
				{
					w = w->Next;
					if (w != NULL)
					{
						while (w->ModalWin != NULL)
						{
							w = w->ModalWin;
						}	
						w->SetActive();
						break;
					}
				}
		
				if (w == NULL)
				{
					w = WindowsList;
					while (w != NULL)
					{
				
						if (w != NULL)
						{
							while (w->ModalWin != NULL)
							{
								w = w->ModalWin;
							}	
							w->SetActive();
							break;
						}
						w = w->Next;
					}
				}
				break;
			}
		case KF_TABPREV:
			{
				TWindowClass *wa = GUI_FindWindowByActive();
				TWindowClass *w = wa;

				while (w != NULL)
				{
					if (wa == w) w = w->Prev;
				
					if (w != NULL)
					{
						if (w->ModalWin != NULL)
						{
							w = w->Prev;
						}	
						else
						{
							w->SetActive();
							break;
						}
					}
					else
					{
						w = WindowsList;
						while (w != NULL)
						{
							if (w->Next == NULL) break;
							w = w->Next;
						}
						if (w == NULL) break;
						if (w == wa) break;
					}
				}
				break;
			}
		case KF_CTRL_CURSORDOWN:
		{
			TWindowClass* wa = GUI_FindWindowByActive();

			if (wa != NULL)
			{
				wa->WindowRect.Top++;
				wa->SetWindowPos(wa->WindowRect.Left, wa->WindowRect.Top);
			}
			break;
		}
		case KF_CTRL_CURSORUP:
		{
			TWindowClass* wa = GUI_FindWindowByActive();

			if (wa != NULL)
			{
				if (wa->WindowRect.Top > 0)
				{
					wa->WindowRect.Top--;
					wa->SetWindowPos(wa->WindowRect.Left, wa->WindowRect.Top);
				}
			}
			break;
		}
		case KF_CTRL_CURSORRIGHT:
		{
			TWindowClass* wa = GUI_FindWindowByActive();

			if (wa != NULL)
			{
				wa->WindowRect.Left++;
				wa->SetWindowPos(wa->WindowRect.Left, wa->WindowRect.Top);
			}
			break;
		}
		case KF_CTRL_CURSORLEFT:
		{
			TWindowClass* wa = GUI_FindWindowByActive();

			if (wa != NULL)
			{
				if (wa->WindowRect.Left > 0)
				{
					wa->WindowRect.Left--;
					wa->SetWindowPos(wa->WindowRect.Left, wa->WindowRect.Top);
				}
			}
			break;
		}

		}
		res = true;
		break;
	}
	case IM_GET_TASKNAME_STRING:
	{
		GET_TASKNAME("GUI");
		res = true;
		break;
	}
	case IM_GET_TASKSTATUS_STRING:
	{
		GET_TASKSTATUS_ADDSTR("Wc(" + String(WindowsList_count) + ") V("+String(GUI_Visible)+")  ");
		res= true;
		break;
	}
	default: break;
	}
	return res;
}
// --------------------------------------------------------------------------------------------------------------

TTaskDef XB_GUI_DefTask = { 3, &GUI_Setup,&GUI_DoLoop,&GUI_DoMessage,&GUI_DoInterrupt };

#endif