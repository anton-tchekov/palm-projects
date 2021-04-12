#include <PalmOS.h>
#include <PalmCompatibility.h>
#include "reversi.h"

#define FIELD_EMPTY              0
#define FIELD_WHITE_PIECE        1
#define FIELD_BLACK_PIECE        2
#define FIELD_POSSIBLE_MOVE      3

#define SIDE_WHITE               1
#define SIDE_BLACK               2

#define MODE_PLAYER_VS_PLAYER    0
#define MODE_PLAYER_VS_COMPUTER  1

#define BOARD_OFFSET_X          15
#define BOARD_OFFSET_Y          18
#define SQUARE_SIZE             16
#define PIECE_SIZE              13
#define PIECE_RADIUS             6
#define PIECE_OFFSET             2
#define INDICATOR_SIZE          11
#define INDICATOR_RADIUS         5
#define INDICATOR_Y            148
#define INDICATOR_TEXT_WIDTH    16

#define INDICATOR_BLACK_X \
	(BOARD_OFFSET_X)

#define INDICATOR_WHITE_X \
	(BOARD_OFFSET_X + 4 * SQUARE_SIZE)

#define INDICATOR_BLACK_TEXT_X \
	(INDICATOR_BLACK_X + INDICATOR_SIZE + INDICATOR_TEXT_WIDTH)

#define INDICATOR_WHITE_TEXT_X \
	(INDICATOR_WHITE_X + INDICATOR_SIZE + INDICATOR_TEXT_WIDTH)


/* the 64 fields which each have 4 possible states: empty, white piece,
	black piece or possible move are each stored in
	2-bits => 128 bits in total */
static UInt32 _board[4];

static Int8 _priority[64] =
{
	 8, -4,  3,  3,  3,  3, -4,  8,
	-4, -4, -2, -2, -2, -2, -4, -4,
	 3, -2,  2,  0,  0,  2, -2,  3,
	 3, -2,  0,  0,  0,  0, -2,  3,
	 3, -2,  0,  0,  0,  0, -2,  3,
	 3, -2,  2,  0,  0,  2, -2,  3,
	-4, -4, -2, -2, -2, -2, -4, -4,
	 8, -4,  3,  3,  3,  3, -4,  8,
};

static char *_msg_white_wins = "White wins";
static char *_msg_black_wins = "Black wins";
static char *_msg_draw = "Draw";

struct { UInt32 _board[4]; } static _undo_stack[64];
static UInt8 _undo_idx;
static Int32 _cnt_white, _cnt_black;
static Int32 _turn = SIDE_WHITE;
static Int32 _mode = MODE_PLAYER_VS_PLAYER;
static Boolean _show_hints = true;

static Err ApplicationStart(void);
static Err ApplicationStop(void);
static void ApplicationEventLoop(void);
static Boolean ApplicationHandleEvent(EventPtr event);
static Boolean FormMainHandleEvent(EventPtr event);
static Boolean FormNewGameHandleEvent(EventPtr event);
static Boolean FormAboutHandleEvent(EventPtr event);
static void GameOver(void);
static void ComputerMove(void);
static void SetFieldText(UInt16 formId, UInt16 fieldId, Char *str);
static void GameReset(void);
static void Undo(Boolean reset);
static void StorePosition(void);
static Boolean MovePrepare(Int32 side);
static void MoveExecute(Int32 x, Int32 y, Int32 side);
static Boolean MoveCheck(Int32 x, Int32 y, Int32 side);
Int32 MoveCountTaken(Int32 x, Int32 y, Int32 side);
static Int32 Opponent(Int32 side);
static void DrawBoard(void);
static void DrawIndicatorCounter
		(Int32 x, Int32 y, Int32 width, Int32 n);
