#include "drawText.h"
#include "menuUtils.h"
#include "classes/nameEditor.h"
#include "classes/window.h"
#include "misc/utils.h"
#include "ttyd/msgdrv.h"
#include "ttyd/windowdrv.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

const char *gNameEditorHelpText =
    "Press Start to set the new text\nPress A to select a character\nPress B to undo/cancel\nPress Y to move backward one "
    "character\nPress X to move forward one character\nPress Z to cancel immediately";

const char *gNameEditorCharacterOptions = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890/. ";

void NameEditor::init(const Window *parentWindow,
                      const char *initialText,
                      char *namePtr,
                      uint32_t nameSize,
                      bool applyNullTerminator)
{
    this->init(parentWindow, initialText, namePtr, nameSize, applyNullTerminator, 0xFF);
}

void NameEditor::init(const Window *parentWindow,
                      const char *initialText,
                      char *namePtr,
                      uint32_t nameSize,
                      bool applyNullTerminator,
                      uint8_t windowAlpha)
{
    this->namePtr = namePtr;
    this->setNameFunc = nullptr;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->enabled = false;
    this->currentStringIndex = 0;
    this->currentIndex = 0;

    this->applyNullTerminator = applyNullTerminator;

    // Make sure nameSize does not exceed the size of the buffer
    constexpr uint32_t bufferSize = sizeof(this->buffer);
    if (nameSize > bufferSize)
    {
        nameSize = bufferSize;
    }

    this->nameSize = static_cast<uint8_t>(nameSize);

    // Make sure the buffer has been cleared before updating it
    clearMemory(this->buffer, bufferSize);

    if (initialText)
    {
#ifdef TTYD_JP
        // Names don't currently support Japanese characters
        if (!_ismbblead(initialText[0]))
        {
            // Text does not start with a Japanese character
#endif
            // Get the length of the initial string
            uint32_t len = strlen(initialText);

            // Make sure there is at least one character in the string
            if (len > 0)
            {
                // Make sure the length does not exceed the max index of the buffer, as otherwise the buffer will not be null
                // terminated
                constexpr uint32_t maxBufferIndex = bufferSize - 1;
                if (len > maxBufferIndex)
                {
                    len = maxBufferIndex;
                }

                // Get the max index
                // Make sure the length does not reach or exceed the max index
                uint32_t maxIndex = nameSize;
                uint32_t maxLenIndex = len;

                if (applyNullTerminator)
                {
                    // Subtract 1 to account for the null terminator
                    maxLenIndex--;

                    if (nameSize >= 1)
                    {
                        // Subtract 1 to account for the null terminator
                        maxIndex--;
                    }
                }

                if (maxLenIndex > maxIndex)
                {
                    len = maxIndex + 1;
                }

                // Get the current index to use for currentStringIndex
                // Initialize it to the character after the last character in the string
                uint32_t currentIndex = len;
                if (currentIndex >= maxIndex)
                {
                    // Reached the end of the string, so set the current index to the last character
                    currentIndex--;
                }

                this->currentStringIndex = static_cast<uint8_t>(currentIndex);
                strncpy(this->buffer, initialText, len);
            }
#ifdef TTYD_JP
        }
#endif
    }

    const uint32_t totalOptons = strlen(gNameEditorCharacterOptions);
    this->totalOptions = static_cast<uint8_t>(totalOptons);

    const uint32_t charsPerRow = intCeil(totalOptons, NAME_EDITOR_CHARS_TOTAL_ROWS);
    this->charsPerRow = static_cast<uint8_t>(charsPerRow);

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    constexpr float scale = MENU_SCALE;
    windowPtr->setWidthHeightFromTextAndInit(gNameEditorHelpText, scale, SPECIAL_WINDOW_COLOR | windowAlpha, 20.f, 20.f);

    // Adjust the width to be the same as the parent window
    windowPtr->setWidth(parentWindow->getWidth());

    // Increase the height of the window to account for the current text, the character count, and the letter options
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float padding = windowPtr->getPadding() * scale;

    float height = windowPtr->getHeight();
    height += lineDecrement * 2.f;                                // Current text + character count
    height += lineDecrement * NAME_EDITOR_CHARS_TOTAL_ROWS_FLOAT; // Letter options
    height += padding * 2.f;
    windowPtr->setHeight(height);

    // Place the window inside of the parent window
    windowPtr->placeInWindow(parentWindow, WindowAlignment::MIDDLE_CENTER, scale);
}

void NameEditor::dpadControls(MenuButtonInput button)
{
    const uint32_t charsPerRow = this->charsPerRow;

    menuControlsHorizontal(button,
                           &this->currentIndex,
                           nullptr,
                           this->totalOptions,
                           charsPerRow * NAME_EDITOR_CHARS_TOTAL_ROWS,
                           charsPerRow,
                           true);
}

