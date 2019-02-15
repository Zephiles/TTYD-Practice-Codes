#include "global.h"
#include "items.h"

#include <cstdio>

namespace mod {

const char *RootLines[] = 
{
	"Return",
	"Inventory",
	"Cheats",
	"Stats",
	"Battles",
	"Displays",
	"Warps",
};

const char *InventoryLines[] = 
{
	"Return",
	"Standard Items",
	"Important Items",
	"Badges",
	"Stored Items",
};

const char *InventoryOptionLines[] = 
{
	"Return",
	"Add By Id",
	"Add By Icon",
	"Duplicate",
	"Change By Id",
	"Change By Icon",
	"Delete",
};

const char *CheatsLines[] = 
{
	"Return",
	"Change Sequence",
	"Save Coordinates",
	"Load Coordinates",
	"Spawn Item",
	"Save Anywhere",
	"Text Storage",
	"Time Stop Text Storage",
	"Speed Up Mario",
	"Disable Non-Cutscene Battles",
	"Auto Action Commands",
	"Infinite Item Usage",
	"Reload Room",
	"Levitate",
	"Lock Mario's HP To Its Max",
	"Allow Running From Any Battle",
	"Bobbery Early",
	"Force NPC Item Drop",
	"Clear Area Flags",
};

const char *CheatsChangeSequenceOptionsLines[] = 
{
	"Return",
	"Change Value",
};

const char *CheatsStandardOptionsLines[] = 
{
	"Return",
	"Turn On/Off",
	"Change Button Combo",
};

const char *CheatsNoButtonComboOptionsLines[] = 
{
	"Return",
	"Turn On/Off",
};

const char *CheatsForceItemDropOptionsLines[] = 
{
	"Return",
	"Turn On/Off",
	"Change Item By Id",
};

const char *CheatsClearAreaFlags[] = 
{
	"Return",
	"Select Area",
	"Clear Flags",
};

const char *CheatsForceItemDropAreas[] = 
{
	"gor",
	"tik",
	"hei",
	"nok",
	"gon",
	"win",
	"mri",
	"tou",
	"usu",
	"gra",
	"jin",
	"muj",
	"dou",
	"hom",
	"rsh",
	"eki",
	"pik",
	"bom",
	"moo",
	"aji",
	"las",
	"jon",
};
uint8_t CheatsForceItemDropAreasSize = sizeof(CheatsForceItemDropAreas) / sizeof(CheatsForceItemDropAreas[0]);

const char *CheatsForceItemDropAreasFullNames[] = 
{
	"Rogueport",
	"Rogueport Sewers",
	"Petal Meadows",
	"Petalburg",
	"Hooktail Castle",
	"Boggly Woods",
	"The Great Tree",
	"Glitzville",
	"Twilight Town",
	"Twilight Trail",
	"Creepy Steeple",
	"Keelhaul Key",
	"Pirate's Grotto",
	"Riverside Station (Outside area with the train)",
	"Excess Express",
	"Riverside Station",
	"Poshley Heights",
	"Fahr Outpost",
	"Moon",
	"X-Naut Fortress",
	"Palace of Shadow",
	"Pit of 100 Trials",
};

const char *StatsLines[] = 
{
	"Return",
	"Mario",
	"Partners",
	"Followers",
};

const char *StatsMarioLines[] = 
{
	"Return",
	"Change Values",
};

const char *StatsPartnerLines[] = 
{
	"Return",
	"Goombella",
	"Koops",
	"Flurrie",
	"Yoshi",
	"Vivian",
	"Bobbery",
	"Ms. Mowz",
};

const char *StatsMarioOptionsLines[] = 
{
	"Coins",
	"HP",
	"FP",
	"BP",
	"Max HP",
	"Max FP",
	"Level",
	"Rank",
	"Star Points",
	"Star Pieces",
	"Shine Sprites",
	"Special Moves",
	"Star Power",
	"Max Star Power",
	"Shop Points",
	"Piantas Stored",
	"Current Piantas",
};
uint8_t StatsMarioOptionsLinesSize = sizeof(StatsMarioOptionsLines) / sizeof(StatsMarioOptionsLines[0]);

uint16_t StatsMarioIcons[] = 
{
	#ifndef TTYD_JP
	403, // Coins
	423, // HP
	424, // FP
	425, // BP
	423, // Max HP
	424, // Max FP
	422, // Level
	422, // Rank
	404, // Star Points
	405, // Star Pieces
	266, // Shine Sprites
	415, // Special Moves
	154, // Star Power
	154, // Max Star Power
	390, // Shop Points
	327, // Piantas Stored
	327, // Current Piantas
	#else
	395, // Coins
	415, // HP
	416, // FP
	417, // BP
	415, // Max HP
	416, // Max FP
	414, // Level
	414, // Rank
	396, // Star Points
	397, // Star Pieces
	258, // Shine Sprites
	407, // Special Moves
	146, // Star Power
	146, // Max Star Power
	382, // Shop Points
	319, // Piantas Stored
	319, // Current Piantas
	#endif
};

const char *MarioStatsSpecialMovesOptions[] = 
{
	"Sweet Treat",
	"Earth Tremor",
	"Clock Out",
	"Power Lift",
	"Art Attack",
	"Sweet Feast",
	"Showstopper",
	"Supernova",
};
uint8_t MarioStatsSpecialMovesOptionsSize = sizeof(MarioStatsSpecialMovesOptions) / sizeof(MarioStatsSpecialMovesOptions[0]);

const char *StatsPartnerOptionsLines[] = 
{
	"HP",
	"Max HP",
	"Rank",
	"Toggle",
	"Bring Out",
	"Remove From Overworld",
};
uint8_t StatsPartnerOptionsLinesSize = sizeof(StatsPartnerOptionsLines) / sizeof(StatsPartnerOptionsLines[0]);

const char *StatsFollowerLines [] = 
{
	"Return",
	"Bring Follower Out",
	"Remove Current Follower",
};

const char *StatsFollowerOptionsLines[] = 
{
	"Egg",
	"Flavio",
	"Punio",
	"Frankly",
	"Gus",
};
uint8_t StatsFollowerOptionsLinesSize = sizeof(StatsFollowerOptionsLines) / sizeof(StatsFollowerOptionsLines[0]);

const char *BattlesLines[] = 
{
	"Return",
	"Choose Actor",
};

const char *BattlesActorsLines[] = 
{
	"Goomba",
	"Paragoomba",
	"Spiky Goomba",
	"Spinia",
	"Spania",
	"Lord Crump - Prologue",
	"Gus",
	"Blooper",
	"Left Tentacle",
	"Right Tentacle",
	"Koopatrol",
	"Magikoopa",
	"Magikoopa Clone",
	"Koopa Troopa",
	"Paratroopa",
	"Fuzzy",
	"Dull Bones",
	"Bald Cleft",
	"Bristle",
	"Gold Fuzzy",
	"Fuzzy Horde",
	"Red Bones",
	"Hooktail",
	"Dark Puff",
	"Pale Piranha",
	"Cleft",
	"Pider",
	"X-Naut",
	"Yux",
	"Mini-Yux",
	"Beldam - Ch2",
	"Marilyn - Ch2",
	"Vivian - Ch2",
	"Magnus",
	"X-Fist",
	"Goomba - Glitzville",
	"KP Koopa",
	"KP Paratroopa",
	"Pokey",
	"Lakitu",
	"Spiny",
	"Hyper Bald Cleft",
	"Bob_omb",
	"Bandit",
	"Big Bandit",
	"Red Spiky Buzzy",
	"Shady Koopa",
	"Shady Paratroopa",
	"Red Magikoopa",
	"Red Magikoopa Clone",
	"White Magikoopa",
	"White Magikoopa Clone",
	"Green Magikoopa",
	"Green Magikoopa Clone",
	"Dark Craw",
	"Hammer Bro",
	"Boomerang Bro",
	"Fire Bro",
	"Red Chomp",
	"Dark Koopatrol",
	"Iron Cleft - Red",
	"Iron Cleft - Green",
	"Bowser - Ch3",
	"Rawk Hawk",
	"Macho Grubba",
	"Hyper Goomba",
	"Hyper Paragoomba",
	"Hyper Spiky Goomba",
	"Crazee Dayzee",
	"Amazy Dayzee",
	"Hyper Cleft",
	"Buzzy Beetle",
	"Spike Top",
	"Swooper",
	"Boo",
	"Atomic Boo",
	"Doopliss - Ch4 - Fight 1",
	"Doopliss - Ch4 - Invincible",
	"Doopliss - Ch4 - Fight 2",
	"Goombella - Ch4",
	"Koops - Ch4",
	"Yoshi - Ch4",
	"Flurrie - Ch4",
	"Ember",
	"Lava Bubble",
	"Green Fuzzy",
	"Flower Fuzzy",
	"Putrid Piranha",
	"Parabuzzy",
	"Bill Blaster",
	"Bullet Bill",
	"Bulky Bob_omb",
	"Cortez",
	"Cortez - ???",
	"Hook",
	"Rapier",
	"Sword",
	"Sabre",
	"Lord Crump - Ch5",
	"X-Nauts - Stack",
	"X-Nauts - Ceiling",
	"X-Nauts - Crowd",
	"Ruff Puff",
	"Poison Pokey",
	"Spiky Parabuzzy",
	"Dark Boo",
	"Smorg",
	"Miasma - Leftmost",
	"Miasma - Topmost",
	"Miasma - Rightmost",
	"Miasma - Claw",
	"Ice Puff",
	"Frost Piranha",
	"Moon Cleft",
	"Z-Yux",
	"Mini-Z-Yux",
	"X-Yux",
	"Mini-X-Yux",
	"X-Naut PhD",
	"Elite X-Naut",
	"Magnus 2.0",
	"X-Punch",
	"Swoopula",
	"Phantom Ember",
	"Bombshell Bill Blaster",
	"Bombshell Bill",
	"Chain Chomp",
	"Dark Wizzerd",
	"Dark Wizzerd Clone",
	"Dry Bones",
	"Dark Bones",
	"Gloomtail",
	"Beldam Ch8",
	"Marilyn Ch8",
	"Doopliss - Ch8",
	"Doopliss - Ch8 - Mario",
	"Doopliss - Ch8 - Goombella",
	"Doopliss - Ch8 - Koops",
	"Doopliss - Ch8 - Yoshi",
	"Doopliss - Ch8 - Flurrie",
	"Doopliss - Ch8 - Vivian",
	"Doopliss - Ch8 - Bobbery",
	"Doopliss - Ch8 - Ms. Mowz",
	"Bowser - Ch8",
	"Kammy Koopa",
	"Grodus",
	"Grodus X",
	"Shadow Queen - Phase 1",
	"Shadow Queen - Invincible",
	"Shadow Queen - Phase 2",
	"Hand",
	"Dead Hands",
	"Gloomba",
	"Paragloomba",
	"Spiky Gloomba",
	"Dark Koopa",
	"Dark Paratroopa",
	"Badge Bandit",
	"Dark Lakitu",
	"Sky Blue Spiny",
	"Wizzerd",
	"Piranha Plant",
	"Spunia",
	"Arantula",
	"Dark Bristle",
	"Poison Puff",
	"Swampire",
	"Bob_ulk",
	"Elite Wizzerd",
	"Elite Wizzerd Clone",
	"Bonetail",
	"Unknown 172",
	"Unknown 173",
	"Unknown 174",
	"Unknown 175",
	"Unknown 176",
	"Unknown 177",
	"Unknown 178",
	"Unknown 179",
	"Unknown 180",
	"Unknown 181",
	"Unknown 182",
	"Unknown 183",
	"Unknown 184",
	"Unknown 185",
	"Unknown 186",
	"Unknown 187",
	"Unknown 188",
	"Unknown 189",
	"Unknown 190",
	"Unknown 191",
	"Unknown 192",
	"Unknown 193",
	"Unknown 194",
	"Unknown 195",
	"Unknown 196",
	"Unknown 197",
	"Unknown 198",
	"Unknown 199",
	"Unknown 200",
	"Unknown 201",
	"Unknown 202",
	"Unknown 203",
	"Unknown 204",
	"Unknown 205",
	"Unknown 206",
	"Unknown 207",
	"Unknown 208",
	"Unknown 209",
	"Unknown 210",
	"Unknown 211",
	"Unknown 212",
	"Unknown 213",
	"Unknown 214",
	"Unknown 215",
	"Unknown 216",
	"Unknown 217",
	"Unknown 218",
	"Bomb Squad - Bomb",
	"System",
	"Unknown 221",
	"Mario",
	"Shell Shield",
	"Goombella",
	"Koops",
	"Yoshi",
	"Flurrie",
	"Vivian",
	"Bobbery",
	"Ms. Mowz",
};

const char *BattlesCurrentActorLines[] = 
{
	"Return",
	"Change HP",
	"Change Max HP",
	"Change FP",
	"Change Max FP",
	"Change Statuses",
};

const char *BattlesCurrentActorStats[] = 
{
	"HP",
	"Max HP",
	"FP",
	"Max FP",
};
uint8_t BattlesCurrentActorStatsSize = sizeof(BattlesCurrentActorStats) / sizeof(BattlesCurrentActorStats[0]);

const char *BattlesStatusesLines[] = 
{
	"Sleep Turns Left",
	"Immobilized Turns Left",
	"Dizzy Turns Left",
	"Poison Turns Left",
	"Poison Damage Amount",
	"Confused Turns Left",
	"Electrified Turns Left",
	"Dodgy Turns Left",
	"Burn Turns Left",
	"Frozen Turns Left",
	"Big/Shrink Turns Left",
	"Big/Shrink Power Amount",
	"Attack Up/Down Turns Left",
	"Attack Up/Down Amount",
	"Defense Up/Down Turns Left",
	"Defense Up/Down Amount",
	"Charged Turns Left",
	"Allergic Turns Left",
	"Invisible Turns Left",
	"Payback Turns Left",
	"Fast Turns Left",
	"Slow Turns Left",
	"HP Regen Turns Left",
	"HP Regen Amount",
	"FP Regen Turns Left",
	"FP Regen Amount",
	"Defeated Flag",
};
uint8_t BattlesStatusesLinesSize = sizeof(BattlesStatusesLines) / sizeof(BattlesStatusesLines[0]);

uint16_t BattlesStatusesIcons[] = 
{
	#ifndef TTYD_JP
	397, // Sleep
	398, // Stop
	379, // Dizzy
	223, // Poision
	223, // Poison Amount
	409, // Confused
	388, // Electrified
	412, // Dodgy
	15, // Burn
	392, // Frozen
	375, // Big/Shrink
	375, // Big/Shrink Amount
	47, // Attack Up/Down
	47, // Attack Up/Down Amount
	48, // Defense Up/Down
	48, // Defense Up/Down Amount
	37, // Charged
	186, // Allergic
	408, // Invisible
	203, // Payback
	426, // Fast
	390, // Slow
	386, // HP Regen
	386, // HP Regen Amount
	389, // FP Regen
	389, // FP Regen Amount
	362, // Defeated Flag
	#else
	389, // Sleep
	390, // Stop
	371, // Dizzy
	215, // Poison
	215, // Poison Amount
	401, // Confused
	380, // Electrified
	404, // Dodgy
	15, // Burn
	384, // Frozen
	367, // Big/Shrink
	367, // Big/Shrink Amount
	47, // Attack Up/Down
	47, // Attack Up/Down Amount
	48, // Defense Up/Down
	48, // Defense Up/Down Amount
	37, // Charged
	178, // Allergic
	400, // Invisible
	195, // Payback
	418, // Fast
	382, // Slow
	378, // HP Regen
	378, // HP Regen Amount
	381, // FP Regen
	381, // FP Regen Amount
	354, // Defeated Flag
	#endif
};

const char *DisplaysLines[] = 
{
	"Return",
	"On-Screen Timer",
	"Mario's Coordinates",
	"Jump Storage",
	"Button Input Display",
	"Art Attack Hitboxes",
	"Yoshi Skip",
	"Palace Skip",
};

const char *OnScreenTimerOptionsLines[] = 
{
	"Return",
	"Turn On/Off",
	"Change Start/Pause/Resume Button Combo",
	"Change Reset Button Combo",
};

const char *OnScreenTimerOptions[] = 
{
	"Start/Pause/Resume Button Combo",
	"Reset Button Combo",
};
uint8_t OnScreenTimerOptionsSize = sizeof(OnScreenTimerOptions) / sizeof(OnScreenTimerOptions[0]);

const char *WarpLines[] = 
{
	"Return",
	"Select Warp",
};

const char *WarpDestinations[] = 
{
	"gor_01",
	"tik_00",
	"hei_00",
	"gon_00",
	"win_06",
	"mri_01",
	"tou_02",
	"tou_05",
	"usu_00",
	"jin_00",
	"muj_01",
	"dou_00",
	"rsh_02_a",
	"eki_00",
	"pik_00",
	"bom_00",
	"moo_00",
	"aji_00",
	"aji_13",
	"las_00",
	"las_09",
	"las_27",
	"las_29",
	"jon_0x",
	"title",
};
uint8_t WarpDestinationsSize = sizeof(WarpDestinations) / sizeof(WarpDestinations[0]);

const char *WarpDescriptions[] = 
{
	"Rogueport Central",
	"Rogueport Sewers - Underground Shop",
	"Petal Meadows - Entrance",
	"Hooktail Castle - Entrance",
	"Boggly Woods - Entrance",
	"The Great Tree - Entrance",
	"Glitzville - Lobby",
	"Glitzville - Grubba's Office",
	"Twilight Town - West Side",
	"Creepy Steeple - Entrance",
	"Keelhaul Key - Shantytown",
	"Pirate's Grotto - Entrance",
	"Excess Express - Passenger Car - Cabins 3-5 - Day",
	"Riverside Station - Entrance",
	"Poshley Heights - Station",
	"Fahr Outpost - Pipe Entrance",
	"Moon - Entrance",
	"X-Naut Fortress - Entrance",
	"X-Naut Fortress - Factory",
	"Palace of Shadow - Entrance",
	"Palace of Shadow - Outside Riddle Tower",
	"Palace of Shadow - Room Before Grodus",
	"Palace of Shadow - Shadow Queen (Battle 2) -\nSequence is automatically set",
	"Pit of 100 Trials - You must select a floor when this\noption is chosen",
	"Title Screen",
};

const char *ReturnPlaceholder[] = 
{
	"Return",
};

char ButtonInputDisplay[] = {'(', ')', 'v', '^', 'Z', 'R', 'L', 'A', 'B', 'X', 'Y', 'S'};

struct Menus Menu[20];
struct Cheats Cheat[17];
bool Displays[7];
char DisplayBuffer[256];

struct AutoIncrement AdjustableValueMenu;
struct CheatsHandleDisplayButtons CheatsDisplayButtons;
struct MarioPartnerPositionsStruct MarioPartnerPositions;
struct SaveAnywhereStruct SaveAnywhere;
struct SpawnItems SpawnItem;
struct ReloadRoomStruct ReloadRoom;
struct TrickDisplay YoshiSkip;
struct TrickDisplay PalaceSkip;
struct OnScreenTimerDisplay OnScreenTimer;

bool MenuIsDisplayed 					= false;
bool PreventClosingMenu 				= false;
bool ChangingCheatButtonCombo 			= false;
uint8_t CurrentMenu 					= 0;
uint8_t CurrentMenuOption 				= 0;
uint8_t MenuSelectedOption 				= 0;
uint8_t SecondaryMenuOption 			= 0;
uint8_t SelectedOption 					= 0;
uint8_t CurrentPage 					= 0;
uint8_t SecondaryPage 					= 0;
int8_t FunctionReturnCode 				= 0;
uint32_t Timer 							= 0;
uint8_t MenuSelectionStates 			= 0;
int32_t MenuSecondaryValue 				= 0;
uint8_t FrameCounter 					= 0;

#ifdef TTYD_US
uint32_t r13 							= 0x8041CF20;
char *NextBero 							= reinterpret_cast<char *>(r13 + 0x1688);
char *NextMap 							= reinterpret_cast<char *>(r13 + 0x16A8);
char *NextArea 							= reinterpret_cast<char *>(r13 + 0x16C8);
ItemData *ItemDataTable 				= reinterpret_cast<ItemData *>(0x803108A8);
uint32_t PauseMenuStartAddress 			= r13 + 0x1D10;
uint32_t wp_fadedrv_Address 			= r13 + 0x1700;
uint32_t _mapEntAddress 				= r13 + 0x1740;
uint32_t NPCAddressesStart 				= r13 + 0x19A0;
uint32_t BattleAddressesStart 			= r13 + 0x1C70;
uint32_t PiantaParlorAddressesStart 	= r13 + 0x1E08;
uint32_t seqMainAddress 				= r13 + 0x1860;
uint32_t FieldItemsAddressesStart 		= 0x803DC294;
#elif defined TTYD_JP
uint32_t r13 							= 0x80417260;
char *NextBero 							= reinterpret_cast<char *>(r13 + 0x1128);
char *NextMap 							= reinterpret_cast<char *>(r13 + 0x1148);
char *NextArea 							= reinterpret_cast<char *>(r13 + 0x1168);
ItemData *ItemDataTable 				= reinterpret_cast<ItemData *>(0x8030EE58);
uint32_t PauseMenuStartAddress 			= r13 + 0x17B0;
uint32_t wp_fadedrv_Address 			= r13 + 0x11A0;
uint32_t _mapEntAddress 				= r13 + 0x11E0;
uint32_t NPCAddressesStart 				= r13 + 0x1448;
uint32_t BattleAddressesStart  			= r13 + 0x1710;
uint32_t PiantaParlorAddressesStart 	= r13 + 0x18F8;
uint32_t seqMainAddress 				= r13 + 0x1300;
uint32_t FieldItemsAddressesStart 		= 0x803D8714;
#elif defined TTYD_EU
uint32_t r13 							= 0x80429760;
char *NextBero 							= reinterpret_cast<char *>(r13 + 0x1768);
char *NextMap 							= reinterpret_cast<char *>(r13 + 0x1788);
char *NextArea 							= reinterpret_cast<char *>(r13 + 0x17A8);
ItemData *ItemDataTable 				= reinterpret_cast<ItemData *>(0x8031C638);
uint32_t PauseMenuStartAddress 			= r13 + 0x1DF0;
uint32_t wp_fadedrv_Address 			= r13 + 0x17E0;
uint32_t _mapEntAddress 				= r13 + 0x1820;
uint32_t NPCAddressesStart 				= r13 + 0x1A80;
uint32_t BattleAddressesStart  			= r13 + 0x1D50;
uint32_t PiantaParlorAddressesStart 	= r13 + 0x1F38;
uint32_t seqMainAddress 				= r13 + 0x1940;
uint32_t FieldItemsAddressesStart 		= 0x803E82F4;
#endif

uint32_t GlobalWorkPointer 				= r13 - 0x6F50;
uint32_t titleMainAddress 				= r13 - 0x7F80;

// Variables used by cheats
int16_t ForcedNPCItemDrop 				= SleepySheep;
bool MarioFreeze 						= false;
uint16_t JumpStorageSetCounter 			= 0;

void initMenuVars()
{
	Menu[ROOT].TotalMenuOptions 						= sizeof(RootLines) / sizeof(RootLines[0]);
	Menu[ROOT].ColumnSplitAmount 						= Menu[ROOT].TotalMenuOptions;
	Menu[ROOT].Line 									= RootLines;

	Menu[INVENTORY].TotalMenuOptions 					= sizeof(InventoryLines) / sizeof(InventoryLines[0]);
	Menu[INVENTORY].ColumnSplitAmount 					= Menu[INVENTORY].TotalMenuOptions;
	Menu[INVENTORY].PreviousMenu 						= ROOT;
	Menu[INVENTORY].Line 								= InventoryLines;
	
	Menu[CHEATS].TotalMenuOptions 						= sizeof(CheatsLines) / sizeof(CheatsLines[0]);
	Menu[CHEATS].ColumnSplitAmount 						= 18;
	Menu[CHEATS].PreviousMenu 							= ROOT;
	Menu[CHEATS].Line 									= CheatsLines;
	
	Menu[STATS].TotalMenuOptions 						= sizeof(StatsLines) / sizeof(StatsLines[0]);
	Menu[STATS].ColumnSplitAmount 						= Menu[STATS].TotalMenuOptions;
	Menu[STATS].PreviousMenu 							= ROOT;
	Menu[STATS].Line 									= StatsLines;
	
	Menu[BATTLES].TotalMenuOptions 						= sizeof(BattlesLines) / sizeof(BattlesLines[0]);
	Menu[BATTLES].ColumnSplitAmount 					= Menu[BATTLES].TotalMenuOptions;
	Menu[BATTLES].PreviousMenu 							= ROOT;
	Menu[BATTLES].Line 									= BattlesLines;
	
	Menu[DISPLAYS].TotalMenuOptions 					= sizeof(DisplaysLines) / sizeof(DisplaysLines[0]);
	Menu[DISPLAYS].ColumnSplitAmount 					= Menu[DISPLAYS].TotalMenuOptions;;
	Menu[DISPLAYS].PreviousMenu 						= ROOT;
	Menu[DISPLAYS].Line 								= DisplaysLines;
	
	Menu[WARPS].TotalMenuOptions 						= sizeof(WarpLines) / sizeof(WarpLines[0]);
	Menu[WARPS].ColumnSplitAmount 						= Menu[WARPS].TotalMenuOptions;
	Menu[WARPS].PreviousMenu 							= ROOT;
	Menu[WARPS].Line 									= WarpLines;
	
	Menu[INVENTORY_MAIN].TotalMenuOptions 				= sizeof(InventoryOptionLines) / sizeof(InventoryOptionLines[0]);
	Menu[INVENTORY_MAIN].ColumnSplitAmount 				= Menu[INVENTORY_MAIN].TotalMenuOptions;
	Menu[INVENTORY_MAIN].PreviousMenu 					= INVENTORY;
	Menu[INVENTORY_MAIN].Line 							= InventoryOptionLines;
	
	Menu[CHEATS_CHANGE_SEQUENCE].TotalMenuOptions 		= sizeof(CheatsChangeSequenceOptionsLines) / sizeof(CheatsChangeSequenceOptionsLines[0]);
	Menu[CHEATS_CHANGE_SEQUENCE].ColumnSplitAmount 		= Menu[CHEATS_CHANGE_SEQUENCE].TotalMenuOptions;
	Menu[CHEATS_CHANGE_SEQUENCE].PreviousMenu 			= CHEATS;
	Menu[CHEATS_CHANGE_SEQUENCE].Line 					= CheatsChangeSequenceOptionsLines;
	
	Menu[CHEATS_STANDARD].TotalMenuOptions 				= sizeof(CheatsStandardOptionsLines) / sizeof(CheatsStandardOptionsLines[0]);
	Menu[CHEATS_STANDARD].ColumnSplitAmount 			= Menu[CHEATS_STANDARD].TotalMenuOptions;
	Menu[CHEATS_STANDARD].PreviousMenu 					= CHEATS;
	Menu[CHEATS_STANDARD].Line 							= CheatsStandardOptionsLines;
	
	Menu[CHEATS_NO_BUTTON_COMBO].TotalMenuOptions 		= sizeof(CheatsNoButtonComboOptionsLines) / sizeof(CheatsNoButtonComboOptionsLines[0]);
	Menu[CHEATS_NO_BUTTON_COMBO].ColumnSplitAmount 		= Menu[CHEATS_NO_BUTTON_COMBO].TotalMenuOptions;
	Menu[CHEATS_NO_BUTTON_COMBO].PreviousMenu 			= CHEATS;
	Menu[CHEATS_NO_BUTTON_COMBO].Line 					= CheatsNoButtonComboOptionsLines;
	
	Menu[CHEATS_NPC_FORCE_DROP].TotalMenuOptions 		= sizeof(CheatsForceItemDropOptionsLines) / sizeof(CheatsForceItemDropOptionsLines[0]);
	Menu[CHEATS_NPC_FORCE_DROP].ColumnSplitAmount 		= Menu[CHEATS_NPC_FORCE_DROP].TotalMenuOptions;
	Menu[CHEATS_NPC_FORCE_DROP].PreviousMenu 			= CHEATS;
	Menu[CHEATS_NPC_FORCE_DROP].Line 					= CheatsForceItemDropOptionsLines;
	
	Menu[CHEATS_CLEAR_AREA_FLAGS].TotalMenuOptions 		= sizeof(CheatsClearAreaFlags) / sizeof(CheatsClearAreaFlags[0]);
	Menu[CHEATS_CLEAR_AREA_FLAGS].ColumnSplitAmount 	= Menu[CHEATS_CLEAR_AREA_FLAGS].TotalMenuOptions;
	Menu[CHEATS_CLEAR_AREA_FLAGS].PreviousMenu 			= CHEATS;
	Menu[CHEATS_CLEAR_AREA_FLAGS].Line 					= CheatsClearAreaFlags;
	
	Menu[STATS_MARIO].TotalMenuOptions 					= sizeof(StatsMarioLines) / sizeof(StatsMarioLines[0]);
	Menu[STATS_MARIO].ColumnSplitAmount 				= Menu[STATS_MARIO].TotalMenuOptions;;
	Menu[STATS_MARIO].PreviousMenu 						= STATS;
	Menu[STATS_MARIO].Line 								= StatsMarioLines;
	
	Menu[STATS_PARTNERS].TotalMenuOptions 				= sizeof(StatsPartnerLines) / sizeof(StatsPartnerLines[0]);
	Menu[STATS_PARTNERS].ColumnSplitAmount 				= Menu[STATS_PARTNERS].TotalMenuOptions;;
	Menu[STATS_PARTNERS].PreviousMenu 					= STATS;
	Menu[STATS_PARTNERS].Line 							= StatsPartnerLines;
	
	Menu[STATS_FOLLOWERS].TotalMenuOptions 				= sizeof(StatsFollowerLines) / sizeof(StatsFollowerLines[0]);
	Menu[STATS_FOLLOWERS].ColumnSplitAmount 			= Menu[STATS_FOLLOWERS].TotalMenuOptions;;
	Menu[STATS_FOLLOWERS].PreviousMenu 					= STATS;
	Menu[STATS_FOLLOWERS].Line 							= StatsFollowerLines;
	
	Menu[BATTLES_CURRENT_ACTOR].TotalMenuOptions 		= sizeof(BattlesCurrentActorLines) / sizeof(BattlesCurrentActorLines[0]);
	Menu[BATTLES_CURRENT_ACTOR].ColumnSplitAmount 		= Menu[BATTLES_CURRENT_ACTOR].TotalMenuOptions;
	Menu[BATTLES_CURRENT_ACTOR].PreviousMenu 			= BATTLES;
	Menu[BATTLES_CURRENT_ACTOR].Line 					= BattlesCurrentActorLines;
	
	Menu[BATTLES_STATUSES].TotalMenuOptions 			= sizeof(BattlesStatusesLines) / sizeof(BattlesStatusesLines[0]);
	Menu[BATTLES_STATUSES].ColumnSplitAmount 			= 12;
	Menu[BATTLES_STATUSES].PreviousMenu 				= BATTLES_CURRENT_ACTOR;
	Menu[BATTLES_STATUSES].Line 						= BattlesStatusesLines;
	
	Menu[DISPLAYS_ONSCREEN_TIMER].TotalMenuOptions 		= sizeof(OnScreenTimerOptionsLines) / sizeof(OnScreenTimerOptionsLines[0]);
	Menu[DISPLAYS_ONSCREEN_TIMER].ColumnSplitAmount 	= Menu[DISPLAYS_ONSCREEN_TIMER].TotalMenuOptions;
	Menu[DISPLAYS_ONSCREEN_TIMER].PreviousMenu 			= DISPLAYS;
	Menu[DISPLAYS_ONSCREEN_TIMER].Line 					= OnScreenTimerOptionsLines;
	
	Menu[DISPLAYS_NO_BUTTON_COMBO].TotalMenuOptions 	= sizeof(CheatsNoButtonComboOptionsLines) / sizeof(CheatsNoButtonComboOptionsLines[0]);
	Menu[DISPLAYS_NO_BUTTON_COMBO].ColumnSplitAmount 	= Menu[DISPLAYS_NO_BUTTON_COMBO].TotalMenuOptions;
	Menu[DISPLAYS_NO_BUTTON_COMBO].PreviousMenu 		= DISPLAYS;
	Menu[DISPLAYS_NO_BUTTON_COMBO].Line 				= CheatsNoButtonComboOptionsLines;
	
	Cheat[SAVE_ANYWHERE].Active 						= false;
	Cheat[LOCK_MARIO_HP_TO_MAX].Active 					= false;
	Cheat[RUN_FROM_BATTLES].Active 						= false;
	Cheat[SPAWN_ITEM].Active 							= false;
	Cheat[FORCE_ITEM_DROP].Active 						= false;
	
	Cheat[SAVE_COORDINATES].ButtonCombo 				= PAD_L | PAD_DPAD_LEFT;
	Cheat[LOAD_COORDINATES].ButtonCombo 				= PAD_L | PAD_DPAD_UP;
	Cheat[SAVE_ANYWHERE].ButtonCombo 					= PAD_Y | PAD_B;
	Cheat[TEXT_STORAGE].ButtonCombo 					= PAD_L | PAD_X;
	Cheat[TIME_STOP_TEXT_STORAGE].ButtonCombo 			= PAD_L | PAD_R;
	Cheat[SPEED_UP_MARIO].ButtonCombo 					= PAD_L | PAD_Y;
	Cheat[DISABLE_BATTLES].ButtonCombo 					= PAD_Y;
	Cheat[AUTO_ACTION_COMMANDS].ButtonCombo 			= PAD_R;
	Cheat[INFINITE_ITEM_USAGE].ButtonCombo 				= PAD_Y;
	Cheat[RELOAD_ROOM].ButtonCombo 						= PAD_L | PAD_B;
	Cheat[LEVITATE].ButtonCombo 						= PAD_L | PAD_A;
	Cheat[SPAWN_ITEM].ButtonCombo 						= PAD_L | PAD_DPAD_DOWN;
	
	OnScreenTimer.ButtonCombo[START_PAUSE_RESUME] 		= PAD_L | PAD_Z;
	OnScreenTimer.ButtonCombo[RESET] 					= PAD_L | PAD_DPAD_RIGHT;
	
	#ifndef TTYD_JP
	*reinterpret_cast<uint8_t *>(&ButtonInputDisplay[0]) = 171; // Left
	*reinterpret_cast<uint8_t *>(&ButtonInputDisplay[1]) = 187; // Right
	*reinterpret_cast<uint8_t *>(&ButtonInputDisplay[2]) = 179; // Down
	*reinterpret_cast<uint8_t *>(&ButtonInputDisplay[3]) = 178; // Up
	#endif
}

}