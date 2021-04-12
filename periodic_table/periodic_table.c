#include <PalmOS.h>
#include <PalmCompatibility.h>
#include "periodic_table.h"

struct Element
{
	char *Symbol;
	char *Group;
	char *Period;
	char *Mass;
	char *Melt;
	char *Boil;
	char *ElNeg;
} _elementData[118] =
{
	{ "H",  "1",   "1", "1,01",   "-259,1", "-252,9", "2,20" },
	{ "He", "18",  "1", "4,00",   "-272,2", "-268,9", "-"    },
	{ "Li", "1",   "2", "6,94",   "180,5",  "1317",   "0,98" },
	{ "Be", "2",   "2", "9,01",   "1278",   "2970",   "1,57" },
	{ "B",  "13",  "2", "10,81",  "2300",   "2550",   "2,04" },
	{ "C",  "14",  "2", "12,01",  "35502",  "35502",  "2,55" },
	{ "N",  "15",  "2", "14,01",  "-209,9", "-195,8", "3,04" },
	{ "O",  "16",  "2", "16,00",  "-218,4", "-182,9", "3,44" },
	{ "F",  "17",  "2", "19,00",  "-219,6", "-188,1", "3,98" },
	{ "Ne", "18",  "2", "20,18",  "-248,7", "-246,1", "-"    },
	{ "Na", "1",   "3", "22,99",  "97,8",   "892",    "0,93" },
	{ "Mg", "2",   "3", "24,30",  "648,8",  "1107",   "1,31" },
	{ "Al", "13",  "3", "26,98",  "660,5",  "2467",   "1,61" },
	{ "Si", "14",  "3", "28,09",  "1410",   "2355",   "1,9"  },
	{ "P",  "15",  "3", "30,97",  "44",     "280",    "2,19" },
	{ "S",  "16",  "3", "32,07",  "113",    "444,7",  "2,58" },
	{ "Cl", "17",  "3", "35,45",  "-101",   "-34,6",  "3,16" },
	{ "Ar", "18",  "3", "39,94",  "-189,4", "-185,9", "-"    },
	{ "K",  "1",   "4", "39,10",  "63,7",   "774",    "0,82" },
	{ "Ca", "2",   "4", "40,08",  "839",    "1487",   "1"    },
	{ "Sc", "3",   "4", "44,96",  "1539",   "2832",   "1,36" },
	{ "Ti", "4",   "4", "47,88",  "1660",   "3260",   "1,54" },
	{ "V",  "5",   "4", "50,94",  "1890",   "3380",   "1,63" },
	{ "Cr", "6",   "4", "52,00",  "1857",   "2482",   "1,66" },
	{ "Mn", "7",   "4", "54,90",  "1244",   "2097",   "1,55" },
	{ "Fe", "8",   "4", "55,85",  "1539",   "2750",   "1,83" },
	{ "Co", "9",   "4", "58,93",  "1495",   "2870",   "1,88" },
	{ "Ni", "10",  "4", "58,69",  "1453",   "2732",   "1,91" },
	{ "Cu", "11",  "4", "63,55",  "1083,5", "2595",   "1,9"  },
	{ "Zn", "12",  "4", "65,39",  "419,6",  "907",    "1,65" },
	{ "Ga", "13",  "4", "69,72",  "29,8",   "2403",   "1,81" },
	{ "Ge", "14",  "4", "72,61",  "937,4",  "2830",   "2,01" },
	{ "As", "15",  "4", "74,92",  "6132",   "6132",   "2,18" },
	{ "Se", "16",  "4", "78,96",  "217",    "685",    "2,55" },
	{ "Br", "17",  "4", "79,90",  "-7,3",   "58,8",   "2,96" },
	{ "Kr", "18",  "4", "83,80",  "-156,6", "-152,3", "3"    },
	{ "Rb", "1",   "5", "85,45",  "39",     "1861",   "0,82" },
	{ "Sr", "2",   "5", "87,62",  "769",    "1384",   "0,95" },
	{ "Y",  "3",   "5", "88,91",  "1523",   "3337",   "1,22" },
	{ "Zr", "4",   "5", "91,22",  "1852",   "4377",   "1,33" },
	{ "Nb", "5",   "5", "92,91",  "2468",   "4927",   "1,6"  },
	{ "Mo", "6",   "5", "95,94",  "2617",   "5560",   "2,16" },
	{ "Tc", "7",   "5", "98,91",  "2172",   "5030",   "1,9"  },
	{ "Ru", "8",   "5", "101,07", "2310",   "3900",   "2,2"  },
	{ "Rh", "9",   "5", "102,91", "1966",   "3727",   "2,28" },
	{ "Pd", "10",  "5", "106,42", "1552",   "3140",   "2,2"  },
	{ "Ag", "11",  "5", "107,87", "961,9",  "2212",   "1,93" },
	{ "Cd", "12",  "5", "112,41", "321",    "765",    "1,69" },
	{ "In", "13",  "5", "114,82", "156,2",  "2080",   "0,25" },
	{ "Sn", "14",  "5", "118,71", "232",    "2270",   "1,96" },
	{ "Sb", "15",  "5", "121,75", "630,7",  "1750",   "2,05" },
	{ "Te", "16",  "5", "127,60", "449,6",  "990",    "2,1"  },
	{ "I",  "17",  "5", "126,90", "113,5",  "184,4",  "2,66" },
	{ "Xe", "18",  "5", "131,29", "-111,9", "-107",   "2,6"  },
	{ "Cs", "1",   "6", "132,91", "28,4",   "690",    "0,79" },
	{ "Ba", "2",   "6", "137,33", "725",    "1640",   "0,89" },
	{ "La", "3",   "6", "138,90", "920",    "3454",   "1,1"  },
	{ "Ce", "6",   "-", "140,11", "798",    "3257",   "1,12" },
	{ "Pr", "6",   "-", "140,91", "931",    "3212",   "1,13" },
	{ "Nd", "6",   "-", "144,24", "1010",   "3127",   "1,14" },
	{ "Pm", "6",   "-", "146,92", "1080",   "2730",   "1,13" },
	{ "Sm", "6",   "-", "150,36", "1072",   "1778",   "1,17" },
	{ "Eu", "6",   "-", "151,96", "822",    "1597",   "1,2"  },
	{ "Gd", "6",   "-", "157,25", "1311",   "3233",   "1,2"  },
	{ "Tb", "6",   "-", "158,93", "1360",   "3041",   "1,2"  },
	{ "Dy", "6",   "-", "162,50", "1409",   "2335",   "1,22" },
	{ "Ho", "6",   "-", "164,93", "1470",   "2720",   "1,23" },
	{ "Er", "6",   "-", "167,26", "1522",   "2510",   "1,24" },
	{ "Tm", "6",   "-", "168,93", "1545",   "1727",   "1,25" },
	{ "Yb", "6",   "-", "173,04", "824",    "1193",   "1,1"  },
	{ "Lu", "6",   "-", "174,97", "1656",   "3315",   "1,27" },
	{ "Hf", "4",   "6", "178,49", "2150",   "5400",   "1,3"  },
	{ "Ta", "5",   "6", "180,95", "2996",   "5425",   "1,5"  },
	{ "W",  "6",   "6", "183,85", "3407",   "5927",   "2,36" },
	{ "Re", "7",   "6", "186,21", "3180",   "5627",   "1,9"  },
	{ "Os", "8",   "6", "190,23", "3045",   "5027",   "2,2"  },
	{ "Ir", "9",   "6", "192,22", "2410",   "4130",   "2,2"  },
	{ "Pt", "10",  "6", "195,08", "1772",   "3827",   "2,28" },
	{ "Au", "11",  "6", "196,97", "1064,4", "2940",   "2,54" },
	{ "Hg", "12",  "6", "200,59", "-38,9",  "356,6",  "2"    },
	{ "Tl", "13",  "6", "204,38", "303,6",  "1457",   "1,62" },
	{ "Pb", "14",  "6", "207,2",  "327,5",  "1740",   "1,87" },
	{ "Bi", "15",  "6", "208,98", "271,4",  "1560",   "2,02" },
	{ "Po", "16",  "6", "208,98", "254",    "962",    "2,0"  },
	{ "At", "17",  "6", "209,99", "-",      "-",      "2,2"  },
	{ "Rn", "18",  "6", "222,02", "-71",    "-61,8",  "2,2"  },
	{ "Fr", "1",   "7", "223,02", "-",      "-",      "0,7"  },
	{ "Ra", "2",   "7", "226,03", "700",    "1140",   "0,9"  },
	{ "Ac", "3",   "7", "227,03", "1047",   "3197",   "1,1"  },
	{ "Th", "7",   "-", "232,04", "1750",   "4787",   "1,3"  },
	{ "Pa", "7",   "-", "231,04", "1554",   "4030",   "1,5"  },
	{ "U",  "7",   "-", "238,03", "1132,4", "3818",   "1,38" },
	{ "Np", "7",   "-", "237,05", "640",    "3902",   "1,36" },
	{ "Pu", "7",   "-", "244,06", "641",    "3327",   "1,28" },
	{ "Am", "7",   "-", "243,06", "994",    "2607",   "1,13" },
	{ "Cm", "7",   "-", "247,07", "1340",   "3110",   "1,28" },
	{ "Bk", "7",   "-", "247,07", "986",    "-",      "1,3"  },
	{ "Cf", "7",   "-", "251,08", "900",    "-",      "1,3"  },
	{ "Es", "7",   "-", "252,08", "980",    "-",      "1,3"  },
	{ "Fm", "7",   "-", "257,10", "-",      "-",      "1,3"  },
	{ "Md", "7",   "-", "258",    "-",      "-",      "1,3"  },
	{ "No", "7",   "-", "259",    "-",      "-",      "1,3"  },
	{ "Lr", "7",   "-", "260",    "1627",   "-",      "1,3"  },
	{ "Rf", "4",   "7", "261,11", "-",      "-",      "-"    },
	{ "Db", "5",   "7", "262,11", "-",      "-",      "-"    },
	{ "Sg", "6",   "7", "263,12", "-",      "-",      "-"    },
	{ "Bh", "7",   "7", "262,12", "-",      "-",      "-"    },
	{ "Hs", "8",   "7", "265",    "-",      "-",      "-"    },
	{ "Mt", "9",   "7", "266",    "-",      "-",      "-"    },
	{ "Ds", "10",  "7", "269",    "-",      "-",      "-"    },
	{ "Rg", "11",  "7", "272",    "-",      "-",      "-"    },
	{ "Cn", "12",  "7", "277",    "-",      "-",      "-"    },
	{ "Nh", "13",  "7", "287",    "-",      "-",      "-"    },
	{ "Fl", "14",  "7", "289",    "-",      "-",      "-"    },
	{ "Mc", "15",  "7", "288",    "-",      "-",      "-"    },
	{ "Lv", "16",  "7", "289",    "-",      "-",      "-"    },
	{ "Ts", "17",  "7", "292",    "-",      "-",      "-"    },
	{ "Og", "18",  "7", "293",    "-",      "-",      "-"    },
};

