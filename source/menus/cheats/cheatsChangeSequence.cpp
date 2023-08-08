#include "menuUtils.h"
#include "drawText.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuOption gCheatsMenuChangeSequenceOptions[] = {
    "Change Value",
    cheatsMenuChangeSequenceStartChangingSequence,
};

const MenuFunctions gCheatsMenuChangeSequenceFuncs = {
    gCheatsMenuChangeSequenceOptions,
    cheatsMenuChangeSequenceControls,
    cheatsMenuChangeSequenceDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuChangeSequenceInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuChangeSequenceOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuChangeSequenceFuncs, totalOptions);
}

void cheatsMenuChangeSequenceControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CHEATS_MENU_USING_VALUE_EDITOR_FLAG))
    {
        gCheatsMenu->getValueEditorPtr()->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void CheatsMenu::drawSequenceInfo() const
{
    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;

    // Draw the text for showing what the current sequence position is
    char buf[32];
    constexpr uint32_t bufSize = sizeof(buf);

    const uint32_t sequencePosition = getSequencePosition() & 0xFFFF; // Trim to avoid possible long values
    snprintf(buf, bufSize, "Current Value: %" PRIu32, sequencePosition);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));

    // Draw the stage and event names for the current sequence position
    const char *names[2];

#ifdef TTYD_JP
    char stageNameBuffer[8];
    if (!getSequenceStageAndEvent(stageNameBuffer, sizeof(stageNameBuffer), sequencePosition, names))
#else
    if (!getSequenceStageAndEvent(sequencePosition, names))
#endif
    {
        return;
    }

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float posYBase = posY - (lineDecrement * 2.f);
    posY = posYBase;

    const char *stageEventText = "Stage\nEvent";
    drawText(stageEventText, posX, posY, scale, getColorWhite(0xFF));

    // Get the text position a bit to the right of the Stage and Event text
    float width;
    getTextWidthHeight(stageEventText, scale, &width, nullptr);

    posX += width + (40.f * scale);
    for (uint32_t i = 0; i < 2; i++)
    {
        drawText(names[i], posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;
    }
}

void cheatsMenuChangeSequenceDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the sequence position
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawSequenceInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

void cheatsMenuChangeSequenceChangeSequence(const ValueType *valuePtr)
{
    setSequencePosition(valuePtr->u32);

    // Close the value editor
    cheatsMenuValueEditorCancelSetValue();
}

void cheatsMenuChangeSequenceStartChangingSequence(Menu *menuPtr)
{
    // Initialize the value editor
    uint32_t currentValue = getSequencePosition();

    // Make sure currentValue is valid
    if (currentValue >= CHEATS_TOTAL_EVENT_NAMES)
    {
        currentValue = 0;
    }

    ValueEditor *valueEditorPtr = gCheatsMenu->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_STAGE_AND_EVENT);

    constexpr uint32_t minValue = 0;
    constexpr uint32_t maxValue = CHEATS_TOTAL_EVENT_NAMES - 1;

    cheatsMenuInitValueEditor(menuPtr,
                              currentValue,
                              minValue,
                              maxValue,
                              flags,
                              VariableType::u16,
                              true,
                              cheatsMenuChangeSequenceChangeSequence);
}
