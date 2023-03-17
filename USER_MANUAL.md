# User manual
# Important Notice
As of **v3.0.53**, the internal filename of the **settings** file has been changed from `rel_settings` to `Practice_Codes_Settings`. As such, if you previously had a settings file made, you must change the internal filename to be able to use it with versions **3.0.53** or newer. This can be done with either a hex editor (in which the internal filename starts at 0x8 in the GCI file) or via the Practice Codes' [Settings](#settings) menu via the **Rename Settings File** option. If changing the internal filename via the **Rename Settings File** option, you must use the **Load Settings** option to load your settings for this sesstion, but all future sessions will have them loaded at boot automatically.

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

* Copy the GCI containing the Practice Codes to the memory card. (This file contains only code required to run the Practice Codes. It has no relation to the standard TTYD save file.)
  * For **Dolphin** users:
    * Use the memcard manager located in **Tools > Memcard Manager (GC)** to import the GCI into the file Dolphin is using for a memory card.
    
    * Alternatively, select "GCI Folder" for the format of the memory card in Slot A (in **Config dialog > GameCube > Device settings**), and copy the .gci file directly into the folder, located by default at **{Dolphin directory}/GC/{Region}/Card A**. **{Region}** will either be USA, JAP, or EUR depending on the region of the game that you're using.
  * For **Nintendont** or other console users:
    * Use the appropriate homebrew apps / folders, etc. to copy the file from your SD card to your GC memory card or virtual memory card.
* Use one of the following:
  1. Use a hacked save file to execute arbitrary code, which can then be used to load the practice codes. This is the most ideal way to load them, as it does not require any additional loaders/cheats. If you don't have any important files that you want to back up, then you can use a premade file, which can be found [here](https://github.com/magcius/TTYDSaveHacker/releases). Otherwise, you can manually hack a file [here](https://magcius.github.io/TTYDSaveHacker/). Once you have a hacked file from one of these, you can simply load the hacked file in-game, which will cause a soft reset to occur, and will also load the practice codes at boot. Furthermore, the practice codes will be loaded at boot for all future resets, and can only be disabled by powering off the console.
  
  2. Use the Gecko loader cheat code in the "relloader-ttyd" directory to make the game load the Practice Codes GCI.
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
1. **Change Sequence** allows you to change the current **Sequence** value, which is the value used to tell what part of the game you're currently at. For clarification, the stage and event listed for the current **Sequence** value is effectively what has just been cleared, which differs from the **Warp By Event** menu, which is explained in the Warps section.

2. **Walk Through Most Objects** allows you to walk through most objects. It does now allow walking through NPCs. The button combination must be held for this code to work.

3. **Fall Through Most Objects** allows you to fall through most floors/objects. It does now allow falling through NPCs. The button combination must be held for this code to work.

4. **Save Coordinates** allows you to save Mario's and your partner's current positions, directions, and angles.

5. **Load Coordinates** allows you to load the positions, directions, and angles saved from the previous code.

6. **Change Mario's Coordinates** allows you to change Mario's coordinates at will. You are able to change them in both decimal and hexidecimal form.
    * Note: When modifying a coordinate in decimal form, the number will start with either a plus or minus sign. This **does not** mean that addition/subtraction is occuring; it simply means that the value is either positive or negative.

7. **Spawn Item** allows you to spawn any item into the field.

8. **Save Anywhere** allows you to save the game at any location, even if a save block is not present in the current room.

9. **Text Storage** sets a specific value that causes Text Storage to be active. A partner is required to be out for this code to have an effect.

10. **Time Stop Text Storage** sets a specific value that causes Time Stop Text Storage to be active. A partner is required to be out for this code to have an effect.

11. **Speed Up Mario** applies a large speed boost to Mario. The button combination must be held for this code to work.

12. **Disable Non-Cutscene Battles** disables any battles in the field that are not started by a cutscene. The button combination must be held for this code to work.

13. **Auto Action Commands** automatically performs action commands for you, the same way that the Debug Badge works. This applies to most guards/superguards as well as for filling the run meter. The button combination must be held for this code to work.

14. **Infinite Item Usage** allows you to use any item without it being removed from the menu. You must hold the button combination when using the item for this code to work. The button combination must be held for this code to work.

15. **Reload Room** reloads the current room. This cannot be used on the title screen nor on the file select screen. When this code is used, the **Resolve Fades** code (listed below) is also used when not in a screen transition.

16. **Levitate** allows you to levitate in the air. The button combination must be held for this code to work.