#if defined(LANG_EN) && LANG_EN

#define ALKALI_METAL         "Alkali metal"
#define ALKALINE_EARTH_METAL "A. earth m."
#define HALOGEN              "Halogen"
#define NOBLE_GAS            "Noble gas"

Char *_elementNames[] =
{
	"Hydrogen",
	"Helium",
	"Lithium",
	"Beryllium",
	"Boron",
	"Carbon",
	"Nitrogen",
	"Oxygen",
	"Fluorine",
	"Neon",
	"Sodium",
	"Magnesium",
	"Aluminium",
	"Silicon",
	"Phosphorus",
	"Sulfur",
	"Chlorine",
	"Argon",
	"Potassium",
	"Calcium",
	"Scandium",
	"Titanium",
	"Vanadium",
	"Chromium",
	"Manganese",
	"Iron",
	"Cobalt",
	"Nickel",
	"Copper",
	"Zinc",
	"Gallium",
	"Germanium",
	"Arsenic",
	"Selenium",
	"Bromine",
	"Krypton",
	"Rubidium",
	"Strontium",
	"Yttrium",
	"Zirconium",
	"Niobium",
	"Molybdenum",
	"Technetium",
	"Ruthenium",
	"Rhodium",
	"Palladium",
	"Silver",
	"Cadmium",
	"Indium",
	"Tin",
	"Antimony",
	"Tellurium",
	"Iodine",
	"Xenon",
	"Caesium",
	"Barium",
	"Lanthanum",
	"Cerium",
	"Praseodymium",
	"Neodymium",
	"Promethium",
	"Samarium",
	"Europium",
	"Gadolinium",
	"Terbium",
	"Dysprosium",
	"Holmium",
	"Erbium",
	"Thulium",
	"Ytterbium",
	"Lutetium",
	"Hafnium",
	"Tantalum",
	"Tungsten",
	"Rhenium",
	"Osmium",
	"Iridium",
	"Platinum",
	"Gold",
	"Mercury",
	"Thallium",
	"Lead",
	"Bismuth",
	"Polonium",
	"Astatine",
	"Radon",
	"Francium",
	"Radium",
	"Actinium",
	"Thorium",
	"Protactinium",
	"Uranium",
	"Neptunium",
	"Plutonium",
	"Americium",
	"Curium",
	"Berkelium",
	"Californium",
	"Einsteinium",
	"Fermium",
	"Mendelevium",
	"Nobelium",
	"Lawrencium",
	"Rutherfordium",
	"Dubnium",
	"Seaborgium",
	"Bohrium",
	"Hassium",
	"Meitnerium",
	"Darmstadtium",
	"Roentgenium",
	"Copernicium",
	"Nihonium",
	"Flerovium",
	"Moscovium",
	"Livermorium",
	"Tennessine",
	"Oganesson"
};

