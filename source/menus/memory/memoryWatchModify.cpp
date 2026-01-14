#include "menuUtils.h"
#include "memoryWatch.h"
#include "drawText.h"
#include "mod.h"
#include "classes/memoryWatchTypeSelector.h"
#include "menus/memoryMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);

static void selectedOptionToggleFlag(Menu *menuPtr);
static void selectedOptionChangeName(Menu *menuPtr);
static void selectedOptionChangeType(Menu *menuPtr);
static void selectedOptionChangePosition(Menu *menuPtr);
static void selectedOptionChangeScale(Menu *menuPtr);

static const MenuOption gOptions[] {
    "Name",
    selectedOptionChangeName,

    "Address",
    memoryMenuMemoryWatchChangeAddressInit,

    "Type",
    selectedOptionChangeType,

    "Show As Hex",
    selectedOptionToggleFlag,

    "Position",
    selectedOptionChangePosition,

    "Scale",
    selectedOptionChangeScale,

    "Display",
    selectedOptionToggleFlag,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void memoryMenuMemoryWatchModifyInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    MemoryMenu *memoryMenuPtr = gMemoryMenu;

    // If the name editor is open, then handle the controls for that
    NameEditor *nameEditorPtr = memoryMenuPtr->getNameEditorPtr();
    if (nameEditorPtr->shouldDraw())
    {
        nameEditorPtr->controls(button);
        return;
    }

    // If the type selector is open, then handle the controls for that
    MemoryWatchTypeSelector *typeSelectorPtr = memoryMenuPtr->getMemoryWatchTypeSelectorPtr();
    if (typeSelectorPtr->shouldDraw())
    {
        typeSelectorPtr->controls(button);
        return;
    }

    // If the position editor is open, then handle the controls for that
    PositionEditor *positionEditorPtr = memoryMenuPtr->getPositionEditorPtr();
    if (positionEditorPtr->shouldDraw())
    {
        positionEditorPtr->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = memoryMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void drawMemoryWatchModifyInfo()
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Get the pointer to the selected watch
    const MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();

    // Get the text position a bit to the right of the longest text
    const float width = getTextWidth("Show As Hex", scale);
    const float widthAdjustment = width + (40.f * scale);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posXBase = tempPosX;
    const float posX = posXBase + widthAdjustment;
    float posY = tempPosY;

    // Draw the name
    drawText(currentEntry->getNamePtrConst(), posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    // Draw the address
    const uint32_t totalAddressOffsets = currentEntry->getTotalAddressOffsets();
    uint8_t *baseAddressPtr = currentEntry->getAddressPtr();

    char buf[128];
    constexpr uint32_t bufSize = sizeof(buf);

    if (totalAddressOffsets > 0)
    {
        // At least one pointer level is being used, so display how many are being used, and draw the final address
        uint8_t *finalAddressPtr =
            getFinalAddressFromPointerPath(baseAddressPtr, currentEntry->getAddressOffsetsPtr(), totalAddressOffsets);

        // If the final address is invalid, then draw ??? in place of the address
        const char *addressStringPtr;

        if (finalAddressPtr)
        {
            char addressBuf[16];
            snprintf(addressBuf, sizeof(addressBuf), "0x%08" PRIX32, reinterpret_cast<uint32_t>(finalAddressPtr));
            addressStringPtr = addressBuf;
        }
        else
        {
            addressStringPtr = "???";
        }

        snprintf(buf, bufSize, "(%" PRIu32 "*) %s", totalAddressOffsets, addressStringPtr);
    }
    else
    {
        // No pointer levels are being used, so just draw the address
        snprintf(buf, bufSize, "0x%08" PRIX32, reinterpret_cast<uint32_t>(baseAddressPtr));
    }

    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the type
    // Make sure the type is valid
    const uint32_t typeIndex = static_cast<uint32_t>(currentEntry->getType());
    const char *typeString;

    if (typeIndex < TOTAL_MEMORY_WATCH_TYPES)
    {
        typeString = gMemoryWatchTypeStrings[typeIndex];
    }
    else
    {
        typeString = "Invalid Type";
    }

    drawText(typeString, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the yes/no text for showing as hex
    const char *tempText;
    uint32_t color;
    getYesNoTextAndColor(currentEntry->shouldShowAsHex(), &tempText, &color, 0xFF);

    drawText(tempText, posX, posY, scale, color);
    posY -= lineDecrement;

    // Draw the position
    snprintf(buf, bufSize, "%" PRId32 ", %" PRId32, floatToInt(currentEntry->getPosX()), floatToInt(currentEntry->getPosY()));
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the scale
    snprintf(buf, bufSize, "%.6f", currentEntry->getScale());
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Draw the on/off text for the watch being displayed
    getOnOffTextAndColor(currentEntry->shouldDisplay(), &tempText, &color, 0xFF);
    drawText(tempText, posX, posY, scale, color);

    // Draw the value two lines under the options
    posY -= (lineDecrement * 2.f);
    drawText("Value", posXBase, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    currentEntry->getValueString(buf, bufSize);
    drawText(buf, posXBase, posY, scale, getColorWhite(0xFF));
}

static void draw(CameraId cameraId, void *user)
{
    if (!gMod->flagIsSet(ModFlag::MOD_FLAG_MENU_IS_HIDDEN))
    {
        // Draw the main window and text
        basicMenuLayoutDraw(cameraId, user);

        // Draw the info for the Modify menu
        drawMemoryWatchModifyInfo();
    }

    // Draw the name editor if applicable
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    NameEditor *nameEditorPtr = memoryMenuPtr->getNameEditorPtr();
    if (nameEditorPtr->shouldDraw())
    {
        nameEditorPtr->draw();
    }

    // Draw the type selector if applicable
    MemoryWatchTypeSelector *typeSelectorPtr = memoryMenuPtr->getMemoryWatchTypeSelectorPtr();
    if (typeSelectorPtr->shouldDraw())
    {
        typeSelectorPtr->draw();
    }

    // Draw the position editor if applicable
    PositionEditor *positionEditorPtr = memoryMenuPtr->getPositionEditorPtr();
    if (positionEditorPtr->shouldDraw())
    {
        positionEditorPtr->draw();
    }

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = memoryMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

static void selectedOptionToggleFlag(Menu *menuPtr)
{
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();

    if (menuPtr->getCurrentIndex() == MemoryMenuMemoryWatchModifyOptions::MEMORY_MENU_MEMORY_WATCH_OPTION_HEX)
    {
        currentEntry->toggleShowAsHex();
    }
    else
    {
        currentEntry->toggleDisplay();
    }
}

static void cancelSetNewName()
{
    gMemoryMenu->getNameEditorPtr()->stopDrawing();
}

static void setNewName(char *newNamePtr, uint32_t newNameSize)
{
    (void)newNamePtr;
    (void)newNameSize;

    // Close the name editor
    cancelSetNewName();
}

static void selectedOptionChangeName(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the name editor
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    char *watchName = currentEntry->getNamePtr();
    const Window *rootWindowPtr = gRootWindow;

    NameEditor *nameEditorPtr = gMemoryMenu->getNameEditorPtr();
    nameEditorPtr->init(rootWindowPtr, watchName, watchName, MEMORY_WATCH_NAME_SIZE, true, rootWindowPtr->getAlpha());
    nameEditorPtr->startDrawing(setNewName, cancelSetNewName);
}

static void cancelSetNewType()
{
    gMemoryMenu->getMemoryWatchTypeSelectorPtr()->stopDrawing();
}

static void setNewType(VariableType type)
{
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    currentEntry->setType(type);

    // Close the type selector
    cancelSetNewType();
}

static void selectedOptionChangeType(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the type selector
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    MemoryWatchTypeSelector *typeSelectorPtr = memoryMenuPtr->getMemoryWatchTypeSelectorPtr();

    const Window *rootWindowPtr = gRootWindow;

    typeSelectorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());

    // Get the current index for the type selector
    const MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    const uint32_t typeIndex = static_cast<uint32_t>(currentEntry->getType());

    // Make sure the index is valid
    if (typeIndex < TOTAL_MEMORY_WATCH_TYPES)
    {
        typeSelectorPtr->setCurrentIndex(typeIndex);
    }

    typeSelectorPtr->startDrawing(setNewType, cancelSetNewType);
}

static void cancelChangePosition()
{
    gMemoryMenu->getPositionEditorPtr()->stopDrawing();
    gMod->clearFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);
}

static bool confirmChangePosition()
{
    // Close the position editor
    cancelChangePosition();

    return true;
}

static void selectedOptionChangePosition(Menu *menuPtr)
{
    (void)menuPtr;

    gMod->setFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);

    // Initialize the position editor
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();

    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    PositionEditor *positionEditorPtr = memoryMenuPtr->getPositionEditorPtr();
    const Window *rootWindowPtr = gRootWindow;

    positionEditorPtr->init(rootWindowPtr, currentEntry->getPosXPtr(), currentEntry->getPosYPtr(), rootWindowPtr->getAlpha());
    positionEditorPtr->startDrawing(confirmChangePosition, cancelChangePosition);
}

static void cancelSetNewScale()
{
    gMemoryMenu->getValueEditorPtr()->stopDrawing();
    gMod->clearFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);
}

static void setNewScale(const ValueType *valuePtr)
{
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    currentEntry->setScale(valuePtr->f32);

    // Close the value editor
    cancelSetNewScale();
}

static void selectedOptionChangeScale(Menu *menuPtr)
{
    (void)menuPtr;

    gMod->setFlag(ModFlag::MOD_FLAG_MENU_IS_HIDDEN);

    // Initialize the value editor
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();

    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    ValueEditor *valueEditorPtr = memoryMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::WINDOW_POSITION_TOP);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_X_HIDE);

    const float minValue = 0.3f;
    const float maxValue = 1.5f;
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(currentEntry->getScalePtr(),
                         &minValue,
                         &maxValue,
                         rootWindowPtr,
                         flags,
                         VariableType::f32,
                         rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(setNewScale, cancelSetNewScale);
}