static void DrawPiece(Int32 x, Int32 y, Int32 side);
static void DrawPossibleMoves(void);
static void UndrawPossibleMoves(void);
static void SetAndDrawPiece(Int32 x, Int32 y, Int32 side);
static void SetField(Int32 x, Int32 y, Int32 side);
static Int32 GetField(Int32 x, Int32 y);

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	Err error;
	if(cmd == sysAppLaunchCmdNormalLaunch)
	{
		/* normal launch */
		if((error = ApplicationStart()) != 0) { return error; }
		ApplicationEventLoop();
		if((error = ApplicationStop()) != 0) { return error; }
	}

	return 0;
}

static Err ApplicationStart(void)
{
	Err returnCode = 0;
	GameReset();
	FrmGotoForm(FormMain);
	return returnCode;
}

static Err ApplicationStop(void)
{
	Err returnCode = 0;
	FrmCloseAllForms();
	return returnCode;
}

static void ApplicationEventLoop(void)
{
	EventType event;
	Err error;
	do
	{
		EvtGetEvent(&event, evtWaitForever);
		if(SysHandleEvent(&event))
		{
			continue;
		}

		if(MenuHandleEvent(NULL, &event, &error))
		{
			continue;
		}

		if(ApplicationHandleEvent(&event))
		{
			continue;
		}

		FrmDispatchEvent(&event);
	}
	while(event.eType != appStopEvent);
}

static Boolean ApplicationHandleEvent(EventPtr event)
{
	Boolean handled = false;
	if(event->eType == frmLoadEvent)
	{
		UInt16 formId = event->data.frmLoad.formID;
		FormType *form = FrmInitForm(formId);
		FrmSetActiveForm(form);
		switch(formId)
		{
			case FormMain:
				FrmSetEventHandler(form, FormMainHandleEvent);
				handled = true;
				break;

			case FormAbout:
				FrmSetEventHandler(form, FormAboutHandleEvent);
				handled = true;
				break;

			case FormNewGame:
				FrmSetEventHandler(form, FormNewGameHandleEvent);
				handled = true;
				break;

			default:
				break;
		}
	}

	return handled;
}

static Boolean FormMainHandleEvent(EventPtr event)
{
	Boolean handled = false;
	FormType *form = FrmGetActiveForm();
	switch(event->eType)
	{
		case frmOpenEvent:
			FrmDrawForm(form);
			DrawBoard();
			handled = true;
			break;

		case menuEvent:
			MenuEraseStatus(0);
			switch(event->data.menu.itemID)
			{
				case MainMenuOptionsNewGame:
					FrmGotoForm(FormNewGame);
					handled = true;
					break;

				case MainMenuOptionsResetGame:
					Undo(true);
					break;

				case MainMenuOptionsEnableHints:
					_show_hints = true;
					DrawPossibleMoves();
					handled = true;
					break;

				case MainMenuOptionsDisableHints:
					_show_hints = false;
					UndrawPossibleMoves();
					handled = true;
					break;

				case MainMenuOptionsUndo:
					Undo(false);
					handled = true;
					break;

				case MainMenuOptionsAbout:
					FrmGotoForm(FormAbout);
					handled = true;
					break;

				default:
					break;
			}
			break;

		case penDownEvent:
			if(event->screenX > BOARD_OFFSET_X &&
				event->screenX < BOARD_OFFSET_X + 8 * SQUARE_SIZE &&
				event->screenY > BOARD_OFFSET_Y &&
				event->screenY < BOARD_OFFSET_Y + 8 * SQUARE_SIZE)
			{
				Int32 x, y;
				x = (event->screenX - BOARD_OFFSET_X) / SQUARE_SIZE;
				y = (event->screenY - BOARD_OFFSET_Y) / SQUARE_SIZE;
				if(_mode == MODE_PLAYER_VS_PLAYER)
				{
					if(GetField(x, y) == FIELD_POSSIBLE_MOVE)
					{
						StorePosition();
						MoveExecute(x, y, _turn);
						SetAndDrawPiece(x, y, _turn);
						_turn = Opponent(_turn);
						if(MovePrepare(_turn))
						{
							_turn = Opponent(_turn);
							if(MovePrepare(_turn))
							{
								GameOver();
							}
						}
					}
				}
				else
				{
					/* Player vs Computer */
					if(GetField(x, y) == FIELD_POSSIBLE_MOVE)
					{
						StorePosition();
						MoveExecute(x, y, SIDE_WHITE);
						SetAndDrawPiece(x, y, SIDE_WHITE);
						if(MovePrepare(SIDE_BLACK))
						{
							if(MovePrepare(SIDE_WHITE))
							{
								GameOver();
								break;
							}
						}
						else
						{
							ComputerMove();
						}

						if(MovePrepare(SIDE_WHITE))
						{
							if(MovePrepare(SIDE_BLACK))
							{
								GameOver();
							}
						}
					}
				}
			}
			break;

		default:
			break;
	}

	return handled;
}