void NameEditor::controls(MenuButtonInput button)
{
    // Make sure the current index is valid
    uint32_t currentIndex = this->currentIndex;
    if (currentIndex > this->totalOptions)
    {
        // Failsafe: Reset the current index to 0
        this->currentIndex = 0;
        return;
    }

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    const bool autoIncrement = handleMenuAutoIncrement(&this->autoIncrement);

    // Handle held button inputs if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_LEFT:
            case MenuButtonInput::DPAD_RIGHT:
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                this->dpadControls(buttonHeld);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    const bool applyNullTerminator = this->applyNullTerminator;
    const uint32_t nameSize = this->nameSize;
    uint32_t maxIndex = 0;

    if (applyNullTerminator)
    {
        if (nameSize >= 2)
        {
            // Subtract 2 from nameSize to account for the current index starting at 0 and the null terminator
            maxIndex = nameSize - 2;
        }
    }
    else if (nameSize >= 1)
    {
        // Subtract 1 from nameSize to account for the current index starting at 0
        maxIndex = nameSize - 1;
    }

    uint32_t currentStringIndex = this->currentStringIndex;
    char *buf = this->buffer;

    // Handle the button inputs pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_LEFT:
        case MenuButtonInput::DPAD_RIGHT:
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            this->dpadControls(button);
            break;
        }
        case MenuButtonInput::Z:
        {
            // Cancel setting the new string
            const NameEditorCancelFunc func = this->cancelFunc;
            if (func)
            {
                return func();
            }
            break;
        }
        case MenuButtonInput::A:
        {
            // Append the selected character to the buffer
            buf[currentStringIndex] = gNameEditorCharacterOptions[currentIndex];

            // If not at the end of the buffer, increment the index
            if (currentStringIndex < maxIndex)
            {
                this->setCurrentStringIndex(currentStringIndex + 1);
            }
            break;
        }
        case MenuButtonInput::B:
        {
            if (currentStringIndex > 0)
            {
                // Move backward one character while clearing a specific char depending on circumstances
                // If at the end of the buffer, don't decrement the current index if buf[currentStringIndex] is not 0
                if (currentStringIndex < maxIndex)
                {
                    // Not at the end of the buffer
                    // If the char at the current index is not 0, then write a space instead of null
                    if (buf[currentStringIndex--] != '\0')
                    {
                        buf[currentStringIndex] = ' ';
                    }
                    else
                    {
                        buf[currentStringIndex] = '\0';
                    }
                }
                else
                {
                    // At the end of the buffer
                    if (buf[currentStringIndex] != '\0')
                    {
                        // The index was not incremented beforehand, so don't decrement it now
                        buf[currentStringIndex] = '\0';
                    }
                    else
                    {
                        // Current char is clear, so move back one and clear that char too
                        buf[--currentStringIndex] = '\0';
                    }
                }

                this->setCurrentStringIndex(currentStringIndex);
            }
            else if (buf[currentStringIndex] != '\0')
            {
                // Valid chars are after this, so set the current char to a space
                buf[currentStringIndex] = ' ';
            }
            else
            {
                // Cancel setting the new string
                const NameEditorCancelFunc func = this->cancelFunc;
                if (func)
                {
                    return func();
                }
            }
            break;
        }
        case MenuButtonInput::X:
        {
            // Move forward one character
            // If the current char is null, then set it to a space to allow more chars afterward
            if (buf[currentStringIndex] == '\0')
            {
                buf[currentStringIndex] = ' ';
            }

            // If not at the end of the buffer, increment the index
            if (currentStringIndex < maxIndex)
            {
                this->setCurrentStringIndex(currentStringIndex + 1);
            }
            break;
        }
        case MenuButtonInput::Y:
        {
            // Move backward one character
            // Only move backward if the current index is not at 0
            if (currentStringIndex > 0)
            {
                this->setCurrentStringIndex(currentStringIndex - 1);
            }
            break;
        }
        case MenuButtonInput::START:
        {
            // Don't set the new text if trying to set an empty string
            if (buf[0] == '\0')
            {
                break;
            }

            // Set the new name
            char *namePtr = strncpy(this->namePtr, buf, nameSize);
            if (applyNullTerminator)
            {
                namePtr[nameSize - 1] = '\0';
            }

            // Call the function for when the new name is set
            const NameEditorSetNameFunc func = this->setNameFunc;
            if (func)
            {
                func(buf, nameSize);
            }
            return;
        }
        default:
        {
            break;
        }
    }
}

