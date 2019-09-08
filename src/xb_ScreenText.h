#ifndef _XB_SCREENTEXT_h
#define _XB_SCREENTEXT_h

#include <Arduino.h>
#include <xb_board.h>
#include "xb_ScreenText_VT100.h"
#define SCREENTEXT_TYPE_BOARDLOG

typedef enum {stUART1VT100} TScreenType;

typedef struct
{
	int16_t X, Y, Width, Height;
} TTextRect;

class TScreenTextClass
{
 public:
	 TScreenTextClass(TScreenType AType);
	 ~TScreenTextClass();

	 void Clear(void);
	 void SetCharMode_Normal(void);
	 void SetCharMode_Bold(void);
	 void SetCharMode_Blink(void);
	 void SetCharMode_Reverse(void);
	 bool PutChar(char Achr);
	 bool PutChar(int16_t Ax, int16_t Ay, char Achr);
	 bool InRegionClip(int16_t Arealx, int16_t Arealy);
	 void SetRegionClip(TTextRect *Arectclip);
	 bool GotoXY(int16_t Ax, int16_t Ay);
	 void PutText(const char *Atxt);
	 void PutText(int16_t Ax, int16_t Ay, const char *Atxt);
	 void PutText(int16_t Ax, int16_t Ay, const char *Atxt, int Awidth, char Afillchar);
	 void SetForegroundColor(TTextForegroundColor ATextForegroundColor);
	 void SetBackgroundColor(TTextBackgroundColor ATextBackgroundColor);
	 void FillRect(TTextRect *Arect, char Achr);
	 void FrameCaption(TTextRect *Arect, const char *Atxt);
	 void FrameRect(TTextRect *Arect);
	 void FillFrameRect(TTextRect *Arect, char Achr);
	 void Send_GetCurrentCursorPosition();
	 void GotoXY_CurrentCursorPosition();
	 bool WaitVT100Response(Stream *Astream);
	 void SaveCursorPosition(void);
	 void RestoreCursorPosition(void);
	 void ShowCursor(void);
	 void HideCursor(void);

	 TScreenType Type;
	 int16_t ScreenWidth;
	 int16_t ScreenHeight;

	 TTextRect RegionClip;
	 int16_t RealCurrentX, RealCurrentY;
	 TTextForegroundColor CurrentForegroundColor;
	 TTextBackgroundColor CurrentBackgroundColor;
	 int16_t DesktopWidth;
	 int16_t DesktopHeight;
	 int16_t VisibleCursorStatus;
	 String VT100_CurrentXYCursor;

};
void PutGui(const char *Atxt);
void PutGui(char Ach);
extern TScreenTextClass ScreenText;

#endif

