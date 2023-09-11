#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/warpsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "misc/functionHooks.h"
#include "ttyd/seq_mapchange.h"
#include "ttyd/camdrv.h"
#include "ttyd/mapdata.h"
#include "ttyd/evtmgr.h"
#include "ttyd/mariost.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

const char *gUnusedMaps[WARPS_INDEX_TOTAL_UNUSED_MAPS] = {
    "tik_09",
    "tik_10",
    "tik_14",
    "rsh_05_b",
    "rsh_05_c",
    "rsh_06_b",
    "rsh_06_c",
};

const MenuOption gWarpsMenuIndexOptions[] {
    "Select New Map",
    warpsMenuIndexSelectNewMapOrEntranceId,

    "Select New Entrance Id",
    warpsMenuIndexSelectNewMapOrEntranceId,

    "View Current Map Entrances",
    warpsMenuIndexViewEntrancesInit,

    "Warp",
    warpsMenuIndexWarp,
};

const MenuFunctions gWarpsMenuIndexFuncs = {
    gWarpsMenuIndexOptions,
    warpsMenuEventIndexControls,
    warpsMenuIndexDraw,
    nullptr, // Exit function not needed
};

void warpsMenuIndexInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gWarpsMenuIndexOptions) / sizeof(MenuOption);
    enterNextMenu(&gWarpsMenuIndexFuncs, totalOptions);
}

void WarpsMenu::drawSelectIndexWarpInfo() const
{
    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posXBase = tempPosX;
    float posX = posXBase;
    float posY = tempPosY;

    // Draw the labels for the maps and entrances
    const char *mapsAndEntrancesLabels = "Current Map\nCurrent Entrance\n\nNew Map\nNew Entrance Id";
    drawText(mapsAndEntrancesLabels, posX, posY, scale, getColorWhite(0xFF));

    // Get the width that is being used by the labels
    float textWidth;
    getTextWidthHeight(mapsAndEntrancesLabels, scale, &textWidth, nullptr);

    // Get the text and color for the current map and entrance
    MapAndBeroDetails mapAndBeroDetails;
    getMapAndBeroTextAndColor(_next_map, _next_bero, &mapAndBeroDetails);

    const WarpByIndex *warpByIndexPtr = gMod->getWarpByIndexPtr();
    const char *mapName = getMapFromIndex(warpByIndexPtr->getMapId());
    const uint32_t entranceId = warpByIndexPtr->getEntranceId();

    // Get the maps and entrances
    char buf[96];
    snprintf(buf,
             sizeof(buf),
             "<col %" PRIx32 ">%s\n<col %" PRIx32 ">%s\n\n%s\n%" PRIu32,
             mapAndBeroDetails.mapColor,
             mapAndBeroDetails.mapTextPtr,
             mapAndBeroDetails.beroColor,
             mapAndBeroDetails.beroTextPtr,
             mapName,
             entranceId);

    // Set the text to be a bit to the right of the labels
    const float posXIncrement = textWidth + (30.f * scale);

    // Draw the maps and entrances
    drawText(buf, posX + posXIncrement, posY, scale, getColorWhite(0xFF));
}

void warpsMenuIndexDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for selecting a map id or entrance id
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    warpsMenuPtr->drawSelectIndexWarpInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = warpsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = warpsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void warpsMenuIndexSelectedNewMapOrEntranceId(const ValueType *valuePtr)
{
    const uint32_t id = valuePtr->u32;
    WarpByIndex *warpByIndexPtr = gMod->getWarpByIndexPtr();

    if (gMenu->getCurrentIndex() == WarpsMenuIndexOptions::WARPS_MENU_INDEX_OPTION_SELECT_NEW_MAP)
    {
        warpByIndexPtr->setMapId(id);
    }
    else
    {
        warpByIndexPtr->setEntranceId(id);
    }

    // Close the value editor
    warpsMenuCloseValueEditor();
}

