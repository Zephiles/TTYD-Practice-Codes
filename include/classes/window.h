#ifndef CLASSES_WINDOW_H
#define CLASSES_WINDOW_H

#include "ttyd/icondrv.h"

#include <cstdint>

// Default color for the subwindows that classes are placed in, such as for the `Value Editor`'s window.
#define SPECIAL_WINDOW_COLOR 0x15151500

/**
 * Gets the default color for the subwindows that classes are placed in, such as for the `Value Editor`'s window.
 *
 * @param alpha The alpha to use for the color.
 *
 * @returns The default color for the subwindows that classes are placed in, `OR`ed with the `alpha` parameter.
 */
inline uint32_t getSpecialWindowColor(uint8_t alpha)
{
    return SPECIAL_WINDOW_COLOR | alpha;
}

// Alignment options for windows/text/icons being positioned inside of other windows.
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

// Handles drawing an arbitrary window at an arbitrary position with an arbitrary size. Also handles getting the coordinantes
// for drawing text and/or icons inside of it based on the `WindowAlignment` options.
class Window
{
   public:
    // Generic constructor.
    Window() {}

    // Generic deconstructor.
    ~Window() {}

    /**
     * Initializes the window.
     *
     * @param color The window's color, in `RGBA` format.
     * @param posX The starting `X` coordinate for the window.
     * @param posY The starting `Y` coordinate for the window.
     * @param width The width for the window.
     * @param height The height for the window.
     * @param cornerRadius How much of a curve will be applied to the corners of the window. A value of `0.f` will cause the
     * window to be a square/rectangle.
     * @param padding The amount of space between the edge(s) of the window and the window/text/icons placed in it. This value
     * will be multiplied by a specified scale for the functions that handle getting a position.
     *
     * @note Moving floats to/from the stack uses a lot of extra instructions, so it's best to inline this function.
     */
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

    // Gets the current `X` coordinate.
    float getPosX() const { return this->posX; }

    // Gets the current `Y` coordinate.
    float getPosY() const { return this->posY; }

    // Gets the current width.
    float getWidth() const { return this->width; }

    // Gets the current height.
    float getHeight() const { return this->height; }

    // Gets the current corner radius.
    float getCornerRadius() const { return this->cornerRadius; }

    // Gets the current padding.
    float getPadding() const { return this->padding; }

    // Gets the current color, in `RGBA` format.
    uint32_t getColor() const { return this->color; }

    // Gets the current color's alpha.
    uint8_t getAlpha() const { return this->color & 0xFF; }

    /**
     * Copies all of the data from a window to this one.
     *
     * @param parent The window to copy from.
     */
    void copyWindow(const Window *parent);

    /**
     * Gets the starting `X` coordinate of text that will be placed in the window.
     *
     * @param text The text that will be placed in the window. Can be `nullptr`, in which the current `X` coordinate specified
     * by `alignment` will be retrieved instead.
     * @param alignment How the text will be aligned in the window.
     * @param scale How big/small the text will be in the window.
     *
     * @returns The starting `X` coordinate of text that will be placed in the window. If the `text` parameter is `nullptr`,
     * then the current `X` coordinate specified by `alignment` will be returned instead.
     *
     * @note Only the `left`/`center`/`right` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `top`/`middle`/`bottom` parts is used.
     */
    float getTextPosX(const char *text, WindowAlignment alignment, float scale) const;

    /**
     * Gets the starting `Y` coordinate of text that will be placed in the window.
     *
     * @param text The text that will be placed in the window. Can be `nullptr`, in which the current `Y` coordinate specified
     * by `alignment` will be retrieved instead.
     * @param alignment How the text will be aligned in the window.
     * @param scale How big/small the text will be in the window.
     *
     * @returns The starting `Y` coordinate of text that will be placed in the window. If the `text` parameter is `nullptr`,
     * then the current `Y` coordinate specified by `alignment` will be returned instead.
     *
     * @note Only the `top`/`middle`/`bottom` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `left`/`center`/`right` parts is used.
     */
    float getTextPosY(const char *text, WindowAlignment alignment, float scale) const;