void NameEditor::draw()
{
    // Draw the main window
    const Window *windowPtr = &this->window;
    windowPtr->draw();

    // Set up a string to hold the letter A
    char currentCharString[2];
    currentCharString[0] = 'A';
    currentCharString[1] = '\0';

    // Draw a window for the current index of the buffer text
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    windowPtr->getTextPosXY(currentCharString, WindowAlignment::BOTTOM_LEFT, scale, &tempPosX, &tempPosY);

    // Move the window back based on the size of the A character centered
    const float textWidth = getTextWidth(currentCharString, scale);
    constexpr float charPosXIncrement = (LINE_HEIGHT_FLOAT - 7.f) * scale;
    const float initialPosXAdjustment = (charPosXIncrement / 2.f) - (textWidth / 2.f);

    const float currentStringIndexFloat = intToFloat(static_cast<int32_t>(this->currentStringIndex));
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float charactersPosYBase = tempPosY + (lineDecrement * (NAME_EDITOR_CHARS_TOTAL_ROWS_FLOAT - 1.f));

    const float currentStringPosX = tempPosX;
    const float currentStringPosY = charactersPosYBase + (lineDecrement * 2.f) + (windowPtr->getPadding() * scale);
    const uint32_t windowCursorColor = getColorBlue(0xFF);

    windowDispGX_Waku_col(0,
                          reinterpret_cast<const uint8_t *>(&windowCursorColor),
                          currentStringPosX - initialPosXAdjustment + (currentStringIndexFloat * charPosXIncrement),
                          currentStringPosY,
                          charPosXIncrement,
                          charPosXIncrement,
                          0.f);

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    const char *helpTextPtr = gNameEditorHelpText;
    windowPtr->getTextPosXY(helpTextPtr, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(helpTextPtr, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Get the position for the characters to choose from
    const uint32_t charsPerRow = this->charsPerRow;
    const float spaceUsedPerRow = lineDecrement * intToFloat(static_cast<int32_t>(charsPerRow));
    const float posXBase = windowPtr->getPosX() + (windowPtr->getWidth() / 2.f) - (spaceUsedPerRow / 2.f);

    float posX = posXBase;
    float posY = charactersPosYBase;
    const char *options = gNameEditorCharacterOptions;
    uint32_t currentIndex = this->currentIndex;

    // Make sure the current index is valid
    const uint32_t totalOptions = this->totalOptions;
    if (currentIndex > totalOptions)
    {
        // Failsafe: Reset the current index to 0
        currentIndex = 0;
        this->setCurrentIndex(currentIndex);
    }

    // Draw the characters to choose from
    for (uint32_t i = 0, counter = 0; i <= totalOptions; i++, counter++)
    {
        if (counter >= charsPerRow)
        {
            counter = 0;

            // Move to the next row
            posX = posXBase;
            posY -= lineDecrement;
        }

        if (i == currentIndex)
        {
            // Draw the window for the cursor
            constexpr float windowCursorAdjustment = 1.5f * scale;
            constexpr float windowCursorWidthHeight = lineDecrement - windowCursorAdjustment;

            const float windowPosX = posX;
            const float windowPosY = posY + windowCursorAdjustment;

            windowDispGX_Waku_col(0,
                                  reinterpret_cast<const uint8_t *>(&windowCursorColor),
                                  windowPosX,
                                  windowPosY,
                                  windowCursorWidthHeight,
                                  windowCursorWidthHeight,
                                  0.f);

            // Re-initialize text drawing
            drawTextInit(false);
        }

        currentCharString[0] = options[i];
        drawText(currentCharString, posX, posY, scale, lineDecrement, getColorWhite(0xFF), TextAlignment::CENTER);
        posX += lineDecrement;
    }

    // Draw the current buffer text
    const char *buffer = this->buffer;
    const uint32_t len = strlen(buffer);

    posX = currentStringPosX - initialPosXAdjustment;
    posY = currentStringPosY;

    for (uint32_t i = 0; i < len; i++)
    {
        const char currentChar = buffer[i];
        if (currentChar == '\0')
        {
            break;
        }

        currentCharString[0] = currentChar;
        drawText(currentCharString, posX, posY, scale, charPosXIncrement, getColorWhite(0xFF), TextAlignment::CENTER);
        posX += charPosXIncrement;
    }

    // Draw the current amount of characters in the buffer out of the maximum
    // Draw the counts as ints, to prevent long text if they somehow become negative
    posX = currentStringPosX;
    posY -= lineDecrement;

    // Get the max index
    const uint32_t nameSize = this->nameSize;
    int32_t maxIndex = static_cast<int32_t>(nameSize);

    if (this->applyNullTerminator)
    {
        if (nameSize >= 1)
        {
            // Subtract 1 to account for the null terminator
            maxIndex--;
        }
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%" PRId32 "/%" PRId32, static_cast<int32_t>(len), maxIndex);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}