void warpsMenuIndexSelectNewMapOrEntranceId(Menu *menuPtr)
{
    // Initialize the value editor
    WarpsMenu *warpsMenuPtr = gWarpsMenu;
    ValueEditor *valueEditorPtr = warpsMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    uint32_t currentValue;
    uint32_t minValue;
    uint32_t maxValue;
    VariableType type;

    const WarpByIndex *warpByIndexPtr = gMod->getWarpByIndexPtr();
    if (menuPtr->getCurrentIndex() == WarpsMenuIndexOptions::WARPS_MENU_INDEX_OPTION_SELECT_NEW_MAP)
    {
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_MAP_STRING);

        // Make sure the current map index is valid
        int32_t mapIndex = getMapIndex();
        if (mapIndex < 0)
        {
            mapIndex = 0;
        }

        currentValue = static_cast<uint32_t>(mapIndex);
        minValue = 0;
        maxValue = WARPS_INDEX_MAX_INDEX;
        type = VariableType::u16;
    }
    else
    {
        currentValue = warpByIndexPtr->getEntranceId();
        minValue = 1;
        maxValue = WARP_BY_INDEX_ENTRANCE_LIST_SIZE;
        type = VariableType::u8;
    }

    const Window *rootWindowPtr = gRootWindow;
    valueEditorPtr->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, type, rootWindowPtr->getAlpha());
    valueEditorPtr->startDrawing(warpsMenuIndexSelectedNewMapOrEntranceId, warpsMenuCloseValueEditor);
}

uint32_t warpToMapByString(const char *map)
{
    // Make sure the player is currently in the game
    if (!checkIfInGame())
    {
        return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME;
    }

    // Warp to the new map
    setNextMap(map);
    reloadRoomMain();

    return WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS;
}

void warpsMenuIndexWarp(Menu *menuPtr)
{
    (void)menuPtr;

    Mod *modPtr = gMod;
    const uint32_t mapId = modPtr->getWarpByIndexPtr()->getMapId();
    const char *map = getMapFromIndex(mapId);

    switch (warpToMapByString(map))
    {
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_SUCCESS:
        {
            // Warped successfully, so set the flag for the loading zone to be adjusted and close the menu
            modPtr->setFlag(ModFlag::MOD_FLAG_WARP_BY_INDEX_INIT);
            return closeAllMenus();
        }
        case WarpsMenuWarpToMapReturnValue::WARPS_MENU_WARP_TO_MAP_NOT_IN_GAME:
        {
            gWarpsMenu->initErrorWindow(gWarpsMenuCannotWarpText);
            break;
        }
        default:
        {
            break;
        }
    }
}

const char *getMapFromIndex(uint32_t index)
{
    // Make sure the index is valid
    if (index >= WARPS_INDEX_MAX_INDEX)
    {
        return "title";
    }
    else if (index == 0)
    {
        return "aaa_00";
    }

    // Check for the unused maps
    const char **unusedMapsPtr = gUnusedMaps;
    switch (index)
    {
        case 18:
        {
            return unusedMapsPtr[0]; // tik_09
        }
        case 19:
        {
            return unusedMapsPtr[1]; // tik_10
        }
        case 23:
        {
            return unusedMapsPtr[2]; // tik_14
        }
        case 135:
        {
            return unusedMapsPtr[3]; // rsh_05_b
        }
        case 136:
        {
            return unusedMapsPtr[4]; // rsh_05_c
        }
        case 138:
        {
            return unusedMapsPtr[5]; // rsh_06_b
        }
        case 139:
        {
            return unusedMapsPtr[6]; // rsh_06_c
        }
        default:
        {
            break;
        }
    }

    uint32_t newIndex = index - 1;

    // Decrement newIndex based on the current index
    if (index >= 138) // rsh_06_c
    {
        newIndex -= 7;
    }
    else if (index >= 135) // rsh_05_c
    {
        newIndex -= 5;
    }
    else if (index >= 23) // tik_14
    {
        newIndex -= 3;
    }
    else if (index >= 19) // tik_10
    {
        newIndex -= 2;
    }

    // Skip invalid values
    uint32_t counter = 0;

    if (index >= 277) // dig_00
    {
        counter += 32;
    }
    else if (index >= 274) // qiz_00
    {
        counter += 24;
    }
    else if (index >= 94) // hei_bt00
    {
        counter += 16;
    }
    else if (index >= 78) // gon_bt00
    {
        counter += 8;
    }

    const uint32_t worldDataPtrRaw = reinterpret_cast<uint32_t>(worldData.unk_18);
    return *reinterpret_cast<const char **>(*reinterpret_cast<uint32_t *>(worldDataPtrRaw + 0x10) + counter + (newIndex * 0x8));
}

int32_t getMapIndex()
{
    for (int32_t i = 0; i <= WARPS_INDEX_MAX_INDEX; i++)
    {
        if (compareStringToNextMap(getMapFromIndex(i)))
        {
            return i;
        }
    }
    return -1;
}