    /**
     * Gets the starting `X` and `Y` coordinates of text that will be placed in the window.
     *
     * @param[in] text The text that will be placed in the window. Can be `nullptr`, in which the current `X` and `Y`
     * coordinates specified by `alignment` will be retrieved instead.
     * @param[in] alignment How the text will be aligned in the window.
     * @param[in] scale How big/small the text will be in the window.
     * @param[out] posXOut The pointer to store the `X` coordinate at. Can be `nullptr` if the `X` coordinate is not needed.
     * @param[out] posYOut The pointer to store the `Y` coordinate at. Can be `nullptr` if the `Y` coordinate is not needed.
     */
    void getTextPosXY(const char *text, WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const;

    /**
     * Gets the desired line position under some main text that is assume to have been drawn already.
     *
     * @param[in] text The text that will be placed in the window. Can be `nullptr`, in which the current `X` and `Y`
     * coordinates specified by `alignment` will be retrieved instead.
     * @param[in] alignment How the text will be aligned in the window.
     * @param[in] mainTextTotalOptions The total amount of options that are contained in the main text.
     * @param[in] linesUnderMainText How many lines under the main text this new text should be placed (not zero-indexed).
     * @param[in] scale How big/small the text will be in the window.
     * @param[out] posXOut The pointer to store the `X` coordinate at. Can be `nullptr` if the `X` coordinate is not needed.
     * @param[out] posYOut The pointer to store the `Y` coordinate at. Can be `nullptr` if the `Y` coordinate is not needed.
     */
    void getTextPosXYUnderMainText(const char *text,
                                   WindowAlignment alignment,
                                   uint32_t mainTextTotalOptions,
                                   uint32_t linesUnderMainText,
                                   float scale,
                                   float *posXOut,
                                   float *posYOut) const;

    /**
     * Gets the starting `X` coordinate of an icon that will be placed in the window.
     *
     * @param icon The id of the icon that will be placed in the window.
     * @param alignment How the icon will be aligned in the window.
     * @param scale How big/small the icon will be in the window.
     *
     * @returns The starting `X` coordinate of the icon that will be placed in the window.
     *
     * @note Only the `left`/`center`/`right` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `top`/`middle`/`bottom` parts is used.
     */
    float getIconPosX(IconId icon, WindowAlignment alignment, float scale) const;

    /**
     * Gets the starting `X` coordinate of an icon that will be placed in the window.
     *
     * @param alignment How the icon will be aligned in the window.
     * @param iconWidth The width of the icon.
     * @param scale How big/small the icon will be in the window.
     *
     * @returns The starting `X` coordinate of the icon that will be placed in the window.
     *
     * @note Only the `left`/`center`/`right` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `top`/`middle`/`bottom` parts is used.
     *
     * @overload
     */
    float getIconPosX(WindowAlignment alignment, float iconWidth, float scale) const;

    /**
     * Gets the starting `X` coordinate of an icon that will be placed in the window.
     *
     * @param alignment How the icon will be aligned in the window.
     * @param scale How big/small the icon will be in the window.
     *
     * @returns The starting `X` coordinate of the icon that will be placed in the window.
     *
     * @note The width of the icon is assumed to be `ICON_SIZE_FLOAT` when this function is used.
     *
     * @note Only the `left`/`center`/`right` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `top`/`middle`/`bottom` parts is used.
     *
     * @overload
     */
    float getIconPosX(WindowAlignment alignment, float scale) const;

    /**
     * Gets the starting `Y` coordinate of an icon that will be placed in the window.
     *
     * @param icon The id of the icon that will be placed in the window.
     * @param alignment How the icon will be aligned in the window.
     * @param scale How big/small the icon will be in the window.
     *
     * @returns The starting `Y` coordinate of the icon that will be placed in the window.
     *
     * @note Only the `top`/`middle`/`bottom` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `left`/`center`/`right` parts is used.
     */
    float getIconPosY(IconId icon, WindowAlignment alignment, float scale) const;

    /**
     * Gets the starting `Y` coordinate of an icon that will be placed in the window.
     *
     * @param alignment How the icon will be aligned in the window.
     * @param iconWidth The width of the icon.
     * @param scale How big/small the icon will be in the window.
     *
     * @returns The starting `Y` coordinate of the icon that will be placed in the window.
     *
     * @note Only the `top`/`middle`/`bottom` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `left`/`center`/`right` parts is used.
     *
     * @overload
     */
    float getIconPosY(WindowAlignment alignment, float iconHeight, float scale) const;

    /**
     * Gets the starting `Y` coordinate of an icon that will be placed in the window.
     *
     * @param alignment How the icon will be aligned in the window.
     * @param scale How big/small the icon will be in the window.
     *
     * @returns The starting `Y` coordinate of the icon that will be placed in the window.
     *
     * @note The width of the icon is assumed to be `ICON_SIZE_FLOAT` when this function is used.
     *
     * @note Only the `left`/`center`/`right` parts of the `alignment` will be used for this function. As such, it does not
     * matter which of the `top`/`middle`/`bottom` parts is used.
     *
     * @overload
     */
    float getIconPosY(WindowAlignment alignment, float scale) const;

    /**
     * Gets the starting `X` and `Y` coordinates of an icon that will be placed in the window.
     *
     * @param[in] icon The id of the icon that will be placed in the window.
     * @param[in] alignment How the icon will be aligned in the window.
     * @param[in] scale How big/small the icon will be in the window.
     * @param[out] posXOut The pointer to store the `X` coordinate at. Can be `nullptr` if the `X` coordinate is not needed.
     * @param[out] posYOut The pointer to store the `Y` coordinate at. Can be `nullptr` if the `Y` coordinate is not needed.
     */
    void getIconPosXY(IconId icon, WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const;

    /**
     * Gets the starting `X` and `Y` coordinates of an icon that will be placed in the window.
     *
     * @param[in] alignment How the icon will be aligned in the window.
     * @param[in] scale How big/small the icon will be in the window.
     * @param[out] posXOut The pointer to store the `X` coordinate at. Can be `nullptr` if the `X` coordinate is not needed.
     * @param[out] posYOut The pointer to store the `Y` coordinate at. Can be `nullptr` if the `Y` coordinate is not needed.
     *
     * @note Both the width and the height of the icon are assumed to be `ICON_SIZE_FLOAT` when this function is used.
     *
     * @overload
     */
    void getIconPosXY(WindowAlignment alignment, float scale, float *posXOut, float *posYOut) const;

    /**
     * Gets the starting `X` and `Y` coordinates of an icon that will be placed in the window.
     *
     * @param[in] alignment How the icon will be aligned in the window.
     * @param[in] iconWidth The width of the icon.
     * @param[in] iconHeight The width of the icon.
     * @param[in] scale How big/small the icon will be in the window.
     * @param[out] posXOut The pointer to store the `X` coordinate at. Can be `nullptr` if the `X` coordinate is not needed.
     * @param[out] posYOut The pointer to store the `Y` coordinate at. Can be `nullptr` if the `Y` coordinate is not needed.
     *
     * @overload
     */
    void getIconPosXY(WindowAlignment alignment, float iconWidth, float iconHeight, float scale, float *posXOut, float *posYOut)
        const;

    /**
     * Updates the current value of the `posX` variable.
     *
     * @param posX The value to set the `posX` variable to.
     */
    void setPosX(float posX) { this->posX = posX; }

    /**
     * Updates the current value of the `posY` variable.
     *
     * @param posY The value to set the `posY` variable to.
     */
    void setPosY(float posY) { this->posY = posY; }

    /**
     * Updates the current value of the `width` variable.
     *
     * @param width The value to set the `width` variable to.
     */
    void setWidth(float width) { this->width = width; }

    /**
     * Updates the current value of the `height` variable.
     *
     * @param height The value to set the `height` variable to.
     */
    void setHeight(float height) { this->height = height; }

    /**
     * Updates the current value of the `cornerRadius` variable.
     *
     * @param cornerRadius The value to set the `cornerRadius` variable to.
     */
    void setCornerRadius(float cornerRadius) { this->cornerRadius = cornerRadius; }

    /**
     * Updates the current value of the `padding` variable.
     *
     * @param padding The value to set the `padding` variable to.
     */
    void setPadding(float padding) { this->padding = padding; }

    /**
     * Updates the current value of the `color` variable.
     *
     * @param color The value to set the `color` variable to.
     */
    void setColor(uint32_t color) { this->color = color; }

    /**
     * Updates the current value of the `color` variable's alpha.
     *
     * @param alpha The value to set the `color` variable's alpha to.
     */
    void setAlpha(uint8_t alpha) { this->color = (this->color & 0xFFFFFF00) | alpha; }

    /**
     * Sets the width and height of the window based on arbitrary text. This effectively places the window around the specified
     * text.
     *
     * @param text The text to use to calculate the width and height of the window.
     * @param scale How big/small the text will be in the window.
     *
     * @note This function calls the `setWidthHeightFromText` function that takes `offsetWidth` and `offsetHeight` as
     * parameters, in which `0.f` is passed in for both values.
     */
    void setWidthHeightFromText(const char *text, float scale);

    /**
     * Sets the width and height of the window based on arbitrary text. This effectively places the window around the specified
     * text.
     *
     * @param text The text to use to calculate the width and height of the window.
     * @param scale How big/small the text will be in the window.
     * @param offsetWidth Amount to adjust the calculated width by.
     * @param offsetHeight Amount to adjust the calculated height by.
     *
     * @overload
     */
    void setWidthHeightFromText(const char *text, float scale, float offsetWidth, float offsetHeight);

    /**
     * Initializes all variables of the window, and sets the width and height of the window based on arbitrary text. This
     * effectively places the window around the specified text.
     *
     * @param text The text to use to calculate the width and height of the window.
     * @param scale How big/small the text will be in the window.
     * @param color The value to set the window's `color` value to.
     * @param cornerRadius The value to set the window's `cornerRadius` value to.
     * @param padding The value to set the window's `padding` value to.
     *
     * @note This function calls the `setWidthHeightFromTextAndInit` function that takes `offsetWidth` and `offsetHeight` as
     * parameters, in which `0.f` is passed in for both values.
     */
    void setWidthHeightFromTextAndInit(const char *text, float scale, uint32_t color, float cornerRadius, float padding);

    /**
     * Initializes all variables of the window, and sets the width and height of the window based on arbitrary text. This
     * effectively places the window around the specified text.
     *
     * @param text The text to use to calculate the width and height of the window.
     * @param scale How big/small the text will be in the window.
     * @param color The value to set the window's `color` value to.
     * @param cornerRadius The value to set the window's `cornerRadius` value to.
     * @param padding The value to set the window's `padding` value to.
     * @param offsetWidth Amount to adjust the calculated width by.
     * @param offsetHeight Amount to adjust the calculated height by.
     *
     * @overload
     */
    void setWidthHeightFromTextAndInit(const char *text,
                                       float scale,
                                       uint32_t color,
                                       float cornerRadius,
                                       float padding,
                                       float offsetWidth,
                                       float offsetHeight);

    /**
     * Places this window inside of another window.
     *
     * @param parent The window to place this window in.
     * @param alignment How to align this window inside of the `parent` window.
     * @param scale How big/small this window will be in the `parent` window. This is only assumed, as only the `padding` will
     * be multipled by this value in this specific function.
     *
     * @note This function calls the `placeInWindow` function that takes `offsetX` and `offsetY` as parameters, in which `0.f`
     * is passed in for both values.
     */
    void placeInWindow(const Window *parent, WindowAlignment alignment, float scale);

    /**
     * Places this window inside of another window.
     *
     * @param parent The window to place this window in.
     * @param alignment How to align this window inside of the `parent` window.
     * @param scale How big/small this window will be in the `parent` window. This is only assumed, as only the `padding` will
     * be multipled by this value in this specific function.
     * @param offsetX Offset on the `X` axis to place this window in the `parent` window, based on the `left` of the `parent`
     * window.
     * @param offsetY Offset on the `Y` axis to place this window in the `parent` window, based on the `top` of the `parent`
     * window.
     *
     * @overload
     */
    void placeInWindow(const Window *parent, WindowAlignment alignment, float scale, float offsetX, float offsetY);

    /**
     * Initializes all variables of the window, places this window inside of another window.
     *
     * @param parent The window to place this window in.
     * @param alignment How to align this window inside of the `parent` window.
     * @param color The value to set the window's `color` value to.
     * @param scale How big/small the text will be in the window.
     * @param width The width for the window.
     * @param height The height for the window.
     * @param cornerRadius The value to set the window's `cornerRadius` value to.
     * @param padding The value to set the window's `padding` value to.
     *
     * @note This function calls the `placeInWindowAndInit` function that takes `offsetX` and `offsetY` as parameters, in which
     * `0.f` is passed in for both values.
     */
    void placeInWindowAndInit(const Window *parent,
                              WindowAlignment alignment,
                              uint32_t color,
                              float scale,
                              float width,
                              float height,
                              float cornerRadius,
                              float padding);

    /**
     * Initializes all variables of the window, places this window inside of another window.
     *
     * @param parent The window to place this window in.
     * @param alignment How to align this window inside of the `parent` window.
     * @param color The value to set the window's `color` value to.
     * @param scale How big/small the text will be in the window.
     * @param width The width for the window.
     * @param height The height for the window.
     * @param cornerRadius The value to set the window's `cornerRadius` value to.
     * @param padding The value to set the window's `padding` value to.
     * @param offsetX Offset on the `X` axis to place this window in the `parent` window, based on the `left` of the `parent`
     * window.
     * @param offsetY Offset on the `Y` axis to place this window in the `parent` window, based on the `top` of the `parent`
     * window.
     *
     * @overload
     */
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

    // Draws the window.
    void draw() const;

   private:
    float posX;         // Current `X` coordinate.
    float posY;         // Current `Y` coordinate.
    float width;        // Current width.
    float height;       // Current height.
    float cornerRadius; // Current corner radius.
    float padding;      // Padding from the edges of the window to where things can be placed.
    uint32_t color;     // Color of the window, in `RGBA` format.
};

#endif
