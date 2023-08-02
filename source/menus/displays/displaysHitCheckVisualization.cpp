#include "menuUtils.h"
#include "drawText.h"
#include "displays.h"
#include "mod.h"
#include "menus/displaysMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuOption gDisplaysMenuHitCheckVisualizationOptions[] = {
    "Turn On/Off",
    displaysMenuHitCheckVisualizationMenuSelectTurnOnOff,

    "Draw Hits",
    displaysMenuHitCheckVisualizationToggleHitOrMissFlags,

    "Draw Misses",
    displaysMenuHitCheckVisualizationToggleHitOrMissFlags,

    "Hits Color",
    displaysMenuHitCheckVisualizationStartSelectingColor,

    "Misses Color",
    displaysMenuHitCheckVisualizationStartSelectingColor,
};

const MenuFunctions gDisplaysMenuHitCheckVisualizationFuncs = {
    gDisplaysMenuHitCheckVisualizationOptions,
    displaysMenuHitCheckVisualizationControls,
    displaysMenuHitCheckVisualizationDraw,
    nullptr, // Exit function not needed
};

void displaysMenuHitCheckVisualizationInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gDisplaysMenuHitCheckVisualizationOptions) / sizeof(MenuOption);
    enterNextMenu(&gDisplaysMenuHitCheckVisualizationFuncs, totalOptions);
}

void displaysMenuHitCheckVisualizationControls(Menu *menuPtr, MenuButtonInput button)
{
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;

    // If the confirmation window is open, then handle the controls for that
    if (menuPtr->flagIsSet(
            DisplaysMenuHitCheckVisualizationFlags::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_FLAG_CURRENTLY_SELECTING_YES_NO))
    {
        displaysMenuPtr->getConfirmationWindow()->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(
            DisplaysMenuHitCheckVisualizationFlags::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_FLAG_CURRENTLY_SELECTING_COLOR))
    {
        displaysMenuPtr->getValueEditor()->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void DisplaysMenu::drawHitCheckVisualizationInfo() const
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Position the text a bit to the right of the longest text
    float width;
    getTextWidthHeight("Misses Color", scale, &width, nullptr);

    const float posX = tempPosX + width + (20.f * scale);
    float posY = tempPosY;

    // Draw the on/off text for the enabled flag
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    Displays *displaysPtr = gDisplays;
    const char *string;
    uint32_t color;

    getOnOffTextAndColor(displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION),
                         &string,
                         &color,
                         0xFF);

    drawText(string, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the yes/no text for drawing the hits
    getYesNoTextAndColor(displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_SHOULD_DRAW_HITS),
                         &string,
                         &color,
                         0xFF);

    drawText(string, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the yes/no text for drawing the misses
    getYesNoTextAndColor(displaysPtr->miscFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_SHOULD_DRAW_MISSES),
                         &string,
                         &color,
                         0xFF);

    drawText(string, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the color for the hits
    HitCheckVisualizationDisplay *hitCheckVisualizationDisplayPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
    color = hitCheckVisualizationDisplayPtr->getHitsColor();

    char buf[16];
    constexpr uint32_t bufSize = sizeof(buf);

    snprintf(buf, bufSize, "0x%08" PRIX32, color);
    drawText(buf, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the color for the misses
    color = hitCheckVisualizationDisplayPtr->getMissesColor();
    snprintf(buf, bufSize, "0x%08" PRIX32, color);
    drawText(buf, posX, posY, scale, color);
}

void displaysMenuHitCheckVisualizationDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Hit Check Visualization display
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    displaysMenuPtr->drawHitCheckVisualizationInfo();

    // Draw the confirmation window if applicable
    ConfirmationWindow *confirmationWindowPtr = displaysMenuPtr->getConfirmationWindow();
    if (confirmationWindowPtr->shouldDraw())
    {
        confirmationWindowPtr->draw();
    }

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditor();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

void displaysMenuHitCheckVisualizationMenuTurnOn(bool selectedYes)
{
    if (selectedYes)
    {
        // The memory for the buffer will be allocated automatically when it is needed, so no need to allocate it here
        Displays *displaysPtr = gDisplays;
        displaysPtr->setEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION);

        // Reset the entry count to be safe
        displaysPtr->getHitCheckVisualizationDisplayPtr()->setEntryCount(0);
    }

    // Close the confirmation window
    gDisplaysMenu->getConfirmationWindow()->stopDrawing();

    gMenu->clearFlag(
        DisplaysMenuHitCheckVisualizationFlags::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_FLAG_CURRENTLY_SELECTING_YES_NO);
}

void displaysMenuHitCheckVisualizationMenuSelectTurnOnOff(Menu *menuPtr)
{
    // If the flag is currently set, then just disable it, reset the entry count, and free the memory used by the buffer
    Displays *displaysPtr = gDisplays;
    if (displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION))
    {
        displaysPtr->clearEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION);

        HitCheckVisualizationDisplay *hitCheckVisualizationDisplayPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
        hitCheckVisualizationDisplayPtr->setEntryCount(0);
        hitCheckVisualizationDisplayPtr->freeBuffer();
        return;
    }

    // Bring up the confirmation window
    menuPtr->setFlag(
        DisplaysMenuHitCheckVisualizationFlags::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_FLAG_CURRENTLY_SELECTING_YES_NO);

    // Initialize the confirmation window
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    ConfirmationWindow *confirmationWindowPtr = displaysMenuPtr->getConfirmationWindow();

    const char *helpText =
        "The Hit Check Visualization display currently\ncrashes on console, so it should only be used\non emulator for "
        "now.\n\nAre you sure you want to use this display?";

    const Window *rootWindowPtr = gRootWindow;

    confirmationWindowPtr->init(rootWindowPtr, helpText, rootWindowPtr->getAlpha());
    confirmationWindowPtr->startDrawing(displaysMenuHitCheckVisualizationMenuTurnOn);
}

