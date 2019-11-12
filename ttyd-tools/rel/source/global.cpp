#include "global.h"
#include "memcard.h"

namespace mod {

const char *VersionNumber = "v3.0.42";

const char *RootLines[] = 
{
	"Inventory",
	"Cheats",
	"Stats",
	"Settings",
	"Memory",
	"Battles",
	"Displays",
	"Warps",
};

const char *InventoryLines[] = 
{
	"Standard Items",
	"Important Items",
	"Badges",
	"Stored Items",
};

const char *InventoryOptionLines[] = 
{
	"Add By Id",
	"Add By Icon",
	"Duplicate",
	"Change By Id",
	"Change By Icon",
	"Delete",
};

const char *CheatsLines[] = 
{
	"Change Sequence",
	"Walk Through Most Objects",
	"Fall Through Most Objects",
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
	"Disable Pause Menu/Z Menu Sounds",
	"Bobbery Early",
	"Force NPC Item Drop",
	"Resolve Fades",
	"Lock Flags",
	"Manage Flags",
	"Clear Area Flags",
};

const char *CheatsChangeSequenceOptionsLines[] = 
{
	"Change Value",
};

#ifdef TTYD_JP
const char *CheatsEventNames[] = 
{
	"Prologue",
	"Plaza",
	"Mario's House",
	"To Rogueport",
	"Goombella Assaulted",
	"1000 X-Nauts (Before)",
	"1000 X-Nauts (After)",
	"Goombella Joins",
	"Stolen Coins",
	"Prof. Frankly",
	"Removing the Fence",
	"Frankly's Tutorial",
	"3 Goomba Goons",
	"Puni Encounter",
	"Paper Stair Switch",
	"Paper Mage 1",
	"First Flight",
	"Front of the Pedestal",
	"The Pedestal",
	"Frankly's House",
	"Badge Tutorial",
	"Blooper Battle",
	"To Stage 1",
	"Petal Meadows",
	"The Shadow of Hooktail",
	"The Flip-Bridge",
	"Petalburg (1st Time)",
	"Talking with Kroop",
	"Petalburg Gatekeeper",
	"Bald Cleft",
	"Bristle",
	"Quiz",
	"Gold Fuzzy",
	"Koops Joins",
	"Opening the Path",
	"Hooktail Castle",
	"Key",
	"Lock",
	"Father's Letter",
	"1000 Dry Bones",
	"Paper Stairs",
	"Key",
	"Lock",
	"A Suspicious Room",
	"50 Seconds to Escape",
	"Paper Mage 2",
	"Key 2",
	"Lock 2",
	"Paper Bridge (Right)",
	"Paper Bridge (Left)",
	"Ms. Mowz",
	"Key 3",
	"Lock 3",
	"Key 4",
	"Lock 4",
	"Hooktail",
	"Father & Son Reunited",
	"Peach: Caught",
	"Peach: An Open Door",
	"Peach: TEC",
	"Bowser: Bowser Castle",
	"Bowser: Castle INT.",
	"Koops Leaves",
	"A Mail from Peach",
	"Zess T.",
	"Frankly's House Stg.1",
	"Puni Encounter 2",
	"A Puni Plea for Help",
	"Puni's Secret Passage",
	"Puni Joins",
	"To Stage 2",
	"Boggly Woods",
	"A Siren Argument",
	"To The Great Tree",
	"The Great Tree",
	"Sealed Entrance",
	"Sealed Entrance 2",
	"Flurrie Intro",
	"She's Lost Her Precious",
	"To Flurrie's Place",
	"Extending Plane Panel",
	"A Shy Flurrie",
	"Flurrie: Living Room",
	"Flurrie: Request",
	"The Sketch",
	"Mario vs. Shadow Sirens",
	"Shadow Sirens Lose",
	"Flurrie Joins",
	"Secret Entrance Revealed!",
	"X-Naut Battle",
	"Rival Arrival",
	"A Dark Figure",
	"Puni Elder Imprisoned",
	"Ms. Mowz",
	"Puni Elder Freed",
	"11 Punies",
	"The Puni Elder's Powers",
	"Bubbles",
	"Jabble",
	"Inside the Blue Cell",
	"Punio & Petuni (Before)",
	"Punio & Petuni",
	"Bubbles 2",
	"Mystery Pillar",
	"Matching the Patterns",
	"Super Boots",
	"Escape",
	"Water Switch",
	"Lord Crump Appears",
	"Time Bomb Activated",
	"X-Naut Battle",
	"Boss Battle",
	"After the Boss Battle",
	"Peach: Crump & X-Nauts",
	"Peach: Shall We Dance?",
	"Bowser: Meadows",
	"Bowser: Action Stage",
	"Bowser: Petalburg",
	"Praise from the Puni",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.2",
	"Finding Don Pianta",
	"Daughter Located",
	"Blimp Ticket",
	"After Getting the Ticket",
	"To Glitzville",
	"Welcome to Glitzville",
	"Arena Intro",
	"The Guard Backs Off",
	"Grubba",
	"Gonzales is Born",
	"Match",
	"1st Match",
	"Minors - 9th Pl.",
	"Minors - 5th Pl. (Egg)",
	"Runaway Egg",
	"The Mini-Yoshi Egg",
	"Majors - 10th Place",
	"Majors - 8th Place",
	"Super Hammer",
	"Back to the Minors",
	"The Yellow Block",
	"Confiscated",
	"Majors - 4th Pl.",
	"Rawk Hawk's Warning",
	"Storage Key",
	"Storage Room Unlocked",
	"Ms. Mowz",
	"Blowing Containers",
	"Hidden Stairway",
	"Hole in the Floor",
	"Jolene's Secret",
	"Majors - 1st Pl.",
	"Lobby Poster",
	"Unlocking the Door",
	"Flattened",
	"1st Match with Rawk Hawk",
	"A New Champion",
	"Air Duct Entrance",
	"Air Duct (1st Time)",
	"Eavesdropping",
	"Air Duct Exit",
	"Grubba's Plans",
	"Boss Battle",
	"Peach: The Secret Weapon",
	"Peach: TEC's Request",
	"Peach: Incognito",
	"Peach: Grodus",
	"Peach: Back to TEC",
	"Bowser: The Great Tree",
	"Bowser: Treasure?",
	"Return to Rogueport",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.3",
	"Darkly",
	"To Stage 4",
	"Welcome to Stage 4",
	"Gatekeeper Conversation",
	"The Mayor's a Pig!",
	"Dinge",
	"The Gatekeeper's a Pig!",
	"Opening the Gate",
	"Shop Key",
	"Storage Room Unlocked",
	"Paper Mage (Tube Mod)",
	"Paper Mage (Tube Mod)",
	"Straw Tepee",
	"Tree Trunk",
	"Blow the Floor",
	"Through the Hole",
	"Trick Rock",
	"Steeple Demo (EXT.)",
	"Steeple Demo (INT.)",
	"Push the Statue",
	"Boo Box",
	"1000 Boos",
	"Key",
	"Lock",
	"Doopliss Battle",
	"Trading Places",
	"Superbombbomb Lost",
	"Waiting Doopliss",
	"Vivian",
	"Superbombbomb Found",
	"Vivian Joins",
	"Waiting Doopliss 2",
	"Breaking the Floor",
	"The Letter p",
	"Name Guessing",
	"Doopliss Battle 2",
	"Back to Normal",
	"The Ruby Star",
	"Shadow Sirens & Doopliss",
	"Peach: X-Naut PhD Report",
	"Bowser: Blimp Area",
	"Bowser: Action Stage",
	"Bowser: Rogueport Dock",
	"Leaving Twilight Town",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.4",
	"Chatting with Flavio",
	"Meet Bobbery",
	"Bobbery Declines",
	"Advice from Podley",
	"Bobbery's Decision",
	"To the Port",
	"Off to the Keys",
	"Logbook",
	"Haunted",
	"Deserted Island",
	"Flavio & Pa-Patch",
	"Bobbery & the Bridge",
	"Runaway Sailors",
	"Flavio's Wine",
	"Wine Obtained",
	"Bobbery Rescue 2",
	"Bobbery Joins",
	"Flavio's Favor 2",
	"Marco's Jewel",
	"Gems and Jingles",
	"Entering the Grotto",
	"The Call of Cortez",
	"The Blue Block",
	"Grotto Key",
	"Boat Mod Obtained",
	"Floodgate Handle",
	"Wave-dodging",
	"Shipwrecked Sailors",
	"Four-Eyes",
	"Cortez's Warning",
	"Cortez and the Treasure",
	"Kicked Off",
	"Escape Route",
	"Hole in the Wall",
	"True Identity",
	"Return to the Grotto",
	"All Aboard",
	"Pirates vs. X-Nauts",
	"X-Nauts Retreat",
	"Peach: TEC's Request 2",
	"Peach: Potion Puzzle",
	"Peach: Disk Search",
	"Peach: TEC Talk",
	"Bowser: Twilight Town",
	"Bowser: Twilight Town 2",
	"Fair Winds!",
	"Rogueport Dock",
	"A Mail from Peach",
	"Return to Keelhaul",
	"Frankly's House Stg.5",
	"An Ill Don Pianta",
	"Keelhaul Anytime",
	"Lost Wedding Ring",
	"Wedding Ring Delivery",
	"F & F Sail Off",
	"F & F Arrive",
	"Train Ticket",
	"Beldam Finds Out",
	"The Train Leaves",
	"Start",
	"The Conductor",
	"A Case of Lost Stew",
	"Stained Carpet",
	"Grilling Heff T.",
	"Galley Kettle Discovered",
	"Pennington's Assistant",
	"Sylvia",
	"Bub",
	"Bub's Request",
	"The Conductor/Zip Toad",
	"Obtain Autograph",
	"Give Autograph",
	"Talk to the Conductor",
	"Ghost T.",
	"Ghost Request",
	"The Baggage Car",
	"Ghost Diary",
	"Blanket Exchange",
	"Give Blanket",
	"End of Day 1",
	"Day 2",
	"Document",
	"Vital Paper",
	"A Vital Clue",
	"Another Clue",
	"Zip Toad Appears",
	"Zip Toad Caught",
	"Riverside Station",
	"Station Key Obtained",
	"Station Door Unlocked",
	"Origami Stairs",
	"Key",
	"Unlocking the Door",
	"Blow the Poster",
	"3 Blue Blocks",
	"Origami Stairs",
	"Spring Jump",
	"Elevator Key",
	"Ride the Elevator",
	"Smorg (1st Encounter)",
	"Swatting Smorgs",
	"Origami Stairs",
	"Drawbridge Down",
	"Conductor Appears",
	"End of Day 2",
	"Day 3 Starts",
	"Engineer",
	"Baggage Car Smorgs",
	"Escaping the Smorgs",
	"Smorg Miasma",
	"Poshley Heights",
	"Poshley Heights",
	"The Sanctum Manager",
	"3 Surprise Guests",
	"The Hidden Pipe",
	"The Real Garnet Star",
	"Welcome Back",
	"Peach: Grodus & X-Naut",
	"Peach: TEC Deleted",
	"Bowser: Rogueport",
	"Bowser: Action Stage",
	"Bowser: Rawk Hawk",
	"Rogueport Station",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.6",
	"3 Minutes Later",
	"3 Minutes Later",
	"Ultra Hammer",
	"Start",
	"Fahr Outpost (1st Visit)",
	"Mayor Bob",
	"Goldbob",
	"Back Online",
	"The Cannon",
	"To the Moon",
	"A Pipe Appears",
	"Inside the Building",
	"Elite X-Nauts",
	"Elevator Key A(Lit Floor)",
	"Elevator Activated",
	"Password for Today",
	"Password Entry",
	"Quiz Cleared",
	"Elevator Key B",
	"Elevator Activated 2",
	"Factory Door",
	"Conveyor Belt",
	"Off-screen Stairs",
	"The Door to Lord Crump",
	"Lord Crump Defeated",
	"Bowser: Poshley Heights",
	"Bowser: Poshley Sanctum",
	"The Last Mail",
	"The Hideout Explodes",
	"A Waiting Prof. Frankly ",
	"Thousand-Year Door",
	"The Pedestal",
	"To the Last Dungeon",
	"Start",
	"1000 Dry Bones",
	"Key",
	"Lock",
	"Follow the Lit Torches",
	"The Palace Garden",
	"Gloomtail",
	"Star Key Slot",
	"Moving Planets",
	"Beldam Confrontation",
	"The New Path",
	"Key",
	"Lock",
	"Star Puzzle",
	"Large Stone Block",
	"Key",
	"Lock",
	"Queen's Chamber",
	"Grodus",
	"The Return of the Queen",
	"After the Battle",
	"Epilogue",
	"End Credits",
	"Return to Rogueport",
};
#endif

#ifdef TTYD_JP
const char *WarpsEventNames[] = 
{
	"Prologue",
	"Plaza",
	"Mario's House",
	"To Rogueport",
	"Rogueport (Arrival)",
	"Goombella Assaulted",
	"1000 X-Nauts (Before)",
	"1000 X-Nauts (After)",
	"Goombella Joins",
	"Stolen Coins",
	"Prof. Frankly",
	"Removing the Fence",
	"Frankly's Tutorial",
	"3 Goomba Goons",
	"Puni Encounter",
	"Paper Stair Switch",
	"Paper Mage 1",
	"First Flight",
	"Thousand-Year Door",
	"Front of the Pedestal",
	"The Pedestal",
	"Frankly's House",
	"Badge Tutorial",
	"Blooper Battle",
	"To Stage 1",
	"Petal Meadows",
	"The Shadow of Hooktail",
	"The Flip-Bridge",
	"Petalburg (1st Time)",
	"Talking with Kroop",
	"Petalburg Gatekeeper",
	"Bald Cleft",
	"Bristle",
	"Quiz",
	"Gold Fuzzy",
	"Koops Joins",
	"Opening the Path",
	"Hooktail Castle",
	"Key",
	"Lock",
	"Father's Letter",
	"1000 Dry Bones",
	"Paper Stairs",
	"Key",
	"Lock",
	"A Suspicious Room",
	"50 Seconds to Escape",
	"Paper Mage 2",
	"Key 2",
	"Lock 2",
	"Paper Bridge (Right)",
	"Paper Bridge (Left)",
	"Ms. Mowz (w/Goombella)",
	"Ms. Mowz (w/Koops)",
	"Key 3",
	"Lock 3",
	"Key 4",
	"Lock 4",
	"Hooktail",
	"Hooktail Defeated",
	"Father & Son Reunited",
	"Peach: Caught",
	"Peach: An Open Door",
	"Peach: TEC",
	"Bowser: Bowser Castle",
	"Bowser: Entrance",
	"Bowser: Castle INT.",
	"Bowser: Kammy Enters",
	"Bowser: Departure",
	"Koops Leaves",
	"A Mail from Peach",
	"Zess T.",
	"Thousand-Year Door",
	"Frankly's House Stg.1",
	"Puni Encounter 2",
	"A Puni Plea for Help",
	"Puni's Secret Passage",
	"Puni Joins",
	"To Stage 2",
	"Boggly Woods",
	"A Siren Argument",
	"To The Great Tree",
	"The Great Tree",
	"Sealed Entrance",
	"Sealed Entrance 2",
	"Flurrie Intro",
	"She's Lost Her Precious",
	"To Flurrie's Place",
	"Extending Plane Panel",
	"The Trail",
	"A Shy Flurrie",
	"Flurrie: Living Room",
	"Flurrie: Request",
	"The Sketch",
	"Mario vs. Shadow Sirens",
	"Shadow Sirens Lose",
	"Flurrie Joins",
	"Secret Entrance Revealed!",
	"X-Naut Battle",
	"Rival Arrival",
	"A Dark Figure",
	"Puni Elder Imprisoned",
	"Ms. Mowz",
	"Puni Elder Freed",
	"11 Punies",
	"The Puni Elder's Powers",
	"Puni Switch",
	"Bubbles",
	"By Plane?",
	"Jabbi Encounter",
	"Jabble",
	"Punies vs. Jabbies",
	"Blue Key Obtained",
	"Inside the Blue Cell",
	"Punio & Petuni(Before)",
	"Punio & Petuni",
	"Bubbles 2",
	"Punies vs. Jabbies 2",
	"Trapped",
	"Mystery Pillar",
	"Matching the Patterns",
	"Super Boots",
	"Escape",
	"Water Switch",
	"The 101 Puni Switch",
	"Lord Crump Appears",
	"Time Bomb Activated",
	"X-Naut Battle",
	"Boss Battle",
	"After the Boss Battle",
	"Peach: Crump & X-Nauts",
	"Peach: An Open Door 2",
	"Peach: Shall We Dance?",
	"Bowser: Meadows",
	"Bowser: Action Stage",
	"Bowser: Petalburg",
	"Praise from the Puni",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.2",
	"Finding Don Pianta",
	"Backdoor",
	"Mafia Boss",
	"Daughter Located",
	"Blimp Ticket",
	"After Getting the Ticket",
	"To Glitzville",
	"Welcome to Glitzville",
	"Arena Intro (1F)",
	"Arena Intro (2F)",
	"The Guard Backs Off",
	"Grubba",
	"Gonzales is Born",
	"Match",
	"1st Match",
	"Minors - 9th Pl.",
	"Minors - 8th Pl.",
	"Minors - 7th Pl.",
	"Minors - 6th Pl.",
	"Minors - 5th Pl. (Egg)",
	"Runaway Egg",
	"The Mini-Yoshi Egg",
	"Minors - 4th Pl.",
	"Minors - 3rd Pl.",
	"Minors - 2nd Place",
	"Minors - 1st Pl. (Iron)",
	"Minors - 1st Pl. (Iron 2)",
	"Majors - 10th Place",
	"Majors - 9th Place",
	"Majors - 8th Place",
	"Super Hammer",
	"Back to the Minors",
	"The Yellow Block",
	"Confiscated",
	"Majors - 7th Pl.(Raid/Cake)",
	"Majors - 6th Pl.(Bowser)",
	"Majors - 5th Pl.",
	"Majors - 4th Pl.",
	"Rawk Hawk's Warning",
	"Storage Key",
	"Storage Room Unlocked",
	"Ms. Mowz",
	"Blowing Containers",
	"Hidden Stairway",
	"Hole in the Floor",
	"Jolene's Secret",
	"Majors - 3rd Pl.(Poison)",
	"Majors - 2nd Pl.",
	"Majors - 1st Pl.",
	"Lobby Poster",
	"Unlocking the Door",
	"Flattened",
	"1st Match with Rawk Hawk",
	"A New Champion",
	"Champion's Room",
	"Air Duct Entrance",
	"Air Duct (1st Time)",
	"Eavesdropping",
	"Air Duct Exit",
	"Grubba's Plans",
	"Boss Battle",
	"Peach: The Secret Weapon",
	"Peach: TEC's Request",
	"Peach: Incognito",
	"Peach: Grodus",
	"Peach: Back to TEC",
	"Bowser: The Great Tree",
	"Bowser: Treasure?",
	"Bowser: Puni Elder",
	"Return to Rogueport",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.3",
	"Darkly",
	"To Stage 4",
	"Welcome to Stage 4",
	"Gatekeeper Conversation",
	"The Mayor's a Pig!",
	"Dinge",
	"The Gatekeeper's a Pig!",
	"Opening the Gate",
	"Shop Key",
	"Storage Room Unlocked",
	"Paper Mage (Tube Mod)",
	"Straw Tepee",
	"Tree Trunk",
	"Blow the Floor",
	"Through the Hole",
	"Trick Rock",
	"Steeple Demo (EXT.)",
	"Steeple Demo (INT.)",
	"Push the Statue",
	"Boo Box",
	"1000 Boos",
	"Key",
	"Lock",
	"Doopliss Battle",
	"Trading Places",
	"Superbombbomb Lost",
	"Waiting Doopliss",
	"Vivian",
	"Superbombbomb Found",
	"Vivian Joins",
	"Waiting Doopliss 2",
	"Breaking the Floor",
	"The Letter p",
	"Name Guessing",
	"Doopliss Battle 2",
	"Back to Normal",
	"The Ruby Star",
	"Shadow Sirens & Doopliss",
	"Peach: X-Naut PhD Report",
	"Peach: TEC's Quiz",
	"Bowser: Blimp Area",
	"Bowser: Flying",
	"Bowser: Action Stage",
	"Bowser: Rogueport Dock",
	"Leaving Twilight Town",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.4",
	"Chatting with Flavio",
	"Meet Bobbery",
	"Bobbery Declines",
	"Advice from Podley",
	"Bobbery's Decision",
	"To the Port",
	"Off to the Keys",
	"Logbook",
	"Haunted",
	"Deserted Island",
	"Flavio & Pa-Patch",
	"Flavio's Favor",
	"Bobbery & the Bridge",
	"Runaway Sailors",
	"Flavio's Rescue Plan",
	"Saving Bobbery",
	"Flavio's Wine",
	"Wine Obtained",
	"Bobbery Rescue 2",
	"Bobbery Joins",
	"Flavio's Favor 2",
	"Marco's Jewel",
	"Gems and Jingles",
	"The Skull Rock",
	"Skull Rock Blast",
	"Entering the Grotto",
	"The Call of Cortez",
	"Spikes (Floor)",
	"The Blue Block",
	"Grotto Key",
	"Spikes (Wall)",
	"Ship Hole",
	"Paper Mage 4",
	"Boat Mod Obtained",
	"Floodgate Handle",
	"Floodgate",
	"Wave-dodging",
	"Shipwrecked Sailors",
	"Switch Test",
	"Thankful Sailors",
	"Four-Eyes",
	"Cortez's Warning",
	"Cortez and the Treasure",
	"Kicked Off",
	"Escape Route",
	"Hole in the Wall",
	"True Identity",
	"Return to the Grotto",
	"Flavio Makes a Deal",
	"All Aboard",
	"Pirates vs. X-Nauts",
	"X-Nauts Retreat",
	"Peach: TEC's Request 2",
	"Peach: Potion Puzzle",
	"Peach: Disk Search",
	"Peach: TEC Talk",
	"Bowser: Twilight Town",
	"Bowser: Twilight Town 2",
	"Bowser: Bowser vs. X-Nauts",
	"Fair Winds!",
	"Rogueport Dock",
	"A Mail from Peach",
	"Return to Keelhaul",
	"Thousand-Year Door",
	"Frankly's House Stg.5",
	"An Ill Don Pianta",
	"Keelhaul Anytime",
	"Lost Wedding Ring",
	"Wedding Ring Delivery",
	"F & F Sail Off",
	"F & F Arrive",
	"Train Ticket",
	"Beldam Finds Out",
	"The Train Leaves",
	"Start",
	"The Conductor",
	"A Case of Lost Stew",
	"Stained Carpet",
	"Grilling Heff T.",
	"Galley Kettle Discovered",
	"Pennington's Assistant",
	"Sylvia",
	"Bub",
	"Bub's Request",
	"The Conductor/Zip Toad",
	"Obtain Autograph",
	"Give Autograph",
	"Talk to the Conductor",
	"Ghost T.",
	"Ghost Request",
	"The Baggage Car",
	"Ghost Diary",
	"Blanket Exchange",
	"Give Blanket",
	"End of Day 1",
	"Day 2",
	"Document",
	"Vital Paper",
	"A Vital Clue",
	"Another Clue",
	"Zip Toad Appears",
	"Zip Toad Caught",
	"Riverside Station",
	"Station Key Obtained",
	"Station Door Unlocked",
	"Origami Stairs",
	"Key",
	"Unlocking the Door",
	"Blow the Poster",
	"3 Blue Blocks",
	"Origami Stairs",
	"Spring Jump",
	"Elevator Key",
	"Ride the Elevator",
	"Smorg (1st Encounter)",
	"Swatting Smorgs",
	"Origami Stairs",
	"Drawbridge Down",
	"Conductor Appears",
	"End of Day 2",
	"Day 3 Starts",
	"Engineer",
	"Baggage Car Smorgs",
	"Escaping the Smorgs",
	"Smorg Miasma",
	"Poshley Heights",
	"The Sanctum Manager",
	"3 Surprise Guests",
	"The Hidden Pipe",
	"The Real Garnet Star",
	"Welcome Back",
	"Peach: Grodus & X-Naut",
	"Peach: TEC Deleted",
	"Bowser: Rogueport",
	"Bowser: Action Stage",
	"Bowser: Rawk Hawk",
	"Rogueport Station",
	"A Mail from Peach",
	"Thousand-Year Door",
	"Frankly's House Stg.6",
	"3 Minutes Later",
	"A Way to the Moon",
	"Ultra Hammer",
	"Stone Block Smash",
	"To Fahr Outpost",
	"Start",
	"Fahr Outpost (1st Visit)",
	"Mayor Bob",
	"Goldbob",
	"Where's White?(Petalburg)",
	"Where's White?(Keelhaul)",
	"Where's White?(Glitzville)",
	"Where's White?(Great Tree)",
	"Where's White?(Twilight)",
	"Gen. White Awakens",
	"Back Online",
	"The Cannon",
	"To the Moon",
	"A Pipe Appears",
	"Inside the Building",
	"Elite X-Nauts",
	"Elevator Key A(Lit Floor)",
	"Elevator Activated",
	"Password for Today",
	"Password Entry",
	"Quiz Cleared",
	"Elevator Key B",
	"Elevator Activated 2",
	"Room Key A",
	"Room Key B",
	"Room Key C",
	"Factory Door",
	"Conveyor Belt",
	"Off-screen Stairs",
	"The Door to Lord Crump",
	"Lord Crump Defeated",
	"Bowser: Poshley Heights",
	"Bowser: Poshley Sanctum",
	"Bowser: Crystal Star",
	"The Last Mail",
	"The Hideout Explodes",
	"A Waiting Prof. Frankly ",
	"Thousand-Year Door",
	"The Pedestal",
	"To the Last Dungeon",
	"Start",
	"1000 Dry Bones",
	"Key",
	"Lock",
	"Follow the Lit Torches",
	"The Palace Garden",
	"Gloomtail",
	"Star Key Slot",
	"Moving Planets",
	"Beldam Confrontation",
	"The New Path",
	"Key",
	"Lock",
	"Star Puzzle",
	"Large Stone Block",
	"Key",
	"Lock",
	"Queen's Chamber",
	"Grodus",
	"Bowser",
	"The Return of the Queen",
	"End of the World 1",
	"The Shadow Queen",
	"End of the World 2",
	"Shadow Peach",
	"The Crystal Stars",
	"Words of Support 1",
	"Words of Support 2",
	"Words of Support 3",
	"After the Battle",
	"Epilogue",
	"Rogueport Dock",
	"Sail Away",
	"Mario's House",
	"Professor Frankly",
	"Goombella",
	"Koops",
	"Mini-Yoshi",
	"Vivian",
	"Bobbery",
	"Ms. Mowz",
	"X-Nauts",
	"Moon",
	"Goombella 2",
	"Mario's House 2",
	"End Credits",
	"Return to Rogueport",
	"Rogueport",
};

uint16_t WarpsEventNamesSize = sizeof(WarpsEventNames) / sizeof(WarpsEventNames[0]);
#endif

const char *CheatsStandardOptionsLines[] = 
{
	"Turn On/Off",
	"Change Button Combo",
};

const char *CheatsNoButtonComboOptionsLines[] = 
{
	"Turn On/Off",
};

const char *CheatsForceItemDropOptionsLines[] = 
{
	"Turn On/Off",
	"Change Item By Id",
};

const char *CheatsManageFlagsOptions[] = 
{
	"Set GSWs",
	"Set GSWFs",
	"Set GWs",
	"Set GFs",
	"Set LSWs",
	"Set LSWFs",
};

const char *CheatsManageGlobalWordsOptions[]
{
	"Change Value",
	"Set New Value",
};

const char *CheatsLockFlagsOptions[]
{
	"Lock GSWs",
	"Lock GSWFs",
	"Lock GWs",
	"Lock GFs",
	"Lock LSWs",
	"Lock LSWFs",
	"Set New LSW Area",
	"Set New LSWF Area",
};

const char *CheatsManageGlobalFlagsOptions[]
{
	"Toggle Value",
};

const char *CheatsClearAreaFlags[] = 
{
	"Select Area",
	"Clear Flags",
};

const char *CheatsClearAreaFlagsAreas[] = 
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

const char *CheatsClearAreaFlagsAreasFullNames[] = 
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
	"Mario",
	"Partners",
	"Followers",
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

const char *StatsPartnerLines[] = 
{
	"Goombella",
	"Koops",
	"Flurrie",
	"Yoshi",
	"Vivian",
	"Bobbery",
	"Ms. Mowz",
};

const char *StatsPartnerOptionsLines[] = 
{
	"HP",
	"Max HP",
	"Rank",
	"Toggle",
};

const char *StatsYoshiColorOptionsLines[] = 
{
	"Green",
	"Red",
	"Blue",
	"Orange",
	"Pink",
	"Black",
	"White",
};

const char *StatsFollowerLines [] = 
{
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

const char *SettingsLines[] = 
{
	"Load Settings",
	"Save Settings",
};

const char *MemoryLines[] = 
{
	"Add Watch",
	"Modify Watch",
	"Duplicate Watch",
	"Delete Watch",
};

const char *MemoryModifyLines[] = 
{
	"Address",
	"Type",
	"Show As Hex",
	"Position",
	"Display",
};

const char *MemoryTypeLines[] = 
{
	"string",
	"time",
	"int8_t",
	"int16_t",
	"int32_t",
	"int64_t",
	"uint8_t",
	"uint16_t",
	"uint32_t",
	"uint64_t",
	"float",
	"double",
};

const char *MemoryChangeAddressLines[] = 
{
	"Change Address/Pointer Levels",
	"Add Pointer Level",
	"Remove Pointer Level",
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
	"Cortez - Bone Pile?",
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
	"Unused - Red Buzzy",
	"Unused - Red Parabuzzy",
	"Unused - Red Spiky Parabuzzy",
	"Unused - Hyper Bob_omb",
	"Unused - Ultra Bob_omb",
	"Goombella - Action Commands Tutorial",
	"Frankly - Action Commands Tutorial",
	"Frankly - Special Moves Tutorial",
	"Frankly - Goomba Trio Fight",
	"Doopliss - Epilogue - Mario",
	"Flurrie - Epilogue",
	"Boo - Epilogue",
	"Atomic Boo - Epilogue",
	"Male Toad",
	"Female Toad",
	"Unused - Test",
	"Unused - Kanbu 2",
	"Unused - Beldam 2",
	"Unused - Marilyn 2",
	"Unused - Vivian 2",
	"Unused - Beldam 3",
	"Unused - Marilyn 3",
	"Unused - Mecha Kuri",
	"Unused - Mecha Kame",
	"Unused - Okorl",
	"Unused - Yowarl",
	"Unused - Tuyonarl",
	"Unused - Wanawana",
	"Unused - Minarai Kamec",
	"Unused - Shy Guy",
	"Unused - Groove Guy",
	"Unused - Pyro Guy",
	"Unused - Spy Guy",
	"Unused - Anti Guy",
	"Unused - Bzzap",
	"Unused - Mini Bzzap",
	"Unused - UFO",
	"Unused - Pennington",
	"Unused - Fighter",
	"Unused - Zess T.",
	"Unused - Master",
	"Unused - Reporter",
	"Unused - Hotdog Master",
	"Unused - Flavio",
	"Unused - Tree",
	"Unused - Switch",
	"Unused - TestNPC",
	"Bomb Squad - Bomb",
	"System",
	"Goombella - Prologue - Lord Crump Fight",
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
	"Change HP",
	"Change Max HP",
	"Change FP",
	"Change Max FP",
	"Change Held Item",
	"Clear Held Item",
	"Change Statuses",
};

const char *BattlesCurrentActorStats[] = 
{
	"HP",
	"Max HP",
	"FP",
	"Max FP",
};

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

const char *DisplaysLines[] = 
{
	"On-Screen Timer",
	"Frame Counter",
	"Mario's Coordinates",
	"Mario's Speed XZ",
	"Jump Storage",
	"Button Inputs",
	"Stick Angle",
	"D-Pad Options",
	"Guard/Superguard Timings",
	"Art Attack Hitboxes",
	"Yoshi Skip",
	"Palace Skip",
	"Blimp Ticket Skip",
};

const char *OnScreenTimerOptionsLines[] = 
{
	"Turn On/Off",
	"Change Start/Pause/Resume Button Combo",
	"Change Reset Button Combo",
};

const char *OnScreenTimerOptions[] = 
{
	"Start/Pause/Resume Button Combo",
	"Reset Button Combo",
};

const char *WarpLines[] = 
{
	"Select Warp",
	"Warp By Event",
	"Warp By Index",
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

const char *WarpEventLines[] = 
{
	"Select Event",
	"Keep Inventory",
	"Set Flags",
	"Warp",
};

const char *WarpIndexLines[] = 
{
	"Select New Map",
	"Select New Entrance Id",
	"View Current Map Entrances",
	"Warp",
};

#ifdef TTYD_JP
uint8_t ButtonLeft[] 	= {0x81, 0xA9, 0};
uint8_t ButtonRight[] 	= {0x81, 0xA8, 0};
uint8_t ButtonDown[] 	= {0x81, 0xAB, 0};
uint8_t ButtonUp[] 		= {0x81, 0xAA, 0};

const char *ButtonInputDisplay[] = 
{
	reinterpret_cast<const char *>(&ButtonLeft),
	reinterpret_cast<const char *>(&ButtonRight),
	reinterpret_cast<const char *>(&ButtonDown),
	reinterpret_cast<const char *>(&ButtonUp),
	"Z",
	"R",
	"L",
	"A",
	"B",
	"X",
	"Y",
	"S",
};

uint8_t PointerTextValues[] = {0x50, 0x81, 0xA8, 0};
const char *PointerText = reinterpret_cast<const char *>(&PointerTextValues);
#else
const char ButtonInputDisplay[] = 
{
	static_cast<char>(171), // Left
	static_cast<char>(187), // Right
	static_cast<char>(179), // Down
	static_cast<char>(178), // Up
	'Z',
	'R',
	'L',
	'A',
	'B',
	'X',
	'Y',
	'S',
};
#endif

uint8_t CheatsManageGlobalWordsOptionsSize 	= sizeof(CheatsManageGlobalWordsOptions) / sizeof(CheatsManageGlobalWordsOptions[0]);
uint8_t CheatsManageGlobalFlagsOptionsSize 	= sizeof(CheatsManageGlobalFlagsOptions) / sizeof(CheatsManageGlobalFlagsOptions[0]);
uint8_t CheatsClearAreaFlagsAreasSize 		= sizeof(CheatsClearAreaFlagsAreas) / sizeof(CheatsClearAreaFlagsAreas[0]);
uint8_t StatsMarioOptionsLinesSize 			= sizeof(StatsMarioOptionsLines) / sizeof(StatsMarioOptionsLines[0]);
uint8_t MarioStatsSpecialMovesOptionsSize 	= sizeof(MarioStatsSpecialMovesOptions) / sizeof(MarioStatsSpecialMovesOptions[0]);
uint8_t StatsPartnerOptionsLinesSize 		= sizeof(StatsPartnerOptionsLines) / sizeof(StatsPartnerOptionsLines[0]);
uint8_t StatsYoshiColorOptionsLinesSize 	= sizeof(StatsYoshiColorOptionsLines) / sizeof(StatsYoshiColorOptionsLines[0]);
uint8_t StatsFollowerOptionsLinesSize 		= sizeof(StatsFollowerOptionsLines) / sizeof(StatsFollowerOptionsLines[0]);
uint8_t MemoryModifyLinesSize 				= sizeof(MemoryModifyLines) / sizeof(MemoryModifyLines[0]);
uint8_t MemoryTypeLinesSize 				= sizeof(MemoryTypeLines) / sizeof(MemoryTypeLines[0]);
uint8_t BattlesCurrentActorStatsSize 		= sizeof(BattlesCurrentActorStats) / sizeof(BattlesCurrentActorStats[0]);
uint8_t BattlesStatusesLinesSize 			= sizeof(BattlesStatusesLines) / sizeof(BattlesStatusesLines[0]);
uint8_t OnScreenTimerOptionsSize 			= sizeof(OnScreenTimerOptions) / sizeof(OnScreenTimerOptions[0]);
uint8_t WarpDestinationsSize 				= sizeof(WarpDestinations) / sizeof(WarpDestinations[0]);

uint16_t StatsMarioIcons[] = 
{
	#ifdef TTYD_JP
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
	#else
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
	#endif
};

uint16_t BattlesStatusesIcons[] = 
{
	#ifdef TTYD_JP
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
	#else
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
	#endif
};

uint8_t CheatsOrder[] = 
{
	CHANGE_SEQUENCE,
	WALK_THROUGH_WALLS,
	SAVE_COORDINATES,
	LOAD_COORDINATES,
	SPAWN_ITEM,
	SAVE_ANYWHERE,
	TEXT_STORAGE,
	TIME_STOP_TEXT_STORAGE,
	SPEED_UP_MARIO,
	DISABLE_BATTLES,
	AUTO_ACTION_COMMANDS,
	INFINITE_ITEM_USAGE,
	RELOAD_ROOM,
	LEVITATE,
	LOCK_MARIO_HP_TO_MAX,
	RUN_FROM_BATTLES,
	DISABLE_MENU_SOUNDS,
	BOBBERY_EARLY,
	FORCE_ITEM_DROP,
	CLEAR_AREA_FLAGS,
	MANAGE_FLAGS,
	RESOLVE_FADES,
	FALL_THROUGH_FLOORS,
	LOCK_FLAGS,
};

uint8_t DisplaysOrder[] = 
{
	ONSCREEN_TIMER,
	MARIO_COORDINATES,
	JUMP_STORAGE,
	BUTTON_INPUT_DISPLAY,
	GUARD_SUPERGUARD_TIMINGS,
	ART_ATTACK_HITBOXES,
	YOSHI_SKIP,
	PALACE_SKIP,
	STICK_ANGLE,
	MARIO_SPEED_XZ,
	DPAD_OPTIONS_DISPLAY,
	BLIMP_TICKET_SKIP,
	ONSCREEN_TIMER_FRAME_COUNTER,
};

struct MenuVars MenuVar;
struct Menus Menu[30];
struct Cheats Cheat[24];
bool Displays[13];
char DisplayBuffer[256];
char HeapBuffer[512];
struct MemoryWatchStruct MemoryWatch[60];

struct AutoIncrement AdjustableValueMenu;
struct AutoIncrement MemoryWatchAdjustableValueMenu;
struct AutoIncrementCoordinates MemoryWatchPosition;
struct CheatsHandleDisplayButtons CheatsDisplayButtons;
struct CheatsManageFlags ManageFlags;
struct MarioPartnerPositionsStruct MarioPartnerPositions;
struct SaveAnywhereStruct SaveAnywhere;
struct SpeedUpMarioStruct SpeedUpMario;
struct ReloadRoomStruct ReloadRoom;
struct SpawnItems SpawnItem;
struct ClearAreaFlagsStruct ClearAreaFlags;
struct ClearCacheForBattlesStruct ClearCacheForBattles;
struct LockFlagsStruct LockFlags;
struct TrickDisplay YoshiSkip;
struct TrickDisplay PalaceSkip;
struct BlimpTicketSkipStruct BlimpTicketSkip;
struct OnScreenTimerDisplay OnScreenTimer;
struct OnScreenTimerDisplayFrameCounter FrameCounter;
struct DisplayActionCommandsTiming DisplayActionCommands;
struct MemoryCardStruct MenuSettings;
struct WarpByEventStruct WarpByEvent;
struct WarpByIndexStruct WarpByIndex;
struct MenuPrevMenuAndOption PrevMenuAndOption;

void initMenuVars()
{
	Menu[ROOT].TotalMenuOptions 						= sizeof(RootLines) / sizeof(RootLines[0]);
	Menu[ROOT].ColumnSplitAmount 						= Menu[ROOT].TotalMenuOptions;
	Menu[ROOT].Line 									= RootLines;

	Menu[INVENTORY].TotalMenuOptions 					= sizeof(InventoryLines) / sizeof(InventoryLines[0]);
	Menu[INVENTORY].ColumnSplitAmount 					= Menu[INVENTORY].TotalMenuOptions;
	Menu[INVENTORY].Line 								= InventoryLines;
	
	Menu[INVENTORY_MAIN].TotalMenuOptions 				= sizeof(InventoryOptionLines) / sizeof(InventoryOptionLines[0]);
	Menu[INVENTORY_MAIN].ColumnSplitAmount 				= Menu[INVENTORY_MAIN].TotalMenuOptions;
	Menu[INVENTORY_MAIN].Line 							= InventoryOptionLines;
	
	Menu[CHEATS].TotalMenuOptions 						= sizeof(CheatsLines) / sizeof(CheatsLines[0]);
	Menu[CHEATS].ColumnSplitAmount 						= 18;
	Menu[CHEATS].Line 									= CheatsLines;
	
	Menu[CHEATS_CHANGE_SEQUENCE].TotalMenuOptions 		= sizeof(CheatsChangeSequenceOptionsLines) / sizeof(CheatsChangeSequenceOptionsLines[0]);
	Menu[CHEATS_CHANGE_SEQUENCE].ColumnSplitAmount 		= Menu[CHEATS_CHANGE_SEQUENCE].TotalMenuOptions;
	Menu[CHEATS_CHANGE_SEQUENCE].Line 					= CheatsChangeSequenceOptionsLines;
	
	Menu[CHEATS_STANDARD].TotalMenuOptions 				= sizeof(CheatsStandardOptionsLines) / sizeof(CheatsStandardOptionsLines[0]);
	Menu[CHEATS_STANDARD].ColumnSplitAmount 			= Menu[CHEATS_STANDARD].TotalMenuOptions;
	Menu[CHEATS_STANDARD].Line 							= CheatsStandardOptionsLines;
	
	Menu[CHEATS_NO_BUTTON_COMBO].TotalMenuOptions 		= sizeof(CheatsNoButtonComboOptionsLines) / sizeof(CheatsNoButtonComboOptionsLines[0]);
	Menu[CHEATS_NO_BUTTON_COMBO].ColumnSplitAmount 		= Menu[CHEATS_NO_BUTTON_COMBO].TotalMenuOptions;
	Menu[CHEATS_NO_BUTTON_COMBO].Line 					= CheatsNoButtonComboOptionsLines;
	
	Menu[CHEATS_NPC_FORCE_DROP].TotalMenuOptions 		= sizeof(CheatsForceItemDropOptionsLines) / sizeof(CheatsForceItemDropOptionsLines[0]);
	Menu[CHEATS_NPC_FORCE_DROP].ColumnSplitAmount 		= Menu[CHEATS_NPC_FORCE_DROP].TotalMenuOptions;
	Menu[CHEATS_NPC_FORCE_DROP].Line 					= CheatsForceItemDropOptionsLines;
	
	Menu[CHEATS_LOCK_FLAGS].TotalMenuOptions 			= sizeof(CheatsLockFlagsOptions) / sizeof(CheatsLockFlagsOptions[0]);
	Menu[CHEATS_LOCK_FLAGS].ColumnSplitAmount 			= Menu[CHEATS_LOCK_FLAGS].TotalMenuOptions;
	Menu[CHEATS_LOCK_FLAGS].Line 						= CheatsLockFlagsOptions;
	
	Menu[CHEATS_MANAGE_FLAGS].TotalMenuOptions 			= sizeof(CheatsManageFlagsOptions) / sizeof(CheatsManageFlagsOptions[0]);
	Menu[CHEATS_MANAGE_FLAGS].ColumnSplitAmount 		= Menu[CHEATS_MANAGE_FLAGS].TotalMenuOptions;
	Menu[CHEATS_MANAGE_FLAGS].Line 						= CheatsManageFlagsOptions;
	
	Menu[CHEATS_CLEAR_AREA_FLAGS].TotalMenuOptions 		= sizeof(CheatsClearAreaFlags) / sizeof(CheatsClearAreaFlags[0]);
	Menu[CHEATS_CLEAR_AREA_FLAGS].ColumnSplitAmount 	= Menu[CHEATS_CLEAR_AREA_FLAGS].TotalMenuOptions;
	Menu[CHEATS_CLEAR_AREA_FLAGS].Line 					= CheatsClearAreaFlags;
	
	Menu[STATS].TotalMenuOptions 						= sizeof(StatsLines) / sizeof(StatsLines[0]);
	Menu[STATS].ColumnSplitAmount 						= Menu[STATS].TotalMenuOptions;
	Menu[STATS].Line 									= StatsLines;
	
	Menu[STATS_PARTNERS].TotalMenuOptions 				= sizeof(StatsPartnerLines) / sizeof(StatsPartnerLines[0]);
	Menu[STATS_PARTNERS].ColumnSplitAmount 				= Menu[STATS_PARTNERS].TotalMenuOptions;
	Menu[STATS_PARTNERS].Line 							= StatsPartnerLines;
	
	Menu[STATS_FOLLOWERS].TotalMenuOptions 				= sizeof(StatsFollowerLines) / sizeof(StatsFollowerLines[0]);
	Menu[STATS_FOLLOWERS].ColumnSplitAmount 			= Menu[STATS_FOLLOWERS].TotalMenuOptions;
	Menu[STATS_FOLLOWERS].Line 							= StatsFollowerLines;
	
	Menu[SETTINGS].TotalMenuOptions 					= sizeof(SettingsLines) / sizeof(SettingsLines[0]);
	Menu[SETTINGS].ColumnSplitAmount 					= Menu[SETTINGS].TotalMenuOptions;
	Menu[SETTINGS].Line 								= SettingsLines;
	
	Menu[MEMORY].TotalMenuOptions 						= sizeof(MemoryLines) / sizeof(MemoryLines[0]);
	Menu[MEMORY].ColumnSplitAmount 						= Menu[MEMORY].TotalMenuOptions;
	Menu[MEMORY].Line 									= MemoryLines;
	
	Menu[MEMORY_MODIFY].TotalMenuOptions 				= sizeof(MemoryModifyLines) / sizeof(MemoryModifyLines[0]);
	Menu[MEMORY_MODIFY].ColumnSplitAmount 				= Menu[MEMORY_MODIFY].TotalMenuOptions;
	Menu[MEMORY_MODIFY].Line 							= MemoryModifyLines;
	
	Menu[MEMORY_CHANGE_ADDRESS].TotalMenuOptions 		= sizeof(MemoryChangeAddressLines) / sizeof(MemoryChangeAddressLines[0]);
	Menu[MEMORY_CHANGE_ADDRESS].ColumnSplitAmount 		= Menu[MEMORY_CHANGE_ADDRESS].TotalMenuOptions;
	Menu[MEMORY_CHANGE_ADDRESS].Line 					= MemoryChangeAddressLines;
	
	Menu[BATTLES_CURRENT_ACTOR].TotalMenuOptions 		= sizeof(BattlesCurrentActorLines) / sizeof(BattlesCurrentActorLines[0]);
	Menu[BATTLES_CURRENT_ACTOR].ColumnSplitAmount 		= Menu[BATTLES_CURRENT_ACTOR].TotalMenuOptions;
	Menu[BATTLES_CURRENT_ACTOR].Line 					= BattlesCurrentActorLines;
	
	Menu[BATTLES_STATUSES].TotalMenuOptions 			= sizeof(BattlesStatusesLines) / sizeof(BattlesStatusesLines[0]);
	Menu[BATTLES_STATUSES].ColumnSplitAmount 			= 12;
	Menu[BATTLES_STATUSES].Line 						= BattlesStatusesLines;
	
	Menu[DISPLAYS].TotalMenuOptions 					= sizeof(DisplaysLines) / sizeof(DisplaysLines[0]);
	Menu[DISPLAYS].ColumnSplitAmount 					= Menu[DISPLAYS].TotalMenuOptions;
	Menu[DISPLAYS].Line 								= DisplaysLines;
	
	Menu[DISPLAYS_ONSCREEN_TIMER].TotalMenuOptions 		= sizeof(OnScreenTimerOptionsLines) / sizeof(OnScreenTimerOptionsLines[0]);
	Menu[DISPLAYS_ONSCREEN_TIMER].ColumnSplitAmount 	= Menu[DISPLAYS_ONSCREEN_TIMER].TotalMenuOptions;
	Menu[DISPLAYS_ONSCREEN_TIMER].Line 					= OnScreenTimerOptionsLines;
	
	Menu[DISPLAYS_NO_BUTTON_COMBO].TotalMenuOptions 	= sizeof(CheatsNoButtonComboOptionsLines) / sizeof(CheatsNoButtonComboOptionsLines[0]);
	Menu[DISPLAYS_NO_BUTTON_COMBO].ColumnSplitAmount 	= Menu[DISPLAYS_NO_BUTTON_COMBO].TotalMenuOptions;
	Menu[DISPLAYS_NO_BUTTON_COMBO].Line 				= CheatsNoButtonComboOptionsLines;
	
	Menu[WARPS].TotalMenuOptions 						= sizeof(WarpLines) / sizeof(WarpLines[0]);
	Menu[WARPS].ColumnSplitAmount 						= Menu[WARPS].TotalMenuOptions;
	Menu[WARPS].Line 									= WarpLines;
	
	Menu[WARPS_INDEX].TotalMenuOptions 					= sizeof(WarpIndexLines) / sizeof(WarpIndexLines[0]);
	Menu[WARPS_INDEX].ColumnSplitAmount 				= Menu[WARPS_INDEX].TotalMenuOptions;
	Menu[WARPS_INDEX].Line 								= WarpIndexLines;
	
	Menu[WARPS_EVENT].TotalMenuOptions 					= sizeof(WarpEventLines) / sizeof(WarpEventLines[0]);
	Menu[WARPS_EVENT].ColumnSplitAmount 				= Menu[WARPS_EVENT].TotalMenuOptions;
	Menu[WARPS_EVENT].Line 								= WarpEventLines;
	
	// Set the initial settings
	setInitialSettings();
	
	// Load the custom settings from the settings file if it exists
	loadSettings();
}

void setInitialSettings()
{
	// Set the Cheats bools
	Cheat[WALK_THROUGH_WALLS].Active 			= false;
	Cheat[FALL_THROUGH_FLOORS].Active 			= false;
	// Cheat[SAVE_COORDINATES].Active 			= false;
	// Cheat[LOAD_COORDINATES].Active 			= false;
	Cheat[SPAWN_ITEM].Active 					= false;
	Cheat[SAVE_ANYWHERE].Active 				= false;
	// Cheat[TEXT_STORAGE].Active 				= false;
	// Cheat[TIME_STOP_TEXT_STORAGE].Active 	= false;
	// Cheat[SPEED_UP_MARIO].Active 			= false;
	// Cheat[DISABLE_BATTLES].Active 			= false;
	// Cheat[AUTO_ACTION_COMMANDS].Active 		= false;
	// Cheat[INFINITE_ITEM_USAGE].Active 		= false;
	// Cheat[RELOAD_ROOM].Active 				= false;
	// Cheat[LEVITATE].Active 					= false;
	Cheat[LOCK_MARIO_HP_TO_MAX].Active 			= false;
	Cheat[RUN_FROM_BATTLES].Active 				= false;
	// Cheat[DISABLE_MENU_SOUNDS].Active 		= false;
	// Cheat[BOBBERY_EARLY].Active 				= false;
	Cheat[FORCE_ITEM_DROP].Active 				= false;
	
	// Set the Cheats button combos
	Cheat[WALK_THROUGH_WALLS].ButtonCombo 		= PAD_Z;
	Cheat[FALL_THROUGH_FLOORS].ButtonCombo 		= PAD_Y | PAD_Z;
	Cheat[SAVE_COORDINATES].ButtonCombo 		= PAD_L | PAD_DPAD_LEFT;
	Cheat[LOAD_COORDINATES].ButtonCombo 		= PAD_L | PAD_DPAD_UP;
	Cheat[SPAWN_ITEM].ButtonCombo 				= PAD_L | PAD_DPAD_DOWN;
	Cheat[SAVE_ANYWHERE].ButtonCombo 			= PAD_Y | PAD_B;
	Cheat[TEXT_STORAGE].ButtonCombo 			= PAD_L | PAD_X;
	Cheat[TIME_STOP_TEXT_STORAGE].ButtonCombo 	= PAD_L | PAD_R;
	Cheat[SPEED_UP_MARIO].ButtonCombo 			= PAD_L | PAD_Y;
	Cheat[DISABLE_BATTLES].ButtonCombo 			= PAD_Y;
	Cheat[AUTO_ACTION_COMMANDS].ButtonCombo 	= PAD_R;
	Cheat[INFINITE_ITEM_USAGE].ButtonCombo 		= PAD_Y;
	Cheat[RELOAD_ROOM].ButtonCombo 				= PAD_L | PAD_B;
	Cheat[LEVITATE].ButtonCombo 				= PAD_L | PAD_A;
	
	// Set the Displays bools
	// Displays[ONSCREEN_TIMER] 				= true;
	// Displays[ONSCREEN_TIMER_FRAME_COUNTER] 	= true;
	// Displays[MARIO_COORDINATES] 				= true;
	// Displays[MARIO_SPEED_XZ] 				= true;
	// Displays[JUMP_STORAGE] 					= true;
	// Displays[BUTTON_INPUT_DISPLAY] 			= true;
	// Displays[STICK_ANGLE] 					= true;
	// Displays[DPAD_OPTIONS_DISPLAY] 			= true;
	Displays[GUARD_SUPERGUARD_TIMINGS] 			= true;
	// Displays[ART_ATTACK_HITBOXES] 			= true;
	// Displays[YOSHI_SKIP] 					= true;
	// Displays[PALACE_SKIP] 					= true;
	// Displays[BLIMP_TICKET_SKIP] 				= true;
	
	// Set the Displays button combos
	OnScreenTimer.ButtonCombo[START_PAUSE_RESUME] 	= PAD_L | PAD_Z;
	OnScreenTimer.ButtonCombo[RESET] 				= PAD_L | PAD_DPAD_RIGHT;
	FrameCounter.ButtonCombo[START_PAUSE_RESUME] 	= PAD_L | PAD_Z;
	FrameCounter.ButtonCombo[RESET] 				= PAD_L | PAD_DPAD_RIGHT;
}

}