#include "menus/cheatsMenu.h"
#include "menus/warpsMenu.h"

#include <cstdint>

// Place the event names for both the Cheats and Warps menus here to avoid a lot of duplicate text

#ifdef TTYD_JP
const char *gCheatsEventNames[CHEATS_TOTAL_EVENT_NAMES] = {
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
    "A Way to the Moon",
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
    "A Waiting Prof. Frankly",
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

const char *gWarpsEventNames[WARPS_TOTAL_EVENT_NAMES] = {
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
    "Punio & Petuni (Before)",
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

#endif