17. **Auto Mash Through Text** allows you to automatically mash through all text boxes. Text boxes with questions (Yes, No, etc.) will still require you to manually select an option. The button combination must be held for this code to work. It should be noted that this cheat will not run when the **Palace Skip** display is on, as it would interfere with the textboxes that are used for **Palace Skip**.

18. **Frame Advance (Experimental)** allows you to advance the game one frame at a time. Note that this cheat is still in an experimental state, and some things do not work properly when using it.

19. **Generate Lag Spike** allows you to generate a lag spike at any given time.

20. **Lock Mario's HP To Max** locks Mario's current HP to the max.

21. **Allow Running From Any Battle** allows you to run from any battle, even if the battle would normally not allow this.

22. **Bobbery Early** clears some values, which allows you to perform Bobbery Early more than once on a save file without the need to reload.

23. **Disable Certain Sounds** allows you to disable certain sounds in the game. The current options for this are as follows:
    * **Disable Pause Menu/Z Menu Sounds** disables the sound effects that play when the pause menu and the Z menu are opened/closed.
    
    * **Background Music** disables the current background music, and prevents new background music from playing.
    
    * **Environment Sounds** disables any environment sounds, and prevents new environment sounds from playing.

24. **Force NPC Item Drop** forces any defeared enemy to drop a specified item. The item can be changed at will.

25. **Resolve Fades** allows you to resolve any fades that are currently active and linger on-screen. Some examples of these are the Mario heads after each chapter, and when the screen pans to black when transitioning to a new screen.

26. **Lock Flags** allows you to lock specific variables/flags. The **LSW**s and **LSWF**s are local variables/flags that change depending on the area that you're in. As such, the cheat will only allow you to lock the flags for one specific area at a time. However, you can lock the **LSW** variables for one area, and then lock the **LSWF** flags for a different area. The **Set New Area** options allow you to set the current area as the area to store the local flags for.
    * It is important to note that each **Lock** option allocates memory, so they shouldn't be left on when not in use.

27. **Manage Flags** allows you to manually change a lot of the important flags in the game.

28. **Clear Area Flags** allows you to clear most of the flags for a specified area. A confirmation message is displayed when trying to do so, to prevent accidently clearing flags. The flags will be cleared on the next screen transition.

### Stats
This menu allows you to modify Mario's stats, all of your partner's stats, whether or not partners are enabled or not, whether a partner is out or not, and whether or not a follower is out or not.
* You are also able to change Yoshi's color and name from within the **Partners** menu. Upon changing the color, you must respawn Yoshi for the changes to take effect. This is done by either swapping partners, entering pipes, or bringing Yoshi out via the menu itself.

