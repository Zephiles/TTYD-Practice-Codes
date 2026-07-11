#include "menuUtils.h"
#include "cxx.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/rootMenu.h"
#include "menus/battlesMenu.h"
#include "ttyd/battle.h"
#include "ttyd/mariost.h"
#include "ttyd/battle_unit.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void exit();

BattlesMenu *gBattlesMenu = nullptr;

static const MenuFunctions gFuncs = {
    nullptr, // The amount of options depends on the amount of actors currently in a battle, so that must be handled manually
    controls,
    draw,
    exit,
};

void battlesMenuInit(Menu *menuPtr)
{
    // Do not enter the Battles menu if not actually in a battle
    if (!getBattleWorkPtr())
    {
        rootMenuExitBattlesMenu();
        return;
    }

    // Need to enter the battles menu before initializing gBattlesMenu
    menuPtr = enterNextMenu(&gFuncs, getbattlesMenuInitMaxIndex(), BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE);

    // Failsafe: Make sure memory isn't already allocated for gBattlesMenu
    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    if (battlesMenuPtr)
    {
        delete battlesMenuPtr;
    }

    gBattlesMenu = new BattlesMenu(menuPtr);
}

static void exit()
{
    delete gBattlesMenu;
    gBattlesMenu = nullptr;
}

static void getBattleHighestIndexAndTotalActors(BattlesMenuIndexAndActors *indexAndActorsPtr)
{
    const uint32_t maxIndex = getbattlesMenuInitMaxIndex();
    uint32_t highestIndex = 0;
    uint32_t totalActors = 0;

    for (uint32_t i = 1; i <= maxIndex; i++) // Start at one to skip the System actor
    {
        if (getActorBattlePtr(i))
        {
            highestIndex = i;
            totalActors++;
        }
    }

    indexAndActorsPtr->highestIndex = static_cast<uint8_t>(highestIndex);
    indexAndActorsPtr->totalActors = static_cast<uint8_t>(totalActors);
}

