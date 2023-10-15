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

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
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

    // Draw the on/off text for the enabled flag
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    Displays *displaysPtr = gDisplays;
    const char *string;
    uint32_t color;

    getOnOffTextAndColor(displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION),
                         &string,
                         &color,
                         0xFF);

    // Position the text a bit to the right of the longest text
    const float width = getTextWidth("Misses Color", scale);
    const float posX = tempPosX + width + (20.f * scale);
    float posY = tempPosY;

    drawText(string, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the yes/no text for drawing the hits
    getYesNoTextAndColor(displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS),
                         &string,
                         &color,
                         0xFF);

    drawText(string, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the yes/no text for drawing the misses
    getYesNoTextAndColor(displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES),
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

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

void displaysMenuHitCheckVisualizationMenuSelectTurnOnOff(Menu *menuPtr)
{
    (void)menuPtr;

    // Reset the entry count to be safe
    Displays *displaysPtr = gDisplays;
    HitCheckVisualizationDisplay *hitCheckVisualizationDisplayPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
    hitCheckVisualizationDisplayPtr->setEntryCount(0);

    // The memory for the buffer will be allocated automatically when it is needed, so only need to check if the flag is
    // disabled
    if (!displaysPtr->toggleEnabledFlag(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_HIT_CHECK_VISUALIZATION))
    {
        // The display is now disabled, so reset the entry count and free the memory used by the buffer
        hitCheckVisualizationDisplayPtr->freeBuffer();
    }
}

void displaysMenuHitCheckVisualizationToggleHitOrMissFlags(Menu *menuPtr)
{
    uint32_t enabledFlag;
    if (menuPtr->getCurrentIndex() == DisplaysMenuHitCheckVisualizationOptions::DISPLAYS_MENU_HIT_CHECK_VISUALIZATION_DRAW_HITS)
    {
        enabledFlag = DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS;
    }
    else
    {
        enabledFlag = DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES;
    }

    Displays *displaysPtr = gDisplays;
    displaysPtr->toggleEnabledFlag(enabledFlag);

    // If both flags are now disabled, then free the memory used by the buffer

    // The memory for the buffer will be allocated automatically when it is needed, so just check for resetting the entry count
    // and freeing the memory for the buffer when both flags are disabled
    if (!displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_HITS) &&
        !displaysPtr->enabledFlagIsSet(DisplaysEnabledFlag::DISPLAYS_ENABLED_FLAG_SHOULD_DRAW_MISSES))
    {
        HitCheckVisualizationDisplay *hitCheckVisualizationDisplayPtr = displaysPtr->getHitCheckVisualizationDisplayPtr();
        hitCheckVisualizationDisplayPtr->setEntryCount(0);
        hitCheckVisualizationDisplayPtr->freeBuffer();
    }
}

void displaysMenuHitCheckVisualizationCancelSelectColor()
{
    gDisplaysMenu->getValueEditorPtr()->stopDrawing();
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
    // Initialize the value editor
    DisplaysMenu *displaysMenuPtr = gDisplaysMenu;
    ValueEditor *valueEditorPtr = displaysMenuPtr->getValueEditorPtr();

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
