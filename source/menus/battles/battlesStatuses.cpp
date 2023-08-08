#include "mod.h"
#include "menuUtils.h"
#include "drawIcon.h"
#include "drawText.h"
#include "classes/window.h"
#include "menus/battlesMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/icondrv.h"
#include "ttyd/battle_unit.h"
#include "ttyd/windowdrv.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const IconId gBattlesStatusesOptionsIcons[] = {
    IconId::ICON_SLEEPY_SHEEP,           // Sleep
    IconId::ICON_STOPWATCH,              // Stop
    IconId::ICON_DIZZY_DIAL,             // Dizzy
    IconId::ICON_BATTLE_POISON_MUSHROOM, // Poision
    IconId::ICON_BATTLE_POISON_MUSHROOM, // Poison Amount
    IconId::ICON_RUIN_POWDER,            // Confused
    IconId::ICON_VOLT_SHROOM,            // Electrified
    IconId::ICON_REPEL_CAPE,             // Dodgy
    IconId::ICON_FIRE_DRIVE,             // Burn
    IconId::ICON_ICE_STORM,              // Frozen
    IconId::ICON_POWER_PUNCH,            // Big/Shrink
    IconId::ICON_POWER_PUNCH,            // Big/Shrink Amount
    IconId::ICON_POWER_PLUS,             // Attack Up/Down
    IconId::ICON_POWER_PLUS,             // Attack Up/Down Amount
    IconId::ICON_DEFEND_PLUS,            // Defense Up/Down
    IconId::ICON_DEFEND_PLUS,            // Defense Up/Down Amount
    IconId::ICON_CHARGE,                 // Charged
    IconId::ICON_SPACE_FOOD,             // Allergic
    IconId::ICON_BOOS_SHEET,             // Invisible
    IconId::ICON_SPITE_POUCH,            // Payback
    IconId::ICON_PAUSE_MENU_CLOCK,       // Fast
    IconId::ICON_KOOPA_CURSE,            // Slow
    IconId::ICON_SLOW_SHROOM,            // HP Regen
    IconId::ICON_SLOW_SHROOM,            // HP Regen Amount
    IconId::ICON_GRADUAL_SYRUP,          // FP Regen
    IconId::ICON_GRADUAL_SYRUP,          // FP Regen Amount
    IconId::ICON_UNKNOWN_WHITE_LARGE_X,  // Defeated Flag
};

const char *gBattlesStatusesOptions[] = {
    "Sleep Turns Left",          "Immobilized Turns Left", "Dizzy Turns Left",
    "Poison Turns Left",         "Poison Damage Amount",   "Confused Turns Left",
    "Electrified Turns Left",    "Dodgy Turns Left",       "Burn Turns Left",
    "Frozen Turns Left",         "Big/Shrink Turns Left",  "Big/Shrink Power Amount",
    "Attack Up/Down Turns Left", "Attack Up/Down Amount",  "Defense Up/Down Turns Left",
    "Defense Up/Down Amount",    "Charged Turns Left",     "Allergic Turns Left",
    "Invisible Turns Left",      "Payback Turns Left",     "Fast Turns Left",
    "Slow Turns Left",           "HP Regen Turns Left",    "HP Regen Amount",
    "FP Regen Turns Left",       "FP Regen Amount",        "Defeated Flag",
};

const MenuFunctions gBattlesMenuStatusesFuncs = {
    nullptr, // All of the options will use the same function, and the drawing code will not use the default function, so avoid
             // an array of MenuOption to save memory

    battlesMenuStatusesControls,
    battlesMenuStatusesDraw,
    nullptr, // Exit function not needed
};

void battlesMenuStatusesInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gBattlesStatusesOptions) / sizeof(gBattlesStatusesOptions[0]);
    enterNextMenu(&gBattlesMenuStatusesFuncs, totalOptions, BATTLES_STATUSES_MAX_STATUSES_PER_PAGE);
}

int8_t *getActorStatusPtr(BattleWorkUnit *actorPtr, uint32_t index)
{
    uint32_t counter = 0;

    // counter must be incremented to account for being on different pages
    if (index >= 20) // Explosion turns left
    {
        counter += 3;
    }
    else if (index >= 18) // Can't use items
    {
        counter += 2;
    }
    else if (index >= 1) // Sleep flags
    {
        counter++;
    }

    return reinterpret_cast<int8_t *>(reinterpret_cast<uint32_t>(&actorPtr->sleep_turns) + index + counter);
}