#elif defined(LANG_DE) && LANG_DE

#define ALKALI_METAL         "Alkalimetall"
#define ALKALINE_EARTH_METAL "Erdalkalimetall"
#define HALOGEN              "Halogen"
#define NOBLE_GAS            "Edelgas"

Char *_elementNames[] =
{
	"Wasserstoff",
	"Helium",
	"Lithium",
	"Beryllium",
	"Bor",
	"Kohlenstoff",
	"Stickstoff",
	"Sauerstoff",
	"Fluor",
	"Neon",
	"Natrium",
	"Magnesium",
	"Aluminium",
	"Silicium",
	"Phosphor",
	"Schwefel",
	"Chlor",
	"Argon",
	"Kalium",
	"Calcium",
	"Scandium",
	"Titan",
	"Vanadium",
	"Chrom",
	"Mangan",
	"Eisen",
	"Cobalt",
	"Nickel",
	"Kupfer",
	"Zink",
	"Gallium",
	"Germanium",
	"Arsen",
	"Selen",
	"Brom",
	"Krypton",
	"Rubidium",
	"Strontium",
	"Yttrium",
	"Zirconium",
	"Niob",
	"Molybdän",
	"Technetium",
	"Ruthenium",
	"Rhodium",
	"Palladium",
	"Silber",
	"Cadmium",
	"Indium",
	"Zinn",
	"Antimon",
	"Tellur",
	"Iod",
	"Xenon",
	"Caesium",
	"Barium",
	"Lanthan",
	"Cer",
	"Praseodym",
	"Neodym",
	"Promethium",
	"Samarium",
	"Europium",
	"Gadolinium",
	"Terbium",
	"Dysprosium",
	"Holmium",
	"Erbium",
	"Thulium",
	"Ytterbium",
	"Lutetium",
	"Hafnium",
	"Tantal",
	"Wolfram",
	"Rhenium",
	"Osmium",
	"Iridium",
	"Platin",
	"Gold",
	"Quecksilber",
	"Thallium",
	"Blei",
	"Bismut",
	"Polonium",
	"Astat",
	"Radon",
	"Francium",
	"Radium",
	"Actinium",
	"Thorium",
	"Protactinium",
	"Uran",
	"Neptunium",
	"Plutonium",
	"Americium",
	"Curium",
	"Berkelium",
	"Californium",
	"Einsteinium",
	"Fermium",
	"Mendelevium",
	"Nobelium",
	"Lawrencium",
	"Rutherfordium",
	"Dubnium",
	"Seaborgium",
	"Bohrium",
	"Hassium",
	"Meitnerium",
	"Darmstadtium",
	"Roentgenium",
	"Copernicium",
	"Nihonium",
	"Flerovium",
	"Moscovium",
	"Livermorium",
	"Tenness",
	"Oganesson"
};

