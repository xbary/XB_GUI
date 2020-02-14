#include <xb_board.h>
#include <utils\cbufSerial.h>
#include "xb_ScreenText.h"

#ifndef VT100_SCREENWIDTH
#define VT100_SCREENWIDTH 195
#endif
#ifndef VT100_SCREENHEIGHT
#define VT100_SCREENHEIGHT 55
#endif

TScreenTextClass ScreenText(stUART1VT100);


//-------------------------------------------------------------------------------------------------------------

void PutGui(char Ach)
{
	static char PutGuiCh = 0;
	PutGuiCh = Ach;
	board.AllPutStreamGui(&PutGuiCh, 1);
}

void PutGui(const char *Atxt)
{
	board.AllPutStreamGui((void *)Atxt,strlen(Atxt));
}

//-------------------------------------------------------------------------------------------------------------
TScreenTextClass::TScreenTextClass(TScreenType AType)
{
	Type = AType;
#ifdef SCREENTEXT_TYPE_BOARDLOG
	RegionClip = { 0,0,VT100_SCREENWIDTH,VT100_SCREENHEIGHT};
	ScreenWidth = VT100_SCREENWIDTH;
	ScreenHeight = VT100_SCREENHEIGHT;
	RealCurrentX = 0;
	RealCurrentY = 0;
	VisibleCursorStatus = 0;
#endif
}
//-------------------------------------------------------------------------------------------------------------
TScreenTextClass::~TScreenTextClass()
{

}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::Clear(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
		uint32_t ltx = board.TXCounter;
		PutGui(VT100_CLS);
		board.TXCounter=ltx;

		RealCurrentX = 0;
		RealCurrentY = 0;
		GotoXY(0, 0);
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SetCharMode_Normal(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	uint32_t ltx = board.TXCounter;
	PutGui(VT100_OFF);
	board.TXCounter = ltx;
//	SetForegroundColor(CurrentForegroundColor);
//	SetBackgroundColor(CurrentBackgroundColor);

#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SetCharMode_Bold(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	uint32_t ltx = board.TXCounter;
	PutGui(VT100_BOLD);
	board.TXCounter = ltx;
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SetCharMode_Blink(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	uint32_t ltx = board.TXCounter;
	PutGui(VT100_BLINK);
	board.TXCounter = ltx;
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SetCharMode_Reverse(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	uint32_t ltx = board.TXCounter;
	PutGui(VT100_REVERSE);
	board.TXCounter = ltx;
#endif
}
//-------------------------------------------------------------------------------------------------------------
bool TScreenTextClass::PutChar(char Achr)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	if (InRegionClip(RealCurrentX, RealCurrentY))
	{
		uint32_t ltx = board.TXCounter;
		PutGui(Achr); RealCurrentX++;
		board.TXCounter=ltx;

		return true;
	}
	else
	{
		return false;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
bool TScreenTextClass::PutChar(int16_t Ax, int16_t Ay,char Achr)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	if (GotoXY(Ax, Ay))
	{
		uint32_t ltx = board.TXCounter;
		PutGui(Achr); RealCurrentX++;
		board.TXCounter=ltx;
		return true;
	}
	else
	{
		return false;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
bool TScreenTextClass::InRegionClip(int16_t Arealx, int16_t Arealy)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG

	if ((Arealx >= RegionClip.X) && (Arealy >= RegionClip.Y))
	{
		if ((Arealx < RegionClip.X + RegionClip.Width) && (Arealy < RegionClip.Y + RegionClip.Height))
		{
			return true;
		}
	}
	return false;
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SetRegionClip(TTextRect *Arectclip)
{
	if (Arectclip==NULL)
	{ 
		RegionClip = { 0,0,255,255 };
	}
	else
	{
		RegionClip = *Arectclip;
		//GotoXY(0, 0);
	}
}
//-------------------------------------------------------------------------------------------------------------
bool TScreenTextClass::GotoXY(int16_t Ax, int16_t Ay)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{

		if ((Ax < RegionClip.Width) && (Ay < RegionClip.Height))
		{
			uint32_t ltx = board.TXCounter;
			RealCurrentX = Ax + RegionClip.X;
			RealCurrentY = Ay + RegionClip.Y;
			{
				if ((RealCurrentY >= 0) && (RealCurrentX >= 0))
				{
					char cbuf[16] = "\033[";
					uint8_t i = strlen(cbuf);
					i += inttoa(RealCurrentY + 1, &cbuf[i]);
					cbuf[i++] = ';';
					i += inttoa(RealCurrentX + 1, &cbuf[i]);
					cbuf[i++] = 'H';
					cbuf[i++] = 0;
					//String cbuf = FSS("\033[") + String(RealCurrentY + 1) + ';' + String(RealCurrentX + 1) + 'H';
					PutGui(cbuf);
				}
			}
			board.TXCounter = ltx;
			return true;
		}
		else
		{
			return false;
		}
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::PutText(const char *Atxt)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	uint32_t ltx = board.TXCounter;

	size_t l = strlen(Atxt);
	for (int i = 0; i < l; i++)
	{
		if (InRegionClip(RealCurrentX, RealCurrentY))
		{
			switch (Atxt[i])
			{
				
			default:
			{
				PutGui(Atxt[i]);
				RealCurrentX++;
				break;
			}
			}
		}
		else
		{

			RealCurrentX++;

		}
	}
	board.TXCounter = ltx;
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::PutText(int16_t Ax, int16_t Ay,const char *Atxt)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	uint32_t ltx = board.TXCounter;
	if (GotoXY(Ax, Ay))
	{
		PutText(Atxt);
	}
	board.TXCounter=ltx;
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::PutText(int16_t Ax, int16_t Ay, const char *Atxt,int Awidth,char Afillchar)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	uint32_t ltx = board.TXCounter;
	if (GotoXY(Ax, Ay))
	{
		PutText(Atxt);
		Awidth -= strlen(Atxt);
		while (Awidth > 0)
		{
			PutChar(Afillchar);
			Awidth--;
		}
	}
	board.TXCounter=ltx;
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SetForegroundColor(TTextForegroundColor ATextForegroundColor)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		CurrentForegroundColor = ATextForegroundColor;
		uint32_t ltx = board.TXCounter;
		{
			char cbuf[16] = "\033[3";
			uint8_t i = strlen(cbuf);
			i += inttoa(ATextForegroundColor, &cbuf[i]);
			cbuf[i++] = 'm';
			cbuf[i++] = 0;

//			String cbuf = FSS("\033[3") + String((uint8_t)ATextForegroundColor) + 'm';
			PutGui(cbuf);
		}
		board.TXCounter = ltx;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SetBackgroundColor(TTextBackgroundColor ATextBackgroundColor)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		CurrentBackgroundColor = ATextBackgroundColor;
		uint32_t ltx = board.TXCounter;
		{
			char cbuf[16] = "\033[4";
			uint8_t i = strlen(cbuf);
			i += inttoa(ATextBackgroundColor, &cbuf[i]);
			cbuf[i++] = 'm';
			cbuf[i++] = 0;

//			String cbuf = FSS("\033[4") + String((uint8_t)ATextBackgroundColor) + 'm';
			PutGui(cbuf);
		}
		board.TXCounter = ltx;
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::FillRect(TTextRect *Arect, char Achr)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		uint32_t ltx = board.TXCounter;
		for (int16_t yi = 0; yi < Arect->Height; yi++)
		{
			if (GotoXY(Arect->X, Arect->Y + yi))
			{
				for (int16_t xi = 0; xi < Arect->Width; xi++)
				{
					PutGui(Achr);
				}
			}
		}
		board.TXCounter = ltx;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::FrameCaption(TTextRect *Arect,const char *Atxt)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		uint32_t ltx = board.TXCounter;

		TTextRect lr = RegionClip;
		SetRegionClip(NULL);
		SetCharMode_Reverse();
		int sl = strlen(Atxt);
		int strxpos = Arect->X + ((Arect->Width / 2) - (sl / 2));

		for (int i = 0; i < sl; i++)
		{
			PutChar(strxpos + i, Arect->Y, Atxt[i]);
		}
		SetCharMode_Normal();
		SetRegionClip(&lr);

		board.TXCounter=ltx;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::FrameRect(TTextRect *Arect)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		uint32_t ltx = board.TXCounter;

		SetCharMode_Normal();
		SetCharMode_Reverse();
		
		bool isput = PutChar(Arect->X, Arect->Y,'|');
		for (int xi = 0; xi < Arect->Width - 2; xi++)
		{
			if (isput)
			{
				isput=PutChar(' ');
				if (!isput) break;
			}
			else
			{
				isput = PutChar(Arect->X+1+xi, Arect->Y, ' ');
			}
		}
		if (isput) PutChar('|');
		SetCharMode_Normal();


		for (int yi = 0; yi < Arect->Height - 2; yi++)
		{
			isput = PutChar(Arect->X, Arect->Y + 1 + yi, '|');
			isput = PutChar(Arect->X + Arect->Width-1, Arect->Y + 1 + yi, '|');
		}

		if (Arect->Height > 1)
		{

			isput = PutChar(Arect->X, Arect->Y+ Arect->Height - 1, '\\');
			for (int xi = 0; xi < Arect->Width - 2; xi++)
			{
				if (isput)
				{
					isput = PutChar('_');
					if (!isput) break;
				}
				else
				{
					isput = PutChar(Arect->X + 1 + xi, Arect->Y + Arect->Height - 1, '_');
				}
			}
			if (isput) PutChar('/');


			

		}

		 board.TXCounter = ltx;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::FillFrameRect(TTextRect *Arect,char Achr)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		uint32_t ltx = board.TXCounter;

		SetCharMode_Reverse();
		bool isput = PutChar(Arect->X, Arect->Y, '|');
		for (int xi = 0; xi < Arect->Width - 2; xi++)
		{
			if (isput)
			{
				isput = PutChar(' ');
				if (!isput) break;
			}
			else
			{
				isput = PutChar(Arect->X + 1 + xi, Arect->Y, ' ');
			}
		}
		if (isput) PutChar('|');
		SetCharMode_Normal();

		for (int yi = 0; yi < Arect->Height - 2; yi++)
		{
			isput = PutChar(Arect->X, Arect->Y + 1 + yi, '|');
			for (int xi = 0; xi < Arect->Width - 2; xi++)
			{
				if (isput)
				{
					isput = PutChar(Achr);
					if (!isput) break;
				}
				else
				{
					isput = PutChar(Arect->X + 1 + xi, Arect->Y + 1 + yi, Achr);
				}
			}
			if (isput) PutChar('|');
		}

		if (Arect->Height > 1)
		{
			isput = PutChar(Arect->X, Arect->Y + Arect->Height - 1, '\\');
			for (int xi = 0; xi < Arect->Width - 2; xi++)
			{
				if (isput)
				{
					isput = PutChar('_');
					if (!isput) break;
				}
				else
				{
					isput = PutChar(Arect->X + 1 + xi, Arect->Y + Arect->Height - 1, '_');
				}
			}
			if (isput) PutChar('/');
		}
		board.TXCounter = ltx ;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::Send_GetCurrentCursorPosition()
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		VT100_CurrentXYCursor = "";
		uint32_t ltx = board.TXCounter;
		PutGui(VT100_GETXY);
		board.TXCounter = ltx;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::GotoXY_CurrentCursorPosition()
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		if (VT100_CurrentXYCursor != "")
		{
			uint32_t ltx = board.TXCounter;
			{
/*
				char cbuf[16] = "\033[";
				uint8_t i = strlen(cbuf);
				i += inttoa(ATextBackgroundColor, &cbuf[i]);
				cbuf[i++] = 'm';
				cbuf[i++] = 0;
*/
				String cbuf = FSS("\033[") + VT100_CurrentXYCursor + 'H';
				PutGui(cbuf.c_str());
			}

			VT100_CurrentXYCursor = "";
			board.TXCounter = ltx;
		}
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
bool TScreenTextClass::WaitVT100Response(Stream *Astream)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		volatile uint32_t tick = SysTickCount;
		while (SysTickCount - tick < 10)
		{
			delay(0);
			if (Astream->available())
			{
				if (Astream->peek() == '\033')
				{
					Astream->read();
					if (Astream->peek() == '[')
					{
						Astream->read();
						ScreenText.VT100_CurrentXYCursor = Astream->readStringUntil('R');
						return true;
					}
				}
			}
		}
		return false;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::SaveCursorPosition(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		uint32_t ltx = board.TXCounter;
		PutGui(VT100_SAVECURSORPOSITION);
		board.TXCounter = ltx;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::RestoreCursorPosition(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		uint32_t ltx = board.TXCounter;
		PutGui(VT100_RESTORECURSORPOSITION);
		board.TXCounter = ltx;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::ShowCursor(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		if (VisibleCursorStatus > 0)
		{
			VisibleCursorStatus--;
		}
		
		if (VisibleCursorStatus == 0)
		{
			uint32_t ltx = board.TXCounter;
			PutGui(VT100_SHOWCURSOR);
			board.TXCounter = ltx;
		}
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------
void TScreenTextClass::HideCursor(void)
{
#ifdef SCREENTEXT_TYPE_BOARDLOG
	{
		if (VisibleCursorStatus == 0)
		{
			uint32_t ltx = board.TXCounter;
			PutGui(VT100_HIDECURSOR);
			board.TXCounter = ltx;
		}
		VisibleCursorStatus++;
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------