### Settings
This menu allows you to save and load various settings. The settings are stored on a separate save file on the memory card. The menu gives you the option to save/load the settings to/from either of the memory card slots, as well as rename (as in, change the internal filename of your current settings file to the new internal filename if the older one was being used) and delete the settings file on either of the memory cards. The settings that are currently saved are as follows:
* Which Cheats are active
* Cheats button combinations
* Which Displays are active
* Displays button combinations
* Memory Watches (See [Memory](#memory))
* Memory Editor settings (See [Memory](#memory))
* Custom States (See [Warps](#warps))

It should also be noted that the settings will be loaded automatically at boot, assuming that a settings file is present on one of the memory cards. If settings files exist on both memory cards, then the one in Slot A will be used.

### Memory
This menu allows you to view and edit parts of the game's memory. The current options in this menu are as follows:
1. **Memory Watches** allows you to watch the values of up to 60 memory addresses, with support for up to 10 pointer levels each. Each of these watches can also be displayed on the screen at will.

2. **Memory Editor** allows you to enable a memory editor as well as adjust its settings. The settings that can be modified are as follows:
      * **Clear Cache** will clear the data cache and instruction cache of all of the memory that is modified via the editor.
   
      * **Raise System Level** will set a value that will cause most things in the overworld to pause. This will take effect once the editor is initially opened, and also if the setting is changed from within the editor itself.
   
      * **Disable Pause Menu** will disable the pause menu from being used. This is particularly useful since the editor uses the D-Pad for movement, but is not forced since you may want to view the game's memory while the pausing menu is running. This will take effect once the editor is initially opened, and also if the setting is changed from within the editor itself.
   
      * **Enable Vertical Separators** enables separators between each byte.
      * **Enable Horizontal Separators** enables separators between each row of bytes

   Upon opening the editor, the settings menu can be opened by pressing Y, which will allow modifying the same settings as listed above. Three other options are also present, which can be moved between with the D-Pad and selected with A:
      * **Address** allows you to change the address in which the editor starts displaying bytes for. The first valid range that can be selected with this is from `0x80000000` to `0x817FFFFF`, and the second valid range that can be selected with this is from `0xC0000000` to `0xC17FFFFF`.
      
      * **Bytes Size** allows you to change the range of bytes that are being modified at a single time. The maximum amount of bytes that can be selected is the total amount that is displayed at a single time in the editor. It's important to note that when the changes are applied, all of the bytes in the selected size will be changed all at once.
   
      * **Edit Bytes** allows you to start editing bytes, with the amount being based on the **Bytes Size**.
   
   Once **Edit Bytes** is selected, the following controls are used:
      * Before A is pressed, the editor will be in a state in which you are selecting which bytes in the bytes size should be edited. In this state, the **D-Pad** can be used to move in all four directions and the bytes that are currently selected will be `blue`. Pressing **B** in this state will cause you to go back to selecting one of the three options from before.
      
      * Pressing **A** in this state will put the editor into a state where you are selecting which digit in the bytes size to edit. In this state, the **D-Pad** can be used to move in all four directions, and a window will be drawn behind the digit that is currently selected. Furthermore, the digit that is selected will also be colored `green`. Pressing **B** in this state will cause you to go back to selecting which bytes in the bytes size should be edited.
      
      * Pressing **A** in this state will put the editor into a state where you are modifying the actual values of each digit. In this state, the **D-Pad** works the same as the previous state for **left and right**, but **up and down** increment and decrement the values respectively. Furthermore, the digit that is being modified will also be colored `orange`. If **A** is pressed while in this state, then all of the current values are applied at once, and the editor is put back into the state where you are selecting a range of bytes to change. If **B** is pressed instead, then the editor will go back to selecting which digit in the bytes size to edit, and all of the changed values will be discarded.
   
   Other notes about the memory editor:
      * When the editor is in a state that is after and including that of selecting which bytes in the bytes size should be edited, any of the **D-Pad** directions can be held to move automatically.
      
      * It is possible to both display and select invalid portions of memory. These are portions of memory that come before `0x80000000` and after `0x817FFFFF`, as well as before `0xC0000000` and after `0xC17FFFFF`. When these portions of memory are displayed, they will be displayed as question marks and will be colored `red`. These portions of memory can be selected, but you will not be able to modify their values.

### Battles
This menu allows you to change the HP, Max HP, FP, Max FP, held items, and statuses of anyone in a battle (refered to as Actors).
* Notes about the Battles menu:
  * This menu can only be used while in a battle.
  
  * Under certain circumstances, it is possible that having an actor recover using a Life Shroom can cause the game to softlock. It is believed that this only occurs when a textbox is supposed to occur, but the Life Shroom interrupts it. One example of where this happens is with Bowser in Chapter 8.

### Displays
1. **On-Screen Timer** displays a timer at the bottom-right of the screen. This timer is set to run at the current FPS, and therefore will count from 0 to 59 (0 to 49 for EU at 50hz) before reaching one second. Unlike the **Frame Counter**, this timer will continue to update even when loads occur. To be more specific, it will freeze during loads, but will update to the proper time once the load is complete. Also unlike the **Frame Counter**, this timer will also properly update when frames are skipped and/or not drawn. When this timer is displayed at the same time as the **Frame Counter**, then the **Frame Counter** will be placed above this timer.

2. **Frame Counter** displays a timer at the bottom-right of the screen. This timer is set to run at the current FPS, and therefore will count from 0 to 59 (0 to 49 for EU at 50hz) before reaching one second. Unlike the **On-Screen Timer**, this timer will pause during all loads and frames that are skipped and/or not drawn, as there aren't any actual frames during these times. When this timer is displayed at the same time as the **On-Screen Timer**, then this timer is placed above the **On-Screen Timer**.

3. **Mario's Coordinates** displays Mario's coordinates at the bottom-left of the screen. If the On-Screen Timer is active, then the position of Mario's Coordinates will be shifted upwards.

4. **Mario's Speed XZ** displays Mario's speed, based on both the X axis and the Z axis.

5. **Jump Storage** displays a value that determines whether you have Jump Storage or not. The value will be 0 when you don't have it, and 1 when you have it. While this display is active, you can press Y three times in succession to manually give yourself Jump Storage.

6. **Button Input Display** displays which buttons are pressed at the bottom-left of the screen. When this display is active, the other displays that are placed at the bottom-left of the screen are shifted upwards.

7. **Stick Angle** displays the current angle held with the left stick. The four main directions are 0 for up, 90 for right, 180 for down, and 270 for left. If no angle is held, then it will simply print **Neutral**. When the stick is not at the neutral position, there are also two additional numbers displayed. These two numbers are what are actually used to calculate the angle. The left number is for the X position, and the right number is for the Y position.

8. **Star Power Value** displays the amount of star power that you currently have. Each "circle" of star power is 100 units.

9. **D-Pad Options** refers to the display at the bottom-left of the screen that shows which d-pad button opens which submenu in the pause menu. You will still be able to press these buttons to open the submenus if the display is disabled.

10. **Guard/Superguard Timings** displays the following:
   * The frame that you guarded/superguarded an attack out of the total amount of possible frames.
   
   * How many frames early you pressed A or B. It should be noted that nothing will be displayed if the button was pressed very early (more than 6 or so frames for guards, and more than 12 or so frames for superguards).
   
   * Whether you pressed too many buttons in a short period of time for the guard/superguard to be accepted.
   
   * Whether the attack can be superguarded or not (will only be displayed if you tried to superguard it).
11. **Art Attack Hitboxes** displays boxes around enemies in battles when using Art Attack. While this display is active, you can hold Y while using Art Attack to pause the timer that is used to check how long you are able to use it for.

12. **Memory Usage** displays various memory information about each heap in the game. Each heap will use two lines: One for the **used** portion of the heap, and one for the **free** portion of the heap. The current amount of chunks for each heap will also be displayed, which is labeled as **cks**. It should also be noted that the information for a heap will not be displayed if the heap is corrupt.

13. **Effs Active** displays the amount of **eff**s active out of the maximum valid amount. In this context, **eff**s are pieces of graphical FX, such as the dust that is created when Mario is walking.

14. **Evts Active** displays the amount of **evt**s active out of the maximum valid amount. In this context, **evt**s are scripts.

15. **Enemy Encounter Notifier** displays a message when a non-cutscene battle would normally occur upon touching an enemy. The message will be displayed for about 3 seconds, and will reset back to 3 seconds if an enemy is touched again. Additionally, all non-cutscene battles will be disabled while this display is active.

16. **Hit Check Visualization** displays lines for collision detections for anything that runs through the **hitCheckVecFilter** function.

17. **Yoshi Skip** displays various information about performing this skip. The main timer (labeled YST) is set to reset and start running once you leave a battle, and pauses once you press A. You can also manually reset this timer by holding B for two seconds.

18. **Palace Skip** displays various information about performing this skip. The main timer (labeled PST) is set to reset and start once you leave the pause menu, and pauses once you press X. You can also manually reset this timer by holding B for two seconds. **PhaEmy** keeps track of the Y coordinate for the phantom ember, **ItemTimer** keeps track of the timer for the current item in the field, and **ParY** keeps track of your partner's Y coordinate.

19. **Jabbi Hive Skip** displays various information about performing this skip. The first field in this display tells you how well your pause jump was (pressed **D-Pad Left** first, pressed **A** first, or pressed both buttons at the same time). **FBP** is the amount of frames between pressing **D-Pad Left** and **A** to do the unspin jump, and **FBUP** is the amount of frames between the two **A** presses for closing the menu.

19. **Bridge Skip** displays various information about performing this skip. The first field in this display is how many frames early/late you were with pressing A. If you press A on the correct frame, then the counter will remain at 0. You can also manually reset this counter by holding B for two seconds. **HRP** is the pointer to the area of ground in the current room that you will respawn at upon touching a hazard. In order for **Bridge Skip** to work (as well as any other **Hazard Respawn Glitch**), this pointer must not be set, in which the value will be `0x00000000`.
     * Note: **Bridge Skip** will be possible in the PAL version of the game when this display is active.

20. **Blimp Ticket Skip** displays various information about performing this skip. The up-right timer (labeled **URT**) is set to reset and start running once you leave the pause menu, and pauses once the held stick angle reaches 25 or higher on JP, and 27 or higher on US/EU. The straight-up timer (labeled **SUT**) is set to reset and start running once you leave the pause menu, and pauses once the held stick angle is 0.00. Both of these timers can be manually reset by holding B for two seconds.

### Warps
This menu allows you to warp to various places around the game. You have three separate options to choose from:
1. **Select Warp** allows you to select a map from a list of useful/commonly-used maps. A description of each map is also displayed.

2. **Warp By Event** allows you to select an event to warp to. In this context, events are effectively a set of predetermined points within the story that can be warped to at will.
   * Notes about warping to an event:
     * Upon warping, all current game states will be cleared. This means that all flags related to the game story will be cleared. This also completely clears everything that is tracked in the pause menu.
     
     * After everything has been cleared, every upgrade/important item/partner/etc. that you should have gotten up to and including the current event will be given to you automatically.
     
     * The **Sequence** value set by this option differs from the **Change Sequence** cheat above, in that this option sets the **Sequence** to what is about to happen, not what has already happened.
     
     * The **Keep Inventory** option allows you to keep the standard items and badges that you currently have when warping to an event. If this is disabled, then all of your standard items and badges will be removed. Due to your BP being reset, all of the badges will be unequipped. The exception to this is Attack FX R, which will be equipped automatically if you're warping directly to the Hooktail fight.
     
     * The **Set Flags** option allows you to set specific flags upon warping to an event. If this is disabled, then the only flags that will be set are the ones that are set by the events.
     
       * The flags that are set are for the following:
         * Shop tutorial
         * Got first email
         * Read any email
         * Save block tutorial
         * Recovery block tutorial
         * Item tutorial

3. **Warp By Index** allows you to manually choose any map, as well as the entrance used when warping to said map. Additionally, the **View Current Map Entrances** shows you which value goes to which entrance in the current map. Due to limitations, it is currently impossible to fetch the list of entrances for any other maps besides the one you're currently in.
   * Notes about manually choosing a warp:
     * If an invalid entrance is used, then Mario will be placed at/near the center of the chosen map.
     
     * Some maps have entrances that don't actually work. Using them will simply place Mario at/near the center of the chosen map.

4. **Warp To Boss** allows you to warp to most bosses/mini-bosses in the game.
   * Note: Warping to a boss using this menu will adjust the game's **Sequence** value.

5. **Custom States** allows you to create up to 15 states that keep track of the following:
    * All of your current **standard items**, **important items**, **badges** (also **equipped badges**), and **stored items**
    * The current **Sequence Position**
    * The following data for **Mario**:
        * Current HP
        * Max HP
        * Current FP
        * Max FP
        * Current SP
        * Max SP
        * Available BP
        * Max BP
        * Rank
        * Level
        * Star Powers
        * Star Points
        * Coins
    * All of the **partner**s' current stats
    * Which **partner**/**follower** is currently out
    * Whether or not **Mario** is currently in **Boat Mode**
    * The current **map** and **loading zone**

## Misc
There are several other changes that have been made outside of the menu:
1. The build date on the title screen is always displayed.

2. When the game crashes, the game's built-in backtrace screen will appear. This would normally only appear when the debug mode code has been entered on the title screen, but is always enabled with this project. Furthermore, the text size of it has been adjusted slightly, and the text in the Japanese version scrolls like it does in the North American/European versions.

3. The current **Sequence** value is displayed in the pause menu next to Mario's stats.

4. The unused Mega badges can be used in battles without the need for a boot/hammer upgrade.

5. The heaps used in the game are checked every frame, and messages are displayed on-screen when errors are found.

6. A function in the game called **npcNameToPtr** can sometimes return an improper pointer, so an error message will be displayed on-screen when this happens. The message will be displayed for about 5 seconds.

7. A function in the game called **animPoseMain** will crash if a negative value is passed to it, so the function will be prevented from running to avoid the crash, and an error message will be displayed on-screen when this would occur. The message will be displayed for about 5 seconds.

8. Several crashes have been fixed/prevented:
   * Petalburg intro cutscene - Mario will not have any greeting animation.
   
   * Excess Express - When going to Poshley Heights - If you enter the western-most train car when the **Sequence** is past 338, then the **Sequence** will be set to 338.
   
   * X-Naut Fortress Factory Room - A global variable is cleared, so that leaving the room will not cause issues if the conveyor belt has not been activated.
   
   * If the Endless Room has been completed, and you enter it when the **Sequence** is at 385, then the room will be set to an incomplete state.
   
   * In some instances where the game tries to apply light effects to partners and/or followers, but one or neither of them are actually spawned, then they will be spawned automatically. When the game needs to spawn a partner for this, it will spawn Goombella. When it needs to spawn a follower for this, it will spawn Gus.
   
   * When defeating Blooper with an attack that causes both the body and the left tentacle to be defeated at roughly the same time.
   
9. The cutscene before the final Shadow Queen fight has been adjusted to prevent one of the game's heaps from becoming corrupted, in which less textboxes will appear at the start of the cutscene. Similarly, the cutscenes in Fahr Outpost and The Great Tree before the final Shadow Queen fight have also been adjusted to prevent one of the game's heaps from becoming corrupted, in which the textboxes that appear in those cutscenes will appear less frequently.

10. A memory leak that would occur when starting a new file has been fixed. This specific memory leak would occur when allocating memory that would be used for the inventory, as the game doesn't check if the memory has already been allocated or not.
