#include "drawText.h"
#include "drawIcon.h"
#include "classes/window.h"
#include "misc/utils.h"
#include "ttyd/windowdrv.h"

#include <cstdint>
#include <cstring>

void Window::copyWindow(const Window *parent)
{
    memcpy(this, parent, sizeof(Window));
}

float Window::getTextPosX(const char *text, WindowAlignment alignment, float scale) const
{
    const float textWidth = getTextWidth(text, scale);

    const float windowPosX = this->posX;
    const float windowWidth = this->width;
    const float padding = this->padding * scale;

    switch (alignment)
    {
        case WindowAlignment::TOP_LEFT:
        case WindowAlignment::MIDDLE_LEFT:
        case WindowAlignment::BOTTOM_LEFT:
        {
            return windowPosX + padding;
        }
        case WindowAlignment::TOP_CENTER:
        case WindowAlignment::MIDDLE_CENTER:
        case WindowAlignment::BOTTOM_CENTER:
        {
            return windowPosX + (windowWidth / 2.f) - (textWidth / 2.f);
        }
        case WindowAlignment::TOP_RIGHT:
        case WindowAlignment::MIDDLE_RIGHT:
        case WindowAlignment::BOTTOM_RIGHT:
        {
            return windowPosX + windowWidth - textWidth - padding;
        }
        default:
        {
            return 0.f;
        }
    }
}

float Window::getTextPosY(const char *text, WindowAlignment alignment, float scale) const
{
    const float textHeight = getTextHeight(text, scale);

    const float windowPosY = this->posY;
    const float windowHeight = this->height;
    const float padding = this->padding * scale;

    // Move up slightly so capital letters look properly positioned
    // It is not a mistake that this uses 4.f and getTextHeight uses 5.f
    const float adjustment = LINE_HEIGHT_ADJUSTMENT_4(scale);

    switch (alignment)
    {
        case WindowAlignment::TOP_LEFT:
        case WindowAlignment::TOP_CENTER:
        case WindowAlignment::TOP_RIGHT:
        {
            return windowPosY - padding + adjustment;
        }
        case WindowAlignment::MIDDLE_LEFT:
        case WindowAlignment::MIDDLE_CENTER:
        case WindowAlignment::MIDDLE_RIGHT:
        {
            return windowPosY - (windowHeight / 2.f) + (textHeight / 2.f) + adjustment;
        }
        case WindowAlignment::BOTTOM_LEFT:
        case WindowAlignment::BOTTOM_CENTER:
        case WindowAlignment::BOTTOM_RIGHT:
        {
            return windowPosY - windowHeight + textHeight + padding + adjustment;
        }
        default:
        {
            return 0.f;
        }
    }
}

