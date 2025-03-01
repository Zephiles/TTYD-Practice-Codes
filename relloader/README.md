## Explanation of REL Loader versions:

### V1:
* Loads the REL in **marioStInit** after various other things have been initialized.
* Does not used fixed linking, so the REL can be easily unloaded/unlinked from memory if linked against other RELs later on.
* Can use **__memAlloc** and **__memFree** for allocating/freeing memory allocations.
* Is not compatible with mods set up to use **V3** (unless they are updated to support all **REL Loader** versions).
* Is included mainly for legacy purposes, and generally won't be used since most project won't ever be unloaded/unlinked.

### V2:
* Loads the REL in **marioStInit** after various other things have been initialized.
* Uses fixed linking, so it is assumed that the REL will never be unloaded/unlinked from memory.
* Can use **__memAlloc** and **__memFree** for allocating/freeing memory allocations.
* Recommended for most REL mods.
* Is not compatible with mods set up to use **V3** (unless they are updated to support all **REL Loader** versions).

### V3:
* Loads the REL at the start of **main**, so the REL's prolog function is effectively running before any of the game's main code (aside from startup code from **__start**) has ran.
* Uses fixed linking, so it is assumed that the REL will never be unloaded/unlinked from memory.
* The REL is placed at the start of the **Arena Lo**, and after its **prolog** function has ran it is assumed that everything after the current **Arena Lo** can be used by the game for other stuff.
* Assumes that the **Arena Hi** has not been modified by the REL's **prolog** function (and anything that it calls) and as such resets it to its original value before the REL's **prolog** function was called.
* Subtracts the REL's used size (how much is used from the **Arena Lo**, which includes the REL itself and anything that it allocates from the **Arena Lo**) from the size of the main heap, which ensures that all of the other game's heap will be given the same amount of memory as the vanilla game.
* Cannot use **__memAlloc** and **__memFree** for allocating/freeing memory allocations in the REL's **prolog** function (and anything that it calls), as the game's main heaps have not been created yet.
* Not recommended for most mods due to not being compatible with mods set up for **V1**/**V2** (unless they are updated to support all **REL Loader** versions), and all older mods use **V1**/**V2**.
