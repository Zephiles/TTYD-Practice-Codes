#include "drawText.h"
#include "patch.h"
#include "gc/types.h"
#include "gc/mtx.h"
#include "ttyd/fontmgr.h"
#include "misc/utils.h"

#include <cstdint>
#include <cstring>

float getTextMultilineIncrement(const char *text, float scale, uint32_t additionalLines)
{
    uint16_t numLines;
    FontGetMessageWidthLine(text, &numLines);

    const float lineCount = intToFloat(numLines + additionalLines);
    return lineCount * (LINE_HEIGHT_FLOAT * scale);
}

void getTextWidthHeight(const char *text, float scale, float *widthOut, float *heightOut)
{
    if (text)
    {
        uint16_t numLines;
        const int32_t textWidth = static_cast<int32_t>(FontGetMessageWidthLine(text, &numLines));

        if (widthOut)
        {
            *widthOut = intToFloat(textWidth) * scale;
        }

        if (heightOut)
        {
            const float height = intToFloat(LINE_HEIGHT * (numLines + 1) - 9) * scale;

            // Reduce the hight slighly so capital letters look properly positioned
            // It is not a mistake that this uses 5.f and getTextPosY uses 4.f
            *heightOut = height - LINE_HEIGHT_ADJUSTMENT_5(scale);
        }
    }
    else
    {
        if (widthOut)
        {
            *widthOut = 0.f;
        }

        if (heightOut)
        {
            *heightOut = 0.f;
        }
    }
}

void drawText(const char *text, float posX, float posY, float scale, uint32_t color)
{
    drawText(text, posX, posY, scale, 0.f, color, TextAlignment::LEFT);
}

void drawText(const char *text, float posX, float posY, float scale, uint32_t color, TextAlignment alignment)
{
    drawText(text, posX, posY, scale, 0.f, color, alignment);
}

void drawText(const char *text, float posX, float posY, float scale, float width, uint32_t color)
{
    drawText(text, posX, posY, scale, width, color, TextAlignment::LEFT);
}

// Credits to Jdaster64 for writing the original code for this function
void drawText(const char *text, float posX, float posY, float scale, float width, uint32_t color, TextAlignment alignment)
{
    char buf[128];
    constexpr int32_t maxLength = sizeof(buf) - 1;
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    // Draw each individual line
    while (1)
    {
        // Find the end of the current line
        const char *newline = strchr(text, '\n');

        // If a newline is not found, then currently at the last line
        if (!newline)
        {
            break;
        }

        // Copy this line to the temporary buffer and append a null byte
        int32_t lineLength = newline - text;

        // Make sure the current line won't be an empty string
        if (lineLength > 0)
        {
            // Prevent a buffer overflow
            if (lineLength > maxLength)
            {
                lineLength = maxLength;
            }

            char *tempBuf = strncpy(buf, text, lineLength);
            tempBuf[lineLength] = '\0';

            drawTextMain(tempBuf, posX, posY, scale, width, color, alignment);
        }

        // Advance to the next line
        text = newline + 1;
        posY -= lineDecrement;
    }

    // Draw the rest of the text
    drawTextMain(text, posX, posY, scale, width, color, alignment);
}

// Set width to 0 or less to not have a width limit
void drawTextMain(const char *text, float posX, float posY, float scale, float width, uint32_t color, TextAlignment alignment)
{
    // Make sure the text isn't an empty string
    if (text[0] == '\0')
    {
        return;
    }

    // Get the width of the text if it is needed
    const bool hasWidthLimit = width > 0.f;
    float textLengthScaled;

    if ((alignment == TextAlignment::CENTER) || (alignment == TextAlignment::RIGHT) || hasWidthLimit)
    {
        // Retrieve the text width as a separate variable to avoid repeatedly loading it from the stack when using it
        float textWidth;
        getTextWidthHeight(text, scale, &textWidth, nullptr);
        textLengthScaled = textWidth;
    }

    // Handle aligning the text
    switch (alignment)
    {
        case TextAlignment::CENTER:
        {
            // Make sure a width limit was provided, and that the text's width does not reach/exceed the width limit
            if (hasWidthLimit && (textLengthScaled < width))
            {
                posX += (width / 2.f) - (textLengthScaled / 2.f);
            }
            break;
        }
        case TextAlignment::RIGHT:
        {
            posX -= textLengthScaled;
            break;
        }
        default:
        {
            break;
        }
    }

    // Handle the width limit if one was provided
    float scaleX = scale;

    if (hasWidthLimit && (textLengthScaled > width))
    {
        scaleX = (width / textLengthScaled) * scale;

        // If aligning the text to the right, account for the new X scale
        if (alignment == TextAlignment::RIGHT)
        {
            posX += textLengthScaled - width;
        }
    }

    // Draw the text
    mtx34 mtxScale;
    PSMTXScale(mtxScale, scaleX, scale, scale);

    mtx34 mtxTrans;
    PSMTXTransApply(mtxScale, mtxTrans, posX, posY, 0.f);

    FontDrawColor(reinterpret_cast<uint8_t *>(&color));
    FontDrawMessageMtx(mtxTrans, text);
}