void Window::getTextPosXY(const char *text, WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const
{
    if (posXOut)
    {
        *posXOut = this->getTextPosX(text, alignment, scale);
    }

    if (posYOut)
    {
        *posYOut = this->getTextPosY(text, alignment, scale);
    }
}

void Window::getTextPosXYUnderMainText(const char *text,
                                       WindowAlignment alignment,
                                       uint32_t mainTextTotalOptions,
                                       uint32_t linesUnderMainText,
                                       float scale,
                                       float *posXOut,
                                       float *posYOut) const
{
    float tempPosX;
    float tempPosY;
    this->getTextPosXY(text, alignment, scale, &tempPosX, &tempPosY);

    if (posXOut)
    {
        *posXOut = tempPosX;
    }

    if (posYOut)
    {
        // Adjust linesUnderMainText to be zero-indexed
        if (linesUnderMainText > 0)
        {
            linesUnderMainText--;
        }

        const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
        *posYOut = tempPosY - (intToFloat(mainTextTotalOptions + linesUnderMainText) * lineDecrement);
    }
}

float Window::getIconPosX(IconId icon, WindowAlignment alignment, float scale) const
{
    float width;
    getIconWidthHeight(icon, &width, nullptr);

    return this->getIconPosX(alignment, width, scale);
}

float Window::getIconPosX(WindowAlignment alignment, float scale) const
{
    return this->getIconPosX(alignment, ICON_SIZE_FLOAT, scale);
}

float Window::getIconPosX(WindowAlignment alignment, float iconWidth, float scale) const
{
    const float windowPosX = this->posX;
    const float windowWidth = this->width;
    const float padding = this->padding * scale;

    // Icons are centered by default, so get the halfway point between the width of it
    const float adjustment = (iconWidth * scale) / 2.f;

    switch (alignment)
    {
        case WindowAlignment::TOP_LEFT:
        case WindowAlignment::MIDDLE_LEFT:
        case WindowAlignment::BOTTOM_LEFT:
        {
            return windowPosX + adjustment + padding;
        }
        case WindowAlignment::TOP_CENTER:
        case WindowAlignment::MIDDLE_CENTER:
        case WindowAlignment::BOTTOM_CENTER:
        {
            return windowPosX + (windowWidth / 2.f);
        }
        case WindowAlignment::TOP_RIGHT:
        case WindowAlignment::MIDDLE_RIGHT:
        case WindowAlignment::BOTTOM_RIGHT:
        {
            return windowPosX + windowWidth - adjustment - padding;
        }
        default:
        {
            return 0.f;
        }
    }
}

float Window::getIconPosY(IconId icon, WindowAlignment alignment, float scale) const
{
    float height;
    getIconWidthHeight(icon, nullptr, &height);

    return this->getIconPosY(alignment, height, scale);
}

float Window::getIconPosY(WindowAlignment alignment, float scale) const
{
    return this->getIconPosY(alignment, ICON_SIZE_FLOAT, scale);
}

float Window::getIconPosY(WindowAlignment alignment, float iconHeight, float scale) const
{
    const float windowPosY = this->posY;
    const float windowHeight = this->height;
    const float padding = this->padding * scale;

    // Icons are rotated on the Y axis by default, so it will need to be moved down by the height of it
    const float adjustment = iconHeight * scale;

    switch (alignment)
    {
        case WindowAlignment::TOP_LEFT:
        case WindowAlignment::TOP_CENTER:
        case WindowAlignment::TOP_RIGHT:
        {
            return windowPosY - adjustment - padding;
        }
        case WindowAlignment::MIDDLE_LEFT:
        case WindowAlignment::MIDDLE_CENTER:
        case WindowAlignment::MIDDLE_RIGHT:
        {
            return windowPosY - (windowHeight / 2.f) + (adjustment / 2.f) - adjustment;
        }
        case WindowAlignment::BOTTOM_LEFT:
        case WindowAlignment::BOTTOM_CENTER:
        case WindowAlignment::BOTTOM_RIGHT:
        {
            return windowPosY - windowHeight + padding;
        }
        default:
        {
            return 0.f;
        }
    }
}

void Window::getIconPosXY(IconId icon, WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const
{
    float width;
    float height;
    getIconWidthHeight(icon, &width, &height);

    this->getIconPosXY(alignment, width, height, scale, posXOut, posYOut);
}

void Window::getIconPosXY(WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const
{
    return this->getIconPosXY(alignment, ICON_SIZE_FLOAT, ICON_SIZE_FLOAT, scale, posXOut, posYOut);
}

void Window::getIconPosXY(WindowAlignment alignment,
                          float iconWidth,
                          float iconHeight,
                          float scale,
                          float *posXOut,
                          float *posYOut) const
{
    if (posXOut)
    {
        *posXOut = this->getIconPosX(alignment, iconWidth, scale);
    }

    if (posYOut)
    {
        *posYOut = this->getIconPosY(alignment, iconHeight, scale);
    }
}

void Window::setWidthHeightFromText(const char *text, float scale)
{
    this->setWidthHeightFromText(text, scale, 0.f, 0.f);
}

void Window::setWidthHeightFromText(const char *text, float scale, float offsetWidth, float offsetHeight)
{
    const float windowPadding = (this->padding * 2.f) * scale;

    const float newWidth = getTextWidth(text, scale);
    this->width = newWidth + windowPadding + offsetWidth;

    const float newHeight = getTextHeight(text, scale);
    this->height = newHeight + windowPadding + offsetHeight;
}

void Window::setWidthHeightFromTextAndInit(const char *text, float scale, uint32_t color, float cornerRadius, float padding)
{
    this->setWidthHeightFromTextAndInit(text, scale, color, cornerRadius, padding, 0.f, 0.f);
}

void Window::setWidthHeightFromTextAndInit(const char *text,
                                           float scale,
                                           uint32_t color,
                                           float cornerRadius,
                                           float padding,
                                           float offsetWidth,
                                           float offsetHeight)
{
    this->cornerRadius = cornerRadius;
    this->padding = padding;
    this->color = color;

    this->setWidthHeightFromText(text, scale, offsetWidth, offsetHeight);
}

void Window::placeInWindow(const Window *parent, WindowAlignment alignment, float scale)
{
    this->placeInWindow(parent, alignment, scale, 0.f, 0.f);
}

void Window::placeInWindow(const Window *parent, WindowAlignment alignment, float scale, float offsetX, float offsetY)
{
    const float parentPadding = parent->padding * scale;
    const float parentPosX = parent->posX;
    const float parentPosY = parent->posY;
    const float parentWidth = parent->width;
    const float parentHeight = parent->height;
    const float childWidth = this->width;
    const float childHeight = this->height;

    // Calculate the X coordinate
    switch (alignment)
    {
        case WindowAlignment::TOP_LEFT:
        case WindowAlignment::MIDDLE_LEFT:
        case WindowAlignment::BOTTOM_LEFT:
        {
            this->posX = (parentPosX + parentPadding) + offsetX;
            break;
        }
        case WindowAlignment::TOP_CENTER:
        case WindowAlignment::MIDDLE_CENTER:
        case WindowAlignment::BOTTOM_CENTER:
        {
            this->posX = (parentPosX + ((parentWidth - childWidth) / 2.f)) + offsetX;
            break;
        }
        case WindowAlignment::TOP_RIGHT:
        case WindowAlignment::MIDDLE_RIGHT:
        case WindowAlignment::BOTTOM_RIGHT:
        {
            this->posX = (parentPosX + ((parentWidth - childWidth) - parentPadding)) + offsetX;
            break;
        }
        default:
        {
            break;
        }
    }

    // Calculate the Y coordinate
    switch (alignment)
    {
        case WindowAlignment::TOP_LEFT:
        case WindowAlignment::TOP_CENTER:
        case WindowAlignment::TOP_RIGHT:
        {
            this->posY = (parentPosY - parentPadding) - offsetY;
            break;
        }
        case WindowAlignment::MIDDLE_LEFT:
        case WindowAlignment::MIDDLE_CENTER:
        case WindowAlignment::MIDDLE_RIGHT:
        {
            this->posY = (parentPosY - ((parentHeight - childHeight) / 2.f)) - offsetY;
            break;
        }
        case WindowAlignment::BOTTOM_LEFT:
        case WindowAlignment::BOTTOM_CENTER:
        case WindowAlignment::BOTTOM_RIGHT:
        {
            this->posY = (parentPosY - ((parentHeight - childHeight) - parentPadding)) - offsetY;
            break;
        }
        default:
        {
            break;
        }
    }
}

void Window::placeInWindowAndInit(const Window *parent,
                                  WindowAlignment alignment,
                                  uint32_t color,
                                  float scale,
                                  float width,
                                  float height,
                                  float cornerRadius,
                                  float padding)
{
    this->placeInWindowAndInit(parent, alignment, color, scale, width, height, cornerRadius, padding, 0.f, 0.f);
}

void Window::placeInWindowAndInit(const Window *parent,
                                  WindowAlignment alignment,
                                  uint32_t color,
                                  float scale,
                                  float width,
                                  float height,
                                  float cornerRadius,
                                  float padding,
                                  float offsetX,
                                  float offsetY)
{
    this->color = color;
    this->width = width;
    this->height = height;
    this->cornerRadius = cornerRadius;
    this->padding = padding;

    this->placeInWindow(parent, alignment, scale, offsetX, offsetY);
}

void Window::draw() const
{
    // If the alpha is 0, then don't draw the window, as it won't be visible
    if (this->getAlpha() == 0)
    {
        return;
    }

    windowDispGX_Waku_col(0,
                          reinterpret_cast<const uint8_t *>(&this->color),
                          this->posX,
                          this->posY,
                          this->width,
                          this->height,
                          this->cornerRadius);
}