MapData *mapDataPtrHandleUnusedMaps(const char *mapName)
{
    // Check if the current map is unused
    WarpByIndex *warpByIndexPtr = gMod->getWarpByIndexPtr();
    MapData *unusedMapDataPtr = warpByIndexPtr->getUnusedMapDataPtr();
    const uint32_t unusedMapNameSize = warpByIndexPtr->getUnusedMapNameSize();

    const char **unusedMapsPtr = gUnusedMaps;
    for (uint32_t i = 0; i < WARPS_INDEX_TOTAL_UNUSED_MAPS; i++)
    {
        if (strncmp(mapName, unusedMapsPtr[i], unusedMapNameSize) == 0)
        {
            // Set up the new data
            strncpy(warpByIndexPtr->getUnusedMapNamePtr(),
                    mapName,
                    unusedMapNameSize - 1); // Subtract 1 to make sure the string is properly null terminated

            unusedMapDataPtr->pInitEvtCode = warpByIndexPtr->getCurrentMapInitEvtCodePtr();
            return unusedMapDataPtr;
        }
    }

    // Call the original function
    return g_mapDataPtr_trampoline(mapName);
}

int32_t setIndexWarpEntrances(EvtEntry *evtPtr, bool isFirstCall)
{
    // Clear the array holding the loading zone names
    Mod *modPtr = gMod;
    WarpByIndex *warpByIndexPtr = modPtr->getWarpByIndexPtr();

    const char **entranceListPtr = warpByIndexPtr->getEntranceListPtr();
    clearMemory(entranceListPtr, WARP_BY_INDEX_ENTRANCE_LIST_SIZE * sizeof(const char *));

    // Get the start of the loading zone addresses
    const uint32_t evtLZAddressesRaw = static_cast<uint32_t>(evtPtr->lwData[0]);

    // Copy each loading zone to the array
    uint32_t evtAddressRaw = evtLZAddressesRaw;
    uint32_t loadingZoneTotal = 0;

    for (; loadingZoneTotal < WARP_BY_INDEX_ENTRANCE_LIST_SIZE; loadingZoneTotal++)
    {
        const char *tempAddressPtr = *reinterpret_cast<const char **>(evtAddressRaw);
        if (!tempAddressPtr)
        {
            break;
        }

        entranceListPtr[loadingZoneTotal] = tempAddressPtr;
        evtAddressRaw += 0x3C;
    }

    // Set the new entrance total
    warpByIndexPtr->setTotalEntrances(loadingZoneTotal);

    // Check to see if warping by index
    if (modPtr->flagIsSet(ModFlag::MOD_FLAG_WARP_BY_INDEX_INIT))
    {
        modPtr->clearFlag(ModFlag::MOD_FLAG_WARP_BY_INDEX_INIT);

        // Make sure the entrance id is not 0 somehow
        const uint32_t entranceId = warpByIndexPtr->getEntranceId();
        uint32_t chosenEntrance;

        if (entranceId > 0)
        {
            chosenEntrance = entranceId - 1;
        }
        else
        {
            chosenEntrance = 0;
        }

        // Make sure the chosen entrance exists for the chosen map
        const char *chosenEntranceName;
        if (chosenEntrance < loadingZoneTotal)
        {
            // Valid entrance chosen
            chosenEntranceName = *reinterpret_cast<const char **>(evtLZAddressesRaw + (chosenEntrance * 0x3C));
        }
        else
        {
            // Invalid entrance chosen
            chosenEntranceName = "";
        }

        // Set the chosen entrance
        strcpy(globalWorkPtr->beroEnterName, chosenEntranceName);
        setNextBero(chosenEntranceName);
    }

    // Call the original function
    return g_evt_bero_get_info_trampoline(evtPtr, isFirstCall);
}

void unloadClearCurrentMapInitScript(const char *currentMap, const char *nextMap, const char *nextBero)
{
    // Clear the current map evt code pointer variable
    // Must clear this variable, as not all maps have an init script
    WarpByIndex *warpByIndexPtr = gMod->getWarpByIndexPtr();
    warpByIndexPtr->clearCurrentMapInitEvtCodePtr();

    // Clear the total entrances, as it won't be properly updated if the map does not have any
    warpByIndexPtr->setTotalEntrances(0);

    // Call the original function
    return g__unload_trampoline(currentMap, nextMap, nextBero);
}

void relSetEvtAddrSetCurrentMapInitScript(const char *mapName, const void *pInitEvtCode)
{
    // Check if setting the init evt code pointer for the current map
    if (compareStringToNextMap(mapName))
    {
        // Set the current map init evt code pointer variable
        gMod->getWarpByIndexPtr()->setCurrentMapInitEvtCodePtr(pInitEvtCode);
    }

    // Call the original function
    return g_relSetEvtAddr_trampoline(mapName, pInitEvtCode);
}