static void GameOver(void)
{
	char buf_pts_white[3], buf_pts_black[3],
			*first, *second, *msg;
	StrIToA(buf_pts_white, _cnt_white);
	StrIToA(buf_pts_black, _cnt_black);
	if(_cnt_white > _cnt_black)
	{
		msg = _msg_white_wins;
		first = buf_pts_white;
		second = buf_pts_black;
	}
	else
	{
		msg = _msg_black_wins;
		if(_cnt_white == _cnt_black)
		{
			msg = _msg_draw;
		}

		first = buf_pts_black;
		second = buf_pts_white;
	}

	FrmCustomAlert(AlertGameOver, msg, first, second);
}

static void StorePosition(void)
{
	Int32 i;
	for(i = 0; i < 4; ++i)
	{
		_undo_stack[_undo_idx]._board[i] = _board[i];
	}

	++_undo_idx;
}

static void ComputerMove(void)
{
	Int32 x, y, max, score, max_x, max_y;
	max = -100;
	max_x = 0;
	max_y = 0;
	for(x = 0; x < 8; ++x)
	{
		for(y = 0; y < 8; ++y)
		{
			if(GetField(x, y) == FIELD_POSSIBLE_MOVE)
			{
				score = MoveCountTaken(x, y, SIDE_BLACK) + _priority[y * 8 + x];
				if(score > max)
				{
					max = score;
					max_x = x;
					max_y = y;
				}
			}
		}
	}

	MoveExecute(max_x, max_y, SIDE_BLACK);
	SetAndDrawPiece(max_x, max_y, SIDE_BLACK);
}

static Boolean FormNewGameHandleEvent(EventPtr event)
{
	Boolean handled = false;
	FormType *form = FrmGetActiveForm();
	switch(event->eType)
	{
		case frmOpenEvent:
			FrmDrawForm(form);
			break;

		case ctlSelectEvent:
			switch(event->data.ctlSelect.controlID)
			{
				case NewGameButtonOk:
				{
					_mode = LstGetSelection(FrmGetObjectPtr(form, FrmGetObjectIndex(form, NewGameListGameMode)));
					GameReset();
					FrmGotoForm(FormMain);
					handled = true;
					break;
				}

				case NewGameButtonCancel:
					FrmGotoForm(FormMain);
					handled = true;
					break;

				default:
					break;
			}

		default:
			break;
	}

	return handled;
}

static Boolean FormAboutHandleEvent(EventPtr event)
{
	Boolean handled = false;
	switch(event->eType)
	{
		case frmOpenEvent:
			SetFieldText(FormAbout, AboutFieldText, "Reversi");
			FrmDrawForm(FrmGetActiveForm());
			handled = true;
			break;

		case ctlSelectEvent:
			if(event->data.ctlSelect.controlID == AboutButtonReturn)
			{
				FrmGotoForm(FormMain);
				handled = true;
			}
			break;

		default:
			break;
	}

	return handled;
}

static void SetFieldText(UInt16 formId, UInt16 fieldId, Char *str)
{
	FormType *form = FrmGetFormPtr(formId);
	UInt16 object = FrmGetObjectIndex(form, fieldId);
	FieldType *field = FrmGetObjectPtr(form, object);
	VoidHand handle;
	Char *content;
	if((handle = FldGetTextHandle(field)) == NULL)
	{
		handle = MemHandleNew(FldGetMaxChars(field) + 1);
		ErrFatalDisplayIf(!handle, "No Memory");
	}

	content = MemHandleLock(handle);
	StrCopy(content, str);
	MemHandleUnlock(handle);
	FldSetTextHandle(field, handle);
}

