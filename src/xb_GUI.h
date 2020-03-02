
#ifndef _XB_GUI_h
#define _XB_GUI_h

#include <xb_board.h>
#ifdef XB_GUI
#include <xb_ScreenText.h>

typedef struct
{
	Tx Left;
	Ty Top;
	Tx Right;
	Ty Bottom;
} TWindowRect;

typedef struct
{
	Tx Left;
	Ty Top;
	Tx Right;
	Ty Bottom;
} TBorderSize;


typedef struct
{
	Tx X;
	Ty Y;
} TPoint;


#define WINDOW_POS_X_CENTER -2
#define WINDOW_POS_Y_CENTER -2
#define WINDOW_POS_LAST_RIGHT -1
#define WINDOW_POS_LAST_BOTTOM -1
#define WINDOW_POS_LAST_RIGHT_ACTIVE -3
#define WINDOW_POS_LAST_BOTTOM_ACTIVE -3
#define WINDOW_POS_X_DEF -4
#define WINDOW_POS_Y_DEF -4


class TWindowClass
{
public:

	TWindowClass(TTaskDef *ATaskDef,int8_t AWindowID=-1,bool Aactive=true,bool Aescapeclose=true, Tx Ax=WINDOW_POS_X_CENTER, Ty Ay= WINDOW_POS_Y_CENTER);
	~TWindowClass();

	TWindowClass *Next;
	TWindowClass *Prev;

	TTaskDef *taskdef; // Zadanie w³aœciciel okna
	int8_t ID; // ID okna
	Tx X;
	Ty Y;
	Tx Width;
	Ty Height;

	void SetWindowPositionFrom(TWindowData* Awd);

	TWindowRect WindowRect; // pozycja okna wyliczona z X,Y,Width,Height
	TBorderSize BorderSize;

	bool DoClose;
	void Close();

	TPoint CursorXY;
	char ClearChar;
	bool IsInit;
	bool Visible;
	bool Active;
	bool EscapeClose;
	TWindowClass *LastActive;
	TWindowClass *ModalWin;
	

	int16_t RepaintCounter;
	int16_t RepaintDataCounter;
	int16_t RepaintBorderCounter;

	int16_t isBeginDraw;
	int16_t PaintOnBorder;

	TTextForegroundColor TextColor;
	TTextBackgroundColor TextBackgroundColor;
	TTextForegroundColor BorderTextActiveColor;
	TTextForegroundColor BorderTextUnActiveColor;
	bool TextWordWrap;
	
	void SetTextColor(TTextForegroundColor ATextColor);
	void SetTextBackgoundColor(TTextBackgroundColor ATextBackgroundColor);

	void GetCaptionWindow(String &Astr);
	void BeginDraw(bool Apaintonborder = false);
	void EndDraw(bool Apaintonborder = false);
	void Clear(char ch=0);
	void ClearWindow();
	void GoToXY(Tx Ax, Ty Ay);
	bool XYisShow(Tx Ax, Ty Ay);
	void SetReverseChar();
	bool ReverseChar;
	void SetBoldChar();
	bool BoldChar;
	void SetNormalChar();
	void PutChar(char Ach);
	void PutStr(Tx Ax, Ty Ay, const char *Astr, Tx Awidth = 0, char Achfill = ' ', TTextAlignment Atextalignment = taLeft);
	void PutStr(const char *Astr, Tx Awidth = 0, char Achfill = ' ', TTextAlignment Atextalignment = taLeft);
	void SetActive(void);
	void DoRepaint();
	void DoRepaintData();
	void Repaint();
	void PaintBorder();
	
	void SetWindowSize(Tx Awidth, Ty Aheight);
	void SetWindowPos(Tx Ax, Ty Ay);
};


int8_t GUI_GetUniqueIDWindow(TTaskDef *ATaskDef);
TWindowClass *GUI_FindWindowByTaskDef(TTaskDef *ATaskDef, int8_t AWindowID);
TWindowClass *GUI_FindWindowByActive();
TWindowClass *GUI_WindowCreate(TTaskDef *ATaskDef, int8_t AWindowID = -1, bool Aactive = true, TWindowClass *Amodalwin = NULL, bool Aescapeclose=true, Tx Ax=WINDOW_POS_X_CENTER, Ty Ay= WINDOW_POS_Y_CENTER);
void GUI_WindowDestroy(TWindowClass **Awindowclass);
void GUI_RepaintAllWindows(void);
void GUI_Hide(void);
void GUI_Show(void);
void GUI_ClearScreen(void);
void GUI_PutChar(char Ach, TPoint &Axy);
bool GUI_XYisShow(Tx Ax, Ty Ay);
int GUI_GetHeightDesktop(void);
int GUI_GetWidthDesktop(void);
void GUI_ClearDesktop(char Ach=' ');
extern void GUI_Setup(void);
extern uint32_t GUI_DoLoop(void);
extern bool GUI_DoMessage(TMessageBoard *Am);

extern TTaskDef XB_GUI_DefTask;


#define BEGIN_WINDOW_DEF(Aid,Acaption,Ax,Ay,Awidth,Aheight,Aptrhandle) \
if ((Am->Data.WindowData.ID == Aid)) \
{ \
TWindowClass* WH = Aptrhandle; \
if (Am->Data.WindowData.WindowAction==waGetCaptionWindow) \
{ \
	*(Am->Data.WindowData.ActionData.GetCaption.PointerString) = Acaption; \
} \
if (Am->Data.WindowData.WindowAction==waCreate) \
{ \
		Am->Data.WindowData.ActionData.Create.X = Ax; \
		Am->Data.WindowData.ActionData.Create.Y = Ay; \
		Am->Data.WindowData.ActionData.Create.Width = Awidth; \
		Am->Data.WindowData.ActionData.Create.Height = Aheight; \
}


#define CREATE_WINDOW() if (Am->Data.WindowData.WindowAction==waCreate) 

#define DESTROY_WINDOW() if ((Am->Data.WindowData.WindowAction==waDestroy) && (WH != NULL))

#define REPAINT_WINDOW() if ((Am->Data.WindowData.WindowAction==waRepaint) && (WH != NULL))

#define REPAINTDATA_WINDOW() if ((Am->Data.WindowData.WindowAction==waRepaintData) && (WH != NULL))

#define END_WINDOW_DEF() }

#endif
#endif

