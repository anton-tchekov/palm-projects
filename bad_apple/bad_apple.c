#include <PalmOS.h>
#include "bad_apple.h"

#define FONT_COUNT 126

static int q, n;

static void render(void)
{
	FntSetFont(128 + n);
	WinDrawChar(q++, 0, 30);
	WinDrawChar(q++, 80, 30);
	if(q == 52)
	{
		q = 0;
		n++;
	}
}

static Boolean EventHandler_Main(EventType *event)
{
	if(event->eType == frmOpenEvent)
	{
		FrmDrawForm(FrmGetActiveForm());
	}
}

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	UInt32 result = 0;
	if(cmd == sysAppLaunchCmdNormalLaunch)
	{
		int i, speed, ticksPerSecond, ticks, newTicks;
		for(i = 0; i < FONT_COUNT; ++i)
		{
			MemHandle fntHandle = DmGetResource('NFNT', 1128 + i);
			FntDefineFont(128 + i, (FontPtr)MemHandleLock(fntHandle));
			MemHandleUnlock(fntHandle);
		}

		FrmGotoForm(FORM_MAIN);

		{
			EventType event;
			UInt16 err;

			ticksPerSecond = SysTicksPerSecond();
			speed = ticksPerSecond / 15;

			do
			{
				EvtGetEvent(&event, 0);
				if((newTicks = TimGetTicks()) > ticks + speed)
				{
					ticks = newTicks;
					render();
				}

				if(SysHandleEvent(&event))
				{
					continue;
				}

				if(MenuHandleEvent(0, &event, &err))
				{
					continue;
				}

				if(event.eType == frmLoadEvent)
				{
					UInt16 form_id = event.data.frmLoad.formID;
					if(form_id == FORM_MAIN)
					{
						FormType *form = FrmInitForm(form_id);
						FrmSetActiveForm(form);
						FrmSetEventHandler(form, EventHandler_Main);
						continue;
					}
				}

				FrmDispatchEvent(&event);
			}
			while(event.eType != appStopEvent);
		}

		FrmCloseAllForms();
	}

	return result;
}