/* Game */
static void GameReset(void)
{
	int i;
	for(i = 0; i < 4; ++i)
	{
		_board[i] = 0;
	}

	_undo_idx = 0;
	_turn = SIDE_WHITE;

	/* start position */
	SetField(3, 3, SIDE_BLACK);
	SetField(3, 4, SIDE_WHITE);
	SetField(4, 3, SIDE_WHITE);
	SetField(4, 4, SIDE_BLACK);
}

/* Move */
static Boolean MovePrepare(Int32 side)
{
	Int32 a, x, y;
	Boolean ret = true;

	UndrawPossibleMoves();
	for(x = 0; x < 8; ++x)
	{
		for(y = 0; y < 8; ++y)
		{
			if(GetField(x, y) == FIELD_POSSIBLE_MOVE)
			{
				SetField(x, y, FIELD_EMPTY);
			}
		}
	}

	_cnt_white = 0;
	_cnt_black = 0;
	for(x = 0; x < 8; ++x)
	{
		for(y = 0; y < 8; ++y)
		{
			if(MoveCheck(x, y, side))
			{
				SetField(x, y, FIELD_POSSIBLE_MOVE);
				ret = false;
			}
			else
			{
				a = GetField(x, y);
				if(a == FIELD_WHITE_PIECE)
				{
					++_cnt_white;
				}
				else if(a == FIELD_BLACK_PIECE)
				{
					++_cnt_black;
				}
			}
		}
	}

	DrawIndicatorCounter(INDICATOR_BLACK_TEXT_X, INDICATOR_Y,
		INDICATOR_TEXT_WIDTH, _cnt_black);

	DrawIndicatorCounter(INDICATOR_WHITE_TEXT_X, INDICATOR_Y,
		INDICATOR_TEXT_WIDTH, _cnt_white);

	if(_show_hints)
	{
		DrawPossibleMoves();
	}

	return ret;
}

static void Undo(Boolean reset)
{
	if(_undo_idx)
	{
		Int32 a, x, y;
		UndrawPossibleMoves();
		if(reset)
		{
			_undo_idx = 0;
			_turn = SIDE_WHITE;
		}
		else
		{
			--_undo_idx;
			_turn = Opponent(_turn);
		}

		for(x = 0; x < 4; ++x)
		{
			_board[x] = _undo_stack[_undo_idx]._board[x];
		}

		_cnt_white = 0;
		_cnt_black = 0;
		for(x = 0; x < 8; ++x)
		{
			for(y = 0; y < 8; ++y)
			{
				DrawPiece(x, y, a = GetField(x, y));
				if(a == FIELD_WHITE_PIECE)
				{
					++_cnt_white;
				}
				else if(a == FIELD_BLACK_PIECE)
				{
					++_cnt_black;
				}
			}
		}

		DrawIndicatorCounter(INDICATOR_BLACK_TEXT_X, INDICATOR_Y,
			INDICATOR_TEXT_WIDTH, _cnt_black);

		DrawIndicatorCounter(INDICATOR_WHITE_TEXT_X, INDICATOR_Y,
			INDICATOR_TEXT_WIDTH, _cnt_white);

		if(_show_hints)
		{
			DrawPossibleMoves();
		}
	}
}