static void dpadControls(Menu *menuPtr, MenuButtonInput button)
{
    // Get the highest index that is currently being used in the menu
    BattlesMenuIndexAndActors indexAndActors;
    getBattleHighestIndexAndTotalActors(&indexAndActors);

    // Failsafe: Make sure the highest index is valid
    int32_t highestIndex = static_cast<int32_t>(indexAndActors.highestIndex);
    if (highestIndex <= 0)
    {
        return;
    }

    uint32_t currentPage = menuPtr->getCurrentPage();
    uint32_t currentIndex = menuPtr->getCurrentIndex();

    // Moving to different pages requires special actions depending on if actors are on the next page or not
    switch (button)
    {
        case MenuButtonInput::DPAD_DOWN:
        {
            // Check to see if at the bottom of the current page
            const uint32_t firstIndexNextPage = (currentPage + 1) * BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE;
            if (currentIndex == (firstIndexNextPage - 1))
            {
                // Prevent going to the next page if there are no more used slots
                if (highestIndex <= static_cast<int32_t>(firstIndexNextPage))
                {
                    // Go to the first page
                    // setCurrentPage sets the current index to the top of the new page
                    menuPtr->setCurrentPage(0);
                    return;
                }
            }
            break;
        }
        case MenuButtonInput::DPAD_UP:
        {
            // Check to see if at the top of the current page
            if (currentIndex == (currentPage * BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE))
            {
                // Only run if on the first page
                if (currentPage == 0)
                {
                    // Go to the last page that has slots in use
                    while (highestIndex > static_cast<int32_t>((currentPage + 1) * BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE))
                    {
                        currentPage++;
                        currentIndex += BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE;
                        highestIndex -= BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE;
                    }

                    // setCurrentIndex sets the current page to where the new index is
                    menuPtr->setCurrentIndex(currentIndex + BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE - 1);
                    return;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }

    menuControlsVertical(button,
                         menuPtr->getCurrentIndexPtr(),
                         menuPtr->getCurrentPagePtr(),
                         getbattlesMenuInitMaxIndex(),
                         BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE,
                         1,
                         false);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    // Close the Battles menu if not in a battle
    if (!getBattleWorkPtr())
    {
        rootMenuExitBattlesMenu();
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    const bool autoIncrement = handleMenuAutoIncrement(battlesMenuPtr->getAutoIncrementPtr());

    // Handle held button input if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                dpadControls(menuPtr, buttonHeld);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Handle the button input pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            dpadControls(menuPtr, button);
            break;
        }
        case MenuButtonInput::A:
        {
            // Make sure the current slot isn't empty
            const uint32_t actorIndex = menuPtr->getCurrentIndex() + 1; // Add one to skip the System actor

            BattleWorkUnit *actorPtr = getActorBattlePtr(actorIndex);
            if (actorPtr)
            {
                // Go to the battles stats menu
                battlesMenuPtr->setCurrentActorIndex(actorIndex);
                battlesMenuStatsInit();
                break;
            }
            break;
        }
        case MenuButtonInput::B:
        {
            enterPrevMenu();
            break;
        }
        default:
        {
            break;
        }
    }
}

static void drawBattlesActors()
{
    // Initialize text drawing
    drawTextInit(false);

    // Get the highest index that is currently being used in the menu as well as the total amount of actors currently in the
    // battle
    BattlesMenuIndexAndActors indexAndActors;
    getBattleHighestIndexAndTotalActors(&indexAndActors);

    // Draw the page number at the top-right of the main window if there is more than one page
    const Menu *menuPtr = gMenu;
    const uint32_t currentPage = menuPtr->getCurrentPage();
    const Window *rootWindowPtr = gRootWindow;

    constexpr float scale = MENU_SCALE;
    constexpr float lineDecrement = MENU_LINE_HEIGHT * scale;

    char buf[16];
    constexpr uint32_t bufSize = sizeof(buf);
    float tempPosX;
    float tempPosY;

    const bool multiplePages = indexAndActors.highestIndex > BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE;
    if (multiplePages)
    {
        // Draw the page as an int, to prevent long text if it somehow becomes negative
        snprintf(buf, bufSize, "Page %" PRId32, currentPage + 1);

        rootWindowPtr->getTextPosXY(buf, WindowAlignment::TOP_RIGHT, scale, &tempPosX, &tempPosY);
        drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));
    }

    // Draw the total amount of actors in the battle out of the maximum, excluding the System actor
    // Draw the amounts as ints, to prevent long text if they somehow becomes negative
    constexpr int32_t maxActors = sizeof(BattleWork::battle_units) / sizeof(BattleWork::battle_units[0]);
    snprintf(buf, bufSize, "%" PRId32 "/%" PRId32, static_cast<int32_t>(indexAndActors.totalActors), maxActors - 1);
    rootWindowPtr->getTextPosXY(buf, WindowAlignment::TOP_RIGHT, scale, &tempPosX, &tempPosY);

    // Retrieve posY as a separate variable to avoid unnecessarily updating the temporary value thats stored on the stack
    float posY = tempPosY;

    if (multiplePages)
    {
        // Move the text down to be below the page count
        posY -= lineDecrement;
    }

    drawText(buf, tempPosX, posY, scale, getColorWhite(0xFF));

    // Draw the actor texts
    // Get the position of the actor texts
    rootWindowPtr->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    posY = tempPosY;

    uint32_t indexStart = currentPage * BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE;
    const uint32_t maxIndex = getbattlesMenuInitMaxIndex();
    const uint32_t currentIndex = menuPtr->getCurrentIndex();
    const char **battleActorsPtr = gBattleActors;

    for (uint32_t i = indexStart; i < (indexStart + BATTLES_SELECT_ACTOR_TOTAL_ACTORS_PER_PAGE); i++)
    {
        if (i >= maxIndex)
        {
            // Reached the end of the array, so exit
            return;
        }

        // Get the current actor text
        BattleWorkUnit *actorPtr = getActorBattlePtr(i + 1); // Add one to skip the System actor
        const char *actorString;
        bool slotIsEmpty = false;

        if (!actorPtr)
        {
            actorString = "Empty Slot";
            slotIsEmpty = true;
        }
        else
        {
            // Failsafe: Make sure current_kind is valid
            const BattleUnitType type = actorPtr->current_kind;
            if (battleUnitTypeIsValid(type))
            {
                actorString = battleActorsPtr[static_cast<int32_t>(type) - 1];
            }
            else
            {
                actorString = "Invalid Actor Id";
            }
        }

        // Get the current actor text color
        uint32_t color;
        if (currentIndex == i)
        {
            color = getColorBlue(0xFF);
        }
        else if (slotIsEmpty)
        {
            color = getColorGrayedOut(0xFF);
        }
        else
        {
            color = getColorWhite(0xFF);
        }

        // Draw the current actor text
        drawText(actorString, posX, posY, scale, color);
        posY -= lineDecrement;
    }
}

static void draw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // Draw each actor
    drawBattlesActors();
}

void battlesMenuCancelChangeValue()
{
    gBattlesMenu->getValueEditorPtr()->stopDrawing();
}

void battlesMenuReturnToInitMenu()
{
    // Close all menus until the Battles menu is reached
    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    Menu *menuPtr = gMenu;
    const Menu *battlesMenu = battlesMenuPtr->getBattlesMenuPtr();

    while (menuPtr != battlesMenu)
    {
        menuPtr = enterPrevMenu();

        // Failsafe: Make sure gMenu doesn't end up being nullptr
        if (!menuPtr)
        {
            return;
        }
    }
}

BattleWork *getBattleWorkPtr()
{
    // _globalWorkPtr->inBattle should be set to anything other than 0 to indicate that the player is currently in a battle
    if (_globalWorkPtr->inBattle == 0)
    {
        return nullptr;
    }

    return _battleWorkPtr;
}

BattleWorkUnit *getMarioBattlePtr()
{
    BattleWork *battleWorkPtr = getBattleWorkPtr();
    if (!battleWorkPtr)
    {
        return nullptr;
    }

    return BattleGetMarioPtr(battleWorkPtr);
}

BattleWorkUnit *getPartnerBattlePtr()
{
    BattleWork *battleWorkPtr = getBattleWorkPtr();
    if (!battleWorkPtr)
    {
        return nullptr;
    }

    return BattleGetPartyPtr(battleWorkPtr);
}

BattleWorkUnit *getActorBattlePtr(uint32_t slot)
{
    BattleWork *battleWorkPtr = getBattleWorkPtr();
    if (!battleWorkPtr)
    {
        return nullptr;
    }

    return BattleGetUnitPtr(battleWorkPtr, slot);
}
