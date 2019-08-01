# User manual

## Table of contents
-   [Setup](#setup)
-   [Menus](#menus)
    -   [Inventory](#inventory)
    -   [Cheats](#cheats)
    -   [Stats](#stats)
    -   [Settings](#settings)
    -   [Memory](#memory)
    -   [Battles](#battles)
    -   [Displays](#displays)
    -   [Warps](#warps)
-   [Misc](#misc)

## Setup
* Download the GCI from the most recent release (found [here](https://github.com/Zephiles/TTYD-Practice-Codes/releases)).
* Copy the GCI containing the Practice Codes to the memory card. (This file contains only code required to run the Practice codes. It has no relation to the standard TTYD save file.)
  * For **Dolphin** users:
    * Use the memcard manager located in **Tools > Memcard Manager (GC)** to import the GCI into the file Dolphin is using for a memory card.
    * Alternatively, select "GCI Folder" for the format of the memory card in Slot A (in **Config dialog > GameCube > Device settings**), and copy the .gci file directly into the folder, located by default at **{Dolphin directory}/GC/{Region}/Card A**. **{Region}** will either be USA, JAP, or EUR depending on the region of the game that you're using.
  * For **Nintendont** or other console users:
    * Use the appropriate homebrew apps / folders, etc. to copy the file from your SD card to your GC memory card or virtual memory card.
* Use the Gecko loader cheat code in the "relloader-ttyd" directory to make the game load the Practice Codes GCI.
  * For **Dolphin** users:
    * Right-click your TTYD game in the games list and go to **Properties > Gecko Codes**, and add the appropriate Gecko loader code for the region of the game you are using (found [here](https://github.com/Zephiles/TTYD-Practice-Codes/tree/master/ttyd-tools/relloader-ttyd/REL_Loader/Gecko)).
    * (For older revisions of Dolphin v5, instead go to **Properties > Edit Config**, and copy the text code into the Gecko codes section of the "G8ME01.ini" file that opens, marked by the text **[Gecko]**; if the section doesn't exist, add it.)
    * Enable cheats in **Config dialog > General**, and enable the loader code in **Properties > Gecko Codes** for TTYD.
  * For **Nintendont** users:
    * Copy the loader GCT (found [here](https://github.com/Zephiles/TTYD-Practice-Codes/tree/master/ttyd-tools/relloader-ttyd/REL_Loader/GCT)) onto your SD card, either to **/codes/** or into the folder with your TTYD ISO. 
        * Note: It is important that you **do not** right click to save the GCT, or it will be saved incorrectly. You must download it via the **Download** button.
    * Enable cheats in Nintendont's settings before booting the game.
  * Alternatively, you can use the Action Replay loader code (e.g., if you have a physical Action Replay disc).

## Menus
### Inventory
This menu allows you to modify your entire inventory, including standard items, important items, badges, and stored items.

### Cheats
1. **Change Sequence** allows you to change the current Sequence value, which is the value used to tell what part of the game you're currently at.
2. **Walk Through Most Objects** allows you to walk through most objects. It does now allow walking through NPCs. The button combination must be held for this code to work.
3. **Save Coordinates** allows you to save Mario's and your partner's current positions, directions, and angles.
4. **Load Coordinates** allows you to load the positions, directions, and angles saved from the previous code.
5. **Spawn Item** allows you to spawn any item into the field.
6. **Save Anywhere** allows you to save the game at any location, even if a save block is not present in the current room.
7. **Text Storage** sets a specific value that causes Text Storage to be active. A partner is required to be out for this code to have an effect.
8. **Time Stop Text Storage** sets a specific value that causes Time Stop Text Storage to be active. A partner is required to be out for this code to have an effect.
9. **Speed Up Mario** applies a large speed boost to Mario. The button combination must be held for this code to work.
10. **Disable Non-Cutscene Battles** disables any battles in the field that are not started by a cutscene. The button combination must be held for this code to work.
11. **Auto Action Commands** automatically performs action commands for you, the same way that the Debug Badge works. This applies to most guards/superguards as well as for filling the run meter. The button combination must be held for this code to work.
12. **Infinite Item Usage** allows you to use any item without it being removed from the menu. You must hold the button combination when using the item for this code to work. The button combination must be held for this code to work.
13. **Reload Room** reloads the current room. This cannot be used on the title screen nor on the file select screen. When this code is used, the **Resolve Fades** code (listed below) is also used when not in a screen transition.
14. **Levitate** allows you to levitate in the air. The button combination must be held for this code to work.
15. **Lock Mario's HP To Max** locks Mario's current HP to the max.
16. **Allow Running From Any Battle** allows you to run from any battle, even if the battle would normally not allow this.
17. **Disable Pause Menu/Z Menu Sounds** disables the sound effects that play when the pause menu and the Z menu are opened/closed.
18. **Bobbery Early** clears some values, which allows you to perform Bobbery Early more than once on a save file without the need to reload.
19. **Force NPC Item Drop** forces any defeared enemy to drop a specified item. The item can be changed at will.
20. **Resolve Fades** allows you to resolve any fades that are currently active and linger on-screen. Some examples of these are the Mario heads after each chapter, and when the screen pans to black when transitioning to a new screen.
21. **Manage Flags** allows you to manually change a lot of the important flags in the game.
22. **Clear Area Flags** allows you to clear most of the flags for a specified area. A confirmation message is displayed when trying to do so, to prevent accidently clearing flags. The flags will be cleared on the next screen transition.

### Stats
This menu allows you to modify Mario's stats, all of your partner's stats, whether or not partners are enabled or not, whether a partner is out or not, and whether or not a follower is out or not.
* You are also able to change Yoshi's color from within the Partners menu. Upon changing it, you must respawn Yoshi for the changes to take effect. This is done by either swapping partners, entering pipes, or bringing Yoshi out via the menu itself.

### Settings
This menu allows you to save and load various settings. The settings are stored on a separate save file on the memory card. The current settings included are as follows:
* Which cheats are active
* Cheats button combos
* Which displays are active
* Displays button combos
* Memory watches (See below)

### Memory
This menu allows you to watch the values of up to 60 memory addresses, with support for up to 10 pointer levels each. Each of these watches can also be displayed on the screen at will.

### Battles
This menu allows you to change the HP, Max HP, FP, Max FP, held items, and statuses of anyone in a battle (refered to as Actors).
* Notes about the Battles menu:
  * This menu can only be used while in a battle.
  * Under certain circumstances, it is possible that having an actor recover using a Life Shroom can cause the game to softlock. It is believed that this only occurs when a textbox is supposed to occur, but the Life Shroom interrupts it. One example of where this happens is with Bowser in Chapter 8.

### Displays
1. **On-Screen Timer** displays a timer at the bottom-right of the screen. This timer is set to run at the current FPS, and therefore will count from 0 to 59 (0 to 49 for EU at 50hz) before reaching one second.
2. **Mario's Coordinates** displays Mario's coordinates at the bottom-left of the screen. If the On-Screen Timer is active, then the position of Mario's Coordinates will be shifted upwards.
3. **Mario's Speed XZ** displays Mario's speed, based on both the X axis and the Z axis.
4. **Jump Storage** displays a value that determines whether you have Jump Storage or not. The value will be 0 when you don't have it, and 1 when you have it. While this display is active, you can press Y three times in succession to manually give yourself Jump Storage.
5. **Button Input Display** displays which buttons are pressed at the bottom-left of the screen. When this display is active, the other displays that are placed at the bottom-left of the screen are shifted upwards.
6. **Stick Angle** displays the current angle held with the left stick. The four main directions are 0 for up, 90 for right, 180 for down, and 270 for left. If no angle is held, then it will simply print **Neutral**. When the stick is not at the neutral position, there are also two additional numbers displayed. These two numbers are what are actually used to calculate the angle. The left number is for the X position, and the right number is for the Y position.
7. **D-Pad Options** refers to the display at the bottom-left of the screen that shows which d-pad button opens which submenu in the pause menu. You will still be able to press these buttons to open the submenus if the display is disabled.
8. **Guard/Superguard Timings** displays the following:
   * The frame that you guarded/superguarded an attack out of the total amount of possible frames.
   * How many frames early you pressed A or B. It should be noted that nothing will be displayed if the button was pressed very early (more than 6 or so frames for guards, and more than 12 or so frames for superguards).
   * Whether you pressed too many buttons in a short period of time for the guard/superguard to be accepted.
   * Whether the attack can be superguarded or not (will only be displayed if you tried to superguard it).
9. **Art Attack Hitboxes** displays boxes around enemies in battles when using Art Attack.
10. **Yoshi Skip** displays various information about performing this skip. The main timer (labeled YST) is set to reset and start running once you leave a battle, and pauses once you press A. You can also manually reset this timer by holding Y for two seconds.
11. **Palace Skip** displays various information about performing this skip. The main timer (labeled PST) is set to reset and start once you leave the pause menu, and pauses once you press X. You can also manually reset this timer by holding Y for two seconds. **PhaEmy** keeps track of the Y coordinate for the phantom ember, **ItemTimer** keeps track of the timer for the current item in the field, and **ParY** keeps track of your partner's Y coordinate.
12. **Blimp Ticket Skip** displays various information about performing this skip. The up-right timer (labeled **URT**) is set to reset and start running once you leave the pause menu, and pauses once the held stick angle reaches 25 or higher on JP, and 27 or higher on US/EU. The straight-up timer (labeled **SUT**) is set to reset and start running once you leave the pause menu, and pauses once the held stick angle is 0.00. Both of these timers can be manually reset by holding Y for two seconds.

### Warps
This menu allows you to warp to various places around the game. The menu allows you to either choose a map from a list, or to choose a map and entrance manually. When choosing from the list, a description for each option is displayed when the cursor is over one.
* Notes about manually choosing a warp:
  * If an invalid entrance is used, then Mario will be placed at/near the center of the chosen map.
  * Some maps have entrances that don't actually work. Using them will simply place Mario at/near the center of the chosen map.

## Misc
There are several other changes that have been made outside of the menu:
1. The build date on the title screen is always displayed.
2. The current Sequence value is displayed in the pause menu next to Mario's stats.
3. The unused Mega badges can be used in battles without the need for a boot/hammer upgrade.
4. Holding Y while using Art Attack will pause the timer that is used to check how long you are able to use it for.
5. The heaps used in the game are checked every frame, and messages are displayed on-screen when errors are found.