void displaysMenuHitCheckVisualizationToggleHitOrMissFlags(Menu *menuPtr)
{
    uint32_t miscFlag;
    if (menuPtr->getCurrentIndex() == DisplaysMenuHitCheckVisualizationOptions::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_DRAW_HITS)
    {
        miscFlag = DisplaysMiscFlag::DISPLAYS_MISC_FLAG_SHOULD_DRAW_HITS;
    }
    else
    {
        miscFlag = DisplaysMiscFlag::DISPLAYS_MISC_FLAG_SHOULD_DRAW_MISSES;
    }

    Displays *displaysPtr = gDisplays;
    displaysPtr->toggleMiscFlag(miscFlag);

    // If both flags are now disabled, then free the memory used by the buffer

    // The memory for the buffer will be allocated automatically when it is needed, so just check for resetting the entry count
    // freeing the memory for the buffer when both flags are disabled
    if (!displaysPtr->enabledFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_SHOULD_DRAW_HITS) &&
        !displaysPtr->enabledFlagIsSet(DisplaysMiscFlag::DISPLAYS_MISC_FLAG_SHOULD_DRAW_MISSES))
    {
        HitCheckVisualizationDisplay *hitCheckVisualizationDisplayPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
        hitCheckVisualizationDisplayPtr->setEntryCount(0);
        hitCheckVisualizationDisplayPtr->freeBuffer();
    }
}

void displaysMenuHitCheckVisualizationCancelSelectColor()
{
    gDisplaysMenu->getValueEditor()->stopDrawing();

    gMenu->clearFlag(
        DisplaysMenuHitCheckVisualizationFlags::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_FLAG_CURRENTLY_SELECTING_COLOR);
}

void displaysMenuHitCheckVisualizationSelectColor(const ValueType *valuePtr)
{
    // Only set the color if the alpha is not 0
    // Do not close the value editor if the alpha is 0
    const uint32_t color = valuePtr->u32;
    if ((color & 0xFF) == 0)
    {
        return;
    }

    // Set the new color
    HitCheckVisualizationDisplay *hitCheckVisualizationDisplayPtr = gDisplays->getHitCheckVisualizationDisplayPtr();

    if (gMenu->getCurrentIndex() ==
        DisplaysMenuHitCheckVisualizationOptions::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_CHANGE_HITS_COLOR)
    {
        hitCheckVisualizationDisplayPtr->setHitsColor(color);
    }
    else
    {
        hitCheckVisualizationDisplayPtr->setMissesColor(color);
    }

    // Close the value editor
    displaysMenuHitCheckVisualizationCancelSelectColor();
}

void displaysMenuHitCheckVisualizationStartSelectingColor(Menu *menuPtr)
{
    // Bring up the window for selecting an id
    menuPtr->setFlag(
        DisplaysMenuHitCheckVisualizationFlags::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_FLAG_CURRENTLY_SELECTING_COLOR);

    // Initialize the value editor
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditor();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);

    const HitCheckVisualizationDisplay *hitCheckVisualizationDisplayPtr = gDisplays->getHitCheckVisualizationDisplayPtr();
    uint32_t currentColor;

    if (menuPtr->getCurrentIndex() ==
        DisplaysMenuHitCheckVisualizationOptions::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_CHANGE_HITS_COLOR)
    {
        currentColor = hitCheckVisualizationDisplayPtr->getHitsColor();
    }
    else
    {
        currentColor = hitCheckVisualizationDisplayPtr->getMissesColor();
    }

    const Window *rootWindowPtr = gRootWindow;
    valueEditorPtr->init(&currentColor, nullptr, nullptr, rootWindowPtr, flags, VariableType::u32, rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(displaysMenuHitCheckVisualizationSelectColor,
                                 displaysMenuHitCheckVisualizationCancelSelectColor);
}