#else
#error Translation language not specified.
#endif

int _lastForm = FormLeft;
int _displayElement = -1;

Char *GetField(UInt16 formId, UInt16 fieldId);
void SetField(UInt16 formId, UInt16 fieldId, Char *str);

Err ApplicationStart(void);
Err ApplicationStop(void);
void ApplicationEventLoop(void);
Boolean ApplicationHandleEvent(EventPtr event);
Boolean FormMainHandleEvent(EventPtr event);
Boolean FormAboutHandleEvent(EventPtr event);
Boolean FormInfoHandleEvent(EventPtr event);

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	Err error;
	if(cmd == sysAppLaunchCmdNormalLaunch)
	{
		if((error = ApplicationStart()) != 0)
		{
			return error;
		}

		ApplicationEventLoop();

		if((error = ApplicationStop()) != 0)
		{
			return error;
		}
	}

	return 0;
}

Err ApplicationStart(void)
{
	Err returnCode = 0;
	FrmGotoForm(FormLeft);
	return returnCode;
}

Err ApplicationStop(void)
{
	Err returnCode = 0;
	FrmCloseAllForms();
	return returnCode;
}

void ApplicationEventLoop(void)
{
	EventType event;
	Err error;
	do
	{
		EvtGetEvent(&event, evtWaitForever);
		if(SysHandleEvent(&event))
			continue;

		if(MenuHandleEvent(NULL, &event, &error))
			continue;

		if(ApplicationHandleEvent(&event))
			continue;

		FrmDispatchEvent(&event);
	}
	while(event.eType != appStopEvent);
}

