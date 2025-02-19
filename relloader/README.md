## Explanation of REL Loader versions:

### V1:
* Loads the REL in **marioStInit** after various other things have been initialized.
* Does not used fixed linking, so the REL can be easily unloaded/unlinked from memory if linked against other RELs later on.
* Can use **__memAlloc** and **__memFree** for allocating/freeing memory allocations.
* Is not compatible with mods set up to use **V3**.

### V2:
* Loads the REL in **marioStInit** after various other things have been initialized.
* Uses fixed linking, so it is assumed that the REL will never be unloaded/unlinked from memory.
* Can use **__memAlloc** and **__memFree** for allocating/freeing memory allocations.
* Recommended for most REL mods.
* Is not compatible with mods set up to use **V3**.

### V3:
* Loads the REL at the start of **main**, so the REL's prolog function is effectively running before any of the game's main code (aside from startup code from **__start**) has ran.
* Cannot use **__memAlloc** and **__memFree** for allocating/freeing memory allocations in the REL's **prolog** function (and anything that it calls), as the game's main heaps have not been created yet.
* Not recommended for most mods due to not being compatible with mods set up for **V1**/**V2**, and all older mods use **V1**/**V2**.