static void MoveExecute(Int32 x, Int32 y, Int32 side)
{
	Int32 a, cx, cy, ax, ay, os = Opponent(side);

	/* left */
	for(cx = x - 1; (a = GetField(cx, y)) == os; --cx) ;
	if(a == side && cx != x - 1)
	{
		for(ax = x; ax != cx; --ax)
		{
			SetAndDrawPiece(ax, y, side);
		}
	}

	/* right */
	for(cx = x + 1; (a = GetField(cx, y)) == os; ++cx) ;
	if(a == side && cx != x + 1)
	{
		for(ax = x; ax != cx; ++ax)
		{
			SetAndDrawPiece(ax, y, side);
		}
	}

	/* top */
	for(cy = y - 1; (a = GetField(x, cy)) == os; --cy) ;
	if(a == side && cy != y - 1)
	{
		for(ay = y; ay != cy; --ay)
		{
			SetAndDrawPiece(x, ay, side);
		}
	}

	/* bottom */
	for(cy = y + 1; (a = GetField(x, cy)) == os; ++cy) ;
	if(a == side && cy != y + 1)
	{
		for(ay = y; ay != cy; ++ay)
		{
			SetAndDrawPiece(x, ay, side);
		}
	}

	/* top left */
	for(cx = x - 1, cy = y - 1;
		(a = GetField(cx, cy)) == os; --cx, --cy) ;
	if(a == side && cx != x - 1 && cy != y - 1)
	{
		for(ax = x, ay = y; ax != cx; --ax, --ay)
		{
			SetAndDrawPiece(ax, ay, side);
		}
	}

	/* top right */
	for(cx = x + 1, cy = y - 1;
		(a = GetField(cx, cy)) == os; ++cx, --cy) ;
	if(a == side && cx != x + 1 && cy != y - 1)
	{
		for(ax = x, ay = y; ax != cx; ++ax, --ay)
		{
			SetAndDrawPiece(ax, ay, side);
		}
	}

	/* bottom left */
	for(cx = x - 1, cy = y + 1;
		(a = GetField(cx, cy)) == os; --cx, ++cy) ;
	if(a == side && cx != x - 1 && cy != y - 1)
	{
		for(ax = x, ay = y; ax != cx; --ax, ++ay)
		{
			SetAndDrawPiece(ax, ay, side);
		}
	}

	/* bottom right */
	for(cx = x + 1, cy = y + 1;
		(a = GetField(cx, cy)) == os; ++cx, ++cy) ;
	if(a == side && cx != x + 1 && cy != y + 1)
	{
		for(ax = x, ay = y; ax != cx; ++ax, ++ay)
		{
			SetAndDrawPiece(ax, ay, side);
		}
	}
}

static Boolean MoveCheck(Int32 x, Int32 y, Int32 side)
{
	Int32 a, cx, cy, os = Opponent(side);
	if(GetField(x, y)) { return false; }

	/* left */
	for(cx = x - 1; (a = GetField(cx, y)) == os; --cx) ;
	if(a == side && cx != x - 1) { return true; }

	/* right */
	for(cx = x + 1; (a = GetField(cx, y)) == os; ++cx) ;
	if(a == side && cx != x + 1) { return true; }

	/* top */
	for(cy = y - 1; (a = GetField(x, cy)) == os; --cy) ;
	if(a == side && cy != y - 1) { return true; }

	/* bottom */
	for(cy = y + 1; (a = GetField(x, cy)) == os; ++cy) ;
	if(a == side && cy != y + 1) { return true; }

	/* top left */
	for(cx = x - 1, cy = y - 1;
		(a = GetField(cx, cy)) == os; --cx, --cy) ;
	if(a == side && cx != x - 1 && cy != y - 1) { return true; }

	/* top right */
	for(cx = x + 1, cy = y - 1;
		(a = GetField(cx, cy)) == os; ++cx, --cy) ;
	if(a == side && cx != x + 1 && cy != y - 1) { return true; }

	/* bottom left */
	for(cx = x - 1, cy = y + 1;
		(a = GetField(cx, cy)) == os; --cx, ++cy) ;
	if(a == side && cx != x - 1 && cy != y - 1) { return true; }

	/* bottom right */
	for(cx = x + 1, cy = y + 1;
		(a = GetField(cx, cy)) == os; ++cx, ++cy) ;
	if(a == side && cx != x + 1 && cy != y + 1) { return true; }

	return false;
}

