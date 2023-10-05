#ifndef CLASSES_WINDOW_H
#define CLASSES_WINDOW_H

#include "ttyd/icondrv.h"

#include <cstdint>

#define SPECIAL_WINDOW_COLOR 0x15151500

// Alignment options for windows/text/icons being positioned inside of other windows
enum class WindowAlignment
{
    TOP_LEFT = 0,
    TOP_CENTER,
    TOP_RIGHT,
    MIDDLE_LEFT,
    MIDDLE_CENTER,
    MIDDLE_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
};

class Window
{
   public:
    Window() {}
    ~Window() {}

    // Initialize separately, as moving floats to/from the stack uses a lot of extra instructions
    void init(uint32_t color, float posX, float posY, float width, float height, float cornerRadius, float padding)
    {
        this->color = color;
        this->posX = posX;
        this->posY = posY;
        this->width = width;
        this->height = height;
        this->cornerRadius = cornerRadius;
        this->padding = padding;
    }

    float getPosX() const { return this->posX; }
    float getPosY() const { return this->posY; }
    float getWidth() const { return this->width; }
    float getHeight() const { return this->height; }
    float getCornerRadius() const { return this->cornerRadius; }
    float getPadding() const { return this->padding; }
    uint32_t getColor() const { return this->color; }
    uint8_t getAlpha() const { return this->color & 0xFF; }

    void copyWindow(const Window *parent);

    // text can be set to nullptr to just get the X coordinate at the desired alignment
    float getTextPosX(const char *text, WindowAlignment alignment, float scale) const;

    // text can be set to nullptr to just get the Y coordinate at the desired alignment
    float getTextPosY(const char *text, WindowAlignment alignment, float scale) const;

    // text can be set to nullptr to just get the X and/or Y coordinates at the desired alignment
    // If either posXOut or posYOut are not needed, then they can be set to nullptr
    void getTextPosXY(const char *text, WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const;

    // This function is intended to get the desired line position under the main text in any given window, in which
    // linesUnderMainText is not zero-indexed. If either posXOut or posYOut are not needed, then they can be set to nullptr.
    void getTextPosXYUnderMainText(const char *text,
                                   WindowAlignment alignment,
                                   uint32_t mainTextTotalOptions,
                                   uint32_t linesUnderMainText,
                                   float scale,
                                   float *posXOut,
                                   float *posYOut) const;

    float getIconPosX(IconId icon, WindowAlignment alignment, float scale) const;
    float getIconPosX(WindowAlignment alignment, float iconWidth, float scale) const;

    // Sets iconWidth to ICON_SIZE_FLOAT
    float getIconPosX(WindowAlignment alignment, float scale) const;

    float getIconPosY(IconId icon, WindowAlignment alignment, float scale) const;

    // Sets iconHeight to ICON_SIZE_FLOAT
    float getIconPosY(WindowAlignment alignment, float scale) const;

    float getIconPosY(WindowAlignment alignment, float iconHeight, float scale) const;

    // If either posXOut or posYOut are not needed, then they can be set to nullptr
    void getIconPosXY(IconId icon, WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const;

    // // Sets iconWidth and iconHeight to ICON_SIZE_FLOAT
    void getIconPosXY(WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const;

    // If either posXOut or posYOut are not needed, then they can be set to nullptr
    void getIconPosXY(WindowAlignment alignment, float iconWidth, float iconHeight, float scale, float *posXOut, float *posYOut)
        const;

    void setPosX(float posX) { this->posX = posX; }
    void setPosY(float posY) { this->posY = posY; }
    void setWidth(float width) { this->width = width; }
    void setHeight(float height) { this->height = height; }
    void setCornerRadius(float cornerRadius) { this->cornerRadius = cornerRadius; }
    void setPadding(float padding) { this->padding = padding; }
    void setColor(uint32_t color) { this->color = color; }
    void setAlpha(uint8_t alpha) { this->color = (this->color & 0xFFFFFF00) | alpha; }

    // Calls setWidthHeightFromText with offsetWidth and offsetHeight set to 0.f
    void setWidthHeightFromText(const char *text, float scale);

    void setWidthHeightFromText(const char *text, float scale, float offsetWidth, float offsetHeight);

    // Calls setWidthHeightFromTextAndInit with offsetWidth and offsetHeight set to 0.f
    void setWidthHeightFromTextAndInit(const char *text, float scale, uint32_t color, float cornerRadius, float padding);

    void setWidthHeightFromTextAndInit(const char *text,
                                       float scale,
                                       uint32_t color,
                                       float cornerRadius,
                                       float padding,
                                       float offsetWidth,
                                       float offsetHeight);

    // Calls placeInWindow with offsetX and offsetY set to 0.f
    void placeInWindow(const Window *parent, WindowAlignment alignment, float scale);
    void placeInWindow(const Window *parent, WindowAlignment alignment, float scale, float offsetX, float offsetY);

    // Calls placeInWindowAndInit with offsetX and offsetY set to 0.f
    void placeInWindowAndInit(const Window *parent,
                              WindowAlignment alignment,
                              uint32_t color,
                              float scale,
                              float width,
                              float height,
                              float cornerRadius,
                              float padding);

    void placeInWindowAndInit(const Window *parent,
                              WindowAlignment alignment,
                              uint32_t color,
                              float scale,
                              float width,
                              float height,
                              float cornerRadius,
                              float padding,
                              float offsetX,
                              float offsetY);

    void draw() const;

   private:
    float posX;
    float posY;
    float width;
    float height;
    float cornerRadius;
    float padding;  // Padding from the edge of the window to where things can be placed
    uint32_t color; // RGBA
};

#endif