Boolean ApplicationHandleEvent(EventPtr event)
{
	Boolean handled = false;
	if(event->eType == frmLoadEvent)
	{
		UInt16 formId = event->data.frmLoad.formID;
		FormType *form = FrmInitForm(formId);
		FrmSetActiveForm(form);
		switch(formId)
		{
			case FormLeft:
				_lastForm = FormLeft;
				FrmSetEventHandler(form, FormMainHandleEvent);
				handled = true;
				break;

			case FormRight:
				_lastForm = FormRight;
				FrmSetEventHandler(form, FormMainHandleEvent);
				handled = true;
				break;

			case FormInfo:
				FrmSetEventHandler(form, FormInfoHandleEvent);
				handled = true;
				break;

			case FormAbout:
				FrmSetEventHandler(form, FormAboutHandleEvent);
				handled = true;
				break;

			default:
				break;
		}
	}

	return handled;
}

Boolean FormMainHandleEvent(EventPtr event)
{
	Boolean handled = false;
	FormType *form = FrmGetActiveForm();
	switch(event->eType)
	{
		case frmOpenEvent:
			FrmDrawForm(form);
			handled = true;
			break;

		case menuEvent:
			MenuEraseStatus(0);
			if(event->data.menu.itemID == MenuOptionsAbout)
			{
				FrmGotoForm(FormAbout);
			}

			handled = true;
			break;

		case ctlSelectEvent:
			if(event->data.ctlSelect.controlID == ButtonGoRight)
			{
				FrmGotoForm(FormRight);
				handled = true;
			}
			else if(event->data.ctlSelect.controlID == ButtonGoLeft)
			{
				FrmGotoForm(FormLeft);
				handled = true;
			}
			else if(event->data.ctlSelect.controlID >= 5000 &&
				event->data.ctlSelect.controlID <= 5117)
			{
				_displayElement = event->data.ctlSelect.controlID - 5000;
				FrmGotoForm(FormInfo);
				handled = true;
			}
			break;
	}

	return handled;
}

