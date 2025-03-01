#include "menuUtils.h"
#include "drawText.h"
#include "menus/memoryMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);

static void selectedOptionChangeValues(Menu *menuPtr);
static void selectedOptionAddPointerLevel(Menu *menuPtr);
static void selectedOptionRemovePointerLevel(Menu *menuPtr);

static const char *gOffsetsLimitReached = "The maximum amount of levels has been reached.";
static const char *gNoOffsets = "There are currently no additional levels.";

static const MenuOption gOptions[] {
    "Change Address/Pointer Levels",
    selectedOptionChangeValues,

    "Add Pointer Level",
    selectedOptionAddPointerLevel,

    "Remove Pointer Level",
    selectedOptionRemovePointerLevel,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    nullptr, // Exit function not needed
};

void memoryMenuMemoryWatchChangeAddressInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

void MemoryMenu::memoryWatchChangeAddressFlagSetControls(MenuButtonInput button)
{
    const MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    const uint32_t totalOptions = currentEntry->getTotalAddressOffsets() + 1; // Add one since address is index 0

    // If there is only one option, then don't bother doing anything
    if (totalOptions <= 1)
    {
        return;
    }

    uint8_t *currentPagePtr = nullptr;
    if (totalOptions >= MAX_MEMORY_WATCH_OFFSETS_PER_PAGE)
    {
        currentPagePtr = &this->changeAddressCurrentPage;
    }

    menuControlsVertical(button,
                         &this->changeAddressCurrentIndex,
                         currentPagePtr,
                         totalOptions,
                         MAX_MEMORY_WATCH_OFFSETS_PER_PAGE,
                         1,
                         false);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
{
    MemoryMenu *memoryMenuPtr = gMemoryMenu;

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = memoryMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // If no flags are set, then use the default controls
    if (!menuPtr->anyFlagIsSet())
    {
        basicMenuLayoutControls(menuPtr, button);
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(memoryMenuPtr->getAutoIncrementPtr());

    // Handle held button input if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                memoryMenuPtr->memoryWatchChangeAddressFlagSetControls(buttonHeld);
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
            memoryMenuPtr->memoryWatchChangeAddressFlagSetControls(button);
            break;
        }
        case MenuButtonInput::A:
        {
            menuPtr->runSelectedOptionFunc();
            break;
        }
        case MenuButtonInput::B:
        {
            // If changing values and have selected the address/an offset to change, then go back to selecting the address/an
            // offset
            if (menuPtr->flagIsSet(
                    MemoryMenuMemoryWatchChangeAddressFlag::MEMORY_MENU_MEMORY_WATCH_CHANGE_ADDRESS_FLAG_CHANGE_VALUES))
            {
                menuPtr->clearFlag(
                    MemoryMenuMemoryWatchChangeAddressFlag::MEMORY_MENU_MEMORY_WATCH_CHANGE_ADDRESS_FLAG_CHANGE_VALUES);
            }
            else
            {
                menuPtr->clearAllFlags();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

static void drawMemoryWatchChangeAddressInfo()
{
    // Get the pointer to the selected watch
    const MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();

    // Draw the page number at the top-right of the main window if there is more than one page
    const MemoryMenu *memoryMenuPtr = gMemoryMenu;
    const uint32_t currentPage = memoryMenuPtr->getChangeAddressCurrentPage();
    const Window *rootWindowPtr = gRootWindow;
    constexpr float scale = MENU_SCALE;
    float tempPosX;
    float tempPosY;

    const uint32_t totalOffsets = currentEntry->getTotalAddressOffsets();

    char buf[128];
    const uint32_t bufSize = sizeof(buf);

    if (totalOffsets >= MAX_MEMORY_WATCH_OFFSETS_PER_PAGE)
    {
        // Draw the page as an int, to prevent long text if it somehow becomes negative
        snprintf(buf, bufSize, "Page %" PRId32, currentPage + 1);

        rootWindowPtr->getTextPosXY(buf, WindowAlignment::TOP_RIGHT, scale, &tempPosX, &tempPosY);
        drawText(buf, tempPosX, tempPosY, scale, getColorWhite(0xFF));
    }

    // Get the text position for the top-left of the window two lines under the main text
    const Menu *menuPtr = gMenu;
    const uint32_t totalOptions = menuPtr->getTotalOptions();
    rootWindowPtr->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;

    constexpr float posXOffsetPosition = 110.f;
    constexpr float posXAddressPosition = posXOffsetPosition + 140.f;

    // Draw the final value
    drawText("Final Value", posX, posY, scale, getColorWhite(0xFF));

    currentEntry->getValueString(buf, bufSize);
    drawText(buf, posX + posXOffsetPosition, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = MENU_LINE_HEIGHT * scale;
    posY -= (lineDecrement * 2.f);

    // Draw the pointer levels
    const bool changingValue =
        menuPtr->flagIsSet(MemoryMenuMemoryWatchChangeAddressFlag::MEMORY_MENU_MEMORY_WATCH_CHANGE_ADDRESS_FLAG_CHANGE_VALUES);

    const int32_t *offsetsPtr = currentEntry->getAddressOffsetsPtr();
    const uint32_t currentIndex = memoryMenuPtr->getChangeAddressCurrentIndex();
    uint8_t *baseAddress = currentEntry->getAddressPtr();

    const uint32_t startingIndex = MAX_MEMORY_WATCH_OFFSETS_PER_PAGE * currentPage;
    const uint32_t endingIndex = startingIndex + MAX_MEMORY_WATCH_OFFSETS_PER_PAGE;

    for (uint32_t i = startingIndex; i < endingIndex; i++)
    {
        if (i >= (totalOffsets + 1)) // Add one to account for the initial address
        {
            // Reached the end of the offsets, so exit
            break;
        }

        // Get the color for the current option
        const bool currentOptionCheck = changingValue && (currentIndex == i);
        const uint32_t color = getCurrentOptionColor(currentOptionCheck, 0xFF);

        if (i == 0)
        {
            // Draw the address text
            drawText("Address", posX, posY, scale, color);

            // Draw the initial address
            snprintf(buf, bufSize, "0x%08" PRIX32, reinterpret_cast<uint32_t>(baseAddress));
            drawText(buf, posX + posXAddressPosition, posY, scale, getColorWhite(0xFF));

            // Draw the offset count to the right of the address
            snprintf(buf,
                     bufSize,
                     "%" PRId32 "/%" PRId32,
                     static_cast<int32_t>(totalOffsets),
                     MAX_MEMORY_WATCH_ADDRESS_OFFSETS);

            drawText(buf, posX + posXOffsetPosition, posY, scale, getColorWhite(0xFF));
        }
        else
        {
            // Get the text for the current level
            snprintf(buf, bufSize, "Level %" PRIu32, i);

            // Draw the current level
            drawText(buf, posX, posY, scale, color);

            // Get the offset being applied
            // Check if the value is negative
            int32_t offset = offsetsPtr[i - 1]; // Subtract one since address is index 0
            const char *format = "-0x%" PRIX32;
            const char *formatPtr;

            if (offset < 0)
            {
                // Convert the value to negative
                offset = -offset;

                formatPtr = format;
            }
            else
            {
                // Skip the negative sign
                formatPtr = format + 1;
            }

            // Draw the offset
            snprintf(buf, bufSize, formatPtr, offset);
            drawText(buf, posX + posXOffsetPosition, posY, scale, getColorWhite(0xFF));

            // Get the address pointed to by the current level
            const uint8_t *currentAddress = getFinalAddressFromPointerPath(baseAddress, offsetsPtr, i);

            // Make sure currentAddress is valid
            if (currentAddress)
            {
                snprintf(buf, bufSize, "0x%08" PRIX32, reinterpret_cast<uint32_t>(currentAddress));
            }
            else
            {
                strcpy(buf, "???");
            }

            // Draw the address
            drawText(buf, posX + posXAddressPosition, posY, scale, getColorWhite(0xFF));
        }

        posY -= lineDecrement;
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDrawMenuLineHeight(cameraId, user);

    // Draw the info for the memory watch's address and pointer levels
    drawMemoryWatchChangeAddressInfo();

    // Draw the value editor if applicable
    MemoryMenu *memoryMenuPtr = gMemoryMenu;
    ValueEditor *valueEditorPtr = memoryMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = memoryMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

static void cancelSetNewOffset()
{
    gMemoryMenu->getValueEditorPtr()->stopDrawing();
}

static void setNewOffset(const ValueType *valuePtr)
{
    const uint32_t currentIndex = gMemoryMenu->getChangeAddressCurrentIndex();
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();

    if (currentIndex == 0)
    {
        // Changing address
        currentEntry->setAddress(reinterpret_cast<uint8_t *>(valuePtr->u32));
    }
    else
    {
        // Changing pointer level
        currentEntry->getAddressOffsetsPtr()[currentIndex - 1] = valuePtr->s32; // Subtract one since address is index 0
    }

    // Close the value editor
    cancelSetNewOffset();
}

static void selectedOptionChangeValues(Menu *menuPtr)
{
    MemoryMenu *memoryMenuPtr = gMemoryMenu;

    // If the Change Address/Pointer Levels option was just selected, then allow the player to choose a watch to modify
    if (!menuPtr->flagIsSet(MemoryMenuMemoryWatchChangeAddressFlag::MEMORY_MENU_MEMORY_WATCH_CHANGE_ADDRESS_FLAG_CHANGE_VALUES))
    {
        menuPtr->setFlag(MemoryMenuMemoryWatchChangeAddressFlag::MEMORY_MENU_MEMORY_WATCH_CHANGE_ADDRESS_FLAG_CHANGE_VALUES);

        // Reset the current index to the top of the current page
        memoryMenuPtr->setChangeAddressCurrentIndex(memoryMenuPtr->getChangeAddressCurrentPage() *
                                                    MAX_MEMORY_WATCH_OFFSETS_PER_PAGE);
        return;
    }

    // Make sure the current index is valid
    const uint32_t currentIndex = memoryMenuPtr->getChangeAddressCurrentIndex();
    if (currentIndex > MAX_MEMORY_WATCH_ADDRESS_OFFSETS)
    {
        menuPtr->clearFlag(MemoryMenuMemoryWatchChangeAddressFlag::MEMORY_MENU_MEMORY_WATCH_CHANGE_ADDRESS_FLAG_CHANGE_VALUES);
        return;
    }

    // Initialize the value editor
    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    ValueEditor *valueEditorPtr = memoryMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);

    uint32_t currentValue;
    VariableType type;

    if (currentIndex == 0)
    {
        // Changing address
        currentValue = reinterpret_cast<uint32_t>(currentEntry->getAddressPtr());
        type = VariableType::u32;
    }
    else
    {
        // Changing pointer level
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::VALUE_IS_SIGNED);

        const int32_t offset = currentEntry->getAddressOffset(currentIndex - 1); // Subtract one since address is index 0
        currentValue = static_cast<uint32_t>(offset);
        type = VariableType::s32;
    }

    const Window *rootWindowPtr = gRootWindow;
    valueEditorPtr->init(&currentValue, nullptr, nullptr, rootWindowPtr, flags, type, rootWindowPtr->getAlpha());
    valueEditorPtr->startDrawing(setNewOffset, cancelSetNewOffset);
}

static void selectedOptionAddPointerLevel(Menu *menuPtr)
{
    (void)menuPtr;

    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    if (!currentEntry->addAddressOffset())
    {
        // The maximum amount of watches have been added, so show an error message
        gMemoryMenu->initErrorWindow(gOffsetsLimitReached);
    }
}

static void selectedOptionRemovePointerLevel(Menu *menuPtr)
{
    (void)menuPtr;

    MemoryWatchEntry *currentEntry = getSelectedMemoryWatchEntryPtr();
    if (!currentEntry->removeAddressOffset())
    {
        // There are currently no offsets, so show an error message
        gMemoryMenu->initErrorWindow(gNoOffsets);
    }
}
