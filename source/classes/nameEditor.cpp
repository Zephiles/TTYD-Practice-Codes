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

const char *nameEditorHelpText =
    "Press Start to set the new text\nPress A to select a character\nPress B to undo/cancel\nPress Y to move backward one "
    "character\nPress X to move forward one character\nPress Z to cancel immediately";

const char *nameEditorCharacterOptions = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890/. ";

void NameEditor::init(const Window *parentWindow, float scale, const char *initialText, char *namePtr, uint32_t nameSize)
{
    this->init(parentWindow, scale, initialText, namePtr, nameSize, 0xFF);
}

void NameEditor::init(const Window *parentWindow,
                      float scale,
                      const char *initialText,
                      char *namePtr,
                      uint32_t nameSize,
                      uint8_t alpha)
{
    this->scale = scale;
    this->namePtr = namePtr;
    this->alpha = alpha;
    this->cancelFunc = nullptr;
    this->autoIncrement.waitFramesToBegin = 0;
    this->autoIncrement.framesBeforeIncrement = 0;
    this->enabled = false;
    this->currentStringIndex = 0;
    this->currentIndex = 0;

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
            uint32_t len = strlen(initialText);

            // Make sure len does not exceed the max index
            constexpr uint32_t maxIndex = bufferSize - 1;
            if (len > maxIndex)
            {
                len = maxIndex;
            }

            this->currentStringIndex = static_cast<uint8_t>(len);
            strncpy(this->buffer, initialText, len);

#ifdef TTYD_JP
        }
#endif
    }

    const uint32_t totalOptons = strlen(nameEditorCharacterOptions);
    this->totalOptions = static_cast<uint8_t>(totalOptons);

    const uint32_t charsPerRow = roundIntUpUnsigned(totalOptons, NAME_EDITOR_CHARS_TOTAL_ROWS);
    this->charsPerRow = static_cast<uint8_t>(charsPerRow);

    // Set up the window
    // Initialize it based on the help text
    Window *windowPtr = &this->window;
    windowPtr->setWidthHeightFromTextAndInit(nameEditorHelpText, scale, SPECIAL_WINDOW_COLOR | alpha, 20.f, 20.f);

    // Adjust the width to be the same as the parent window
    windowPtr->setWidth(parentWindow->getWidth());

    // Increase the height of the window to account for the current text, the character count, and the letter options
    const float padding = windowPtr->getPadding() * scale;
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

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

void NameEditor::controls(MenuButtonInput button, bool applyNullTerminator)
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

    uint32_t currentStringIndex = this->currentStringIndex;
    const uint32_t nameSize = this->nameSize;
    char *buf = this->buffer;

    // Subtract 2 from nameSize to account for the current index starting at 0 and the null terminator
    const uint32_t maxIndex = nameSize - 2;

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
                func();
            }
            break;
        }
        case MenuButtonInput::A:
        {
            // Append the selected character to the buffer
            buf[currentStringIndex] = nameEditorCharacterOptions[currentIndex];

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
                    func();
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

            // Subtract 1 from nameSize to account for the null terminator
            char *namePtr = strncpy(this->namePtr, buf, nameSize - 1);

            if (applyNullTerminator)
            {
                namePtr[nameSize - 1] = '\0';
            }

            // Close the name editor
            const NameEditorCancelFunc func = this->cancelFunc;
            if (func)
            {
                func();
            }
            break;
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

    // Draw a window for the current index of the buffer text
    // Set up a string to hold the current character being drawn
    char currentCharString[2];
    currentCharString[0] = 'A';
    currentCharString[1] = '\0';

    float tempPosX;
    float tempPosY;
    windowPtr->getTextPosXY(currentCharString, WindowAlignment::BOTTOM_LEFT, scale, &tempPosX, &tempPosY);

    const float currentStringIndexFloat = intToFloat(static_cast<int32_t>(this->currentStringIndex));
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float charactersPosYBase = tempPosY + (lineDecrement * (NAME_EDITOR_CHARS_TOTAL_ROWS_FLOAT - 1.f));

    const float currentStringPosX = tempPosX;
    const float currentStringPosY = charactersPosYBase + (lineDecrement * 2.f) + (windowPtr->getPadding() * scale);

    const uint32_t windowCursorColor = getColorBlue(0xFF);
    const float charPosXIncrement = (LINE_HEIGHT_FLOAT - 7.f) * scale;
    const float posXadjustment6 = 6.f * scale;

    windowDispGX_Waku_col(0,
                          reinterpret_cast<const uint8_t *>(&windowCursorColor),
                          currentStringPosX + (currentStringIndexFloat * charPosXIncrement) - posXadjustment6,
                          currentStringPosY,
                          charPosXIncrement,
                          charPosXIncrement,
                          0.f);

    // Initialize text drawing
    drawTextInit(false);

    // Draw the help text
    const float scale = this->scale;
    const char *helpTextPtr = nameEditorHelpText;

    windowPtr->getTextPosXY(helpTextPtr, WindowAlignment::TOP_CENTER, scale, &tempPosX, &tempPosY);
    drawText(helpTextPtr, tempPosX, tempPosY, scale, getColorWhite(0xFF));

    // Get the position for the characters to choose from
    const uint32_t charsPerRow = this->charsPerRow;
    const float spaceUsedPerRow = lineDecrement * intToFloat(static_cast<int32_t>(charsPerRow));
    const float posXBase = windowPtr->getPosX() + (windowPtr->getWidth() / 2.f) - (spaceUsedPerRow / 2.f) + posXadjustment6;

    float posX = posXBase;
    float posY = charactersPosYBase;
    const char *options = nameEditorCharacterOptions;
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
            const float windowCursorAdjustment = 1.5f * scale;
            const float windowPosX = posX - (7.f * scale);
            const float windowPosY = posY + windowCursorAdjustment;
            const float windowCursorWidthHeight = lineDecrement - windowCursorAdjustment;

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
        drawText(currentCharString, posX, posY, scale, getColorWhite(0xFF));
        posX += lineDecrement;
    }

    // Draw the current buffer text
    const char *buffer = this->buffer;
    const uint32_t currentStringLen = strlen(buffer);

    posX = currentStringPosX;
    posY = currentStringPosY;

    for (uint32_t i = 0; i < currentStringLen; i++)
    {
        char currentChar = buffer[i];
        if (currentChar == '\0')
        {
            break;
        }

        currentCharString[0] = currentChar;
        drawText(currentCharString, posX, posY, scale, getColorWhite(0xFF));
        posX += charPosXIncrement;
    }

    // Draw the current amount of characters in the buffer out of the maximum
    // Draw the counts as int32_ts, to prevent long text if they somehow become negative
    posX = currentStringPosX;
    posY -= lineDecrement;

    char *displayBufferPtr = gDisplayBuffer;
    snprintf(displayBufferPtr,
             DISPLAY_BUFFER_SIZE,
             "%" PRId32 "/%" PRId32,
             static_cast<int32_t>(currentStringLen),
             static_cast<int32_t>(this->nameSize - 1));

    drawText(displayBufferPtr, posX, posY, scale, getColorWhite(0xFF));
}