Boolean FormInfoHandleEvent(EventPtr event)
{
	Boolean handled = false;
	FormType *form = FrmGetActiveForm();
	switch(event->eType)
	{
		case frmOpenEvent:
		{
			int i = 0;
			char buf[48];
			RectangleType rect;
			StrCopy(buf, _elementData[_displayElement].Group);
			if(buf[0] == '1' && buf[1] == '8')
			{
				StrCat(buf, " (");
				StrCat(buf, NOBLE_GAS);
				StrCat(buf, ")");
			}
			else if(buf[0] == '1' && buf[1] == '7')
			{
				StrCat(buf, " (");
				StrCat(buf, HALOGEN);
				StrCat(buf, ")");
			}
			else if(buf[0] == '1' && buf[1] == '\0' &&
				_elementData[_displayElement].Symbol[0] != 'H')
			{
				StrCat(buf, " (");
				StrCat(buf, ALKALI_METAL);
				StrCat(buf, ")");
			}
			else if(buf[0] == '2' && buf[1] == '\0')
			{
				StrCat(buf, " (");
				StrCat(buf, ALKALINE_EARTH_METAL);
				StrCat(buf, ")");
			}

			SetField(FormInfo, InfoFieldGroup, buf);
			SetField(FormInfo, InfoFieldPeriod, _elementData[_displayElement].Period);
			SetField(FormInfo, InfoFieldMelt, _elementData[_displayElement].Melt);
			SetField(FormInfo, InfoFieldBoil, _elementData[_displayElement].Boil);
			SetField(FormInfo, InfoFieldElNeg, _elementData[_displayElement].ElNeg);

			FrmDrawForm(form);
			rect.topLeft.x = 4;
			rect.topLeft.y = 16;
			rect.extent.x = 32;
			rect.extent.y = 38;
			WinDrawRectangleFrame(rectangleFrame, &rect);
			FntSetFont(0);
			StrIToA(buf, _displayElement + 1);
			WinDrawChars(buf, StrLen(buf), 8, 18);
			FntSetFont(2);
			i = _elementData[_displayElement].Symbol[1] ? 2 : 1;
			WinDrawChars(_elementData[_displayElement].Symbol, i, 20 - FntCharsWidth(_elementData[_displayElement].Symbol, i) / 2, 28);
			FntSetFont(1);
			WinDrawChars(_elementNames[_displayElement], StrLen(_elementNames[_displayElement]), 42, 16);
			FntSetFont(0);
			i = StrLen(_elementData[_displayElement].Mass);
			WinDrawChars(_elementData[_displayElement].Mass, i,
				20 - FntCharsWidth(_elementData[_displayElement].Mass, i) / 2, 42);

			handled = true;
			break;
		}

		case ctlSelectEvent:
			if(event->data.ctlSelect.controlID == InfoButtonReturn)
			{
				FrmGotoForm(_lastForm);
				handled = true;
			}
			break;
	}

	return handled;
}

Boolean FormAboutHandleEvent(EventPtr event)
{
	Boolean handled = false;
	switch(event->eType)
	{
		case frmOpenEvent:
			SetField(FormAbout, AboutFieldText,
				"Periodic Table for the PalmPilot\n"
				"---\n"
				"Anton Tchekov");
			FrmDrawForm(FrmGetActiveForm());
			handled = true;
			break;

		case ctlSelectEvent:
			if(event->data.ctlSelect.controlID == AboutButtonReturn)
			{
				FrmGotoForm(FormLeft);
				handled = true;
			}
			break;
	}

	return handled;
}

void SetField(UInt16 formId, UInt16 fieldId, Char *str)
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

Char *GetField(UInt16 formId, UInt16 fieldId)
{
	FormPtr form = FrmGetFormPtr(formId);
	UInt16 object = FrmGetObjectIndex(form, fieldId);
	FieldPtr field = (FieldPtr)FrmGetObjectPtr(form, object);
	return FldGetTextPtr(field);
}

