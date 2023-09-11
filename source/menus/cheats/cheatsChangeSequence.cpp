#include "menuUtils.h"
#include "drawText.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"
#include "ttyd/event.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const MenuOption gCheatsMenuChangeSequenceOptions[] = {
    "Change Value",
    cheatsMenuChangeSequenceStartChangingSequence,
};

const MenuFunctions gCheatsMenuChangeSequenceFuncs = {
    gCheatsMenuChangeSequenceOptions,
    cheatsMenuDefaultControlsWithValueEditor,
    cheatsMenuChangeSequenceDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuChangeSequenceInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuChangeSequenceOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuChangeSequenceFuncs, totalOptions);
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
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (lineDecrement * 2.f);
    drawStageAndEvent(sequencePosition, posX, posY);
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
    (void)menuPtr;

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

    cheatsMenuInitValueEditor(currentValue,
                              minValue,
                              maxValue,
                              flags,
                              VariableType::u16,
                              true,
                              cheatsMenuChangeSequenceChangeSequence);
}

#ifdef TTYD_JP
bool getStageString(char *stageNameBuffer, uint32_t stageNameSize, uint32_t sequencePosition)
{
    // Make sure the sequence position is valid
    if (sequencePosition >= CHEATS_TOTAL_EVENT_NAMES)
    {
        return false;
    }

    if (sequencePosition <= 22)
    {
        // Use snprintf to make sure stageNameSize is not exceeded, and that a null terminator is properly applied
        snprintf(stageNameBuffer, stageNameSize, "Opening");
        return true;
    }

    if ((sequencePosition >= 403) && (sequencePosition <= 405))
    {
        // Use snprintf to make sure stageNameSize is not exceeded, and that a null terminator is properly applied
        snprintf(stageNameBuffer, stageNameSize, "Ending");
        return true;
    }

    uint32_t stageNumber;
    if ((sequencePosition >= 23) && (sequencePosition <= 70))
    {
        stageNumber = 1;
    }
    else if ((sequencePosition >= 71) && (sequencePosition <= 126))
    {
        stageNumber = 2;
    }
    else if ((sequencePosition >= 127) && (sequencePosition <= 177))
    {
        stageNumber = 3;
    }
    else if ((sequencePosition >= 178) && (sequencePosition <= 229))
    {
        stageNumber = 4;
    }
    else if ((sequencePosition >= 230) && (sequencePosition <= 281))
    {
        stageNumber = 5;
    }
    else if ((sequencePosition >= 282) && (sequencePosition <= 351))
    {
        stageNumber = 6;
    }
    else if ((sequencePosition >= 352) && (sequencePosition <= 381))
    {
        stageNumber = 7;
    }
    else // if ((sequencePosition >= 382) && (sequencePosition <= 402))
    {
        stageNumber = 8;
    }

    snprintf(stageNameBuffer, stageNameSize, "Stage %" PRIu32, stageNumber);
    return true;
}

bool getSequenceStageAndEvent(char *stageNameBuffer,
                              uint32_t stageNameSize,
                              uint32_t sequencePosition,
                              const char *stageEventOut[2])
#else
bool getSequenceStageAndEvent(uint32_t sequencePosition, const char *stageEventOut[2])
#endif
{
    const char *stageName;
    const char *eventName;

#ifdef TTYD_JP
    if (!getStageString(stageNameBuffer, stageNameSize, sequencePosition))
    {
        return false;
    }

    stageName = stageNameBuffer;
    eventName = gCheatsEventNames[sequencePosition];
#else
    const int32_t totalStages = eventStgNum();
    bool foundName = false;

    for (int32_t i = 0; i < totalStages; i++)
    {
        const EventStageDescription *stageDesc = eventStgDtPtr(i);
        const EventStageEventDescription *eventDesc = &stageDesc->pEvents[0];
        const int32_t eventCount = stageDesc->eventCount;

        for (int32_t j = 0; j < eventCount; j++, eventDesc++)
        {
            if (eventDesc->sequencePosition >= sequencePosition)
            {
                stageName = stageDesc->nameEn;
                eventName = eventDesc->nameEn;
                foundName = true;
                break;
            }
        }

        if (foundName)
        {
            break;
        }
    }

    if (!foundName)
    {
        return false;
    }
#endif

    stageEventOut[0] = stageName;
    stageEventOut[1] = eventName;
    return true;
}

void drawStageAndEvent(uint32_t sequencePosition, float posX, float posY)
{
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

    constexpr float scale = MENU_SCALE;
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const char *stageEventText = "Stage\nEvent";
    drawText(stageEventText, posX, posY, scale, getColorWhite(0xFF));

    // Get the text position a bit to the right of the Stage and Event text
    float width;
    getTextWidthHeight(stageEventText, scale, &width, nullptr);

    posX += width + (30.f * scale);
    for (uint32_t i = 0; i < 2; i++)
    {
        drawText(names[i], posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;
    }
}