Int32 MoveCountTaken(Int32 x, Int32 y, Int32 side)
{
	Int32 taken, temp, a, cx, cy, os = Opponent(side);
	if(GetField(x, y))
	{
		return 0;
	}

	taken = 0;

	/* left */
	for(temp = 0, cx = x - 1;
		(a = GetField(cx, y)) == os;
		--cx, ++temp) ;

	if(a && cx != x - 1)
	{
		taken += temp;
	}

	/* right */
	for(
			temp = 0, cx = x + 1;
			(a = GetField(cx, y)) == os;
			++cx, ++temp) ;

	if(a && cx != x + 1)
	{
		taken += temp;
	}

	/* top */
	for(
			temp = 0, cy = y - 1;
			(a = GetField(x, cy)) == os;
			--cy, ++temp) ;

	if(a && cy != y - 1)
	{
		taken += temp;
	}

	/* bottom */
	for(
			temp = 0, cy = y + 1;
			(a = GetField(x, cy)) == os;
			++cy, ++temp) ;

	if(a && cy != y + 1)
	{
		taken += temp;
	}

	/* top left */
	for(
			temp = 0, cx = x - 1, cy = y - 1;
			(a = GetField(cx, cy)) == os;
			--cx, --cy, ++temp) ;

	if(a && cx != x - 1 && cy != y - 1)
	{
		taken += temp;
	}

	/* top right */
	for(
			temp = 0, cx = x + 1, cy = y - 1;
			(a = GetField(cx, cy)) == os;
			++cx, --cy, ++temp) ;

	if(a && cx != x + 1 && cy != y - 1)
	{
		taken += temp;
	}

	/* bottom left */
	for(
			temp = 0, cx = x - 1, cy = y + 1;
			(a = GetField(cx, cy)) == os;
			--cx, ++cy, ++temp) ;

	if(a && cx != x - 1 && cy != y - 1)
	{
		taken += temp;
	}

	/* bottom right */
	for(
			temp = 0, cx = x + 1, cy = y + 1;
			(a = GetField(cx, cy)) == os;
			++cx, ++cy, ++temp) ;

	if(a && cx != x + 1 && cy != y + 1)
	{
		taken += temp;
	}

	return taken;
}

static Int32 Opponent(Int32 side)
{
	if(side == 0) { return 0; }
	else { return side == SIDE_WHITE ? SIDE_BLACK : SIDE_WHITE; }
}

/* Draw */
static void DrawBoard(void)
{
	RectangleType rect;
	Int32 x, y;

	/* draw grid */
	for(x = 0; x < 9 * SQUARE_SIZE; x += SQUARE_SIZE)
	{
		/* vertical grid lines */
		WinDrawLine(
			BOARD_OFFSET_X + x, BOARD_OFFSET_Y,
			BOARD_OFFSET_X + x, BOARD_OFFSET_Y + 8 * SQUARE_SIZE);

		/* horizontal grid lines */
		WinDrawLine(
			BOARD_OFFSET_X, BOARD_OFFSET_Y + x,
			BOARD_OFFSET_X + 8 * SQUARE_SIZE, BOARD_OFFSET_Y + x);
	}

	/* draw pieces */
	for(x = 0; x < 8; ++x)
	{
		for(y = 0; y < 8; ++y)
		{
			DrawPiece(x, y, GetField(x, y));
		}
	}

	/* draw indicator */
	rect.extent.x = rect.extent.y = INDICATOR_SIZE;
	rect.topLeft.y = INDICATOR_Y;

	rect.topLeft.x = INDICATOR_BLACK_X;
	WinDrawRectangle(&rect, INDICATOR_RADIUS);

	rect.topLeft.x = INDICATOR_WHITE_X;
	WinDrawRectangle(&rect, INDICATOR_RADIUS);
	++rect.topLeft.x;
	++rect.topLeft.y;
	rect.extent.x -= 2;
	rect.extent.y -= 2;
	WinEraseRectangle(&rect, INDICATOR_RADIUS - 2);

	MovePrepare(_turn);
}