void menuBattlesStatusesChangeValue(const ValueType *valuePtr)
{
    const int32_t value = valuePtr->s32;
    BattleWorkUnit *actorPtr = getActorBattlePtr(gBattlesMenu->getCurrentActorIndex());
    const uint32_t currentIndex = gMenu->getCurrentIndex();

    // Set the new value
    *getActorStatusPtr(actorPtr, currentIndex) = static_cast<int8_t>(value);

    // Close the value editor
    battlesMenuCancelChangeValue();
}

void battlesMenuStatusesDPadControls(MenuButtonInput button, Menu *menuPtr, uint32_t totalOptions)
{
    menuControlsVertical(button,
                         menuPtr->getCurrentIndexPtr(),
                         menuPtr->getCurrentPagePtr(),
                         totalOptions,
                         BATTLES_STATUSES_MAX_STATUSES_PER_PAGE,
                         1,
                         false);
}

void battlesMenuStatusesControls(Menu *menuPtr, MenuButtonInput button)
{
    // Close the Battles menu if not in a battle
    if (!getBattleWorkPtr())
    {
        rootMenuExitBattlesMenu();
        return;
    }

    // If the pointer to the selected actor is invalid, then assume that the actor is no longer in the battle
    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    BattleWorkUnit *actorPtr = getActorBattlePtr(battlesMenuPtr->getCurrentActorIndex());

    if (!actorPtr)
    {
        battlesMenuReturnToInitMenu();
        return;
    }

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr;
    if (valueEditorPtr = battlesMenuPtr->getValueEditorPtr(), valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Make sure the current index is valid
    const uint32_t currentIndex = menuPtr->getCurrentIndex();
    constexpr uint32_t totalOptions = sizeof(gBattlesStatusesOptions) / sizeof(gBattlesStatusesOptions[0]);

    if (currentIndex >= totalOptions)
    {
        // Failsafe: Reset the current index to 0
        menuPtr->setCurrentIndex(0);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(battlesMenuPtr->getAutoIncrementPtr());

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                battlesMenuStatusesDPadControls(buttonHeld, menuPtr, totalOptions);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    int8_t *valuePtr = getActorStatusPtr(actorPtr, currentIndex);

    // Handle the button inputs pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            battlesMenuStatusesDPadControls(button, menuPtr, totalOptions);
            break;
        }
        case MenuButtonInput::A:
        {
            if (currentIndex == StatsBattlesStatusesOptions::BATTLES_STATUSES_DEFEATED_FLAG)
            {
                // Toggle the value
                if (*valuePtr == 0)
                {
                    *valuePtr = 1;
                }
                else
                {
                    *valuePtr = 0;
                }
            }
            else
            {
                // Get the current, min, and max values
                int32_t currentValue = *valuePtr;
                int32_t minValue = 0;
                int32_t maxValue = 99;

                switch (currentIndex)
                {
                    case StatsBattlesStatusesOptions::BATTLES_STATUSES_BIG_SHRINK_POWER_AMOUNT:
                    case StatsBattlesStatusesOptions::BATTLES_STATUSES_ATTACK_UP_DOWN_POWER_AMOUNT:
                    case StatsBattlesStatusesOptions::BATTLES_STATUSES_DEFENSE_UP_DOWN_POWER_AMOUNT:
                    {
                        minValue = -99;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Initialize the value editor
                ValueEditor *valueEditorPtr = battlesMenuPtr->getValueEditorPtr();

                uint32_t flags = 0;
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
                flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

                if (minValue < 0)
                {
                    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::VALUE_IS_SIGNED);
                }

                const Window *rootWindowPtr = gRootWindow;

                valueEditorPtr->init(&currentValue,
                                     &minValue,
                                     &maxValue,
                                     rootWindowPtr,
                                     flags,
                                     VariableType::s8,
                                     rootWindowPtr->getAlpha());

                valueEditorPtr->startDrawing(menuBattlesStatusesChangeValue, battlesMenuCancelChangeValue);
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

void BattlesMenu::drawBattleActorStatuses(BattleWorkUnit *actorPtr) const
{
    // To avoid a lot of unnecessary extra GX calls, draw the icons first
    // Get the starting position for the icons
    float posXBase;
    float posYBase;

    constexpr float scale = MENU_SCALE;
    const Window *rootWindowPtr = gRootWindow;
    rootWindowPtr->getIconPosXY(WindowAlignment::TOP_LEFT, scale, &posXBase, &posYBase);

    // Get the starting position for the texts and values
    float valuesPosXBase;
    float valuesPosYBase;
    getTextPosXYByIcon(posXBase, posYBase, scale, &valuesPosXBase, &valuesPosYBase);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = posXBase;
    float posY = posYBase;

    // Get the starting position for the values
    constexpr float lineAdjustment = 10.f * scale;
    constexpr float lineDecrement = (ICON_SIZE_FLOAT * scale) + lineAdjustment;

    constexpr float valuesPosXOffset = 525.f * scale;
    const float valuesPosX = valuesPosXBase + valuesPosXOffset;

    const Menu *menuPtr = gMenu;
    const uint32_t currentPage = menuPtr->getCurrentPage();
    const uint32_t currentIndex = menuPtr->getCurrentIndex();

    const uint32_t startingIndex = currentPage * BATTLES_STATUSES_MAX_STATUSES_PER_PAGE;
    const uint32_t endingIndex = startingIndex + BATTLES_STATUSES_MAX_STATUSES_PER_PAGE;

    const IconId *optionsIcons = gBattlesStatusesOptionsIcons;
    constexpr uint32_t totalOptions = sizeof(gBattlesStatusesOptions) / sizeof(gBattlesStatusesOptions[0]);

    // Draw the icons
    uint32_t counter = 0;
    for (uint32_t i = startingIndex; i < endingIndex; i++, counter++)
    {
        if (i >= totalOptions)
        {
            // Reached the end of the options
            break;
        }

        if (i == currentIndex)
        {
            // Draw a window for the current line
            float textWidth;
            getTextWidthHeight("Yes", scale, &textWidth, nullptr);

            const float padding = (rootWindowPtr->getPadding() * scale) * 2.f;

            const float windowPosX = rootWindowPtr->getPosX();
            const float windowPosY = valuesPosYBase + (lineAdjustment * 1.5f) - (counter * lineDecrement);
            const float windowWidth = valuesPosXOffset + textWidth + padding;
            constexpr float windowHeight = lineDecrement + LINE_HEIGHT_ADJUSTMENT_5(scale);
            const uint32_t windowColor = SPECIAL_WINDOW_COLOR | 0xFF;

            windowDispGX_Waku_col(0,
                                  reinterpret_cast<const uint8_t *>(&windowColor),
                                  windowPosX,
                                  windowPosY,
                                  windowWidth,
                                  windowHeight,
                                  0.f);
        }

        drawIcon(posX, posY, scale, optionsIcons[i]);
        posY -= lineDecrement;
    }

    // Initialize text drawing
    drawTextInit(false);

    // Draw the page number at the top-right of the main window
    char buf[16];

    // Draw the page as an int, to prevent long text if it somehow becomes negative
    snprintf(buf, sizeof(buf), "Page %" PRId32, currentPage + 1);

    float tempPosX;
    float tempPosY;
    rootWindowPtr->getTextPosXY(buf, WindowAlignment::TOP_RIGHT, scale, &tempPosX, &tempPosY);
    drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    posX = valuesPosXBase;
    posY = valuesPosYBase;

    // Draw the text
    const char **statusesOptionsPtr = gBattlesStatusesOptions;

    for (uint32_t i = startingIndex; i < endingIndex; i++)
    {
        if (i >= totalOptions)
        {
            // Reached the end of the options
            break;
        }

        // Draw the main text
        uint32_t color = getCurrentOptionColor(currentIndex == i, 0xFF);
        drawText(statusesOptionsPtr[i], posX, posY, scale, color);

        // Draw the current value
        int32_t value = *getActorStatusPtr(actorPtr, i);
        const char *valueText;

        if (i == StatsBattlesStatusesOptions::BATTLES_STATUSES_DEFEATED_FLAG)
        {
            getOnOffTextAndColor(value != 0, &valueText, &color, 0xFF);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%" PRId32, value);
            valueText = buf;
            color = getColorWhite(0xFF);
        }

        drawText(valueText, valuesPosX, posY, scale, color, true);
        posY -= lineDecrement;
    }
}

void battlesMenuStatusesDraw(CameraId cameraId, void *user)
{
    (void)cameraId;
    (void)user;

    // Draw the main window
    drawMainWindow();

    // If the pointer to the selected actor is invalid, then assume that the actor is no longer in the battle
    BattlesMenu *battlesMenuPtr = gBattlesMenu;
    BattleWorkUnit *actorPtr = getActorBattlePtr(battlesMenuPtr->getCurrentActorIndex());

    if (!actorPtr)
    {
        battlesMenuReturnToInitMenu();
        return;
    }

    // Draw the actor's statuses
    battlesMenuPtr->drawBattleActorStatuses(actorPtr);

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = battlesMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}