static void DrawIndicatorCounter(Int32 x, Int32 y, Int32 width, Int32 n)
{
	RectangleType rect;
	Int32 len;
	char buf[16];
	StrIToA(buf, n);
	len = StrLen(buf);
	rect.topLeft.x = x - width;
	rect.topLeft.y = y;
	rect.extent.x = width;
	rect.extent.y = FntCharHeight();
	WinEraseRectangle(&rect, 0);
	WinDrawChars(buf, len, x - FntCharsWidth(buf, len), y);
}

static void DrawPiece(Int32 x, Int32 y, Int32 side)
{
	RectangleType rect;
	rect.topLeft.x = BOARD_OFFSET_X + SQUARE_SIZE * x + 1;
	rect.topLeft.y = BOARD_OFFSET_Y + SQUARE_SIZE * y + 1;
	rect.extent.x = SQUARE_SIZE - 2;
	rect.extent.y = SQUARE_SIZE - 2;
	WinEraseRectangle(&rect, 0);
	rect.topLeft.x = BOARD_OFFSET_X + SQUARE_SIZE * x + PIECE_OFFSET;
	rect.topLeft.y = BOARD_OFFSET_Y + SQUARE_SIZE * y + PIECE_OFFSET;
	rect.extent.x = rect.extent.y = PIECE_SIZE;
	if(side == SIDE_WHITE)
	{
		WinDrawRectangle(&rect, PIECE_RADIUS);
		++rect.topLeft.x; ++rect.topLeft.y;
		rect.extent.x -= 2; rect.extent.y -= 2;
		WinEraseRectangle(&rect, PIECE_RADIUS - 2);
	}
	else if(side == SIDE_BLACK)
	{
		WinDrawRectangle(&rect, PIECE_RADIUS);
	}
}

static void DrawPossibleMoves(void)
{
	Int32 x, y;
	for(x = 0; x < 8; ++x)
	{
		for(y = 0; y < 8; ++y)
		{
			if(GetField(x, y) == FIELD_POSSIBLE_MOVE)
			{
				/* draw cross */
				Int32 leftX, rightX, topY, bottomY;
				leftX = BOARD_OFFSET_X + SQUARE_SIZE * x + 2;
				topY = BOARD_OFFSET_Y + SQUARE_SIZE * y + 2;

				rightX = BOARD_OFFSET_X + SQUARE_SIZE * x +
					SQUARE_SIZE - 2;

				bottomY = BOARD_OFFSET_Y + SQUARE_SIZE * y +
					SQUARE_SIZE - 2;

				WinDrawLine(leftX, topY, rightX, bottomY);
				WinDrawLine(rightX, topY, leftX, bottomY);
			}
		}
	}
}

static void UndrawPossibleMoves(void)
{
	Int32 x, y;
	for(x = 0; x < 8; ++x)
	{
		for(y = 0; y < 8; ++y)
		{
			if(GetField(x, y) == FIELD_POSSIBLE_MOVE)
			{
				RectangleType rect;
				rect.topLeft.x = BOARD_OFFSET_X + SQUARE_SIZE * x + 1;
				rect.topLeft.y = BOARD_OFFSET_Y + SQUARE_SIZE * y + 1;
				rect.extent.x = SQUARE_SIZE - 2;
				rect.extent.y = SQUARE_SIZE - 2;
				WinEraseRectangle(&rect, 0);
			}
		}
	}
}

static void SetAndDrawPiece(Int32 x, Int32 y, Int32 side)
{
	SetField(x, y, side);
	DrawPiece(x, y, side);
}

/* Field */
static void SetField(Int32 x, Int32 y, Int32 side)
{
	UInt32 a, b;
	if(x < 0 || x > 7 || y < 0 || y > 7) { return; }
	a = y >> 1;
	b = x << 1;
	if(y & 1) { b += 16; }
	_board[a] &= ~(0x03L << b);
	_board[a] |= side << b;
}

static Int32 GetField(Int32 x, Int32 y)
{
	UInt32 a, b;
	if(x < 0 || x > 7 || y < 0 || y > 7) { return 0; }
	a = y >> 1;
	b = x << 1;
	if(y & 1) { b += 16; }
	return (_board[a] >> b) & 0x03L;
}

