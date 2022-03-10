#include "draw.h"
#include "global.h"
#include "commonfunctions.h"
#include "menufunctions.h"
#include "memory.h"
#include "mod.h"
#include "assembly.h"
#include "main.h"

#include <gc/OSTime.h>
#include <gc/OSError.h>
#include <gc/card.h>
#include <gc/gx.h>
#include <gc/ppc.h>
#include <gc/OSContext.h>
#include <gc/OSFont.h>
#include <gc/DEMOPad.h>
#include <gc/DEMOInit.h>
#include <gc/mtx.h>
#include <gc/vi.h>
#include <ttyd/dispdrv.h>
#include <ttyd/windowdrv.h>
#include <ttyd/icondrv.h>
#include <ttyd/item_data.h>
#include <ttyd/fontmgr.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/evt_yuugijou.h>
#include <ttyd/party.h>
#include <ttyd/system.h>
#include <ttyd/event.h>
#include <ttyd/fadedrv.h>
#include <ttyd/seq_mapchange.h>
#include <ttyd/mario.h>
#include <ttyd/statuswindow.h>
#include <ttyd/win_main.h>
#include <ttyd/npcdrv.h>
#include <ttyd/itemdrv.h>
#include <ttyd/battle_ac.h>
#include <ttyd/effdrv.h>
#include <ttyd/evtmgr.h>
#include <ttyd/battle_unit.h>
#include <ttyd/battle_disp.h>
#include <ttyd/pmario_sound.h>
#include <ttyd/memory.h>

#include <cstring>
#include <cstdio>
#include <cinttypes>

namespace mod {

bool disableDPadOptionsDisplay(uint16_t unkVar)
{
    if (!Displays[DPAD_OPTIONS_DISPLAY])
    {
        return false;
    }
    
    return (unkVar & (1 << 8)); // Check the 8 bit
}

void drawFunctionOnDebugLayer(void (*func)())
{
    ttyd::dispdrv::dispEntry(ttyd::dispdrv::CameraId::kDebug3d, 2, 0.f, 
        [](ttyd::dispdrv::CameraId cameraId, void *user)
    {
        (void)cameraId;
        reinterpret_cast<void (*)()>(user)();
    }, reinterpret_cast<void *>(func));
}

void drawFunctionOnDebugLayerWithOrder(void (*func)(), float order)
{
    ttyd::dispdrv::dispEntry(ttyd::dispdrv::CameraId::kDebug3d, 2, order, 
        [](ttyd::dispdrv::CameraId cameraId, void *user)
    {
        (void)cameraId;
        reinterpret_cast<void (*)()>(user)();
    }, reinterpret_cast<void *>(func));
}

void drawFunctionOn2DLayer(void (*func)())
{
    ttyd::dispdrv::dispEntry(ttyd::dispdrv::CameraId::k2d, 2, 0.f, 
        [](ttyd::dispdrv::CameraId cameraId, void *user)
    {
        (void)cameraId;
        reinterpret_cast<void (*)()>(user)();
    }, reinterpret_cast<void *>(func));
}

void drawFunctionOn2DLayerWithOrder(void (*func)(), float order)
{
    ttyd::dispdrv::dispEntry(ttyd::dispdrv::CameraId::k2d, 2, order, 
        [](ttyd::dispdrv::CameraId cameraId, void *user)
    {
        (void)cameraId;
        reinterpret_cast<void (*)()>(user)();
    }, reinterpret_cast<void *>(func));
}

void drawMenuWindow()
{
    uint32_t color    = 0x000000F4;
    // uint32_t color = 0x444444F4;
    int32_t x         = -245;
    int32_t y         = 190;
    int32_t width     = 490;
    int32_t height    = 375;
    int32_t curve     = 0;
    
    drawWindow(color, x, y, width, height, curve);
}

void drawWindow(uint32_t color, int32_t x, int32_t y, int32_t width, int32_t height, int32_t curve)
{
    uint8_t *NewColor = reinterpret_cast<uint8_t *>(&color);
    float NewX        = static_cast<float>(x);
    float NewY        = static_cast<float>(y);
    float NewWidth    = static_cast<float>(width);
    float NewHeight   = static_cast<float>(height);
    float NewCurve    = static_cast<float>(curve);
    
    ttyd::windowdrv::windowDispGX_Waku_col(0, NewColor, NewX, NewY, NewWidth, NewHeight, NewCurve);
}

int32_t *drawIcon(int32_t position[3], int16_t iconNum, float scale)
{
    float NewPosition[3];
    int32_t ArraySize = static_cast<int32_t>(sizeof(NewPosition) / sizeof(float));
    
    for (int32_t i = 0; i < ArraySize; i++)
    {
        NewPosition[i] = static_cast<float>(position[i]);
    }
    
    ttyd::icondrv::iconDispGx(NewPosition, 24, iconNum, scale);
    return position;
}

int32_t *drawIconFromItem(int32_t position[3], int16_t itemNum, float scale)
{
#ifdef TTYD_JP
    const int16_t DiamondStarIconValue  = 407;
    const int16_t EmeraldStarIconValue  = 408;
    const int16_t GarnetStarIconValue   = 409;
    const int16_t GoldStarIconValue     = 410;
    const int16_t CrystalStarIconValue  = 411;
    const int16_t RubyStarIconValue     = 412;
    const int16_t SapphireStarIconValue = 413;
#else
    const int16_t DiamondStarIconValue  = 415;
    const int16_t EmeraldStarIconValue  = 416;
    const int16_t GarnetStarIconValue   = 417;
    const int16_t GoldStarIconValue     = 418;
    const int16_t CrystalStarIconValue  = 419;
    const int16_t RubyStarIconValue     = 420;
    const int16_t SapphireStarIconValue = 421;
#endif
    
    ttyd::item_data::ItemData *ItemDb = ttyd::item_data::itemDataTable;
    int16_t iconNum;
    
    // Get different icons for the crystal stars
    switch (itemNum)
    {
        case ttyd::item_data::Item::WeddingRing:
        {
            // Use the larger icon; the smaller one is used by default
            iconNum = ItemDb[itemNum].iconId - 1;
            break;
        }
        case ttyd::item_data::Item::DiamondStar:
        {
            iconNum = DiamondStarIconValue;
            break;
        }
        case ttyd::item_data::Item::EmeraldStar:
        {
            iconNum = EmeraldStarIconValue;
            break;
        }
        case ttyd::item_data::Item::GoldStar:
        {
            iconNum = GoldStarIconValue;
            break;
        }
        case ttyd::item_data::Item::RubyStar:
        {
            iconNum = RubyStarIconValue;
            break;
        }
        case ttyd::item_data::Item::SapphireStar:
        {
            iconNum = SapphireStarIconValue;
            break;
        }
        case ttyd::item_data::Item::GarnetStar:
        {
            iconNum = GarnetStarIconValue;
            break;
        }
        case ttyd::item_data::Item::CrystalStar:
        {
            iconNum = CrystalStarIconValue;
            break;
        }
        default:
        {
            iconNum = ItemDb[itemNum].iconId;
            break;
        }
    }
    
    return drawIcon(position, iconNum, scale);
}

// Replace the first \n found with \0, and return the index of the character after that
uint32_t getNextLineIndex(char *str)
{
    // Set the initial index
    uint32_t i = 0;
    
    // Loop through the string until \n or \0 is found
    while ((str[i] != '\n') && (str[i] != '\0'))
    {
        i++;
    }
    
    // Replace \n with \0 and increment the index to the next line
    if (str[i] != '\0')
    {
        str[i] = '\0';
        i++;
    }
    
    // Return the index of the next line
    // Returns 0 when at the end of the string
    return i;
}

void drawStringMultiline(float x, float y, const char *text)
{
    // Copy the text to a temporary array, as it will be modified
    uint32_t textSize = strlen(text);
    char tempText[textSize + 1];
    strcpy(tempText, text);
    
    // Get the index for the next line
    uint32_t index = getNextLineIndex(tempText);
    
    // Draw the first line
    char *currentLine = tempText;
    
    do
    {
        // Only draw the line if not pointing to an empty string
        // This will only occur if multiple newlines were directly next to each other
        if (currentLine[0] != '\0')
        {
            ttyd::fontmgr::FontDrawString(x, y, currentLine);
        }
        
        // Set currentLine to the next line
        currentLine += index;
        
        // Get the following line index
        index = getNextLineIndex(currentLine);
        
        // Implement the new line space
        y -= 20.f;
    }
    while (index != 0);
}

void drawText(const char *text, int32_t x, int32_t y, uint32_t color, float scale)
{
    ttyd::fontmgr::FontDrawScale(scale);
    ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
    
    uint32_t i = 0;
    char EndOfLineChar;
    bool LoopDone = false;
    
    while (!LoopDone)
    {
        EndOfLineChar = text[i];
        if ((EndOfLineChar == '\n') || (EndOfLineChar == '\0'))
        {
            LoopDone = true;
        }
        i++;
    }
    
    float NewX = static_cast<float>(x);
    float NewY = static_cast<float>(y);
    
    if (EndOfLineChar == '\n')
    {
        // The text has multiple lines
        drawStringMultiline(NewX, NewY, text);
    }
    else
    {
        // The text has one line
        ttyd::fontmgr::FontDrawString(NewX, NewY, text);
    }
}

void drawTextInit(uint8_t alpha, bool drawFontEdge)
{
    ttyd::fontmgr::FontDrawStart_alpha(alpha);
    
    // Check if the font edge should be on or off
    if (drawFontEdge)
    {
        ttyd::fontmgr::FontDrawEdge();
    }
}

void drawTextAndInit(const char *text, int32_t x, int32_t y, uint8_t alpha, uint32_t color, bool drawFontEdge, float scale)
{
    drawTextInit(alpha, drawFontEdge);
    drawText(text, x, y, color, scale);
}

int64_t reportCurrentFrameTime(const char *label)
{
    int64_t CurrentFrameTime = gc::OSTime::OSGetTime();
    
    gc::OSError::OSReport(
        "%s Raw: 0x%016" PRIX64 "\n",
        label,
        CurrentFrameTime);
    
    return CurrentFrameTime;
}

int64_t reportFrameTimeDifference(const char *label, int64_t startFrameTime, int64_t endFrameTime)
{
    int64_t FrameTimeDifference = endFrameTime - startFrameTime;
    
    gc::OSError::OSReport(
        "%s Dif: 0x%" PRIX64 "\n",
        label,
        FrameTimeDifference);
    
    return FrameTimeDifference;
}

uint16_t getMessageWidth(const char *text, float scale)
{
    uint16_t LineLength = ttyd::fontmgr::FontGetMessageWidth(text);
    float Width = static_cast<float>(LineLength + 15) * (scale + 0.05f);
    return static_cast<uint16_t>(Width);
}

void getOnOffTextAndColor(bool valueToCheck, const char **textOut, uint32_t *colorOut)
{
    if (valueToCheck)
    {
        *textOut = "On";
        *colorOut = 0x1BBE23FF; // Green
    }
    else
    {
        *textOut = "Off";
        *colorOut = 0xFF1414FF; // Red
    }
}

void getYesNoTextAndColor(bool valueToCheck, const char **textOut, uint32_t *colorOut)
{
    if (valueToCheck)
    {
        *textOut = "Yes";
        *colorOut = 0x1BBE23FF; // Green
    }
    else
    {
        *textOut = "No";
        *colorOut = 0xFF1414FF; // Red
    }
}

uint32_t getSelectedTextColor(bool valueToCheck)
{
    if (valueToCheck)
    {
        return 0x5B59DEFF; // Blue
    }
    else
    {
        return 0xFFFFFFFF; // White
    }
}

void drawTextWithWindow(const char *text, int32_t textPosX, int32_t textPosY, uint8_t alpha, 
    uint32_t textColor, float textScale, int32_t windowWidth, uint32_t windowColor, float windowCurve)
{
    // Check if the text is one line or multiple lines
    uint32_t AdditionalLines = 0;
    uint32_t i = 0;
    
    bool LoopDone = false;
    while (!LoopDone)
    {
        if (text[i] == '\n')
        {
            AdditionalLines++;
        }
        else if (text[i] == '\0')
        {
            LoopDone = true;
        }
        i++;
    }
    
    // Get the Height, X, and Y for the window
    int32_t WindowHeight = (AdditionalLines * 20) + 44;
    int32_t WindowPosX   = textPosX - 15;
    int32_t WindowPosY   = textPosY + 13;
    
    // Draw the window
    drawWindow(windowColor, WindowPosX, WindowPosY, windowWidth, WindowHeight, windowCurve);
    
    // Draw the text
    drawTextAndInit(text, textPosX, textPosY, alpha, textColor, false, textScale);
}

void drawSingleColumnMain()
{
    uint32_t tempTotalMenuOptions = Menu[MenuVar.CurrentMenu].TotalMenuOptions;
    uint32_t tempCurrentPage = 0;
    int32_t PosY = 180;
    
    drawSingleColumn(PosY, tempTotalMenuOptions, tempCurrentPage, false);
}

void drawSingleColumnSelectedOption()
{
    uint32_t tempTotalMenuOptions = Menu[MenuVar.CurrentMenu].TotalMenuOptions;
    uint32_t tempCurrentPage = 0;
    int32_t PosY = 180;
    
    drawSingleColumn(PosY, tempTotalMenuOptions, tempCurrentPage, true);
}

void drawSingleColumn(int32_t posY, uint32_t maxOptionsPerPage, 
    uint32_t currentPage, bool adjustSelectedOption)
{
    uint32_t tempCurrentMenu      = MenuVar.CurrentMenu;
    uint32_t tempSelectedOption   = MenuVar.SelectedOption;
    uint32_t tempTotalMenuOptions = Menu[tempCurrentMenu].TotalMenuOptions;
    uint32_t IndexStart           = currentPage * maxOptionsPerPage;
    
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    // int32_t PosY  = 180;
    float Scale      = 0.6f;
    uint32_t Color;
    
    for (uint32_t i = IndexStart; i < (IndexStart + maxOptionsPerPage); i++)
    {
        if (i >= tempTotalMenuOptions)
        {
            // Reached the end of the array, so exit
            return;
        }
        
        bool CurrentOptionCheck;
        if (tempSelectedOption == 0)
        {
            CurrentOptionCheck = MenuVar.CurrentMenuOption == i;
        }
        else if (adjustSelectedOption)
        {
            CurrentOptionCheck = (tempSelectedOption - 1) == i;
        }
        else
        {
            CurrentOptionCheck = tempSelectedOption == i;
        }
        
        const char *CurrentLine = Menu[tempCurrentMenu].Line[i];
        Color = getSelectedTextColor(CurrentOptionCheck);
        drawText(CurrentLine, PosX, posY, Color, Scale);
        posY -= 20;
    }
}

void drawMultipleColumnsVertical(int32_t posX, int32_t posY, uint32_t currentMenuOption, 
    uint32_t currentPage, uint32_t size, uint32_t maxOptionsPerPage, 
        uint32_t maxOptionsPerRow, bool showCurrentOption, 
            uint32_t posXIncrementAmount, const char **lines)
{
    uint32_t IndexStart        = currentPage * maxOptionsPerPage;
    
    uint32_t TotalRowsPerPage  = 1 + ((maxOptionsPerPage - 1) / maxOptionsPerRow); // Round up
    uint32_t TotalColumns      = 1 + ((maxOptionsPerPage - 1) / TotalRowsPerPage); // Round up
    uint32_t ColumnSplitAmount = 1 + ((maxOptionsPerPage - 1) / TotalColumns); // Round up
    
    // uint8_t Alpha           = 0xFF;
    float Scale                = 0.6f;
    int32_t tempPosY           = posY;
    uint32_t Color;
    
    for (uint32_t j = 0; j < TotalColumns; j++)
    {
        uint32_t CurrentStartingIndex = IndexStart + (j * ColumnSplitAmount);
        
        for (uint32_t i = CurrentStartingIndex; i < 
            (CurrentStartingIndex + ColumnSplitAmount); i++)
        {
            if (i >= size)
            {
                // Reached the end of the array, so exit
                return;
            }
            
            bool CurrentOptionCheck = (currentMenuOption == i) && showCurrentOption;
            Color = getSelectedTextColor(CurrentOptionCheck);
            
            const char *CurrentLine = lines[i];
            drawText(CurrentLine, posX, posY, Color, Scale);
            posY -= 20;
        }
        
        posX += posXIncrementAmount;
        posY = tempPosY;
    }
}

void drawInventoryIconAndTextColumns()
{
    uint32_t tempCurrentMenu = MenuVar.MenuSelectedOption;
    uint32_t tempCurrentPage = MenuVar.CurrentPage;
    
    uint32_t Address_and_Size[2];
    uint32_t *tempArray = getPouchAddressAndSize(Address_and_Size);
    
    if (!tempArray)
    {
        return;
    }
    
    uint32_t tempAddress         = tempArray[0];
    uint32_t tempSize            = tempArray[1];
    
    uint8_t Alpha                = 0xFF;
    float TextScale              = 0.6f;
    float IconScale              = 0.6f;
    int32_t PosX                 = -90;
    int32_t PosY                 = 125;
    
    const int32_t SetPosXText    = PosX + 15;
    const int32_t SetPosYText    = PosY + 20;
    const int32_t AdjustPosX     = PosX + 260;
    const int32_t AdjustPosYIcon = PosY;
    const int32_t AdjustPosYText = SetPosYText;
    
    // Set up array to use for displaying icons
    int32_t IconPosition[3];
    const int32_t IconPositionX  = 0;
    const int32_t IconPositionY  = 1;
    IconPosition[2]              = 0;
    IconPosition[IconPositionX]  = 217;
    
#ifdef TTYD_JP
    const int16_t ButtonL = 126;
    const int16_t ButtonR = 128;
#else
    const int16_t ButtonL = 134;
    const int16_t ButtonR = 136;
#endif
    
    uint32_t IncrementAmount;
    uint32_t MultiplyAmount;
    
    if ((tempCurrentMenu == INVENTORY_BADGES) || 
        (tempCurrentMenu == INVENTORY_STORED_ITEMS))
    {
        IncrementAmount = 1;
        MultiplyAmount = 10;
    }
    else
    {
        IncrementAmount = 2;
        MultiplyAmount = 1;
    }
    
    // Draw the icons
    uint32_t LoopCount = 2;
    for (uint32_t i = 0; i < LoopCount; i++)
    {
        // Draw the left and right columns
        drawItemIconsColumn(i * MultiplyAmount, IncrementAmount, tempSize, 
            PosX, PosY, tempAddress, IconScale);
        
        // Adjust position for right column
        PosX += AdjustPosX;
        PosY = AdjustPosYIcon;
    }
    
    // Draw the L and/or R buttons if appropriate
    if (tempCurrentMenu != INVENTORY_STANDARD)
    {
        float L_and_R_buttons_scale = 0.5f;
    
        // Draw the L button if currently not on the first page
        if (tempCurrentPage > 0)
        {
            IconPosition[IconPositionY] = 155;
            int16_t iconNum = ButtonL;
            drawIcon(IconPosition, iconNum, L_and_R_buttons_scale);
        }
        
        // Draw the R button if more items are on the next page
        if (checkForItemsOnNextPage(tempCurrentPage))
        {
            IconPosition[IconPositionY] = -175;
            int16_t iconNum = ButtonR;
            drawIcon(IconPosition, iconNum, L_and_R_buttons_scale);
        }
    }
    
    // Draw the text for how many items/badges the player currently has out of the max
    int16_t *CurrentAddress = reinterpret_cast<int16_t *>(tempAddress);
    uint32_t CurrentItemCount = 0;
    
    for (uint32_t i = 0; i < tempSize; i++)
    {
        if (*CurrentAddress)
        {
            CurrentItemCount++;
        }
        else
        {
            break;
        }
        
        CurrentAddress++;
    }
    
    // Draw the counts as int32_ts, to prevent long text if they somehow become negative
    char *tempDisplayBuffer = DisplayBuffer;
    
    sprintf(tempDisplayBuffer,
        "%" PRId32 "/%" PRId32,
        static_cast<int32_t>(CurrentItemCount),
        static_cast<int32_t>(tempSize));
    
    int32_t ItemCountPosX = 23;
    int32_t ItemCountPosY = 180;
    uint32_t Color = 0xFFFFFFFF;
    drawTextAndInit(tempDisplayBuffer, ItemCountPosX, ItemCountPosY, Alpha, Color, false, TextScale);
    
    // Adjust the initial position of the text
    PosX = SetPosXText;
    PosY = SetPosYText;
    
    // Draw the text for each item/badge
    for (uint32_t i = 0; i < LoopCount; i++)
    {
        // Draw the left and right columns
        drawItemTextColumn(i * MultiplyAmount, IncrementAmount, 
            tempSize, PosX, PosY, tempAddress, TextScale);
        
        // Adjust position for right column
        PosX += AdjustPosX;
        PosY = AdjustPosYText;
    }
}

void drawItemIconsColumn(uint32_t indexStart, uint32_t indexIncrement, uint32_t size, 
    int32_t posX, int32_t posY, uint32_t address, float scale)
{
    uint32_t MaxIconsPerColumn = 10;
    uint32_t MaxIconsPerPage   = 20;
    uint32_t newIndexStart     = indexStart + (MenuVar.CurrentPage * MaxIconsPerPage);
    uint32_t IndexCounter      = newIndexStart;
    float tempScale;
    
    // Set up array to use for displaying icons
    int32_t IconPosition[3];
    const int32_t IconPositionX = 0;
    const int32_t IconPositionY = 1;
    IconPosition[2]             = 0;
    IconPosition[IconPositionX] = posX;
    
    for (uint32_t i = newIndexStart; i < (newIndexStart + MaxIconsPerColumn); i++)
    {
        if (i >= size)
        {
            // Reached the end of the array, so exit
            return;
        }
        
        IconPosition[IconPositionY] = posY;
        
        int16_t CurrentItem = *reinterpret_cast<int16_t *>(address + (IndexCounter * 0x2));
        if (CurrentItem == 0)
        {
            // Reached the end of the array, so exit
            return;
        }
        else if ((CurrentItem >= ttyd::item_data::Item::PaperModeCurse) && 
            (CurrentItem <= ttyd::item_data::Item::UltraHammer))
        {
            // Decrease the size of these icons
            tempScale = 0.5f;
        }
        else if (CurrentItem == ttyd::item_data::Item::MagicalMapBigger)
        {
            // Decrease the size of the bigger magical map
            tempScale = 0.4f;
        }
        else
        {
            tempScale = scale;
        }
        
        drawIconFromItem(IconPosition, CurrentItem, tempScale);
        posY -= 30;
        IndexCounter += indexIncrement;
    }
}

void drawItemTextColumn(uint32_t indexStart, uint32_t indexIncrement, 
    uint32_t size, int32_t posX, int32_t posY, uint32_t address, float scale)
{
    uint32_t MaxIconsPerColumn  = 10;
    uint32_t MaxIconsPerPage    = 20;
    uint32_t tempCurrentPage    = MenuVar.CurrentPage;
    uint32_t tempSelectedOption = MenuVar.SelectedOption;
    uint32_t newIndexStart      = indexStart + (tempCurrentPage * MaxIconsPerPage);
    uint32_t IndexCounter       = newIndexStart;
    
    uint32_t TextColor;
    
    for (uint32_t i = newIndexStart; i < (newIndexStart + MaxIconsPerColumn); i++)
    {
        if (i >= size)
        {
            // Reached the end of the array, so exit
            return;
        }
        
        int16_t CurrentItem = *reinterpret_cast<int16_t *>(address + (IndexCounter * 0x2));
        if (CurrentItem == 0)
        {
            // Reached the end of the array, so exit
            return;
        }
        
        bool CurrentOptionCheck = (tempSelectedOption >= DUPLICATE) && 
            (tempSelectedOption <= DELETE) && 
                (MenuVar.FunctionReturnCode >= 0) && 
                    (MenuVar.CurrentMenuOption == IndexCounter);
        
        TextColor = getSelectedTextColor(CurrentOptionCheck);
        
        const char *ItemName = getItemName(CurrentItem);
        drawText(ItemName, posX, posY, TextColor, scale);
        posY -= 30;
        IndexCounter += indexIncrement;
    }
}

void drawMarioSpecialMovesOptions()
{
    // Check for button inputs
    marioSpecialMovesButtonControls();
    
    // Draw the window
    uint32_t Color = 0x151515F6;
    int32_t PosX   = -120;
    int32_t PosY   = 169;
    int32_t Width  = 243;
    int32_t Height = 332;
    int32_t Curve  = 20;
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Set the values for the text to use
    int32_t NewPosY = PosY - 79;
    float Scale = 0.6f;
    PosX += 55;
    PosY = NewPosY;
    
    // Set up array to use for displaying icons
    int32_t IconPosition[3];
    const int32_t IconPositionX = 0;
    const int32_t IconPositionY = 1;
    IconPosition[2]             = 0;
    IconPosition[IconPositionX] = PosX - 15;
    IconPosition[IconPositionY] = PosY - 18;
    
    // Draw the icons and main text
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    int16_t SpecialMoveIcon;
    
    // Draw the icons
    uint32_t LoopCount = 8;
    for (uint32_t i = 0; i < LoopCount; i++)
    {
        // Draw the icon
        if (i == 0)
        {
            // Set the icon for Sweet Treat
            SpecialMoveIcon = ttyd::item_data::Item::MagicalMap;
        }
        else
        {
            SpecialMoveIcon = (ttyd::item_data::Item::DiamondStar + i) - 1;
        }
        
        drawIconFromItem(IconPosition, SpecialMoveIcon, Scale);
        IconPosition[IconPositionY] -= 30;
    }
    
    // Draw the help text
    const char *HelpText = "Press A to turn on/off\nPress B to cancel";
    uint8_t Alpha = 0xFF;
    int32_t HelpTextPosX = PosX - 25;
    int32_t HelpTextPosY = PosY + 54;
    Color = 0xFFFFFFFF;
    
#ifdef TTYD_JP
    HelpTextPosX -= 1;
#endif
    
    drawTextAndInit(HelpText, HelpTextPosX, HelpTextPosY, Alpha, Color, false, Scale);
    
    // Draw the main text
    PosY = NewPosY;
    
    for (uint32_t i = 0; i < LoopCount; i++)
    {
        // Draw the main text
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(MarioStatsSpecialMovesOptions[i], PosX, PosY, Color, Scale);
        PosY -= 30;
    }
    
    // Draw the on/off text
    uint16_t SpecialMovesBits = getSpecialMoveBits();
    PosX += 130;
    PosY = NewPosY;
    
    for (uint32_t i = 0; i < LoopCount; i++)
    {
        // Check if the current special move is on or off
        bool SpecialMoveOnOrOff = SpecialMovesBits & (1 << i);
        const char *CurrentSpecialMove;
        
        getOnOffTextAndColor(SpecialMoveOnOrOff, &CurrentSpecialMove, &Color);
        
        drawText(CurrentSpecialMove, PosX, PosY, Color, Scale);
        PosY -= 30;
    }
}

void drawFollowersOptions()
{
    // Check for button inputs
    followersOptionsButtonControls();
    
    // Draw the window
    uint32_t Color = 0x151515F6;
    int32_t PosX   = -108;
    int32_t PosY   = 110;
    int32_t Width  = 213;
    int32_t Height = 198;
    int32_t Curve  = 20;
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Draw the help text
    const char *HelpText = "Press A to confirm\nPress B to cancel";
    uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    Color = 0xFFFFFFFF;
    PosX += 30;
    PosY -= 25;
    
#ifdef TTYD_JP
    PosX -= 1;
#endif
    
    drawTextAndInit(HelpText, PosX, PosY, Alpha, Color, false, Scale);
    
    PosX += 45;
    PosY -= 54;
    
#ifdef TTYD_JP
    PosX += 4;
#endif
    
    // Draw the main text
    uint32_t tempStatsFollowerOptionsLinesSize = StatsFollowerOptionsLinesSize;
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    
    for (uint32_t i = 0; i < tempStatsFollowerOptionsLinesSize; i++)
    {
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(StatsFollowerOptionsLines[i], PosX, PosY, Color, Scale);
        PosY -= 20;
    }
}

void drawMarioStats()
{
    // Create array for each stat to go in
    int32_t MarioStatsArray[16];
    
    uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
    
    // Add each stat to the array
    uint32_t Counter = 0;
    for (uint32_t i = 1; i <= 15; i++)
    {
        if (i == SPECIAL_MOVES)
        {
            i++;
        }
        
        uint32_t offset = getMarioStatsValueOffset(i);
        MarioStatsArray[Counter] = *reinterpret_cast<int16_t *>(PouchPtr + offset);
        Counter++;
    }
    
    uint32_t PiantaParlorPtr = reinterpret_cast<uint32_t>(ttyd::evt_yuugijou::yuugijouWorkPointer);
    MarioStatsArray[14] = *reinterpret_cast<int32_t *>(PiantaParlorPtr + 0x4); // Piantas Stored
    MarioStatsArray[15] = *reinterpret_cast<int32_t *>(PiantaParlorPtr + 0x8); // Current Piantas
    
    uint32_t Color              = 0xFFFFFFFF;
    uint8_t Alpha               = 0xFF;
    float TextScale             = 0.6f;
    float IconScale             = 0.6f;
    int32_t TextPosX            = -208;
    int32_t TextPosY            = 175;
    int32_t ValuesPosX          = TextPosX + 105;
    int32_t IconPosX            = TextPosX - 15;
    int32_t IconPosY            = TextPosY - 20;
    
    // Set up array to use for displaying icons
    int32_t IconPosition[3];
    const int32_t IconPositionX = 0;
    const int32_t IconPositionY = 1;
    IconPosition[2]             = 0;
    IconPosition[IconPositionX] = IconPosX;
    IconPosition[IconPositionY] = IconPosY;
    
    uint32_t tempStatsMarioOptionsLinesSize = StatsMarioOptionsLinesSize;
    uint32_t TotalRowsPerPage   = 1 + ((tempStatsMarioOptionsLinesSize - 1) / 2); // Round up
    uint32_t TotalColumns       = 1 + ((tempStatsMarioOptionsLinesSize - 1) / TotalRowsPerPage); // Round up
    uint32_t ColumnSplitAmount  = 1 + ((tempStatsMarioOptionsLinesSize - 1) / TotalColumns); // Round up
    
    int32_t PosX = TextPosX;
    int32_t PosY = TextPosY;
    
    // Draw the main text and icons
    char *tempDisplayBuffer = DisplayBuffer;
    int32_t IconPosYIncrement;
    uint32_t SelectionTextColor;
    Counter = 0;
    
    const uint32_t BeforeStarPowerSlot = 11;
    
    // Draw the icons
    bool ExitLoop = false;
    
    for (uint32_t j = 0; j < TotalColumns; j++)
    {
        uint32_t StartingIndex = j * ColumnSplitAmount;
        for (uint32_t i = StartingIndex; i < 
            (StartingIndex + ColumnSplitAmount); i++)
        {
            if (i >= tempStatsMarioOptionsLinesSize)
            {
                // Reached the end of the array
                ExitLoop = true;
                break;
            }
            
            if (i == BeforeStarPowerSlot) // 1 before Star Power
            {
                // Move the icons up
                IconPosYIncrement = -5;
            }
            else if (i == (BeforeStarPowerSlot + 2)) // 1 after Max Star Power
            {
                // Reset the value
                IconPosYIncrement = 5;
            }
            else
            {
                IconPosYIncrement = 0;
            }
            
            drawIcon(IconPosition, StatsMarioIcons[i], IconScale);
            
            PosY -= 30;
            IconPosition[IconPositionY] -= 30 + IconPosYIncrement;
        }
        
        if (ExitLoop)
        {
            break;
        }
        
        PosX += 190;
        ValuesPosX += 230;
        IconPosition[IconPositionX] += 190;
        PosY = TextPosY;
        IconPosition[IconPositionY] = IconPosY;
    }
    
    // Draw the special moves
    float SpecialMovesScale = 0.37f;
    int16_t SpecialMoveIcon;
    IconPosition[IconPositionX] = 133;
    IconPosition[IconPositionY] = 101;
    
    uint16_t SpecialMovesBits = getSpecialMoveBits();
    for (uint32_t i = 0; i < 8; i++)
    {
        if (SpecialMovesBits & (1 << i))
        {
            if (i == 0)
            {
                // Set the icon for Sweet Treat
                SpecialMoveIcon = ttyd::item_data::Item::MagicalMap;
            }
            else
            {
                SpecialMoveIcon = (ttyd::item_data::Item::DiamondStar + i) - 1;
            }
            
            drawIconFromItem(IconPosition, SpecialMoveIcon, SpecialMovesScale);
        }
        
        IconPosition[IconPositionX] += 13;
    }
    
    // Restart the text drawing sequence
    drawTextInit(Alpha, false);
    
    // Draw the text
    PosX = TextPosX;
    PosY = TextPosY;
    ValuesPosX = TextPosX + 105;
    ExitLoop = false;
    
    for (uint32_t j = 0; j < TotalColumns; j++)
    {
        uint32_t StartingIndex = j * ColumnSplitAmount;
        for (uint32_t i = StartingIndex; i < 
            (StartingIndex + ColumnSplitAmount); i++)
        {
            if (i >= tempStatsMarioOptionsLinesSize)
            {
                // Reached the end of the array
                ExitLoop = true;
                break;
            }
            
            bool CurrentOptionCheck = (MenuVar.CurrentMenuOption == i);
            SelectionTextColor = getSelectedTextColor(CurrentOptionCheck);
            
            drawText(StatsMarioOptionsLines[i], PosX, PosY, SelectionTextColor, TextScale);
            
            // Draw the values
            if (i != BeforeStarPowerSlot) // Special moves -- drawn separately
            {
                sprintf(tempDisplayBuffer,
                    "%" PRId32,
                    MarioStatsArray[Counter]);
                
                drawText(tempDisplayBuffer, ValuesPosX, PosY, Color, TextScale);
                Counter++;
            }
            
            PosY -= 30;
        }
        
        if (ExitLoop)
        {
            break;
        }
        
        PosX += 190;
        ValuesPosX += 230;
        PosY = TextPosY;
    }
}

void drawPartnerStats()
{
    uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
    uint32_t tempSelectedOption = MenuVar.SelectedOption;
    
    // Create array for each stat to go in
    int16_t PartnerStats[3];
    
    // Add each stat to the array
    uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(getPartnerEnabledAddress());
    PartnerStats[0] = *reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x6); // HP
    PartnerStats[1] = *reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0x2); // Max HP
    PartnerStats[2] = *reinterpret_cast<int16_t *>(PartnerEnabledAddress + 0xA); // Level
    
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -120;
    int32_t PosY = 180;
    float Scale = 0.6f;
    uint32_t Color;
    
    // Draw the main text
    uint32_t tempStatsPartnerOptionsLinesSize = StatsPartnerOptionsLinesSize;
    for (uint32_t i = 0; i < tempStatsPartnerOptionsLinesSize; i++)
    {
        bool CurrentOptionCheck = (tempCurrentMenuOption == i) && (tempSelectedOption > 0);
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(StatsPartnerOptionsLines[i], PosX, PosY, Color, Scale);
        PosY -= 20;
    }
    
    // Draw the color text for Yoshi
    ttyd::party::PartyMembers CurrentPartner = getSelectedOptionPartnerValue();
    
    uint32_t FirstFreeSlot = TOGGLE; // enum index starts at 1
    if (CurrentPartner == ttyd::party::PartyMembers::kYoshi)
    {
        bool CurrentOptionCheck = (tempCurrentMenuOption == FirstFreeSlot) && (tempSelectedOption > 0);
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        const char *String = "Color";
        drawText(String, PosX, PosY, Color, Scale);
        PosY -= 20;
    }
    
    // Check if the current partner out is the selected one
    const char *OptionToDrawText;
    
    if (checkIfPartnerOutSelected())
    {
        OptionToDrawText = "Remove From Overworld";
    }
    else
    {
        OptionToDrawText = "Bring Out";
    }
    
    // Draw the option
    uint32_t AdditionalOptions = 0;
    if (CurrentPartner == ttyd::party::PartyMembers::kYoshi)
    {
        // Add an extra line for Yoshi
        AdditionalOptions++;
    }
    
    bool CurrentOptionCheck = (tempCurrentMenuOption == (
        FirstFreeSlot + AdditionalOptions)) && (tempSelectedOption > 0);
    
    Color = getSelectedTextColor(CurrentOptionCheck);
    drawText(OptionToDrawText, PosX, PosY, Color, Scale);
    
    PosX = -20;
    PosY = 180;
    Color = 0xFFFFFFFF;
    
    // Draw the HP, Max HP, and Level
    char *tempDisplayBuffer = DisplayBuffer;
    for (uint32_t i = 0; i < (tempStatsPartnerOptionsLinesSize - 1); i++)
    {
        sprintf(tempDisplayBuffer,
            "%" PRId16,
            PartnerStats[i]);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        PosY -= 20;
    }
    
    // Draw the on or off text
    bool PartnerEnabled = *reinterpret_cast<bool *>(PartnerEnabledAddress + 0x1);
    const char *String;
    
    getOnOffTextAndColor(PartnerEnabled, &String, &Color);
    drawText(String, PosX, PosY, Color, Scale);
    
    // Draw Yoshi's color
    if (CurrentPartner == ttyd::party::PartyMembers::kYoshi)
    {
        // Get the current Yoshi color
        uint32_t YoshiColorId = getCurrentYoshiColorId();
        const char *YoshiColorString = "";
        
        // Make sure the current color is valid
        const uint32_t ColorIdWhite = 6;
        if (YoshiColorId <= ColorIdWhite)
        {
            YoshiColorString = StatsYoshiColorOptionsLines[YoshiColorId];
        }
        
        const uint32_t ColorIdGreen    = 0;
        const uint32_t ColorIdRed      = 1;
        const uint32_t ColorIdBlue     = 2;
        const uint32_t ColorIdOrange   = 3;
        const uint32_t ColorIdPink     = 4;
        const uint32_t ColorIdBlack    = 5;
        // const uint32_t ColorIdWhite = 6;
        
        uint32_t YoshiColorText;
        switch (YoshiColorId)
        {
            case ColorIdGreen:
            {
                YoshiColorText = 0x29AD21FF;
                break;
            }
            case ColorIdRed:
            {
                YoshiColorText = 0xE8385AFF;
                break;
            }
            case ColorIdBlue:
            {
                YoshiColorText = 0x8E7DDFFF;
                break;
            }
            case ColorIdOrange:
            {
                YoshiColorText = 0xF47529FF;
                break;
            }
            case ColorIdPink:
            {
                YoshiColorText = 0xF58BB6FF;
                break;
            }
            case ColorIdBlack:
            {
                YoshiColorText = 0x635D63FF;
                break;
            }
            case ColorIdWhite:
            {
                YoshiColorText = 0xC6E1ECFF;
                break;
            }
            default:
            {
                YoshiColorText = 0xFFFFFFFF;
                break;
            }
        }
        
        PosY -= 20;
        drawText(YoshiColorString, PosX, PosY, YoshiColorText, Scale);
    }
    
    // Check if a partner is out or not
    uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
    if (!PartnerPointer)
    {
        // Draw text explaining that no partner is currently out
        const char *String = "No partner is currently out";
        Color = 0xFFFFFFFF;
        PosX = -120;
        PosY -= 60;
        drawText(String, PosX, PosY, Color, Scale);
    }
}

void drawPartnerChangeYoshiColorOptions()
{
    // Check for button inputs
    partnerChangeYoshiColorButtonControls();
    
    // Draw the window
    uint32_t Color = 0x151515F6;
    int32_t PosX   = -106;
    int32_t PosY   = 123;
    int32_t Width  = 212;
    int32_t Height = 239;
    int32_t Curve  = 20;
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Draw the help text
    const char *HelpText = "Press A to confirm\nPress B to cancel";
    uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    Color = 0xFFFFFFFF;
    PosX += 30;
    PosY -= 25;
    
#ifdef TTYD_JP
    PosX -= 1;
#endif
    
    drawTextAndInit(HelpText, PosX, PosY, Alpha, Color, false, Scale);
    
    // Set the values for the text to use
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    PosX += 47;
    PosY -= 54;
    
#ifdef TTYD_JP
    PosX += 2;
#endif
    
    uint32_t Size = StatsYoshiColorOptionsLinesSize;
    for (uint32_t i = 0; i < Size; i++)
    {
        // Draw the main text
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(StatsYoshiColorOptionsLines[i], PosX, PosY, Color, Scale);
        PosY -= 20;
    }
}

void drawBattlesActorStats()
{
    // Get the starting address for the current actor
    uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuVar.MenuSelectedOption));
    if (!ActorAddress)
    {
        return;
    }
    
    // Draw the name of the current actor
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t NamePosX = -232;
    int32_t NamePosY = 20;
    float Scale = 0.6f;
    
    uint32_t CurrentActor = *reinterpret_cast<uint32_t *>(ActorAddress + 0x8);
    char *tempDisplayBuffer = DisplayBuffer;
    
    sprintf(tempDisplayBuffer,
        "Selected Actor\n%s",
        BattlesActorsLines[CurrentActor - 1]);
    
    drawText(tempDisplayBuffer, NamePosX, NamePosY, Color, Scale);
    
    // Create array for each stat to go in
    int16_t ActorStats[4];
    
    // Add each stat to the array
    ActorStats[0] = *reinterpret_cast<int16_t *>(ActorAddress + 0x10C); // HP
    ActorStats[1] = *reinterpret_cast<int16_t *>(ActorAddress + 0x108); // Max HP
    ActorStats[2] = *reinterpret_cast<int16_t *>(ActorAddress + 0x112); // FP
    ActorStats[3] = *reinterpret_cast<int16_t *>(ActorAddress + 0x10E); // Max FP
    
    int32_t PosX = -50;
    int32_t PosY = 180;
    
    // Draw the main text
    uint32_t tempBattlesCurrentActorStatsSize = BattlesCurrentActorStatsSize;
    for (uint32_t i = 0; i < tempBattlesCurrentActorStatsSize; i++)
    {
        drawText(BattlesCurrentActorStats[i], PosX, PosY, Color, Scale);
        PosY -= 20;
    }
    
    PosX += 100;
    PosY = 180;
    
    // Draw the stats
    for (uint32_t i = 0; i < (tempBattlesCurrentActorStatsSize); i++)
    {
        sprintf(tempDisplayBuffer,
            "%" PRId16,
            ActorStats[i]);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        PosY -= 20;
    }
}

void drawBattlesActorsHeldItem()
{
    // Get the starting address for the current actor
    uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuVar.MenuSelectedOption));
    if (!ActorAddress)
    {
        return;
    }
    
    // Only run when not displaying for Mario or the partners
    /*uint32_t CurrentActorId = *reinterpret_cast<uint32_t *>(ActorAddress + 0x8);
    const uint32_t MarioId     = 222;
    const uint32_t GoombellaId = 224;
    const uint32_t MowzId      = 230;
    
    if ((CurrentActorId == MarioId) || 
        ((CurrentActorId >= GoombellaId) && 
            (CurrentActorId <= MowzId)))
    {
        return;
    }*/
    
    uint8_t Alpha  = 0xFF;
    uint32_t Color = 0xFFFFFFFF;
    int32_t PosX   = -50;
    int32_t PosY   = 80;
    float Scale    = 0.6f;
    
    // Draw the text for showing what the current item is
    const char *CurrentLine = "Current Held Item";
    drawText(CurrentLine, PosX, PosY, Color, Scale);
    
    // Draw the current item icon
    // Make sure the actor is actually holding an item
#ifdef TTYD_US
    uint32_t offset = 0x308;
#elif defined TTYD_JP
    uint32_t offset = 0x304;
#elif defined TTYD_EU
    uint32_t offset = 0x30C;
#endif
    
    int32_t tempitem = *reinterpret_cast<uint32_t *>(ActorAddress + offset);
    if (tempitem > 0)
    {
        PosX += 10;
        PosY -= 45;
        
        // Set up array to use for displaying icons
        int32_t IconPosition[3];
        const int32_t IconPositionX = 0;
        const int32_t IconPositionY = 1;
        IconPosition[IconPositionX] = PosX;
        IconPosition[IconPositionY] = PosY;
        IconPosition[2]             = 0;
        
        drawIconFromItem(IconPosition, tempitem, Scale);
        
        // Draw the text for the item
        PosX += 17;
        PosY += 18;
        const char *ItemName = getItemName(tempitem);
        
        drawTextAndInit(ItemName, PosX, PosY, Alpha, Color, false, Scale);
    }
    else
    {
        PosY -= 20;
        
        drawText("None", PosX, PosY, Color, Scale);
    }
}

void drawCurrentFollowerOut()
{
    // Draw the text for the current follower out
    uint32_t FollowerPointer = reinterpret_cast<uint32_t>(getFollowerPointer());
    const char *String;
    
    if (!FollowerPointer)
    {
        String = "None";
    }
    else
    {
        uint8_t CurrentFollowerOut = *reinterpret_cast<uint8_t *>(FollowerPointer + 0x31);
        if ((CurrentFollowerOut < 8) || (CurrentFollowerOut > 12))
        {
            String = "Other";
        }
        else
        {
            String = StatsFollowerOptionsLines[CurrentFollowerOut - 8];
        }
    }
    
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Current Follower Out\n%s",
        String);
    
    // uint8_t Alpha = 0xFF;
    uint32_t Color = 0xFFFFFFFF;
    int32_t PosX = -232;
    int32_t PosY = 120;
    float Scale = 0.6f;
    
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
}

void drawMemoryWatchValueString(int32_t slot, int32_t posX, 
    int32_t posY, uint32_t color, bool drawFontEdge, float scale)
{
    // Check if the font edge should be on or off
    if (drawFontEdge)
    {
        ttyd::fontmgr::FontDrawEdge();
    }
    else
    {
        ttyd::fontmgr::FontDrawEdgeOff();
    }
    
    // Get the string value
    const char *ValueString = getValueString(slot);
    
    // Check to see what type of memory watch is being drawn
    uint32_t MemoryWatchType = MemoryWatch[slot].Type;
    
    // Check if the current value is not text
    if (MemoryWatchType != string)
    {
        // Draw the value
        drawText(ValueString, posX, posY, color, scale);
    }
    else
    {
        // Draw a maximum of 127 characters
        char TextString[128]; // 1 byte for NULL
        uint32_t Size = sizeof(TextString);
        clearMemory(TextString, Size);
        
        // Get each character to draw one at a time
        // Stop if an invalid address is reached
        for (uint32_t i = 0; i < (Size - 1); i++)
        {
            if (checkIfPointerIsValid(&ValueString[i]) && (ValueString[i] != '\0'))
            {
                TextString[i] = ValueString[i];
            }
            else
            {
                break;
            }
        }
        
        // Draw the text
        drawText(TextString, posX, posY, color, scale);
    }
}

void drawMemoryWatches()
{
    // Make sure there's at least one watch to draw
    if (!MemoryWatch[0].Address)
    {
        return;
    }
    
    uint32_t tempCurrentPage  = MenuVar.CurrentPage;
    int32_t TotalMenuOptions  = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
    int32_t MaxOptionsPerPage = 10;
    int32_t IndexStart        = tempCurrentPage * MaxOptionsPerPage;
    
    uint32_t Color   = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = 80;
    float Scale      = 0.6f;
    
    const int32_t TypeOffset  = 150;
    const int32_t ValueOffset = TypeOffset + 90;
    
    // Draw the current page
    drawPageNumber(PosX, PosY, tempCurrentPage);
    
    // Draw the total amount of watches out of the max
    int32_t CurrentWatchCount = 0;
    for (int32_t i = 0; i < TotalMenuOptions; i++)
    {
        if (MemoryWatch[i].Address)
        {
            CurrentWatchCount++;
        }
        else
        {
            break;
        }
    }
    
    // Draw the counts as int32_ts, to prevent long text if they somehow become negative
    char *tempDisplayBuffer = DisplayBuffer;
    
    sprintf(tempDisplayBuffer,
        "%" PRId32 "/%" PRId32,
        CurrentWatchCount,
        TotalMenuOptions);
    
    drawText(tempDisplayBuffer, PosX + TypeOffset, PosY, Color, Scale);
    PosY -= 40;
    
    // Draw the headers for the values
    drawText("Address", PosX, PosY, Color, Scale);
    drawText("Display", PosX + TypeOffset, PosY, Color, Scale);
    drawText("Value", PosX + ValueOffset, PosY, Color, Scale);
    PosY -= 20;
    
    // Draw the main text
    for (int32_t i = IndexStart; i < (IndexStart + MaxOptionsPerPage); i++)
    {
        if (i >= TotalMenuOptions)
        {
            // Reached the end of the slots, so exit
            return;
        }
        
        // Check if the current address is valid
        if (!MemoryWatch[i].Address)
        {
            // Reached the end of the valid watches, so exit
            return;
        }
        
        // Draw the On/Off text for whether the watch is displayed or not
        const char *OnOffText;
        
        getOnOffTextAndColor(MemoryWatch[i].Display, &OnOffText, &Color);
        drawText(OnOffText, PosX + TypeOffset, PosY, Color, Scale);
        
        // Set the color of the address text
        bool CurrentOptionCheck = (MenuVar.CurrentMenuOption == i) && (MenuVar.SelectedOption > 0);
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        // Draw the current address
        drawText(getAddressString(i), PosX, PosY, Color, Scale);
        
        // Draw the value
        Color = 0xFFFFFFFF;
        drawMemoryWatchValueString(i, PosX + ValueOffset, PosY, Color, false, Scale);
        
        PosY -= 20;
    }
}

void drawMemoryModifyList()
{
    uint32_t tempSelectedOption     = MenuVar.SelectedOption;
    uint32_t tempCurrentMenuOption  = MenuVar.CurrentMenuOption;
    uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
    uint32_t TotalOptions           = MemoryWatchModifyLinesSize;
    
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = 180;
    float Scale      = 0.6f;
    uint32_t Color;
    
    const int32_t PosX_Offset = 130;
    
    for (uint32_t i = 0; i < TotalOptions; i++)
    {
        // Get the color of the main text
        bool CurrentOptionCheck;
        
        if (tempSelectedOption == 0)
        {
            CurrentOptionCheck = tempCurrentMenuOption == i;
        }
        else
        {
            CurrentOptionCheck = (tempSelectedOption - 1) == i;
        }
        
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        // Draw the main text
        drawText(MemoryWatchModifyLines[i], PosX, PosY, Color, Scale);
        
        // Reset the color back to white
        Color = 0xFFFFFFFF;
        
        // Draw the value for the text
        switch (i + 1)
        {
            case CHANGE_ADDRESS:
            {
                // Draw the current address
                drawText(getAddressString(tempMenuSelectedOption), PosX + PosX_Offset, PosY, Color, Scale);
                break;
            }
            case CHANGE_TYPE:
            {
                // Draw the current type
                drawText(MemoryTypeLines[MemoryWatch[tempMenuSelectedOption].Type], 
                    PosX + PosX_Offset, PosY, Color, Scale);
                break;
            }
            case SHOW_AS_HEX:
            {
                // Draw the Yes/No text for whether the watch is shown as hex or not
                const char *YesNoText;
                getYesNoTextAndColor(MemoryWatch[tempMenuSelectedOption].ShowAsHex, &YesNoText, &Color);
                drawText(YesNoText, PosX + PosX_Offset, PosY, Color, Scale);
                break;
            }
            case CHANGE_WATCH_POSITION:
            {
                // Draw the X and Y positions
                char *tempDisplayBuffer = DisplayBuffer;
                sprintf(tempDisplayBuffer,
                    "%" PRId32 ", %" PRId32,
                    MemoryWatch[tempMenuSelectedOption].PosX,
                    MemoryWatch[tempMenuSelectedOption].PosY);
                
                drawText(tempDisplayBuffer, PosX + PosX_Offset, PosY, Color, Scale);
                break;
            }
            case DISPLAY_OUT_OF_MENU:
            {
                // Draw the On/Off text for whether the watch is displayed or not
                const char *OnOffText;
                getOnOffTextAndColor(MemoryWatch[tempMenuSelectedOption].Display, &OnOffText, &Color);
                drawText(OnOffText, PosX + PosX_Offset, PosY, Color, Scale);
                break;
            }
            default:
            {
                break;
            }
        }
        
        PosY -= 20;
    }
    
    // Draw the value stored at the current address
    Color = 0xFFFFFFFF;
    PosY -= 20;
    
    drawText("Value", PosX, PosY, Color, Scale);
    drawMemoryWatchValueString(tempMenuSelectedOption, PosX + PosX_Offset, PosY, Color, false, Scale);
}

void drawMemoryTypeList()
{
    // Check for button inputs
    memoryAddressTypeButtonControls();
    
    // Draw the window
    uint32_t Color = 0x151515F6;
    int32_t PosX   = -110;
    int32_t PosY   = 169;
    int32_t Width  = 212;
    int32_t Height = 332;
    int32_t Curve  = 20;
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Draw the help text
    const char *HelpText = "Press A to confirm\nPress B to cancel";
    uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    Color = 0xFFFFFFFF;
    PosX += 30;
    PosY -= 25;
    
#ifdef TTYD_JP
    PosX -= 1;
#endif
    
    drawTextAndInit(HelpText, PosX, PosY, Alpha, Color, false, Scale);
    PosX += 42;
    PosY -= 50;
    
#ifdef TTYD_JP
    PosX += 3;
#endif
    
    // Draw the main text
    uint32_t Size = MemoryTypeLinesSize;
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    
    for (uint32_t i = 0; i < Size; i++)
    {
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(MemoryTypeLines[i], PosX, PosY, Color, Scale);
        PosY -= 20;
    }
}

void drawMemoryChangeWatchPosition()
{
    // Check for button inputs
    memoryChangeWatchPositionButtonControls();
    
    uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
    
    // Draw the current watch
    int32_t PosX       = MemoryWatchPosition.PosX;
    int32_t PosY       = MemoryWatchPosition.PosY;
    uint32_t TextColor = 0xFFFFFFFF;
    uint8_t Alpha      = 0xFF;
    float Scale        = 0.65f;
    
    // Start the text drawing sequence
    drawTextInit(Alpha, false); // Set to false since drawMemoryWatchValueString will manually enable the font edge
    
    drawMemoryWatchValueString(tempMenuSelectedOption, PosX, PosY, TextColor, true, Scale);
    
    // Don't draw the window and text if Y is being held
    if (checkButtonComboEveryFrame(PAD_Y))
    {
        return;
    }
    
    // Draw the help window
    int32_t Curve        = 20;
    uint32_t WindowColor = 0x151515F6;
    int32_t TextPosX     = -135;
    int32_t TextPosY     = 140;
    Scale                = 0.6f;
    
    const char *HelpText = "Press/Hold the D-Pad directions\nto move the watch\n\nHold Y to hide this window\n\nPress A to confirm\n\nPress B to cancel";
    int32_t Width = static_cast<int32_t>(getMessageWidth(HelpText, Scale));
    
    drawTextWithWindow(HelpText, TextPosX, TextPosY, Alpha, 
        TextColor, Scale, Width, WindowColor, Curve);
}

void drawMemoryWatchChangeAddressListWindow(int32_t posY)
{
    uint32_t WindowColor = 0x151515E0;
    int32_t WindowPosX = -245;
    int32_t WindowPosY = posY;
    int32_t Width = 379;
    int32_t Height = 15;
    int32_t Curve = 0;
    
#ifdef TTYD_JP
    Width -= 16;
#endif
    
    drawWindow(WindowColor, WindowPosX, WindowPosY, Width, Height, Curve);
}

void drawMemoryChangeAddressList()
{
    uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
    uint32_t tempCurrentMenuOption  = MenuVar.CurrentMenuOption;
    uint32_t tempSelectedOption     = MenuVar.SelectedOption;
    
    uint8_t Alpha = 0xFF;
    int32_t PosX  = -232;
    int32_t PosY  = 100;
    float Scale   = 0.6f;
    uint32_t Color;
    
    const int32_t PosX_Offset_Position  = 110;
    const int32_t PosX_Address_Position = 250;
    
    // Get the color for the address text
    bool CurrentOptionCheck = (tempSelectedOption != 0) && (tempCurrentMenuOption == 0);
    Color = getSelectedTextColor(CurrentOptionCheck);
    
    if (CurrentOptionCheck)
    {
        // Draw a window for the current line
        drawMemoryWatchChangeAddressListWindow(PosY);
    }
    
    // Draw the address
    drawTextAndInit("Address", PosX, PosY, Alpha, Color, false, Scale);
    
    Color = 0xFFFFFFFF;
    drawText(getAddressStringNoLetterP(tempMenuSelectedOption), 
        PosX + PosX_Address_Position, PosY, Color, Scale);
    
    PosY -= 20;
    char *tempDisplayBuffer = DisplayBuffer;
    
    // Draw the pointer levels
    uint32_t TotalLevels = MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount;
    for (uint32_t i = 1; i <= TotalLevels; i++)
    {
        // Get the color for the pointer text
        bool CurrentOptionCheck = (tempSelectedOption != 0) && (tempCurrentMenuOption == i);
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        if (CurrentOptionCheck)
        {
            // Draw a window for the current line
            drawMemoryWatchChangeAddressListWindow(PosY);
            
            // Restart the text drawing sequence
            drawTextInit(Alpha, false);
        }
        
        // Draw the text for the current level
        sprintf(tempDisplayBuffer,
            "Level %" PRIu32,
            i);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        
        // Reset the color back to white
        Color = 0xFFFFFFFF;
        
        // Draw the offset being applied
        // Check if the value is negative
        int32_t tempOffset = MemoryWatch[tempMenuSelectedOption].AddressOffset[i - 1];
        
        const char *Format;
        if (tempOffset < 0)
        {
            // Convert the value to negative
            tempOffset = -tempOffset;
            
            Format = "-0x%" PRIX32;
        }
        else
        {
            Format = "0x%" PRIX32;
        }
        
        sprintf(tempDisplayBuffer,
            Format,
            tempOffset);
        
        drawText(tempDisplayBuffer, PosX + PosX_Offset_Position, PosY, Color, Scale);
        
        // Draw the address pointed to by the current level
        drawText(getAddressStringFromOffsets(tempMenuSelectedOption, i), 
            PosX + PosX_Address_Position, PosY, Color, Scale);
        
        PosY -= 20;
    }
    
    // Draw the final value
    Color = 0xFFFFFFFF;
    PosY -= 20;
    
    drawText("Final Value", PosX, PosY, Color, Scale);
    drawMemoryWatchValueString(tempMenuSelectedOption, PosX + PosX_Offset_Position, PosY, Color, false, Scale);
}

uint16_t *drawMemoryEditorSetup()
{
    // Draw the text for the options
    drawSingleColumnMain();
    
    // Draw the bool for whether the editor is enabled or not
    bool EditorEnabled = MemoryEditor.CurrentSelectionStatus & EDITOR_ENABLED;
    const char *CurrentLine = "Memory Editor Display";
    
    int32_t PosY = 100;
    drawBoolOnOrOff(EditorEnabled, CurrentLine, PosY);
    
    // Draw the button combo
    uint16_t *ButtonCombo = &MemoryEditor.ButtonCombo;
    CurrentLine = "Button Combo (Can be pressed in any order)";
    PosY -= 60;
    drawButtonCombo(*ButtonCombo, PosY, CurrentLine);
    
    return ButtonCombo;
}

void drawMemoryEditorSettingsWindow()
{
    const int32_t InitialPosY      = 180;
    const int32_t ValuesPosXOffset = 260;
    
    uint8_t Alpha = 0xFF;
    int32_t PosX  = -232;
    int32_t PosY  = InitialPosY;
    float Scale   = 0.6f;
    uint32_t Color;
    
    // Start the text drawing sequence
    drawTextInit(Alpha, false);
    
    // Draw the main text
    uint32_t CurrentSettingsMenuOption = MemoryEditor.CurrentSettingsMenuOption;
    const char **StringArray = MemoryEditorSettingsLines;
    uint32_t Size = MemoryEditorSettingsLinesSize;
    
    for (uint32_t i = 0; i < Size; i++)
    {
        bool CurrentOptionCheck = CurrentSettingsMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        if (CurrentOptionCheck)
        {
            // Draw a window for the current option
            uint32_t WindowColor = 0x151515E0;
            int32_t WindowPosX = PosX - 5;
            int32_t WindowPosY = PosY;
            int32_t Width = 293;
            int32_t Height = 15;
            int32_t Curve = 0;
            drawWindow(WindowColor, WindowPosX, WindowPosY, Width, Height, Curve);
            
            drawTextAndInit(StringArray[i], PosX, PosY, Alpha, Color, false, Scale);
            PosY -= 20;
        }
        else
        {
            drawText(StringArray[i], PosX, PosY, Color, Scale);
            PosY -= 20;
        }
    }
    
    // Draw the values
    uint32_t CurrentSelectionStatus = MemoryEditor.CurrentSelectionStatus;
    const char *TextToDraw;
    PosX += ValuesPosXOffset;
    PosY = InitialPosY;
    
    for (uint32_t i = 0; i < Size; i++)
    {
        // Values start at EDITOR_CLEAR_CACHE
        bool CurrentValue = CurrentSelectionStatus & (EDITOR_CLEAR_CACHE << i);
        getOnOffTextAndColor(CurrentValue, &TextToDraw, &Color);
        
        drawText(TextToDraw, PosX, PosY, Color, Scale);
        PosY -= 20;
    }
}

void drawMemoryEditorMainWindow()
{
    const int32_t InitialPosX       = -232;
    const int32_t InitialPosY       = 180;
    const int32_t InitialEditorPosY = InitialPosY - 80;
    const int32_t InitialEditorPosX = InitialPosX + 103;
    const int32_t DigitSpace        = 12;
    
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = InitialPosX;
    int32_t PosY   = InitialPosY;
    float Scale    = 0.6f;
    
#ifdef TTYD_JP
    Scale += 0.05;
#endif
    
    // Draw the vertical and/or horizontal lines
    uint32_t tempCurrentSelectionStatus = MemoryEditor.CurrentSelectionStatus;
    if (tempCurrentSelectionStatus & EDITOR_VERTICAL_LINES)
    {
        // Draw a line between each byte
        uint32_t WindowColor = 0xFFFFFFE0;
        int32_t WindowPosX = InitialEditorPosX + 24;
        int32_t WindowPosY = InitialEditorPosY;
        int32_t Width = 2;
        int32_t Height = 275;
        int32_t Curve = 0;
        
#ifdef TTYD_JP
        WindowPosX -= 1;
#endif
        
        for (uint32_t i = 0; i < (EDITOR_BYTES_PER_ROW - 1); i++)
        {
            drawWindow(WindowColor, WindowPosX, WindowPosY, Width, Height, Curve);
            WindowPosX += 28;
        }
    }
    
    if (tempCurrentSelectionStatus & EDITOR_HORIZONTAL_LINES)
    {
        // Draw a line between each row
        uint32_t WindowColor = 0xFFFFFFE0;
        int32_t WindowPosX = InitialPosX - 5;
        int32_t WindowPosY = InitialEditorPosY - 36;
        int32_t Width = 470;
        int32_t Height = 2;
        int32_t Curve = 0;
        
#ifdef TTYD_JP
        WindowPosY -= 1;
#endif
        
        for (uint32_t i = 0; i < (EDITOR_BYTES_PER_ROW - 1); i++)
        {
            drawWindow(WindowColor, WindowPosX, WindowPosY, Width, Height, Curve);
            WindowPosY -= 20;
        }
    }
    
    // Draw the text stating to press Y to open the options
    const char *String = "Press Y to open the settings menu";
    int32_t SettingsTextPosX = -143;
    drawTextAndInit(String, SettingsTextPosX, PosY, Alpha, Color, false, Scale);
    PosY -= 20;
    
    // Draw the text for the options
    uint8_t *CurrentAddress = MemoryEditor.CurrentAddress;
    uint32_t tempCurrentEditorMenuOption = MemoryEditor.CurrentEditorMenuOption;
    bool ColorCondition;
    
    char *tempDisplayBuffer = DisplayBuffer;
    for (uint32_t i = 0; i < EDITOR_HEADER_TOTAL_OPTIONS; i++)
    {
        switch (i)
        {
            case EDITOR_HEADER_CHANGE_ADDRESS:
            {
                // Get the values for the address string
                String = "Address";
                
                // Draw the starting address
                sprintf(tempDisplayBuffer,
                    "0x%08" PRIX32,
                    reinterpret_cast<uint32_t>(CurrentAddress));
                
                uint32_t tempColor = 0xFFFFFFFF;
                drawText(tempDisplayBuffer, PosX, PosY - 20, tempColor, Scale);
                break;
            }
            case EDITOR_HEADER_CHANGE_BYTE_SIZE:
            {
                // Get the values for the number of bytes being changed
                String = "Bytes Size";
                PosX += 178;
                
#ifdef TTYD_JP
                PosX -= 2;
#endif
                
                // Draw the number of bytes being changed
                uint32_t NumBytesBeingEdited = MemoryEditor.NumBytesBeingEdited;
                
                sprintf(tempDisplayBuffer,
                    "%" PRIu32,
                    NumBytesBeingEdited);
                
                // Calculate the X offset
                uint32_t tempNumBytesBeingEdited = NumBytesBeingEdited;
                int32_t tempPosX = 0;
                
                while (tempNumBytesBeingEdited >= 10)
                {
                    tempNumBytesBeingEdited /= 10;
                    tempPosX += 9;
                }
                
                int32_t IncrementAmount = 37;
#ifdef TTYD_JP
                IncrementAmount += 4;
#endif
                
                tempPosX = (PosX + IncrementAmount) - (tempPosX / 2);
                
                uint32_t tempColor = 0xFFFFFFFF;
                drawText(tempDisplayBuffer, tempPosX, PosY - 20, tempColor, Scale);
                break;
            }
            case EDITOR_HEADER_GOTO_EDITOR:
            {
                String = "Edit Bytes";
                PosX += 199;
                
#ifdef TTYD_JP
                PosX -= 1;
#endif
                break;
            }
            default:
            {
                break;
            }
        }
        
        // Values start at EDITOR_SELECT_CHANGE_ADDRESS
        ColorCondition = tempCurrentSelectionStatus & (EDITOR_SELECT_CHANGE_ADDRESS << i);
        
        Color = getSelectedTextColor(ColorCondition || 
            ((tempCurrentEditorMenuOption == i) && 
            !checkIfOtherHeaderOptionsSelected(i)));
        
        drawText(String, PosX, PosY, Color, Scale);
    }
    
    // Draw the current addresses displayed in the editor
    Color = 0xFFFFFFFF;
    PosX = InitialPosX;
    PosY -= 80;
    
    for (uint32_t i = 0; i < EDITOR_TOTAL_ROWS; i++)
    {
        sprintf(tempDisplayBuffer,
            "%08" PRIX32,
            reinterpret_cast<uint32_t>(CurrentAddress) + (i * EDITOR_BYTES_PER_ROW));
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        PosY -= 20;
    }
    
    // Draw the headers numbers indicating each byte in the current line in the editor
    PosX = InitialEditorPosX + 6; // Add a bit extra to start at at the middle of the 2 digits
    PosY = InitialEditorPosY;
    
    // Start at the last digit of the current address
    uint32_t HeaderNumber = reinterpret_cast<uint32_t>(CurrentAddress) & 0xF;
    for (uint32_t i = 0; i < EDITOR_BYTES_PER_ROW; i++)
    {
        sprintf(tempDisplayBuffer,
            "%" PRIX32,
            HeaderNumber);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        PosX += (DigitSpace * 2) + 4;
        
        // Increment and handle looping
        HeaderNumber++;
        if (HeaderNumber > 0xF)
        {
            HeaderNumber = 0;
        }
    }
    
    // Draw the bytes displayed in the editor
    uint32_t SelectedBytesCurrentDigit = 0;
    for (uint32_t i = 0; i < EDITOR_MAX_NUM_DIGITS_DISPLAYED; i++)
    {
        if ((i % EDITOR_DIGITS_PER_ROW) == 0)
        {
            // Go to the next line
            // This also handles setting the first line
            PosX = InitialEditorPosX;
            PosY = (InitialEditorPosY - 20) - ((i / EDITOR_DIGITS_PER_ROW) * 20);
        }
        
        // Set the default color to white
        Color = 0xFFFFFFFF;
        
        // Get the address of the current digit
        uint8_t *DigitAddress = getDigitAddress(
            CurrentAddress,
            EDITOR_MAX_NUM_BYTES_DISPLAYED,
            i);
        
        // Check which digit to get
        int32_t CurrentDigit;
        
        // Check to see if currently hovering over addresses
        if (tempCurrentSelectionStatus & EDITOR_SELECT_GOTO_EDITOR)
        {
            // Check if the current address is part of the hover address
            if (checkIfAddressInHoverRange(DigitAddress, SelectedBytesCurrentDigit))
            {
                // Check if the current digit address is valid
                bool DigitAddressIsValid = checkIfPointerIsValid(DigitAddress);
                
                // Check to see if an address has been selected
                if (tempCurrentSelectionStatus & EDITOR_SELECTED_BYTES_TO_EDIT)
                {
                    // Check if the current digit is being hovered over
                    if (MemoryEditor.CurrentEditorSelectedMenuOption == SelectedBytesCurrentDigit)
                    {
                        // Draw a window under the current digit
                        uint32_t WindowColor = 0x1C4F57FF;
                        int32_t WindowPosX = PosX - 2;
                        int32_t WindowPosY = PosY + 2;
                        int32_t Width = 13;
                        int32_t Height = 18;
                        int32_t Curve = 0;
                        
#ifdef TTYD_JP
                        WindowPosX -= 1;
                        WindowPosY -= 1;
#endif
                        
                        drawWindow(WindowColor, WindowPosX, WindowPosY, Width, Height, Curve);
                        
                        // Restart the text drawing sequence
                        drawTextInit(Alpha, false);
                        
                        // Set the text color
                        if (tempCurrentSelectionStatus & EDITOR_EDITING_BYTES)
                        {
                            // Make the text orange
                            Color = 0xFFA31AFF;
                            
                            if (!DigitAddressIsValid)
                            {
                                // Digit address is invalid, so set the digit to an invalid number
                                CurrentDigit = -1;
                            }
                            else
                            {
                                // Get the current digit of the edited bytes
                                CurrentDigit = getDigitValueFromRawAddress(
                                    MemoryEditor.BytesBeingEdited, 
                                    MemoryEditor.NumBytesBeingEdited, 
                                    SelectedBytesCurrentDigit);
                            }
                        }
                        else if (!DigitAddressIsValid)
                        {
                            // Digit address is invalid, so make the text green
                            Color = 0x2EE866FF;
                            
                            // Set the digit to an invalid number
                            CurrentDigit = -1;
                        }
                        else
                        {
                            // Make the text green
                            Color = 0x2EE866FF;
                            
                            // Get the current digit of the edited bytes
                            CurrentDigit = getDigitValueFromRawAddress(
                                MemoryEditor.BytesBeingEdited, 
                                MemoryEditor.NumBytesBeingEdited, 
                                SelectedBytesCurrentDigit);
                        }
                    }
                    else if (!DigitAddressIsValid)
                    {
                        // Digit address is invalid, so make the text red
                        Color = 0xFF1414FF;
                        
                        // Set the digit to an invalid number
                        CurrentDigit = -1;
                    }
                    else
                    {
                        // Make the text blue
                        Color = 0x5B59DEFF;
                        
                        // Get the current digit of the edited bytes
                        CurrentDigit = getDigitValueFromRawAddress(
                            MemoryEditor.BytesBeingEdited, 
                            MemoryEditor.NumBytesBeingEdited, 
                            SelectedBytesCurrentDigit);
                    }
                }
                else
                {
                    // Set the text color
                    if (!DigitAddressIsValid)
                    {
                        // Digit address is invalid, so make the text red
                        Color = 0xFF1414FF;
                        
                        // Set the digit to an invalid number
                        CurrentDigit = -1;
                    }
                    else
                    {
                        // Make the text blue
                        Color = 0x5B59DEFF;
                        
                        // Get the current digit of the current address
                        CurrentDigit = getDigitValue(
                            DigitAddress, 
                            EDITOR_MAX_NUM_BYTES_DISPLAYED, 
                            i);
                    }
                }
                
                SelectedBytesCurrentDigit++;
            }
            else
            {
                // Get the current digit of the current address
                CurrentDigit = getDigitValue(
                    DigitAddress, 
                    EDITOR_MAX_NUM_BYTES_DISPLAYED, 
                    i);
            }
        }
        else
        {
            // Get the current digit of the current address
            CurrentDigit = getDigitValue(
                DigitAddress, 
                EDITOR_MAX_NUM_BYTES_DISPLAYED, 
                i);
        }
        
        // Draw the current digit
        if ((CurrentDigit < 0) || (CurrentDigit > 0xF))
        {
            // Digit is invalid, so draw a ?
            drawText("?", PosX, PosY, Color, Scale);
        }
        else
        {
            // Digit is valid
            sprintf(tempDisplayBuffer,
                "%" PRIX32,
                CurrentDigit);
            
            drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        }
        
        // Add extra space between every 2 digits
        if ((i % 2) == 1)
        {
            PosX += 4;
        }
        
        PosX += DigitSpace;
    }
}

void drawMemoryEditor()
{
    // Draw the window for the editor
    drawMenuWindow();
    
    // Check for button inputs
    // Don't check if the adjustable value menu is open
    uint32_t Button = 0;
    
    if (!(MemoryEditor.CurrentSelectionStatus & 
        (EDITOR_SELECT_CHANGE_ADDRESS | EDITOR_SELECT_CHANGE_BYTES)))
    {
        Button = memoryEditorButtonControls();
    }
    
    // Check if the settings menu is open
    // Need to retrive CurrentSelectionStatus after memoryEditorButtonControls is called
    uint32_t CurrentSelectionStatus = MemoryEditor.CurrentSelectionStatus;
    if (CurrentSelectionStatus & EDITOR_OPEN_SETTINGS)
    {
        // Draw the settings menu
        drawMemoryEditorSettingsWindow();
    }
    else
    {
        // Draw the main window
        drawMemoryEditorMainWindow();
        
        // Draw the adjustable value menu if it's open
        if (CurrentSelectionStatus & (EDITOR_SELECT_CHANGE_ADDRESS | EDITOR_SELECT_CHANGE_BYTES))
        {
            // Don't draw on the first frame that it's enabled
            if (Button != MEMORY_EDITOR_ADJUSTABLE_VALUE)
            {
                drawAdjustableValueMemoryEditor(); // Also handles inputs
            }
        }
    }
}

void drawBattlesActorsList()
{
    uint32_t tempCurrentPage   = MenuVar.CurrentPage;
    uint32_t TotalMenuOptions  = 62; // Excluding System
    uint32_t MaxOptionsPerPage = 18;
    uint32_t IndexStart        = tempCurrentPage * MaxOptionsPerPage;
    
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = 180;
    float Scale      = 0.6f;
    uint32_t Color;
    
    // Draw the main text
    for (uint32_t i = IndexStart; i < (IndexStart + MaxOptionsPerPage); i++)
    {
        if (i >= TotalMenuOptions)
        {
            // Reached the end of the array, so exit
            return;
        }
        
        // Get the current actor out
        uint32_t CurrentActorPointer = reinterpret_cast<uint32_t>(getActorPointer(i + 1)); // Add 1 to skip System
        bool SlotIsEmpty = false;
        const char *CurrentActorString;
        
        if (!CurrentActorPointer)
        {
            CurrentActorString = "Empty Slot";
            SlotIsEmpty = true;
        }
        else
        {
            uint32_t CurrentActor = *reinterpret_cast<uint32_t *>(CurrentActorPointer + 0x8);
            
            // Make sure the current value is valid
            if (CurrentActor > 0)
            {
                CurrentActorString = BattlesActorsLines[CurrentActor - 1];
            }
            else
            {
                CurrentActorString = "Invalid Id";
            }
        }
        
        if (MenuVar.CurrentMenuOption == i)
        {
            Color = 0x5B59DEFF;
        }
        else if (SlotIsEmpty)
        {
            Color = 0x4B4B4BFF;
        }
        else
        {
            Color = 0xFFFFFFFF;
        }
        
        drawText(CurrentActorString, PosX, PosY, Color, Scale);
        PosY -= 20;
    }
}

void drawBattlesStatusesList()
{
    // Get the starting address for the current actor
    uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuVar.MenuSelectedOption));
    if (!ActorAddress)
    {
        return;
    }
    
    uint32_t MaxOptionsPerPage = 12;
    uint32_t IndexStart        = MenuVar.CurrentPage * MaxOptionsPerPage;
    uint32_t TotalOptions      = BattlesStatusesLinesSize;
    
    uint8_t Alpha    = 0xFF;
    float TextScale  = 0.6f;
    float IconScale  = 0.6f;
    int32_t TextPosX = -210;
    int32_t TextPosY = 175;
    int32_t IconPosX = TextPosX - 15;
    int32_t IconPosY = TextPosY - 20;
    
    // Set up array to use for displaying icons
    int32_t IconPosition[3];
    const int32_t IconPositionX = 0;
    const int32_t IconPositionY = 1;
    IconPosition[2]             = 0;
    IconPosition[IconPositionX] = IconPosX;
    IconPosition[IconPositionY] = IconPosY;
    
    // Get the initial address offset
    uint32_t DisplayValueCounter = 0;
    
    if (IndexStart >= 20) // Explosion turns left
    {
        DisplayValueCounter += 3;
    }
    else if (IndexStart >= 18) // Can't use items
    {
        DisplayValueCounter += 2;
    }
    else if (IndexStart >= 1) // Sleep flags
    {
        DisplayValueCounter++;
    }
    
    // Draw the text and icons
    uint32_t CurrentMenuOption = MenuVar.CurrentMenuOption;
    char *tempDisplayBuffer = DisplayBuffer;
    uint32_t Color;
    
    // Draw the icons
    uint32_t DrawWindowCounter = 0;
    for (uint32_t i = IndexStart; i < (IndexStart + MaxOptionsPerPage); i++)
    {
        if (i >= TotalOptions)
        {
            // Reached the end of the array, so exit
            break;
        }
        
        bool CurrentOptionCheck = CurrentMenuOption == i;
        if (CurrentOptionCheck)
        {
            // Draw a window for the current line
            uint32_t WindowColor = 0x151515E0;
            int32_t WindowPosX = -245;
            int32_t WindowPosY = 185 - (DrawWindowCounter * 30);
            int32_t Width = 363;
            int32_t Height = 35;
            int32_t Curve = 0;
            drawWindow(WindowColor, WindowPosX, WindowPosY, Width, Height, Curve);
        }
        
        // Draw the icon
        drawIcon(IconPosition, BattlesStatusesIcons[i], IconScale);
        
        IconPosition[IconPositionY] -= 30;
        DrawWindowCounter++;
    }
    
    // Restart the text drawing sequence
    drawTextInit(Alpha, false);
    
    // Draw the main text
    for (uint32_t i = IndexStart; i < (IndexStart + MaxOptionsPerPage); i++)
    {
        if (i >= TotalOptions)
        {
            // Reached the end of the array, so exit
            break;
        }
        
        bool CurrentOptionCheck = CurrentMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(BattlesStatusesLines[i], TextPosX, TextPosY, Color, TextScale);
        
        // Get the current value
        // Skip specific values
        switch (i)
        {
            case 1: // Sleep flags
            case 18: // Can't use items
            case 20: // Explosion turns left
            {
                DisplayValueCounter++;
                break;
            }
            default:
            {
                break;
            }
        }
        
        int8_t CurrentValue = *reinterpret_cast<int8_t *>(
            ActorAddress + 0x118 + i + DisplayValueCounter);
        
        // Draw the text of the value
        const char *TextToDraw;
        
        if (i == (TotalOptions - 1))
        {
            // Drawing the defeated flag
            getOnOffTextAndColor(CurrentValue, &TextToDraw, &Color);
        }
        else
        {
            // Not drawing the defeated flag
            sprintf(tempDisplayBuffer,
                "%" PRId8,
                CurrentValue);
            
            TextToDraw = tempDisplayBuffer;
            Color = 0xFFFFFFFF;
        }
        
        drawText(TextToDraw, TextPosX + 290, TextPosY, Color, TextScale);
        TextPosY -= 30;
    }
}

void drawErrorWindow(const char *message, int32_t textPosX, int32_t textPosY, int32_t windowWidth)
{
    // int32_t TextPosX    = -200;
    // int32_t TextPosY    = 60;
    uint8_t Alpha          = 0xFF;
    uint32_t TextColor     = 0xFFFFFFFF;
    float Scale            = 0.6f;
    // int32_t WindowWidth = 415;
    uint32_t WindowColor   = 0x151515F4;
    int32_t WindowCurve    = 20;
    
    drawTextWithWindow(message, textPosX, textPosY, Alpha, 
        TextColor, Scale, windowWidth, WindowColor, WindowCurve);
}

void drawErrorWindowAutoWidth(const char *message, int32_t textPosX, int32_t textPosY)
{
    if ((MenuVar.FunctionReturnCode < 0) && (MenuVar.Timer > 0))
    {
        if (checkForClosingErrorMessage())
        {
            MenuVar.SelectedOption = 0;
            return;
        }
        
        // int32_t TextPosX = -130;
        // int32_t TextPosY = 60;
        float Scale         = 0.6f;
        int32_t WindowWidth = static_cast<int32_t>(getMessageWidth(message, Scale));
        
        drawErrorWindow(message, textPosX, textPosY, WindowWidth);
    }
    else
    {
        resetAndCloseSecondaryMenu();
    }
}

void drawPartnerFollowerMessage(int32_t textPosY, bool drawForPartner)
{
    if (!checkErrorMessageReqs())
    {
        return;
    }
    
    int32_t TextPosX = -172;
    int32_t WindowWidth = 375;
    
    // Get the text to use
    const char *PartnerOrFollowerText;
    if (drawForPartner)
    {
        PartnerOrFollowerText = "partner";
        
#ifdef TTYD_JP
        TextPosX += 3;
        WindowWidth -= 7;
#endif
    }
    else
    {
        PartnerOrFollowerText = "follower";
        
#ifdef TTYD_JP
        WindowWidth -= 1;
#endif
    }
    
    // Print error text if currently trying to spawn a partner/follower when not able to
    char *tempDisplayBuffer = DisplayBuffer;
    
    sprintf(tempDisplayBuffer,
        "To spawn a %s, you must have a file\nloaded and not be in a battle nor a\nscreen transition.",
        PartnerOrFollowerText);
    
    drawErrorWindow(tempDisplayBuffer, TextPosX, textPosY, WindowWidth);
}

void drawNotInBattleErrorMessage()
{
    if (!checkErrorMessageReqs())
    {
        return;
    }
    
    const char *CurrentLine = "You must be in a battle to use the Battles menu.";
    int32_t TextPosX        = -205;
    int32_t TextPosY        = 0;
    int32_t WindowWidth     = 440;
    
#ifdef TTYD_JP
    TextPosX += 6;
    WindowWidth -= 13;
#endif
    
    drawErrorWindow(CurrentLine, TextPosX, TextPosY, WindowWidth);
}

void drawMarioCoordinatesErrorMessage()
{
    if (!checkErrorMessageReqs())
    {
        return;
    }
    
    const char *CurrentLine = "This value must be modified as a hex value.";
    int32_t TextPosX = -181;
    int32_t TextPosY = -17;
    int32_t WindowWidth = 392;
    
#ifdef TTYD_JP
    TextPosX += 1;
    WindowWidth -= 7;
#endif
    
    drawErrorWindow(CurrentLine, TextPosX, TextPosY, WindowWidth);
}

void drawResolveFadesMessage()
{
    int32_t tempFunctionReturnCode = MenuVar.FunctionReturnCode;
    if (tempFunctionReturnCode == 0)
    {
        return;
    }
    
    if (MenuVar.Timer == 0)
    {
        return;
    }
    
    if (checkForClosingErrorMessage())
    {
        return;
    }
    
    int32_t TextPosX;
    int32_t WindowWidth;
    const char *Message;
    
    switch (tempFunctionReturnCode)
    {
        case FADE_RESOLVE_SUCCESS:
        {
            TextPosX = -188;
            WindowWidth = 404;
            Message = "The selected fade was successfully resolved.";
            
#ifdef TTYD_JP
            TextPosX += 2;
            WindowWidth -= 2;
#endif
            break;
        }
        case FADE_NOT_ACTIVE:
        {
            TextPosX = -135;
            WindowWidth = 299;
            Message = "The selected fade is not active.";
            
#ifdef TTYD_JP
            TextPosX += 5;
            WindowWidth -= 9;
#endif
            break;
        }
        case FADE_DONT_RESOLVE:
        {
            TextPosX = -205;
            WindowWidth = 440;
            Message = "The selected fade does not need to be resolved.";
            
#ifdef TTYD_JP
            TextPosX += 5;
            WindowWidth -= 11;
#endif
            break;
        }
        default:
        {
            return;
        }
    }
    
    int32_t TextPosY = 5;
    drawErrorWindow(Message, TextPosX, TextPosY, WindowWidth);
}

void drawWarpsErrorMessage(int32_t textPosY)
{
    if (!checkErrorMessageReqs())
    {
        return;
    }
    
    // Print error text if currently trying to warp when not able to
    const char *CurrentLine = "To warp, you must have a file loaded and not\nbe in a battle nor a screen transition.";
    int32_t TextPosX        = -195;
    int32_t WindowWidth     = 415;
    
#ifdef TTYD_JP
    TextPosX += 8;
    WindowWidth -= 13;
#endif
    
    drawErrorWindow(CurrentLine, TextPosX, textPosY, WindowWidth);
}

void drawConfirmationWindow(const char *message)
{
    // Manage the D-Pad
    uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    
    if (ButtonInputTrg & PAD_DPAD_DOWN)
    {
        if (tempSecondaryMenuOption == 1)
        {
            MenuVar.SecondaryMenuOption = 0;
        }
        else
        {
            MenuVar.SecondaryMenuOption = 1;
        }
    }
    else if (ButtonInputTrg & PAD_DPAD_UP)
    {
        if (tempSecondaryMenuOption == 0)
        {
            MenuVar.SecondaryMenuOption = 1;
        }
        else
        {
            MenuVar.SecondaryMenuOption = 0;
        }
    }
    
    // Draw the window
    uint32_t Color = 0x151515F4;
    int32_t PosX   = -245;
    int32_t PosY   = 110;
    int32_t Width  = 490;
    int32_t Height = 220;
    int32_t Curve  = 0;
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Draw the confirmation message
    Color         = 0xFFFFFFFF;
    uint8_t Alpha = 0xFF;
    float Scale   = 0.6f;
    PosX          += 70;
    PosY          -= 40;
    
#ifdef TTYD_JP
    PosX += 7;
#endif
    
    drawTextAndInit(message, PosX, PosY, Alpha, Color, false, Scale);
    
    // Draw the yes/no options
    PosX += 155;
    PosY -= 110;
    Scale = 0.8f;
    const char *Option;
    
#ifdef TTYD_JP
    PosX -= 5;
#endif
    
    for (uint32_t i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            Option = "Yes";
        }
        else
        {
            Option = "No";
        }
        
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(Option, PosX, PosY, Color, Scale);
        PosY -= 23;
    }
}

void drawSingleLineFromArray(int32_t posX, int32_t posY, 
    int32_t currentMenuOption, const char **line)
{
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    const char *CurrentLine = line[currentMenuOption];
    
    drawText(CurrentLine, posX, posY, Color, Scale);
}

void drawSingleLineFromStringAndValue(int32_t posX, 
    int32_t posY, const char *line, int32_t value)
{
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "%s%" PRId32,
        line,
        value);
    
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    
    drawText(tempDisplayBuffer, posX, posY, Color, Scale);
}

void drawSingleLineFromStringAndArray(int32_t posX, int32_t posY, 
    int32_t currentMenuOption, const char *string, const char **line)
{
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "%s%s",
        string,
        line[currentMenuOption]);
    
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    
    drawText(tempDisplayBuffer, posX, posY, Color, Scale);
}

void drawAdjustableValueSpawnItem()
{
    drawAdjustableValue(true, SPAWN_ITEM_MENU_VALUE);
}

void drawAdjustableValueMemoryEditor()
{
    if (MemoryEditor.CurrentEditorMenuOption == EDITOR_HEADER_CHANGE_ADDRESS)
    {
        drawAdjustableValueHex(MEMORY_EDITOR_MENU);
    }
    else
    {
        drawAdjustableValue(false, MEMORY_EDITOR_MENU);
    }
}

void drawAdjustableValue(bool changingItem, uint32_t currentMenu)
{
    // Check for button inputs
    // Don't check if the current menu is for spawning items
    if (currentMenu != SPAWN_ITEM_MENU_VALUE)
    {
        uint32_t ReturnCode = adjustableValueButtonControls(currentMenu);
        switch (ReturnCode)
        {
            case NO_NUMBERS_TO_DISPLAY: // There are no numbers to display, so close the menu
            {
                return;
            }
            default:
            {
                break;
            }
        }
    }
    
    // Adjust the current value if necessary
    adjustMenuItemBounds(0, currentMenu);
    
    // Draw the window
    uint32_t color = 0x151515E0;
    int32_t PosX   = -189;
    int32_t y      = 105;
    int32_t width  = 377;
    int32_t curve  = 10;
    int32_t height;
    
#ifdef TTYD_JP
    PosX += 4;
    width -= 6;
#endif
    
    uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
    
    if (changingItem)
    {
        height = 208;
    }
    else if (currentMenu == CHEATS_CHANGE_SEQUENCE)
    {
        height = 235;
    }
    else if (currentMenu == WARPS_EVENT)
    {
        y += 62;
        height = 330;
    }
    else if (currentMenu == WARPS_INDEX)
    {
        if ((tempCurrentMenuOption + 1) == INDEX_SELECT_MAP)
        {
            height = 212;
        }
        else
        {
            height = 173;
        }
    }
    else
    {
        height = 173;
    }
    
    drawWindow(color, PosX, y, width, height, curve);
    
    // Draw the help text
    uint8_t alpha = 0xFF;
    float scale   = 0.6f;
    int32_t x     = PosX + 15;
    y             -= 13;
    color         = 0xFFFFFFFF;
    const char *HelpText = "Press D-Pad Up/Down to adjust the value\nPress D-Pad Left/Right to change digits\nPress Y to set the value to max\nPress Z to set the value to min\nPress A to confirm\nPress B to cancel";
    drawTextAndInit(HelpText, x, y, alpha, color, false, scale);
    
    int32_t tempMenuSecondaryValue = MenuVar.MenuSecondaryValue;
    y -= 100;
    
    char *tempDisplayBuffer = DisplayBuffer;
    if (changingItem)
    {
        int32_t IconPosX = x + 10;
        y -= 50;
        
        // Draw the icon for the current item
        // Set up array to use for displaying icons
        int32_t IconPosition[3];
        const int32_t IconPositionX = 0;
        const int32_t IconPositionY = 1;
        IconPosition[IconPositionX] = IconPosX;
        IconPosition[IconPositionY] = y;
        IconPosition[2]             = 0;
        
        float IconScale = 0.6f;
        
        if ((tempMenuSecondaryValue >= ttyd::item_data::Item::PaperModeCurse) && 
            (tempMenuSecondaryValue <= ttyd::item_data::Item::UltraHammer))
        {
            // Decrease the size of these icons
            IconScale = 0.5f;
        }
        else if (tempMenuSecondaryValue == ttyd::item_data::Item::MagicalMapBigger)
        {
            // Decrease the size of the bigger magical map
            IconScale = 0.4f;
        }
        
        // Draw the icon for the item
        int16_t CurrentItem = static_cast<int16_t>(tempMenuSecondaryValue);
        drawIconFromItem(IconPosition, CurrentItem, IconScale);
        
        // Draw the text for the item
        color = 0xFFFFFFFF;
        IconPosX += 17;
        y += 18;
        const char *ItemName = getItemName(CurrentItem);
        
        drawTextAndInit(ItemName, IconPosX, y, alpha, color, false, scale);
    }
    else if (currentMenu == CHEATS_CHANGE_SEQUENCE)
    {
        // Draw the Stage and Event for the current Sequence value
        const char *StageAndEventNames[2];
        
#ifdef TTYD_JP
        char StageNameBuffer[8];
        getSequenceStageAndEvent(StageAndEventNames, StageNameBuffer, 
            static_cast<uint32_t>(tempMenuSecondaryValue));
#else
        getSequenceStageAndEvent(StageAndEventNames, 
            static_cast<uint32_t>(tempMenuSecondaryValue));
#endif
        
        sprintf(tempDisplayBuffer,
            "%s\n%s",
            StageAndEventNames[0], // Stage name
            StageAndEventNames[1]); // Event name
        
        int32_t NamesPosY = y - 35;
        drawText(tempDisplayBuffer, x + 80, NamesPosY, color, scale);
        
        const char *String = "Stage\nEvent";
        drawText(String, x, NamesPosY, color, scale);
        
        y -= 60;
    }
    else if (currentMenu == WARPS_EVENT)
    {
        // Draw the details for the current event
        drawEventDetails(x, y - 35, tempMenuSecondaryValue);
        y -= 152;
    }
    else if ((currentMenu == WARPS_INDEX) && 
        ((tempCurrentMenuOption + 1) == INDEX_SELECT_MAP))
    {
        sprintf(tempDisplayBuffer,
            "Map: %s",
            getMapFromIndex(tempMenuSecondaryValue));
        
        drawText(tempDisplayBuffer, x, y - 35, color, scale);
        y -= 35;
    }
    
    // Check if the number is negative
    bool NumberIsNegative = false;
    
    if (tempMenuSecondaryValue < 0)
    {
        // Convert the number to positive
        NumberIsNegative = true;
        tempMenuSecondaryValue = -tempMenuSecondaryValue;
    }
    
    // Handle the value as unsigned - needed to avoid a display bug with the value being 0x80000000
    uint32_t tempMenuSecondaryValueUnsigned = static_cast<uint32_t>(tempMenuSecondaryValue);
    
    // Get the amount of numbers to draw
    uint32_t AmountOfNumbers = getHighestAdjustableValueDigit(currentMenu);
    
    // Set up array for each digit of the number
    uint8_t AdjustableValue[AmountOfNumbers];
    
    // Get the values for the array
    for (uint32_t i = 0; i < AmountOfNumbers; i++)
    {
        AdjustableValue[AmountOfNumbers - i - 1] = tempMenuSecondaryValueUnsigned % 10;
        tempMenuSecondaryValueUnsigned /= 10;
    }
    
    /*AdjustableValue[0] = tempMenuSecondaryValueUnsigned / 100;
    AdjustableValue[1] = (tempMenuSecondaryValueUnsigned % 100) / 10;
    AdjustableValue[2] = (tempMenuSecondaryValueUnsigned % 100) % 10;*/
    
    x += 166;
    y -= 30;
    
#ifdef TTYD_JP
    scale = 1.f;
    x -= 1;
    y += 3;
#else
    scale = 0.9f;
#endif
    
    // Calculate the X offset
    int32_t tempPosX = 0;
    for (uint32_t i = 0; i < (AmountOfNumbers - 1); i++)
    {
        tempPosX += 17;
    }
    x -= (tempPosX / 2);
    
    // Draw the negative sign if the current value is negative
    if (NumberIsNegative)
    {
        // Draw the negative sign
        int32_t NegativeSignOffset = 10;
        
#ifdef TTYD_JP
        NegativeSignOffset += 2;
#endif
        
        drawText("-", (x - NegativeSignOffset), y, color, scale);
    }
    
    // Draw each digit of the number
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    for (uint32_t i = 0; i < AmountOfNumbers; i++)
    {
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        color = getSelectedTextColor(CurrentOptionCheck);
        
        sprintf(tempDisplayBuffer,
            "%" PRIu8,
            AdjustableValue[i]);

        drawText(tempDisplayBuffer, x, y, color, scale);
        x += 17;
    }
}

void drawAdjustableValueHex(uint32_t currentMenu)
{
    // Check for button inputs
    uint32_t ReturnCode;
    if (currentMenu == MEMORY_WATCH_CHANGE_ADDRESS)
    {
        ReturnCode = adjustWatchValueControls(MenuVar.MenuSelectedOption);
    }
    else
    {
        ReturnCode = adjustableValueButtonControls(currentMenu);
    }
    
    switch (ReturnCode)
    {
        case NO_NUMBERS_TO_DISPLAY: // There are no numbers to display, so close the menu
        {
            return;
        }
        default:
        {
            break;
        }
    }
    
    // Adjust the current value if necessary
    if ((currentMenu != CHEATS_MODIFY_COORDINATES) && 
        (currentMenu != MEMORY_WATCH_CHANGE_ADDRESS))
    {
        adjustMenuItemBounds(0, currentMenu);
    }
    
    // Draw the window
    uint32_t color = 0x151515E0;
    int32_t PosX   = -189;
    int32_t y      = 105;
    int32_t width  = 377;
    int32_t curve  = 10;
    int32_t height = 148;
    
#ifdef TTYD_JP
    PosX += 4;
    width -= 6;
#endif
    
    drawWindow(color, PosX, y, width, height, curve);
    
    // Draw the help text
    uint8_t alpha = 0xFF;
    float scale   = 0.6f;
    int32_t x     = PosX + 15;
    y             -= 13;
    color         = 0xFFFFFFFF;
    const char *HelpText = "Press D-Pad Up/Down to adjust the value\nPress D-Pad Left/Right to change digits\nPress A to confirm\nPress B to cancel";
    drawTextAndInit(HelpText, x, y, alpha, color, false, scale);
    
    uint32_t tempMenuSecondaryValueUnsigned = MenuVar.MenuSecondaryValueUnsigned;
    int32_t tempMenuSecondaryValue;
    y -= 100;
    
    // Check if the number is negative and get the amount of numbers to draw
    bool NumberIsNegative = false;
    uint32_t AmountOfNumbers;
    
    switch (currentMenu)
    {
        case CHEATS_MODIFY_COORDINATES:
        {
            AmountOfNumbers = 8;
            break;
        }
        case MEMORY_WATCH_CHANGE_ADDRESS:
        {
            AmountOfNumbers = 8;
            
            switch (MenuVar.CurrentMenuOption)
            {
                case 0:
                {
                    // Modifying the address, which can never be 0
                    break;
                }
                default:
                {
                    // Modifying the pointer offsets, which can be negative
                    tempMenuSecondaryValue = static_cast<int32_t>(tempMenuSecondaryValueUnsigned);
                    if (tempMenuSecondaryValue < 0)
                    {
                        NumberIsNegative = true;
                    }
                }
            }
            break;
        }
        case MEMORY_EDITOR_MENU:
        {
            AmountOfNumbers = 8;
            
            // Values can never be negative, so no further adjustments needed
            break;
        }
        default:
        {
            return;
        }
    }
    
    // Set up array for each digit of the number
    uint8_t AdjustableValue[AmountOfNumbers];
    
    x += 180;
    
#ifdef TTYD_JP
    scale = 1.f;
    x -= 1;
    y += 2;
#else
    scale = 0.9f;
#endif
    
    // Calculate the X offset
    int32_t tempPosX = 0;
    for (uint32_t i = 0; i < (AmountOfNumbers - 1); i++)
    {
        tempPosX += 17;
    }
    x -= (tempPosX / 2);
    
    if (NumberIsNegative)
    {
        // Convert the number to positive
        tempMenuSecondaryValue = -tempMenuSecondaryValue;
        
        // Get the values for the array
        for (uint32_t i = 0; i < AmountOfNumbers; i++)
        {
            AdjustableValue[AmountOfNumbers - i - 1] = tempMenuSecondaryValue % 0x10;
            tempMenuSecondaryValue /= 0x10;
        }
        
        // Draw the negative sign
        int32_t NegativeSignOffset = 40;
        
#ifdef TTYD_JP
        NegativeSignOffset += 2;
#endif
        
        drawText("-", (x - NegativeSignOffset), y, color, scale);
    }
    else
    {
        // Get the values for the array
        for (uint32_t i = 0; i < AmountOfNumbers; i++)
        {
            AdjustableValue[AmountOfNumbers - i - 1] = tempMenuSecondaryValueUnsigned % 0x10;
            tempMenuSecondaryValueUnsigned /= 0x10;
        }
    }
    
    // Draw the 0x
    drawText("0x", (x - 30), y, color, scale);
    
    // Draw each digit of the number
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    for (uint32_t i = 0; i < AmountOfNumbers; i++)
    {
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        color = getSelectedTextColor(CurrentOptionCheck);
        
        char *tempDisplayBuffer = DisplayBuffer;
        sprintf(tempDisplayBuffer,
            "%01" PRIX8,
            AdjustableValue[i]);

        drawText(tempDisplayBuffer, x, y, color, scale);
        x += 17;
    }
}

void drawAdjustableValueDouble(uint32_t currentMenu, const char *value)
{
    // Check for button inputs
    uint32_t ReturnCode;
    if (currentMenu == CHEATS_MODIFY_COORDINATES)
    {
        ReturnCode = adjustableValueButtonControls(currentMenu);
    }
    else
    {
        // Nothing else currently uses this yet
        return;
    }
    
    switch (ReturnCode)
    {
        case NO_NUMBERS_TO_DISPLAY: // There are no numbers to display, so close the menu
        {
            return;
        }
        default:
        {
            break;
        }
    }
    
    // Draw the window
    uint32_t Color = 0x151515E0;
    int32_t PosX   = -189;
    int32_t PosY   = 72;
    int32_t Width  = 377;
    int32_t Curve  = 10;
    int32_t Height = 138;
    
#ifdef TTYD_JP
    PosX += 4;
    Width -= 6;
#endif
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Draw the help text
    uint8_t Alpha = 0xFF;
    float Scale   = 0.6f;
    PosX          += 15;
    PosY          -= 13;
    Color         = 0xFFFFFFFF;
    const char *HelpText = "Press D-Pad Up/Down to adjust the value\nPress D-Pad Left/Right to change digits\nPress A to confirm\nPress B to cancel";
    drawTextAndInit(HelpText, PosX, PosY, Alpha, Color, false, Scale);
    
    PosX += 166;
    PosY -= 90;
    
#ifdef TTYD_JP
    Scale = 1.f;
    PosX -= 1;
    PosY += 3;
#else
    Scale = 0.9f;
#endif
    
    uint32_t StringLength = strlen(value);
    
    // Calculate the X offset
    int32_t tempPosX = 0;
    for (uint32_t i = 0; i < (StringLength - 1); i++)
    {
        tempPosX += 17;
    }
    PosX -= (tempPosX / 2);
    
    // Draw each digit of the number
    uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
    
    char *tempDisplayBuffer = DisplayBuffer;
    for (uint32_t i = 0; i < StringLength; i++)
    {
        bool CurrentOptionCheck = tempSecondaryMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        char CurrentChar = value[i];
        sprintf(tempDisplayBuffer,
            "%c",
            CurrentChar);
        
        // If drawing the decimal point, move it further right
        tempPosX = PosX;
        if (CurrentChar == '.')
        {
            tempPosX += 5;
        }
        
        drawText(tempDisplayBuffer, tempPosX, PosY, Color, Scale);
        PosX += 17;
    }
}

void drawAddByIconMain(uint32_t currentMenu)
{
    // Check for button inputs
    addByIconButtonControls(currentMenu);
    
    // Draw the main window
    int32_t UpperAndLowerBounds[2];
    getUpperAndLowerBounds(UpperAndLowerBounds, MenuVar.MenuSelectedOption);
    
    int32_t LowerBound         = UpperAndLowerBounds[0];
    int32_t UpperBound         = UpperAndLowerBounds[1];
    
    uint32_t MaxIconsPerRow    = 16;
    uint32_t IconSpaceOccupied = 30;
    
    uint32_t Color             = 0x151515E0;
    // uint32_t Color          = 0x000000E0;
    int32_t PosX               = -245;
    int32_t PosY               = 160;
    int32_t Width              = 490;
    int32_t TotalIcons         = UpperBound - LowerBound + 1;
    int32_t TotalLines         = 1 + ((TotalIcons - 1) / MaxIconsPerRow); // Round up;
    int32_t Height             = (TotalLines * 34) + 40;
    int32_t Curve              = 0;
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Draw the window for the current menu option
    uint32_t tempCurrentMenuOption = MenuVar.SecondaryMenuOption;
    Color  = 0x5B59DEC0;
    PosY   = 96 - ((tempCurrentMenuOption / MaxIconsPerRow) * IconSpaceOccupied);
    Width  = 29;
    Height = 29;
    
    if (tempCurrentMenuOption >= MaxIconsPerRow)
    {
        PosX = -239 + ((tempCurrentMenuOption % MaxIconsPerRow) * IconSpaceOccupied);
    }
    else
    {
        PosX = -239 + (tempCurrentMenuOption * IconSpaceOccupied);
    }
    
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Set up array to use for displaying icons
    int32_t IconPosition[3];
    const int32_t IconPositionX = 0;
    const int32_t IconPositionY = 1;
    IconPosition[2]             = 0;
    
    int32_t tempIconPositionX   = -225;
    int32_t tempIconPositionY   = 70;
    IconPosition[IconPositionY] = tempIconPositionY;
    
    // Draw all of the icons for the current menu
    float Scale = 0.6f;
    uint32_t Counter = 0;
    
    for (int32_t i = LowerBound; i <= UpperBound; i++)
    {
        if ((i >= ttyd::item_data::Item::PaperModeCurse) && 
            (i <= ttyd::item_data::Item::UltraHammer))
        {
            // Decrease the size of these icons
            Scale = 0.5f;
        }
        else if (i == ttyd::item_data::Item::MagicalMapBigger)
        {
            // Decrease the size of the bigger magical map
            Scale = 0.4f;
        }
        else
        {
            Scale = 0.6f;
        }
        
        IconPosition[IconPositionX] = tempIconPositionX;
        drawIconFromItem(IconPosition, static_cast<int16_t>(i), Scale);
        
        tempIconPositionX += IconSpaceOccupied;
        
        if (Counter >= (MaxIconsPerRow - 1))
        {
            Counter = 0;
            tempIconPositionX = -225;
            tempIconPositionY -= IconSpaceOccupied;
            IconPosition[IconPositionY] = tempIconPositionY;
        }
        else
        {
            Counter++;
        }
    }
    
    // Draw the help text
    uint8_t Alpha           = 0xFF;
    int32_t HelpTextPosX    = -75;
    int32_t HelpTextPosPosY = 145;
    Color                   = 0xFFFFFFFF;
    const char *HelpText    = "Press A to confirm\nPress B to cancel";
    
#ifdef TTYD_JP
    HelpTextPosX -= 2;
#endif
    
    drawTextAndInit(HelpText, HelpTextPosX, HelpTextPosPosY, Alpha, Color, false, Scale);
}

void drawAddByIcon(uint32_t currentMenu)
{
    if (getFreeSlotPointer())
    {
        drawAddByIconMain(currentMenu);
    }
    else
    {
        const char *CurrentLine = "The inventory is currently full.";
        int32_t TextPosX = -130;
        int32_t TextPosY = 40;
        drawErrorWindowAutoWidth(CurrentLine, TextPosX, TextPosY);
    }
}

void drawAddById(uint32_t currentMenu)
{
    if (getFreeSlotPointer())
    {
        drawAdjustableValue(true, currentMenu);
    }
    else
    {
        const char *CurrentLine = "The inventory is currently full.";
        int32_t TextPosX = -130;
        int32_t TextPosY = 40;
        drawErrorWindowAutoWidth(CurrentLine, TextPosX, TextPosY);
    }
}

void drawVersionNumber(int32_t posX, int32_t posY)
{
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    
    drawText(VersionNumber, posX, posY, Color, Scale);
}

void drawPageNumber(int32_t posX, int32_t posY, uint32_t currentPage)
{
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Page %" PRIu32,
        currentPage + 1);
    
    drawText(tempDisplayBuffer, posX, posY, Color, Scale);
}

void drawBoolOnOrOff(bool tempBool, const char *currentLine, int32_t posY)
{
    // uint8_t Alpha = 0xFF;
    uint32_t Color   = 0xFFFFFFFF;
    int32_t PosX     = -232;
    float Scale      = 0.6f;
    
    // Draw the current option
    drawText(currentLine, PosX, posY, Color, Scale);
    
    // Draw the bool value
    const char *StringValue;
    getOnOffTextAndColor(tempBool, &StringValue, &Color);
    
    posY -= 20;
    drawText(StringValue, PosX, posY, Color, Scale);
}

void drawCheatsChangeSequence()
{
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = 140;
    float Scale = 0.6f;
    
    uint32_t SequencePosition = getSequencePosition();
    
    // Draw the text for showing what the current Sequence value is
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Current Value: %" PRIu32,
        SequencePosition);
    
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
    
    // Draw the Stage and Event names for the current Sequence value
    const char *StageAndEventNames[2];
    
#ifdef TTYD_JP
    char StageNameBuffer[8];
    if (!getSequenceStageAndEvent(StageAndEventNames, StageNameBuffer, SequencePosition))
#else
    if (!getSequenceStageAndEvent(StageAndEventNames, SequencePosition))
#endif
    {
        return;
    }
    
    sprintf(tempDisplayBuffer,
        "%s\n%s",
        StageAndEventNames[0], // Stage name
        StageAndEventNames[1]); // Event name
    
    PosY -= 40;
    drawText(tempDisplayBuffer, PosX + 80, PosY, Color, Scale);
    
    const char *String = "Stage\nEvent";
    drawText(String, PosX, PosY, Color, Scale);
}

void drawCheatsModifyMarioCoordinates()
{
    uint32_t Color;
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = 80;
    float Scale = 0.6f;
    
    // Draw the value for the bool
    const char *YesNoText;
    getYesNoTextAndColor(ChangeMarioCoordinates.ModifyAsHex, &YesNoText, &Color);
    drawText(YesNoText, PosX + 130, PosY + 40, Color, Scale);
    
    // Draw each of the coordinates, both as a float and as hex
    Color = 0xFFFFFFFF;
    char CoordinateLetter = 'X';
    char *tempDisplayBuffer = DisplayBuffer;
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    
    for (uint32_t i = 0; i < 3; i++)
    {
        float Coordinate = player->playerPosition[i];
        
        // Get the coordinate in text form first
        const char *CoordinateText;
        char CoordinateStringBuffer[128];
        
        int32_t StringLength = sprintf(
            CoordinateStringBuffer,
            "%.6f",
            Coordinate);
        
        // Make sure the text wont go offscreen
        int32_t CoordinateMaxStringLength = 34;
        
#ifdef TTYD_JP
        CoordinateMaxStringLength += 4;
#endif

        if ((StringLength <= 0) || (StringLength > CoordinateMaxStringLength))
        {
            CoordinateText = "Too Big";
        }
        else
        {
            CoordinateText = CoordinateStringBuffer;
        }
        
        // Get the actual text to draw
        uint32_t *CoordinateHex = reinterpret_cast<uint32_t *>(&Coordinate);
        sprintf(tempDisplayBuffer,
            "%c = %s, 0x%08" PRIX32,
            CoordinateLetter,
            CoordinateText,
            *CoordinateHex);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        CoordinateLetter++;
        PosY -= 20;
    }
}

void drawCheatsBool(int32_t posY)
{
    uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
    
    bool CheatActive = Cheat[tempMenuSelectedOption].Active;
    const char *CurrentLine = CheatsLines[tempMenuSelectedOption];
    
    drawBoolOnOrOff(CheatActive, CurrentLine, posY);
}

void drawButtonCombo(uint32_t buttonCombo, int32_t posY, const char *description)
{
    // uint8_t Alpha = 0xFF;
    uint32_t Color   = 0xFFFFFFFF;
    int32_t PosX     = -232;
    float Scale      = 0.6f;
    
    // Draw the text for telling what the button combo is
    drawText(description, PosX, posY, Color, Scale);
    
    // Draw the button combo
    uint8_t ButtonArray[14]; // Extra spot for 0 at the end of the array
    clearMemory(ButtonArray, sizeof(ButtonArray));
    getButtonsPressed(ButtonArray, buttonCombo);
    
    // uint8_t *ButtonArray = nullptr;
    // getButtonsPressed(ButtonArray);
    
    char ButtonString[128];
    createButtonStringArray(ButtonArray, ButtonString, sizeof(ButtonString));
    
    posY -= 20;
    drawText(ButtonString, PosX, posY, Color, Scale);
    // delete[] (ButtonArray);
    // ButtonArray = nullptr;
}

void drawChangeButtonCombo(uint16_t *currentButtonCombo)
{
    // Check for if this menu should be closed or not
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    if (incrementCheatsBButtonCounter(ButtonInput))
    {
        // Close this menu
        MenuVar.Timer                    = 0;
        MenuVar.SelectedOption           = 0;
        MenuVar.ChangingCheatButtonCombo = false;
        clearMemory(CheatsDisplayButtons.CheatsPreviousButtonsHeld, (14 * sizeof(uint8_t)));
        clearMemory(CheatsDisplayButtons.CheatsCurrentButtonsHeld, (14 * sizeof(uint8_t)));
        return;
    }
    
    // Draw the window
    uint32_t Color = 0x151515F0;
    int32_t PosX   = -245;
    int32_t PosY   = 130;
    int32_t Width  = 490;
    int32_t Height = 255;
    int32_t Curve  = 0;
    drawWindow(Color, PosX, PosY, Width, Height, Curve);
    
    // Draw the help text
    uint8_t Alpha        = 0xFF;
    float Scale          = 0.6f;
    PosX                 += 65;
    PosY                 -= 30;
    Color                = 0xFFFFFFFF;
    const char *HelpText = "Hold the button(s) for 3 seconds to set the\nnew button combo\n\nPress B three times in succession to cancel";
    
    drawTextAndInit(HelpText, PosX, PosY, Alpha, Color, false, Scale);
    
    // Draw the timer
    // Get the proper FPS for the timer
    uint32_t FPS = getCurrentFPS();
    
    uint32_t tempTimer = MenuVar.Timer;
    uint32_t second = (tempTimer / FPS) % 60;
    uint32_t frame = tempTimer % FPS;
    char *tempDisplayBuffer = DisplayBuffer;
    
    sprintf(tempDisplayBuffer,
        "Time Left: %02" PRIu32 ".%02" PRIu32,
        second,
        frame);
    
    PosY -= 120;
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
    
    // Draw the text for telling what buttons are currently held
    Color = 0xFFFFFFFF;
    const char *CurrentLine = "Currently held buttons";
    PosY -= 40;
    drawText(CurrentLine, PosX, PosY, Color, Scale);
    
    // Draw the currently held buttons
    uint8_t *tempButtonArray = CheatsDisplayButtons.CheatsCurrentButtonsHeld;
    
    // Clear the memory, so that the previous results do not interfere with the new results
    clearMemory(tempButtonArray, (14 * sizeof(uint8_t)));
    getButtonsPressed(tempButtonArray, ButtonInput);
    
    char ButtonString[128];
    createButtonStringArray(tempButtonArray, ButtonString, sizeof(ButtonString));
    
    PosY -= 20;
    drawText(ButtonString, PosX, PosY, Color, Scale);
    
    // Decrement the timer and check to see if it's at 0
    if (!cheatsManageTimer(ButtonInput))
    {
        return;
    }
    
    // The timer reached 0, so set the currently held buttons to the current button combo for the cheat
    // Make sure the button combo isn't L + Start
    if (ButtonInput != (PAD_L | PAD_START))
    {
        *currentButtonCombo = static_cast<uint16_t>(ButtonInput);
    }
    
    // Close this menu
    MenuVar.SelectedOption           = 0;
    MenuVar.ChangingCheatButtonCombo = false;
    clearMemory(CheatsDisplayButtons.CheatsPreviousButtonsHeld, (14 * sizeof(uint8_t)));
    clearMemory(CheatsDisplayButtons.CheatsCurrentButtonsHeld, (14 * sizeof(uint8_t)));
}

void drawCheatsFrameAdvance()
{
    // uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha  = 0xFF;
    // int32_t PosX   = -232;
    int32_t PosY      = 100;
    // float Scale    = 0.6f;
    
    // Draw the bool
    drawCheatsBool(PosY);
    
    // Draw the button combos
    PosY -= 60;
    const char *Description = "Frame Advance Button Combo";
    drawButtonCombo(FrameAdvance.FrameAdvanceButtonCombos.AdvanceFrameButtonCombo, PosY, Description);
    
    PosY -= 60;
    Description = "Pause/Resume Button Combof";
    drawButtonCombo(FrameAdvance.FrameAdvanceButtonCombos.PauseButtonCombo, PosY, Description);
}

void drawCheatsGenerateLagSpike(uint16_t currentButtonCombo)
{
    uint32_t Color   = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = 100;
    float Scale      = 0.6f;
    
    // Draw the bool
    drawCheatsBool(PosY);
    
    // Draw the button combo
    PosY -= 60;
    const char *Description = "Button Combo (Can be pressed in any order)";
    drawButtonCombo(currentButtonCombo, PosY, Description);
    
    // Draw the duration
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Duration (in milliseconds)\n%" PRIu16,
        MenuVar.LagSpikeDuration);
    
    PosY -= 60;
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
}

void drawCheatsDisableCertainSounds()
{
    // uint32_t Color   = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    // int32_t PosX     = -232;
    int32_t PosY     = 100;
    // float Scale      = 0.6f;
    
    // Draw the bools for the options
    bool *Bools = MenuVar.DisableCertainSounds;
    const char** Lines = Menu[CHEATS_DISABLE_CERTAIN_SOUNDS].Line;
    uint32_t TotalLines = Menu[CHEATS_DISABLE_CERTAIN_SOUNDS].TotalMenuOptions;
    
    for (uint32_t i = 0; i < TotalLines; i++)
    {
        // Disable Pause Menu/Z Menu Sounds must be handled separately
        if (i == 0)
        {
            drawBoolOnOrOff(Cheat[MenuVar.MenuSelectedOption].Active, Lines[i], PosY);
        }
        else
        {
            drawBoolOnOrOff(Bools[i - 1], Lines[i], PosY);
        }
        
        PosY -= 60;
    }
}

void drawCheatsForcedDropItem()
{
    uint8_t Alpha  = 0xFF;
    uint32_t Color = 0xFFFFFFFF;
    int32_t PosX   = -222;
    int32_t PosY   = 15;
    float Scale    = 0.6f;
    
    // Draw the current item icon
    // Set up array to use for displaying icons
    int32_t IconPosition[3];
    const int32_t IconPositionX = 0;
    const int32_t IconPositionY = 1;
    IconPosition[IconPositionX] = PosX;
    IconPosition[IconPositionY] = PosY;
    IconPosition[2]             = 0;
    
    int16_t tempForcedNPCItemDrop = MenuVar.ForcedNPCItemDrop;
    drawIconFromItem(IconPosition, tempForcedNPCItemDrop, Scale);
    
    // Draw the text for showing what the current item is
    const char *CurrentLine = "Current Item";
    drawTextAndInit(CurrentLine, PosX - 10, PosY + 45, Alpha, Color, false, Scale);
    
    // Draw the text for the item
    PosX += 17;
    PosY += 18;
    const char *ItemName = getItemName(tempForcedNPCItemDrop);
    
    drawText(ItemName, PosX, PosY, Color, Scale);
}

void drawCheatsResolveFades()
{
    // Draw the text explaining how to clear a fade
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = 180;
    float Scale = 0.6f;
    
    const int32_t PosYStart = PosY;
    
    const char *ExplainText = "Select a fade to resolve, if applicable.";
    drawText(ExplainText, PosX, PosY, Color, Scale);
    PosY -= 40;
    
    // Draw each fade entry
    uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
    char *tempDisplayBuffer = DisplayBuffer;
    uint32_t MaxFadeEntries = 5;
    
    // Draw each slot number
    for (uint32_t i = 0; i < MaxFadeEntries; i++)
    {
        sprintf(tempDisplayBuffer,
            "Slot %" PRIu32 ":",
            i + 1);
        
        bool CurrentOptionCheck = tempCurrentMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        PosY -= 20;
    }
    
    // Draw each value
    ttyd::fadedrv::FadeWork *FadeWork = ttyd::fadedrv::gpFadeWork;
    PosX += 60;
    PosY = PosYStart - 40;
    
    for (uint32_t i = 0; i < MaxFadeEntries; i++)
    {
        // Check if the current fade is active
        ttyd::fadedrv::FadeEntry *FadeEntry = &FadeWork->entry[i];
        if (FadeEntry->flags & (1 << 0)) // Check if 0 bit is active
        {
            sprintf(tempDisplayBuffer,
                "%" PRId32,
                static_cast<int32_t>(FadeEntry->fadeType));
            
            Color = 0xFFFFFFFF;
            drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        }
        else
        {
            const char *String = "None";
            Color = 0x4B4B4BFF;
            drawText(String, PosX, PosY, Color, Scale);
        }
        PosY -= 20;
    }
}

void drawCheatsLockFlags()
{
    // Draw the Yes/No text for each option
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = 180;
    float Scale = 0.6f;
    uint32_t Color;
    
    const char *String;
    
    uint32_t Size = sizeof(LockFlags.Region) / sizeof(LockFlags.Region[0]);
    for (uint32_t i = 0; i < Size; i++)
    {
        getYesNoTextAndColor(LockFlags.Region[i].MemoryRegionLocked, &String, &Color);
        drawText(String, PosX + 120, PosY, Color, Scale);
        PosY -= 20;
    }
    PosY -= 60;
    
    auto getAreaText = [](char *stringOut, char *areaLocked, bool flag)
    {
        // Make sure the current area with its flags locked is valid
        if (!flag || areaLocked[0] == '\0')
        {
            strcpy(stringOut, "None");
            return static_cast<uint32_t>(0x4B4B4BFF);
        }
        else
        {
            strcpy(stringOut, areaLocked);
            
            // Check to see if the current area is one that can have its flags cleared
            const char **tempAreaText = CheatsClearAreaFlagsAreas;
            uint32_t Size = CheatsClearAreaFlagsAreasSize;
            for (uint32_t i = 0; i < Size; i++)
            {
                if (compareStrings(areaLocked, tempAreaText[i]))
                {
                    strcpy(stringOut, CheatsClearAreaFlagsAreasFullNames[i]);
                    break;
                }
            }
        }
        return static_cast<uint32_t>(0xFFFFFFFF);
    };
    
    // Draw the text indicating which area has its LSW flags locked
    Color = 0xFFFFFFFF;
    String = "Current Area LSW Flags Locked";
    drawText(String, PosX, PosY, Color, Scale);
    PosY -= 20;
    
    // Get the text for which area has its LSW flags locked
    char CurrentAreaLockedText[64];
    
    Color = getAreaText(CurrentAreaLockedText, 
        lockFlagsMenuGetAreaLockedString(LSW), 
        LockFlags.Region[LSW].MemoryRegionLocked);
    
    // Draw the text for which area has its flags locked
    drawText(CurrentAreaLockedText, PosX, PosY, Color, Scale);
    PosY -= 40;
    
    // Draw the text indicating which area has its LSWF flags locked
    Color = 0xFFFFFFFF;
    String = "Current Area LSWF Flags Locked";
    drawText(String, PosX, PosY, Color, Scale);
    PosY -= 20;
    
    // Get the text for which area has its LSWF flags locked
    Color = getAreaText(CurrentAreaLockedText, lockFlagsMenuGetAreaLockedString(LSWF), LockFlags.Region[LSWF].MemoryRegionLocked);
    
    // Draw the text for which area has its flags locked
    drawText(CurrentAreaLockedText, PosX, PosY, Color, Scale);
}

void drawCheatsManageFlagsMain(uint32_t currentMenu)
{
    bool ChangingWord = false;
    const char **Line;
    uint32_t Size;
    
    switch (currentMenu)
    {
        case SET_GSW:
        case SET_GW:
        case SET_LSW:
        {
            ChangingWord = true;
            Line = CheatsManageGlobalWordsOptions;
            Size = CheatsManageGlobalWordsOptionsSize + 1;
            break;
        }
        case SET_GSWF:
        case SET_GF:
        case SET_LSWF:
        default:
        {
            Line = CheatsManageGlobalFlagsOptions;
            Size = CheatsManageGlobalFlagsOptionsSize + 1;
        }
    }
    
    const char *FlagText;
    switch (currentMenu)
    {
        case SET_GSW:
        {
            FlagText = "GSW";
            break;
        }
        case SET_GSWF:
        {
            FlagText = "GSWF";
            break;
        }
        case SET_GW:
        {
            FlagText = "GW";
            break;
        }
        case SET_GF:
        {
            FlagText = "GF";
            break;
        }
        case SET_LSW:
        {
            FlagText = "LSW";
            break;
        }
        case SET_LSWF:
        {
            FlagText = "LSWF";
            break;
        }
        default:
        {
            FlagText = "";
        }
    }
    
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = 180;
    float Scale      = 0.6f;
    uint32_t Color;
    
    uint32_t tempSelectedOption = MenuVar.SelectedOption;
    char *tempDisplayBuffer = DisplayBuffer;
    
    // Draw the main text
    uint32_t TextIndex = 0;
    for (uint32_t i = 0; i < Size; i++)
    {
        bool CurrentOptionCheck;
        if (tempSelectedOption <= 1)
        {
            CurrentOptionCheck = MenuVar.CurrentMenuOption == i;
        }
        else
        {
            CurrentOptionCheck = (tempSelectedOption - 1) == i;
        }
        
        Color = getSelectedTextColor(CurrentOptionCheck);
        
        if (i == 0)
        {   
            sprintf(tempDisplayBuffer,
                "Change %s",
                FlagText);
            
            drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        }
        else
        {
            drawText(Line[TextIndex], PosX, PosY, Color, Scale);
            TextIndex++;
        }
        
        PosY -= 20;
    }
    
    // Draw the text for the values
    uint32_t FlagToSet = ManageFlags.FlagToSet;
    int32_t FlagValue = getGlobalFlagValue(currentMenu, FlagToSet);
    Color = 0xFFFFFFFF;
    PosY -= 20;
    
    if (ChangingWord)
    {
        // Draw the text descriptions
        sprintf(tempDisplayBuffer,
            "%s To Set\nCurrent Value\nNew Value",
            FlagText);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        
        // Draw the values
        sprintf(tempDisplayBuffer,
            "%" PRIu32 "\n%" PRId32 "\n%" PRId32,
            FlagToSet,
            FlagValue,
            ManageFlags.ValueToSet);
        
        drawText(tempDisplayBuffer, PosX + 150, PosY, Color, Scale);
    }
    else
    {
        // Draw the text descriptions
        sprintf(tempDisplayBuffer,
            "%s To Set\nCurrent Value",
            FlagText);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        
        // Draw the values
        // Draw the flag being changed
        sprintf(tempDisplayBuffer,
            "%" PRIu32,
            FlagToSet);
        
        drawText(tempDisplayBuffer, PosX + 150, PosY, Color, Scale);
        
        // Draw the On/Off text
        const char *TextToDraw;
        getOnOffTextAndColor(FlagValue, &TextToDraw, &Color);
        drawText(TextToDraw, PosX + 150, PosY - 20, Color, Scale);
    }
}

void drawCheatsClearArea()
{
    // Draw the main text
    uint32_t tempCheatsClearAreaFlagsAreasSize = CheatsClearAreaFlagsAreasSize;
    const char **tempCheatsClearAreaFlagsAreas = CheatsClearAreaFlagsAreas;
    
    int32_t PosX                               = -232;
    int32_t PosY                               = 120;
    uint32_t Size                              = tempCheatsClearAreaFlagsAreasSize;
    uint32_t MaxOptionsPerPage                 = tempCheatsClearAreaFlagsAreasSize;
    uint32_t MaxOptionsPerRow                  = 4;
    uint32_t PosXIncrementAmount               = 50;
    uint32_t tempPage                          = 0;
    uint32_t tempSecondaryMenuOption           = MenuVar.SecondaryMenuOption;
    
    bool CurrentlySelectingOption;
    if (MenuVar.SelectedOption == SELECT_AREA)
    {
        CurrentlySelectingOption = true;
    }
    else
    {
        CurrentlySelectingOption = false;
    }
    
    drawMultipleColumnsVertical(PosX, PosY, tempSecondaryMenuOption, tempPage, 
        Size, MaxOptionsPerPage, MaxOptionsPerRow, CurrentlySelectingOption, 
            PosXIncrementAmount, tempCheatsClearAreaFlagsAreas);
    
    // Draw the description for the current option
    if (CurrentlySelectingOption)
    {
        PosY -= 140;
        drawSingleLineFromArray(PosX, PosY, tempSecondaryMenuOption, 
            CheatsClearAreaFlagsAreasFullNames);
    }
    
    // Draw the current area selected
    PosX = 0;
    PosY = 160;
    const char *String = "Current Area: ";
    drawSingleLineFromStringAndArray(PosX, PosY, MenuVar.MenuSecondaryValue, 
        String, tempCheatsClearAreaFlagsAreas);
}

void drawDisplaysMemoryUsageMenu()
{
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = 180;
    float Scale = 0.6f;
    uint32_t Color;
    
    // Draw each option for displaying memory usage about each heap
    bool *DisplayHeapInfo = HeapInfo.DisplayHeapInfo;
    int32_t NumHeaps = HeapInfo.ArrayCount - 1; // Remove the smart heap from the total
    
    int32_t tempCurrentMenuOption = static_cast<int32_t>(MenuVar.CurrentMenuOption);
    char *tempDisplayBuffer = DisplayBuffer;
    const char *String;
    
    for (int32_t i = 0; i < NumHeaps; i++)
    {
        // Draw each heap number
        sprintf(tempDisplayBuffer,
            "Main Heap %" PRIu32,
            i);
        
        bool CurrentOptionCheck = tempCurrentMenuOption == i;
        Color = getSelectedTextColor(CurrentOptionCheck);
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        
        // Draw the bool for each heap
        getOnOffTextAndColor(DisplayHeapInfo[i], &String, &Color);
        drawText(String, PosX + 120, PosY, Color, Scale);
        PosY -= 20;
    }
    
    // Draw the smart heap text
    bool CurrentOptionCheck = tempCurrentMenuOption == NumHeaps;
    Color = getSelectedTextColor(CurrentOptionCheck);
    drawText("Smart Heap", PosX, PosY, Color, Scale);
    
    // Draw the bool for the smart heap
    getOnOffTextAndColor(DisplayHeapInfo[NumHeaps], &String, &Color);
    drawText(String, PosX + 120, PosY, Color, Scale);
}

void drawWarpsOptions()
{
    // Draw the main text
    uint32_t tempWarpDestinationsSize = WarpDestinationsSize;
    const char **tempWarpDestinations = WarpDestinations;
    
    int32_t PosX                      = -232;
    int32_t PosY                      = 80;
    uint32_t Size                     = tempWarpDestinationsSize;
    uint32_t MaxOptionsPerPage        = tempWarpDestinationsSize;
    uint32_t MaxOptionsPerRow         = 4;
    uint32_t PosXIncrementAmount      = 100;
    uint32_t tempPage                 = 0;
    uint32_t tempCurrentMenuOption    = MenuVar.CurrentMenuOption;
    
    bool CurrentlySelectingOption;
    if (MenuVar.SelectedOption == SELECT_WARP)
    {
        CurrentlySelectingOption = true;
    }
    else
    {
        CurrentlySelectingOption = false;
    }
    
    drawMultipleColumnsVertical(PosX, PosY, tempCurrentMenuOption, tempPage, 
        Size, MaxOptionsPerPage, MaxOptionsPerRow, CurrentlySelectingOption, 
            PosXIncrementAmount, tempWarpDestinations);
    
    // Draw the description for the current option
    if (CurrentlySelectingOption)
    {
        PosY -= 140;
        drawSingleLineFromArray(PosX, PosY, 
            tempCurrentMenuOption, WarpDescriptions);
    }
}

void drawEventDetails(int32_t posX, int32_t posY, int32_t index)
{
    // Draw the labels for the details for the current event
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    float Scale = 0.6f;
    
    const char *Details = "Stage\nEvent\nSequence\nPartner\nFollower\nMap\nLZ";
    drawText(Details, posX, posY, Color, Scale);
    
    // Draw the values for the details for the current event
    WarpByEventDetailsStruct WarpByEventDetails;
    if (!getEventDetails(index, &WarpByEventDetails))
    {
        return;
    }
    
    char *tempDisplayBuffer = DisplayBuffer;
    posX += 100;
    
    // Draw the values for the stage, event, and sequence
    sprintf(tempDisplayBuffer,
        "%s\n%s\n%" PRIu16,
        WarpByEventDetails.Stage,
        WarpByEventDetails.Event,
        WarpByEventDetails.SequencePosition);
    
    drawText(tempDisplayBuffer, posX, posY, Color, Scale);
    posY -= 60;
    
    // Draw the value for the partner
    const char *PartnerString = WarpByEventDetails.Partner;
    if (!compareStrings(PartnerString, "None"))
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        Color = 0x4B4B4BFF;
    }
    
    drawText(PartnerString, posX, posY, Color, Scale);
    posY -= 20;
    
    // Draw the value for the follower
    const char *FollowerString = WarpByEventDetails.Follower;
    if (!compareStrings(FollowerString, "None"))
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        Color = 0x4B4B4BFF;
    }
    
    drawText(FollowerString, posX, posY, Color, Scale);
    posY -= 20;
    
    // Draw the value for the map
    const char *Map = WarpByEventDetails.Map;
    if (Map && (Map[0] != '\0'))
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        Map = "None";
        Color = 0x4B4B4BFF;
    }
    
    drawText(Map, posX, posY, Color, Scale);
    posY -= 20;
    
    // Draw the value for the bero
    const char *Bero = WarpByEventDetails.Bero;
    if (Bero && (Bero[0] != '\0'))
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        Bero = "None";
        Color = 0x4B4B4BFF;
    }
    
    drawText(Bero, posX, posY, Color, Scale);
}

void drawWarpByEventMenuDetails()
{
    // Draw the text explaining that warping with this menu will clear all game states
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = 180;
    float Scale = 0.6f;
    
    const char *ExplainText = "Note: Warping via this menu will clear all game states.";
    drawText(ExplainText, PosX, PosY, Color, Scale);
    PosY -= 60;
    
    // Draw the text for whether the standard inventory should be kept or not
    const char *String;
    getYesNoTextAndColor(WarpByEvent.ShouldKeepInventory, &String, &Color);
    
    int32_t tempPosX = PosX + 147;
    
#ifdef TTYD_JP
    tempPosX -= 4;
#endif
    
    drawText(String, tempPosX, PosY, Color, Scale);
    PosY -= 20;
    
    // Draw the text for whether specific flags should be set or not
    getYesNoTextAndColor(WarpByEvent.ShouldSetFlags, &String, &Color);
    drawText(String, tempPosX, PosY, Color, Scale);
    PosY -= 60;
    
    // Draw the details for the current event
    drawEventDetails(PosX, PosY, WarpByEvent.CurrentIndex);
}

void drawWarpsBossesOptions()
{
    // Draw the text stating that using this menu will modify the sequence
    uint32_t Color = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = 180;
    float Scale = 0.6f;
    
    const char *ExplainText = "Note: Using this menu will modify the Sequence position";
    drawText(ExplainText, PosX, PosY, Color, Scale);
    PosY -= 40;
    
    // Draw the main text
    uint32_t tempWarpBossLinesSize = Menu[WARPS_BOSS].TotalMenuOptions;
    const char **tempWarpBossLines = WarpBossLines;
    
    uint32_t Size                  = tempWarpBossLinesSize;
    uint32_t MaxOptionsPerPage     = tempWarpBossLinesSize;
    uint32_t MaxOptionsPerRow      = 2;
    uint32_t PosXIncrementAmount   = 200;
    uint32_t tempPage              = 0;
    uint32_t tempCurrentMenuOption = MenuVar.CurrentMenuOption;
    
    drawMultipleColumnsVertical(PosX, PosY, tempCurrentMenuOption, tempPage, 
        Size, MaxOptionsPerPage, MaxOptionsPerRow, true, 
            PosXIncrementAmount, tempWarpBossLines);
}

void drawWarpIndexMapAndEntrance()
{
    uint32_t Color   = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = 80;
    float Scale      = 0.6f;
    
    const char *String = "Current Map\nCurrent Entrance\n\nNew Map\nNew Entrance Id";
    drawText(String, PosX, PosY, Color, Scale);
    PosX += 170;
    PosY -= 60;
    
    const char *MapName = getMapFromIndex(static_cast<int32_t>(WarpByIndex.MapId));
    uint32_t EntranceId = WarpByIndex.EntranceId;
    
    // Draw the new map and entrance
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "%s\n%" PRIu32,
        MapName,
        EntranceId);
    
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
    PosY += 60;
    
    // Draw the current map
    const char *NextMap = ttyd::seq_mapchange::NextMap;
    if (NextMap[0] != '\0')
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        NextMap = "None";
        Color = 0x4B4B4BFF;
    }
    
    drawText(NextMap, PosX, PosY, Color, Scale);
    PosY -= 20;
    
    // Draw the current bero
    const char *NextBero = ttyd::seq_mapchange::NextBero;
    if (NextBero[0] != '\0')
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        NextBero = "None";
        Color = 0x4B4B4BFF;
    }
    
    drawText(NextBero, PosX, PosY, Color, Scale);
}

void drawWarpIndexEntranceList()
{
    // Draw the window for the text
    uint32_t WindowColor = 0x000000E5;
    int32_t PosX         = -245;
    int32_t PosY         = 190;
    int32_t Width        = 490;
    int32_t Height       = 375;
    int32_t Curve        = 0;
    
    drawWindow(WindowColor, PosX, PosY, Width, Height, Curve);
    
    // Draw the help text
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    float Scale    = 0.6f;
    PosY -= 20;
    
    const char *HelpText = "Press B to close this window\nPress D-Pad Up/Down to adjust the page";
    drawTextAndInit(HelpText, PosX + 72, PosY, Alpha, Color, false, Scale);
    PosX = -232;
    PosY -= 55;
    
    const char *String = "Current Map\nCurrent Entrance";
    drawText(String, PosX, PosY, Color, Scale);
    
    // Draw the current map
    const char *NextMap = ttyd::seq_mapchange::NextMap;
    if (NextMap[0] != '\0')
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        NextMap = "None";
        Color = 0x4B4B4BFF;
    }
    
    drawText(NextMap, PosX + 170, PosY, Color, Scale);
    
    // Draw the current bero
    const char *NextBero = ttyd::seq_mapchange::NextBero;
    if (NextBero[0] != '\0')
    {
        Color = 0xFFFFFFFF;
    }
    else
    {
        NextBero = "None";
        Color = 0x4B4B4BFF;
    }
    
    drawText(NextBero, PosX + 170, PosY - 20, Color, Scale);
    
    // Draw the current page
    int32_t PageNumberPosX = 150;
    uint32_t tempCurrentPage = MenuVar.CurrentPage;
    drawPageNumber(PageNumberPosX, PosY, tempCurrentPage);
    
    // Draw the entrances
    const char **tempEntranceArray = WarpByIndex.EntranceList;
    Color = 0xFFFFFFFF;
    PosY -= 60;
    
    // Make sure at least one entrance exists
    if (!tempEntranceArray[0])
    {
        const char *String = "No entrances exist for this map";
        drawText(String, PosX, PosY, Color, Scale);
        return;
    }
    
    uint32_t MaxSlotsInArray = sizeof(WarpByIndex.EntranceList) / sizeof(WarpByIndex.EntranceList[0]);
    uint32_t MaxEntrancesPerPage = 12;
    uint32_t Index = MaxEntrancesPerPage * tempCurrentPage;
    
    char *tempDisplayBuffer = DisplayBuffer;
    for (uint32_t i = Index; i < (Index + MaxEntrancesPerPage); i++)
    {
        if (i >= MaxSlotsInArray)
        {
            // Reached the end of the array
            return;
        }
        
        if (!tempEntranceArray[i])
        {
            // Reached the end of the valid entries
            return;
        }
        
        // Draw the entrance number
        sprintf(tempDisplayBuffer,
            "Entrance %" PRIu32,
            i + 1);
        
        drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        
        // Draw the entrance name
        drawText(tempEntranceArray[i], PosX + 130, PosY, Color, Scale);
        PosY -= 20;
    }
}

void drawOnScreenTimerButtonCombos(uint16_t *buttonCombo)
{
    int32_t PosY = 40;
    uint32_t tempOnScreenTimerOptionsSize = OnScreenTimerOptionsSize;
    
    for (uint32_t i = 0; i < tempOnScreenTimerOptionsSize; i++)
    {
        drawButtonCombo(buttonCombo[i], PosY, OnScreenTimerOptions[i]);
        PosY -= 60;
    }
}

void drawOnScreenTimer()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = 115;
    int32_t PosY   = -160;
    float Scale    = 0.7f;
    
#ifdef TTYD_JP
    PosX  += 5;
    PosY  += 2;
    Scale += 0.05;
#endif
    
    // Move the timer up if Mario's Coordinates are currently displayed
    // Don't move up if the input display is active
    if (!Displays[BUTTON_INPUT_DISPLAY])
    {
        if (Displays[MARIO_COORDINATES] || 
            Displays[YOSHI_SKIP] || 
            Displays[PALACE_SKIP] || 
            Displays[BRIDGE_SKIP])
        {
            PosY += 20;
        }
    }
    
    char *CurrentTimeString = getTimeString(DisplayBuffer, OnScreenTimer.MainTimer);
    drawTextAndInit(CurrentTimeString, PosX, PosY, Alpha, Color, true, Scale);
}

void drawFrameCounter()
{
    // Get the proper FPS for the timer
    uint32_t FPS = getCurrentFPS();
    
    uint32_t tempTimer = FrameCounter.MainTimer;
    uint32_t hour = tempTimer / 3600 / FPS;
    uint32_t minute = (tempTimer / 60 / FPS) % 60;
    uint32_t second = (tempTimer / FPS) % 60;
    uint32_t frame = tempTimer % FPS;
    
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 ".%02" PRIu32,
        hour,
        minute,
        second,
        frame);
    
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = 115;
    int32_t PosY   = -160;
    float Scale    = 0.7f;
    
#ifdef TTYD_JP
    PosX  += 5;
    PosY  += 2;
    Scale += 0.05;
#endif
    
    // Move the timer up if Mario's Coordinates are currently displayed
    // Don't move up if the input display is active
    if (!Displays[BUTTON_INPUT_DISPLAY])
    {
        if (Displays[MARIO_COORDINATES] || 
            Displays[YOSHI_SKIP] || 
            Displays[PALACE_SKIP] || 
            Displays[BRIDGE_SKIP])
        {
            PosY += 20;
        }
    }
    
    // Move the timer up if the On-Screen Timer is displayed
    if (Displays[ONSCREEN_TIMER])
    {
        PosY += 20;
        
#ifdef TTYD_JP
        PosY += 2;
#endif
    }
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
    
    if (!FrameCounter.TimerPaused)
    {
        FrameCounter.MainTimer++;
    }
}

void drawMarioCoordinatesInit()
{
    // Start the text drawing sequence
    uint8_t Alpha = 0xFF;
    drawTextInit(Alpha, true);
    
    drawMarioCoordinates();
}

void drawMarioCoordinates()
{
    uint32_t Color   = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = -160;
    float Scale      = 0.65f;
    
    // Move the text up if the input display is active
    if (Displays[BUTTON_INPUT_DISPLAY])
    {
        PosY += 20;
    }
    
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    char *tempDisplayBuffer = DisplayBuffer;
    
    sprintf(tempDisplayBuffer,
        "MarPos: %.2f  %.2f  %.2f",
        player->playerPosition[0], player->playerPosition[1], player->playerPosition[2]);
    
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
}

void drawMarioSpeedXZ()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = -160;
    float Scale    = 0.65f;
    
    // Move the text up if the input display is active
    if (Displays[BUTTON_INPUT_DISPLAY])
    {
        PosY += 20;
    }
    
    // Move the text up if the Mario Coordinates display is active
    if (Displays[MARIO_COORDINATES] || 
        Displays[YOSHI_SKIP] || 
        Displays[PALACE_SKIP] || 
        Displays[BRIDGE_SKIP] || 
        Displays[BLIMP_TICKET_SKIP])
    {
        PosY += 20;
    }
    
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    char *tempDisplayBuffer = DisplayBuffer;
    
    sprintf(tempDisplayBuffer,
        "MarSpeedXZ: %.6f",
        player->wPlayerEffectiveSpeed);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
}

void drawJumpStorageDetails()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = 140;
    int32_t PosY   = 120;
    float Scale    = 0.65f;
    
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    char *tempDisplayBuffer = DisplayBuffer;
    
    // Draw the main text
    sprintf(tempDisplayBuffer,
        "JS:\nSpdY: %.2f",
        player->wJumpVelocityY);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
    
    // Draw the On/Off Text
    bool JumpStorageFlag = (player->flags3 & (1 << 16)) >> 16; // Get only the 16 bit
    const char *TextToDraw;
    
    getOnOffTextAndColor(JumpStorageFlag, &TextToDraw, &Color);
    drawText(TextToDraw, PosX + 33, PosY, Color, Scale);
}

void drawButtonInputs()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha = 0xFF;
    int32_t PosX = -232;
    int32_t PosY = -160;
    float Scale = 0.65f;
    
    uint32_t ButtonInput = ttyd::system::keyGetButton(0);
    char *tempDisplayBuffer = DisplayBuffer;
    
    const char *Format;
#ifdef TTYD_JP
    Format = "%s";
#else
    Format = "%c";
#endif
    
    uint32_t Counter = 0;
    bool Initialized = false;
    
    for (uint32_t i = 0; i < 13; i++)
    {
        if (i == 7)
        {
            // Skip unused input value
            i++;
        }
        
        if (ButtonInput & (1 << i))
        {
            if (!Initialized)
            {
                Initialized = true;
                
                // Start the text drawing sequence
                drawTextInit(Alpha, true);
            }
            
            sprintf(tempDisplayBuffer,
                Format,
                ButtonInputDisplay[Counter]);
            
            drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
        }
        
        PosX += 20;
        Counter++;
    }
}

void drawStickAngleInit()
{
    // Start the text drawing sequence
    uint8_t Alpha = 0xFF;
    drawTextInit(Alpha, true);
    
    drawStickAngle();
}

void drawStickAngle()
{
    uint32_t Color   = 0xFFFFFFFF;
    // uint8_t Alpha = 0xFF;
    int32_t PosX     = -232;
    int32_t PosY     = -160;
    float Scale      = 0.65f;
    
    // Move the text up if the input display is active
    if (Displays[BUTTON_INPUT_DISPLAY])
    {
        PosY += 20;
    }
    
    // Move the text up if the Mario Speed XZ display is active
    if (Displays[MARIO_SPEED_XZ])
    {
        PosY += 20;
    }
    
    // Move the text up if the Mario Coordinates display is active
    if (Displays[MARIO_COORDINATES] || 
        Displays[YOSHI_SKIP] || 
        Displays[PALACE_SKIP] || 
        Displays[BRIDGE_SKIP] || 
        Displays[BLIMP_TICKET_SKIP])
    {
        PosY += 20;
    }
    
    char StickAngleString[32];
    getStickAngleString(StickAngleString);
    
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "StickAngle: %s",
        StickAngleString);
    
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
}

void Mod::drawStarPowerValueUnderStatusWindow()
{
    // Call the original function immediately
    mPFN_statusWinDisp_trampoline();
    
    if (!Displays[STAR_POWER_VALUE])
    {
        return;
    }
    
    // Credits to Jdaster64 for writing the original code for this function
    // Don't display SP if no Star Powers have been unlocked yet
    uint16_t SpecialMovesBits = getSpecialMoveBits();
    if (!SpecialMovesBits)
    {
        return;
    }
    
    // Failsafe: Make sure the current SP is valid to prevent the text from glitching
    int16_t CurrentSP = ttyd::mario_pouch::pouchGetAP();
    if (CurrentSP <= 0)
    {
        return;
    }
    
    // Don't display SP if the status bar is not on-screen
    float *StatusBarPos = reinterpret_cast<float *>(
        reinterpret_cast<uint32_t>(ttyd::statuswindow::statusWindowWorkPointer) + 0x20);
    
    float StatusBarPosY = StatusBarPos[1];
    if ((StatusBarPosY > 330.f) || (StatusBarPosY < 100.f))
    {
        return;
    }
    
    // Display SP
    gc::mtx::mtx34 Mtx;
    gc::mtx::PSMTXTrans(Mtx, 192.f, StatusBarPosY - 100.f, 0.f);
    
    uint32_t Color = 0xFFFFFFFF;
    ttyd::icondrv::iconNumberDispGx(Mtx, CurrentSP, true, reinterpret_cast<uint8_t *>(&Color));
}

void Mod::drawSequenceInPauseMenu(ttyd::dispdrv::CameraId cameraId, void *winWorkPtr, int32_t unkIndex)
{
    // Call the original function immediately
    mPFN_winMarioDisp_trampoline(cameraId, winWorkPtr, unkIndex);
    
    // Begin the drawing sequence
    drawTextInit(0xFF, true);
    
    // Draw the sequence text
#ifdef TTYD_JP
    static float TextScale[3] = { 0.9f, 0.9f, 0.9f };
#else
    static float TextScale[3] = { 0.8f, 0.8f, 0.8f };
#endif
    
    uint8_t Color[4];
    *reinterpret_cast<uint32_t *>(Color) = 0xFFFFFFFF;
    
    // Get the position of the window
    float *WindowPos = reinterpret_cast<float *>(
        reinterpret_cast<uint32_t>(winWorkPtr) + 0xC4);
    
    float WindowPosX = WindowPos[0];
    float WindowPosY = WindowPos[1];
    float PosYIncrement = 50.f;
    
#ifdef TTYD_JP
    PosYIncrement += 1.f;
#endif
    
    float Pos[3];
    Pos[0] = WindowPosX + 46.f;
    Pos[1] = WindowPosY + PosYIncrement;
    Pos[2] = 0.f;
    
    ttyd::win_main::winFontSet(
        Pos, 
        TextScale, 
        Color, 
        "Sequence");
    
    // Draw the sequence value
#ifdef TTYD_JP
    float *ValueScale = TextScale;
#else
    static float ValueScale[3] = { 0.9f, 0.9f, 0.9f };
#endif
    
    Pos[0] = WindowPosX + 214.f;
    // Pos[1] = WindowPosY + PosYIncrement;
    // Pos[2] = 0.f;
    
    uint32_t SequencePosition = getSequencePosition();
    
    ttyd::win_main::winFontSetR(
        Pos, 
        ValueScale, 
        Color, 
        "%" PRIu32, 
        SequencePosition);
    
    // Need to manually turn the font edge off
    ttyd::fontmgr::FontDrawEdgeOff();
}

void drawMemoryWatchesOnOverworld()
{
    // Figure out which watches are going to be displayed
    bool tempHideMenu = MenuVar.HideMenu;
    uint32_t tempMenuSelectionOption = MenuVar.MenuSelectedOption;
    uint32_t Size = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
    
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha = 0xFF;
    float Scale = 0.65f;
    
    bool Initialized = false;
    for (uint32_t i = 0; i < Size; i++)
    {
        if (MemoryWatch[i].Display)
        {
            // Check to see if this watch is currently being repositioned
            if (tempHideMenu && (tempMenuSelectionOption == i))
            {
                // This watch is currently being repositioned, so do not draw it
            }
            else
            {
                if (!Initialized)
                {
                    Initialized = true;
                    
                    // Start the text drawing sequence
                    drawTextInit(Alpha, false); // Set to false since drawMemoryWatchValueString will manually enable the font edge
                }
                
                // Draw the watch
                int32_t PosX = MemoryWatch[i].PosX;
                int32_t PosY = MemoryWatch[i].PosY;
                drawMemoryWatchValueString(i, PosX, PosY, Color, true, Scale);
            }
        }
    }
}

void drawYoshiSkipDetails()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = -120;
    float Scale    = 0.65f;
    
    // Move the text up if the input display is active
    if (Displays[BUTTON_INPUT_DISPLAY])
    {
        PosY += 20;
    }
    
    // Move the text up if the Mario Speed XZ display is active
    if (Displays[MARIO_SPEED_XZ])
    {
        PosY += 20;
    }
    
    // Draw the text
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "YST: %" PRIu32,
        YoshiSkip.MainTimer);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
    
    // Draw the Stick Angle if it's not already drawn
    if (!Displays[STICK_ANGLE])
    {
        drawStickAngle();
    }
    
    // Draw Mario's coordinates if they're not already drawn
    if (!Displays[MARIO_COORDINATES])
    {
        drawMarioCoordinates();
    }
    
    // Increment the main timer
    if (!YoshiSkip.TimerStopped)
    {
        YoshiSkip.MainTimer++;
    }
}

void drawPalaceSkipDetails()
{
    // Get Partner Y coordinate
    uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
    float PartnerPosY;
    
    if (PartnerPointer)
    {
        // A partner is out, so set PartnerPosY
        PartnerPosY = *reinterpret_cast<float *>(PartnerPointer + 0x58 + 0x4);
    }
    else
    {
        // No partner is out, so set PartnerPosY to 0
        PartnerPosY = 0.f;
    }
    
    // Get Phantom Ember Y coordinate
    ttyd::npcdrv::NpcEntry *NPC = getNpcEntryData(1, false); // NPC 2
    
    // Check if NPC 2 is active, followed by NPC 1, and then default to 0 if neither is active
    float PhantomEmberPosY = 0.f;
    
    if (NPC->flags & (1 << 0)) // Check if 0 bit is active
    {
        PhantomEmberPosY = NPC->position[1];
    }
    else
    {
        NPC = getNpcEntryData(0, false); // NPC 1
        if (NPC->flags & (1 << 0)) // Check if 0 bit is active
        {
            PhantomEmberPosY = NPC->position[1];
        }
    }
    
    // Get Field Item Timer
    uint32_t FieldItemsAddress = reinterpret_cast<uint32_t>(ttyd::itemdrv::itemDrvWork.wItemsInField);
    bool FoundItem = false;
    int32_t ItemTimer;
    
    for (uint32_t i = 0; i < 32; i++)
    {
        int32_t tempItem;
        uint16_t tempItemAction;
        uint16_t tempBit;
        
        tempItem = *reinterpret_cast<uint32_t *>(FieldItemsAddress + 0x4);
        
        if ((tempItem == 0) || (tempItem == ttyd::item_data::Item::Coin) || 
            (tempItem == ttyd::item_data::Item::HeartFromBattlesDropped) || 
                (tempItem == ttyd::item_data::Item::FlowerFromBattlesDropped))
        {
            // Don't want this item, so go to the next one
            FieldItemsAddress += 0x98;
        }
        else
        {
            // Get current item action
            tempItemAction = *reinterpret_cast<uint16_t *>(FieldItemsAddress + 0x24);
            if (tempItemAction == 7)
            {
                // Item is already spawned upon entering the room, so go to the next one
                FieldItemsAddress += 0x98;
            }
            else
            {
                // Check if item is currently spawned
                tempBit = *reinterpret_cast<uint16_t *>(FieldItemsAddress);
                tempBit &= (1 << 0); // Check if 0 bit is active
                if (!tempBit)
                {
                    // Item is not spawned, so go to the next one
                    FieldItemsAddress += 0x98;
                }
                else
                {
                    // Found the item we want, so assign the timer and exit the loop
                    ItemTimer = *reinterpret_cast<int32_t *>(FieldItemsAddress + 0x7C);
                    FoundItem = true;
                    break;
                }
            }
        }
    }
    
    if (!FoundItem)
    {
        // Didn't find an item to use, so set timer to 0
        ItemTimer = 0;
    }
    
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = -80;
    float Scale    = 0.65f;
    
    // Move the text up if the input display is active
    if (Displays[BUTTON_INPUT_DISPLAY])
    {
        PosY += 20;
    }
    
    // Move the text up if the stick angle display is active
    if (Displays[STICK_ANGLE])
    {
        PosY += 20;
    }
    
    // Move the text up if the Mario Speed XZ display is active
    if (Displays[MARIO_SPEED_XZ])
    {
        PosY += 20;
    }
    
    // Draw the text
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "PST: %" PRIu32 "\nItemTimer: %" PRId32 "\nPhaEmY: %.2f\nParY: %.2f",
        PalaceSkip.MainTimer,
        ItemTimer,
        PhantomEmberPosY,
        PartnerPosY);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
    
    // Draw Mario's coordinates if they're not already drawn
    if (!Displays[MARIO_COORDINATES])
    {
        drawMarioCoordinates();
    }
    
    // Increment the main timer
    if (!PalaceSkip.TimerStopped)
    {
        PalaceSkip.MainTimer++;
    }
}

void drawBridgeSkipDetails()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = -120;
    float Scale    = 0.65f;
    
    // Move the text up if the input display is active
    if (Displays[BUTTON_INPUT_DISPLAY])
    {
        PosY += 20;
    }
    
    // Move the text up if the Mario Speed XZ display is active
    if (Displays[MARIO_SPEED_XZ])
    {
        PosY += 20;
    }
    
    // Move the text up if the Stick Angle display is active
    if (Displays[STICK_ANGLE])
    {
        PosY += 20;
    }
    
    uint32_t MainTimer = BridgeSkip.MainTimer;
    
    // Check whether A was pressed early or late
    const char *EarlyOrLate;
    if (!BridgeSkip.PressedEarly)
    {
        EarlyOrLate = "Late";
    }
    else
    {
        // Increment the timer by 1 to account for being early by X frames
        MainTimer += 1;
        EarlyOrLate = "Early";
    }
    
    // Check if the Frames text should be plural
    const char *FramesPlural;
    if (MainTimer == 1)
    {
        FramesPlural = "";
    }
    else
    {
        FramesPlural = "s";
    }
    
    // Draw the text
    char *tempDisplayBuffer = DisplayBuffer;
    ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
    
    sprintf(tempDisplayBuffer,
        "%" PRIu32 " Frame%s %s\nHRP: 0x%08" PRIX32,
        MainTimer,
        FramesPlural,
        EarlyOrLate,
        reinterpret_cast<uint32_t>(player->wObjHazardRespawn));
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
    
    // Draw Mario's coordinates if they're not already drawn
    if (!Displays[MARIO_COORDINATES])
    {
        drawMarioCoordinates();
    }
    
    // Increment the main timer
    if (!BridgeSkip.TimerPaused && !BridgeSkip.TimerStopped)
    {
        BridgeSkip.MainTimer++;
    }
}

void drawBlimpTicketSkipDetails()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = -100;
    float Scale    = 0.65f;
    
    // Move the text up if the input display is active
    if (Displays[BUTTON_INPUT_DISPLAY])
    {
        PosY += 20;
    }
    
    // Move the text up if the Mario Speed XZ display is active
    if (Displays[MARIO_SPEED_XZ])
    {
        PosY += 20;
    }
    
    // Draw the text
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "URT: %" PRIu32 "\nSUT: %" PRIu32,
        BlimpTicketSkip.UpRightTimer,
        BlimpTicketSkip.StraightUpTimer);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
    
    // Draw the Stick Angle if it's not already drawn
    if (!Displays[STICK_ANGLE])
    {
        drawStickAngle();
    }
    
    // Draw Mario's coordinates if they're not already drawn
    if (!Displays[MARIO_COORDINATES])
    {
        drawMarioCoordinates();
    }
    
    // Increment the up-right timer
    if (!BlimpTicketSkip.UpRightTimerStopped)
    {
        BlimpTicketSkip.UpRightTimer++;
    }
    
    // Increment the straight-up timer
    if (!BlimpTicketSkip.StraightUpTimerStopped)
    {
        BlimpTicketSkip.StraightUpTimer++;
    }
}

void drawActionCommandsTiming()
{
    if (!Displays[GUARD_SUPERGUARD_TIMINGS])
    {
        DisplayActionCommands.DisplayTimer = 0;
        return;
    }
    
    // const int8_t CommandDifficulty = ttyd::battle_ac::BattleActionCommandGetDifficulty(getBattleWorkPointer());
#ifdef TTYD_US
    uint32_t SimplifierOffset   = 0x305;
    uint32_t UnsimplifierOffset = 0x306;
#elif defined TTYD_JP
    uint32_t SimplifierOffset   = 0x301;
    uint32_t UnsimplifierOffset = 0x302;
#elif defined TTYD_EU
    uint32_t SimplifierOffset   = 0x309;
    uint32_t UnsimplifierOffset = 0x30A;
#endif
    
    uint32_t MarioBattlePointer = reinterpret_cast<uint32_t>(getMarioBattlePointer());
    if (!MarioBattlePointer)
    {
        DisplayActionCommands.DisplayTimer = 0;
        return;
    }
    
    uint32_t SimplifiersEquipped = *reinterpret_cast<uint8_t *>(MarioBattlePointer + SimplifierOffset);
    uint32_t UnsimplifiersEquipped = *reinterpret_cast<uint8_t *>(MarioBattlePointer + UnsimplifierOffset);
    
    int32_t CommandDifficulty = UnsimplifiersEquipped - SimplifiersEquipped + 3;
    if (CommandDifficulty < 0)
    {
        CommandDifficulty = 0;
    }
    else if (CommandDifficulty > 6)
    {
        CommandDifficulty = 6;
    }
    
    int32_t temp_Last_A_Frame = DisplayActionCommands.Last_A_Frame;
    int32_t temp_Last_B_Frame = DisplayActionCommands.Last_B_Frame;
    
    uint32_t TypeToDraw = DisplayActionCommands.TypeToDraw;
    char *tempDisplayBuffer = DisplayBuffer;
    const char *TextToDraw;
    
    const uint32_t SuccessfulTiming      = 1;
    const uint32_t PressedTooManyButtons = 2;
    const uint32_t PressedTooEarly       = 3;
    const uint32_t CannotBeSuperguarded  = 4;
    
    // Check to see which text should be displayed
    switch (TypeToDraw)
    {
        case SuccessfulTiming:
        {
            uint32_t CurrentDifficultyFrames;
            int32_t FramePressed;
            char Button;
            
            if (temp_Last_A_Frame > -1)
            {
                CurrentDifficultyFrames = ttyd::battle_ac::GuardFrames[CommandDifficulty];
                FramePressed = CurrentDifficultyFrames - temp_Last_A_Frame;
                Button = 'A';
            }
            else
            {
                CurrentDifficultyFrames = ttyd::battle_ac::SuperguardFrames[CommandDifficulty];
                FramePressed = CurrentDifficultyFrames - temp_Last_B_Frame;
                Button = 'B';
            }
            
            sprintf(tempDisplayBuffer,
                "Pressed %c on frame %" PRId32 " of %" PRIu32,
                Button,
                FramePressed,
                CurrentDifficultyFrames);
            
            TextToDraw = tempDisplayBuffer;
            break;
        }
        case PressedTooManyButtons:
        {
            TextToDraw = "Pressed too many buttons";
            break;
        }
        case PressedTooEarly:
        {
            uint32_t CurrentDifficultyFrames;
            int32_t FramesEarly;
            char Button;
            
            if (temp_Last_A_Frame > -1)
            {
                CurrentDifficultyFrames = ttyd::battle_ac::GuardFrames[CommandDifficulty];
                FramesEarly = temp_Last_A_Frame - CurrentDifficultyFrames + 1;
                Button = 'A';
            }
            else
            {
                CurrentDifficultyFrames = ttyd::battle_ac::SuperguardFrames[CommandDifficulty];
                FramesEarly = temp_Last_B_Frame - CurrentDifficultyFrames + 1;
                Button = 'B';
            }
            
            const char *CheckForPlural;
            if (FramesEarly != 1)
            {
                CheckForPlural = "s";
            }
            else
            {
                CheckForPlural = "";
            }
            
            sprintf(tempDisplayBuffer,
                "Pressed %c %" PRId32 " frame%s early",
                Button,
                FramesEarly,
                CheckForPlural);
            
            TextToDraw = tempDisplayBuffer;
            break;
        }
        case CannotBeSuperguarded:
        {
            TextToDraw = "Cannot superguard this attack";
            break;
        }
        default:
        {
            DisplayActionCommands.DisplayTimer = 0;
            return;
        }
    }
    
    uint32_t TextColor     = 0xFFFFFFFF;
    uint32_t WindowColor   = 0x000000D0;
    uint8_t Alpha          = 0xFF;
    int32_t TextPosX       = -232;
    int32_t TextPosY       = -105;
    // int32_t WindowWidth = 320;
    int32_t WindowCurve    = 10;
    float Scale            = 0.7f;
    
    int32_t WindowWidth    = static_cast<int32_t>(getMessageWidth(TextToDraw, Scale));
    
    drawTextWithWindow(TextToDraw, TextPosX, TextPosY, Alpha, 
        TextColor, Scale, WindowWidth, WindowColor, WindowCurve);
    
    DisplayActionCommands.DisplayTimer--;
}

void drawEffsActive()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = 60;
    float Scale    = 0.65f;
    
    // Get the total amount of effs active
    ttyd::effdrv::EffWork *EffWrk = ttyd::effdrv::effWorkPointer;
    uint32_t MaxEntries = EffWrk->maxEntries;
    uint32_t CurrentCount = 0;
    
    ttyd::effdrv::EffEntry *EffEntry = &EffWrk->pEntries[0];
    for (uint32_t i = 0; i < MaxEntries; i++)
    {
        if (EffEntry->wFlags)
        {
            CurrentCount++;
        }
        EffEntry++;
    }
    
    // Draw the total amount of effs active out of the max
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Effs Active: %" PRIu32 "/%" PRIu32,
        CurrentCount,
        MaxEntries);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
}

void drawEvtsActive()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = 60;
    float Scale    = 0.65f;
    
    // Move the text down if the effs display is active
    if (Displays[EFFS_ACTIVE])
    {
        PosY -= 20;
    }
    
    // Get the total amount of evts active
    ttyd::evtmgr::EvtWork *EvtWrk = ttyd::evtmgr::evtGetWork();
    int32_t MaxEntries = EvtWrk->entryCount;
    int32_t CurrentCount = 0;
    
    ttyd::evtmgr::EvtEntry *EvtEntry = &EvtWrk->entries[0];
    for (int32_t i = 0; i < MaxEntries; i++)
    {
        if (EvtEntry->flags & (1 << 0)) // Check if 0 bit is active
        {
            CurrentCount++;
        }
        EvtEntry++;
    }
    
    // Draw the total amount of evts active out of the max
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Evts Active: %" PRId32 "/%" PRId32,
        CurrentCount,
        MaxEntries);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
}

void drawEnemyEncounterNotifier()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = 60;
    float Scale    = 0.65f;
    
    // Move the text down if the effs display is active
    if (Displays[EFFS_ACTIVE])
    {
        PosY -= 20;
    }
    
    // Move the text down if the evts display is active
    if (Displays[EVTS_ACTIVE])
    {
        PosY -= 20;
    }
    
    // Draw the text
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Encounter occured x%" PRIu32,
        EnemyEncounterNotifier.Counter);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
}

void drawSettingsMemoryCardUsed()
{
    uint32_t Color = 0xFFFFFFFF;
    int32_t PosX   = -232;
    int32_t PosY   = 60;
    float Scale    = 0.6f;
    
    // Get the memory card slot being used
    char CurrentMemoryCardSlot;
    if (MenuSettings.MemoryCardSlot == CARD_SLOT_A)
    {
        CurrentMemoryCardSlot = 'A';
    }
    else
    {
        CurrentMemoryCardSlot = 'B';
    }
    
    // Draw the text
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Memory Card Slot\nSlot %c",
        CurrentMemoryCardSlot);
    
    drawText(tempDisplayBuffer, PosX, PosY, Color, Scale);
}

void drawSettingsCurrentWork()
{
    const char *String;
    int32_t WindowWidth;
    int32_t TextPosX = -50;
    
    switch (MenuSettings.ReturnCode)
    {
        case FILE_NOT_FOUND:
        {
            String = "Settings File Not Found";
            WindowWidth = 226;
            TextPosX -= 48;
            
#ifdef TTYD_JP
            WindowWidth -= 8;
            TextPosX += 3;
#endif
            break;
        }
        case RENAME_FAILED:
        {
            String = "Rename Failed";
            WindowWidth = 151;
            TextPosX -= 11;
            
#ifdef TTYD_JP
            WindowWidth -= 7;
            TextPosX += 4;
#endif
            break;
        }
        case RENAME_SUCCESSFUL:
        {
            String = "Rename Successful";
            WindowWidth = 183;
            TextPosX -= 27;
            
#ifdef TTYD_JP
            WindowWidth += 1;
#endif
            break;
        }
        case FILE_ALREADY_RENAMED:
        {
            String = "Settings File Already Renamed";
            WindowWidth = 284;
            TextPosX -= 77;
            
#ifdef TTYD_JP
            WindowWidth -= 6;
            TextPosX += 3;
#endif
            break;
        }
        case DELETE_SUCCESSFUL:
        {
            String = "Delete Successful";
            WindowWidth = 175;
            TextPosX -= 23;
            
#ifdef TTYD_JP
            WindowWidth -= 2;
            TextPosX += 2;
#endif
            break;
        }
        case DELETE_FAILED:
        {
            String = "Delete Failed";
            WindowWidth = 141;
            TextPosX -= 6;
            
#ifdef TTYD_JP
            WindowWidth -= 8;
            TextPosX += 5;
#endif
            break;
        }
        case SAVE_FAILED:
        {
            String = "Save Failed";
            WindowWidth = 129;
            
#ifdef TTYD_JP
            WindowWidth -= 7;
            TextPosX += 4;
#endif
            break;
        }
        case SAVE_SUCCESSFUL:
        {
            String = "Save Successful";
            WindowWidth = 160;
            TextPosX -= 15;
            
#ifdef TTYD_JP
            WindowWidth += 4;
            TextPosX -= 2;
#endif
            break;
        }
        case LOAD_FAILED:
        {
            String = "Load Failed";
            WindowWidth = 129;
            
#ifdef TTYD_JP
            WindowWidth -= 7;
            TextPosX += 4;
#endif
            break;
        }
        case LOAD_SUCCESSFUL:
        {
            String = "Load Successful";
            WindowWidth = 162;
            TextPosX -= 16;
            break;
        }
        case MEMCARD_IN_USE:
        {
            String = "Memory Card Currently In Use";
            WindowWidth = 275;
            TextPosX -= 73;
            
#ifdef TTYD_JP
            WindowWidth -= 3;
            TextPosX += 2;
#endif
            break;
        }
        default:
        {
            MenuVar.Timer = 0;
            return;
        }
    }
    
    uint32_t TextColor   = 0xFFFFFFFF;
    uint32_t WindowColor = 0x151515F4;
    uint8_t Alpha        = 0xFF;
    int32_t TextPosY     = 0;
    int32_t WindowCurve  = 10;
    float Scale          = 0.6f;
    
    drawTextWithWindow(String, TextPosX, TextPosY, Alpha, 
        TextColor, Scale, WindowWidth, WindowColor, WindowCurve);
    
    uint32_t tempTimer = MenuVar.Timer;
    MenuVar.Timer = tempTimer - 1;
}

void drawHeapArrayErrors()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = 100;
    float Scale    = 0.6f;
    
    // Draw the text
    char *tempHeapCorruptionBuffer = HeapInfo.HeapCorruptionBuffer;
    drawTextAndInit(tempHeapCorruptionBuffer, PosX, PosY, Alpha, Color, true, Scale);
    
    // Clear the heap corruption buffer
    clearHeapCorruptionBuffer();
}

void drawMemoryUsage()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = 60;
    float Scale    = 0.6f;
    
    // Start the text drawing sequence
    drawTextInit(Alpha, true);
    
    // Move the text down if the effs display is active
    if (Displays[EFFS_ACTIVE])
    {
        PosY -= 20;
    }
    
    // Move the text down if the evts display is active
    if (Displays[EVTS_ACTIVE])
    {
        PosY -= 20;
    }
    
    // Move the text down if the enemy encounter notifier is active
    if (Displays[ENEMY_ENCOUNTER_NOTIFIER])
    {
        PosY -= 20;
    }
    
    char *tempMemoryUsageBuffer = HeapInfo.MemoryUsageBuffer;
    bool *DisplayHeapInfo = HeapInfo.DisplayHeapInfo;
    
    int32_t NumHeaps = HeapInfo.ArrayCount - 1; // Remove the smart heap from the total
    uint32_t MemoryUsageCounter = 0;
    
    // Draw the text for the main heaps
    for (int32_t i = 0; i < NumHeaps; i++)
    {
        // Only draw if the current option is enabled
        if (DisplayHeapInfo[i])
        {
            // Draw the used and free text
            uint32_t MemoryUsageBufferIndex = MemoryUsageCounter * MEMORY_USAGE_LINE_BUFFER_SIZE;
            if (tempMemoryUsageBuffer[MemoryUsageBufferIndex] != '\0')
            {
                drawText(&tempMemoryUsageBuffer[MemoryUsageBufferIndex], PosX, PosY, Color, Scale);
                PosY -= 20;
            }
            
            MemoryUsageBufferIndex += MEMORY_USAGE_LINE_BUFFER_SIZE;
            if (tempMemoryUsageBuffer[MemoryUsageBufferIndex] != '\0')
            {
                drawText(&tempMemoryUsageBuffer[MemoryUsageBufferIndex], PosX, PosY, Color, Scale);
                PosY -= 20;
            }
        }
        MemoryUsageCounter += 2;
    }
    
    // Draw the text for the smart heap
    if (DisplayHeapInfo[NumHeaps])
    {
        // Draw the used text
        uint32_t MemoryUsageBufferIndex = MemoryUsageCounter * MEMORY_USAGE_LINE_BUFFER_SIZE;
        if (tempMemoryUsageBuffer[MemoryUsageBufferIndex] != '\0')
        {
            drawText(&tempMemoryUsageBuffer[MemoryUsageBufferIndex], PosX, PosY, Color, Scale);
        }
    }
    
    // Clear each of the memory usage buffers
    clearMemoryUsageBuffer();
}

void drawNpcNameToPtrError()
{
    uint32_t Color = 0xFFFFFFFF;
    uint8_t Alpha  = 0xFF;
    int32_t PosX   = -232;
    int32_t PosY   = 120;
    float Scale    = 0.6f;
    
    // Draw the text
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "npcNameToPtr error occured x%" PRIu32,
        NpcNameToPtrError.Counter);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, Color, true, Scale);
}

void drawTitleScreenInfo()
{
    // Draw the window for the text
    uint32_t WindowColor = 0x000000E5;
    int32_t PosX         = -225;
    int32_t PosY         = -16;
    int32_t WindowWidth  = 450;
    int32_t WindowHeight = 70;
    int32_t Curve        = 10;
    
#ifdef TTYD_JP
    PosY += 30;
    WindowWidth -= 10;
#endif
    
    drawWindow(WindowColor, PosX, PosY, WindowWidth, WindowHeight, Curve);
    
    // Draw the text
    uint32_t TextColor = 0xFFFFFFFF;
    uint8_t Alpha      = 0xFF;
    float Scale        = 0.75f;
    PosX               += 108;
    PosY               -= 14;
    
    char *tempDisplayBuffer = DisplayBuffer;
    sprintf(tempDisplayBuffer,
        "Practice Codes %s\nCreated by Zephiles",
        VersionNumber);
    
    drawTextAndInit(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, false, Scale);
}

void drawFileSelectScreenInfo()
{
    // Draw the window for the text
    uint32_t WindowColor = 0x000000E5;
    int32_t PosX         = -225;
    int32_t PosY         = -20;
    int32_t WindowWidth  = 450;
    int32_t WindowHeight = 102;
    int32_t Curve        = 10;
    
    drawWindow(WindowColor, PosX, PosY, WindowWidth, WindowHeight, Curve);
    
    // Draw the text
    uint32_t TextColor   = 0xFFFFFFFF;
    uint8_t Alpha        = 0xFF;
    float Scale          = 0.6f;
    PosX                 += 15;
    PosY                 -= 13;
    
    const char *String = "Press L + Start to open the menu\n\nFor more info, go to the main page:\nhttps://github.com/Zephiles/TTYD-Practice-Codes";
    
    drawTextAndInit(String, PosX, PosY, Alpha, TextColor, false, Scale);
}

void Mod::drawArtAttackHitboxes(ttyd::dispdrv::CameraId cameraId)
{
    // Call the original function immediately, as several functions need to run for the lines to be drawn properly
    mPFN_scissor_disp_control_trampoline(cameraId);
    
    if (!Displays[ART_ATTACK_HITBOXES])
    {
        return;
    }
    
    // Set the initial color to use for the lines of the hitboxes
    uint8_t HSVA[4];
    *reinterpret_cast<uint32_t *>(&HSVA) = 0x00FFCCFF; // RGBA value is 0xCC0000FF
    
    for (uint32_t Slot = 0; Slot < 64; Slot++)
    {
        void *BattleUnitPtr = getActorPointer(Slot);
        if (!BattleUnitPtr)
        {
            continue;
        }
        
        // Check if the current actor is a normal enemy that can be attacked
        ttyd::battle_unit::ActorGroupBelong BattleUnitBelong = ttyd::battle_unit::BtlUnit_GetBelong(BattleUnitPtr);
        if (BattleUnitBelong != ttyd::battle_unit::ActorGroupBelong::kEnemy)
        {
            // The current actor is not a normal enemy that can be attacked, so don't draw the hitbox
            continue;
        }
        
        // Check if the defeated flag is set for the current actor
        const uint32_t DefeatedFlag = 27;
        bool DefeatedFlagIsOn = ttyd::battle_unit::BtlUnit_CheckStatus(BattleUnitPtr, DefeatedFlag);
        if (DefeatedFlagIsOn)
        {
            // The defeated flag is set for the current actor, so don't draw the hitbox
            continue;
        }
        
        // Check if the current actor can be attacked
        int32_t BodyPartsId = ttyd::battle_unit::BtlUnit_GetBodyPartsId(BattleUnitPtr);
        void *BodyPartsPtr = ttyd::battle_unit::BtlUnit_GetPartsPtr(BattleUnitPtr, BodyPartsId);
        
        uint32_t BodyPartsFlags = *reinterpret_cast<uint32_t *>(
            reinterpret_cast<uint32_t>(BodyPartsPtr) + 0x1AC);
        
        if (BodyPartsFlags & (1 << 16)) // Check if 16 bit is on
        {
            // The current actor cannot be attacked, so don't draw the hitbox
            continue;
        }
        
        // Get the RGBA equivalent of the HSVA value
        uint32_t HitboxLineColor = ttyd::fontmgr::HSV2RGB(HSVA);
        
        // Set the color of the lines of the current hitbox
        gc::gx::GXSetChanMatColor(gc::gx::GXChannelID::GX_COLOR0A0, reinterpret_cast<uint8_t *>(&HitboxLineColor));
        
        // Adjust the hue for the lines of the next hitbox
        HSVA[0] += 45;
        
        uint32_t BattleUnitPtrUint = reinterpret_cast<uint32_t>(BattleUnitPtr);
        
        // Check if the current actor is hanging from the ceiling
        uint32_t ActorFlags = *reinterpret_cast<uint32_t *>(BattleUnitPtrUint + 0x104);
        float DrawHitboxDirection;
        
        if (ActorFlags & (1 << 1)) // Check if 1 bit is on
        {
            // The current actor is hanging from the ceiling
            DrawHitboxDirection = -1.f; // Draw down
        }
        else
        {
            // The current actor is not hanging from the ceiling
            DrawHitboxDirection = 1.f; // Draw up
        }
        
        // Get the variables used to calculate the position of the hitbox
        float ActorHitboxWidth      = *reinterpret_cast<float *>(BattleUnitPtrUint + 0xE4);
        float ActorHitboxHeight     = *reinterpret_cast<float *>(BattleUnitPtrUint + 0xE8);
        float ActorHitboxPosOffsetX = *reinterpret_cast<float *>(BattleUnitPtrUint + 0xD8);
        float ActorHitboxPosOffsetY = *reinterpret_cast<float *>(BattleUnitPtrUint + 0xDC);
        float ActorSizeScale        = *reinterpret_cast<float *>(BattleUnitPtrUint + 0x114);
        
        // Get the position of the current actor
        float ActorPos[3];
        ttyd::battle_unit::BtlUnit_GetPos(BattleUnitPtr, &ActorPos[0], &ActorPos[1], &ActorPos[2]);
        
        // Get the X and Y coordinate starting positions
        float DrawHitboxPosXStart = (ActorHitboxPosOffsetX * ActorSizeScale) + ActorPos[0];
        float DrawHitboxPosYStart = (DrawHitboxDirection * ActorHitboxPosOffsetY * ActorSizeScale) + ActorPos[1];
        
        // Get the amount to adjust the starting positions by
        float DrawHitboxAdjustPosX = (ActorHitboxWidth * ActorSizeScale) * 0.5f;
        float DrawHitboxAdjustPosY = (ActorHitboxHeight * ActorSizeScale) * 0.5f;
        
        // Set up a set of points, used to get the starts and ends of lines
        float ScreenPoint[3];
        
        // Set the Z coordinate for all calculated points, as it will not change
        ScreenPoint[2] = ActorPos[2];
        
        // Set up 2 sets of points; One for the start of a line and one for the end of a line
        float ScreenPointOutLineStart[3];
        float ScreenPointOutLineEnd[3];
        
        // Draw the 4 lines that show the hitbox
        for (uint32_t i = 0; i < 4; i++)
        {
            if (i == 0)
            {
                // Get the top-left corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart - DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart + DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineStart);
                
                // Get the top-right corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart + DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart + DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineEnd);
            }
            else if (i == 1)
            {
                // Get the top-right corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart + DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart + DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineStart);
                
                // Get the bottom-right corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart + DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart - DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineEnd);
            }
            else if (i == 2)
            {
                // Get the bottom-right corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart + DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart - DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineStart);
                
                // Get the bottom-left corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart - DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart - DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineEnd);
            }
            else // if (i == 3)
            {
                // Get the bottom-left corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart - DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart - DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineStart);
                
                // Get the top-left corner of the hitbox
                ScreenPoint[0] = DrawHitboxPosXStart - DrawHitboxAdjustPosX;
                ScreenPoint[1] = DrawHitboxPosYStart + DrawHitboxAdjustPosY;
                ttyd::battle_disp::btlGetScreenPoint(ScreenPoint, ScreenPointOutLineEnd);
            }
            
            // Draw the line from corner 1 to corner 2
            gc::gx::GXBegin(gc::gx::GXPrimitive::GX_LINES, gc::gx::GXVtxFmt::GX_VTXFMT0, 2);
            
            gc::ppc::writeGatherPipe.f32 = ScreenPointOutLineStart[0];
            gc::ppc::writeGatherPipe.f32 = ScreenPointOutLineStart[1];
            gc::ppc::writeGatherPipe.f32 = 0;
            
            gc::ppc::writeGatherPipe.f32 = ScreenPointOutLineEnd[0];
            gc::ppc::writeGatherPipe.f32 = ScreenPointOutLineEnd[1];
            gc::ppc::writeGatherPipe.f32 = 0;
        }
    }
}

void Mod::errorHandler(uint16_t error, gc::OSContext::OSContext *context, uint32_t dsisr, uint32_t dar)
{
    // Reinit the smart heap
    ttyd::memory::L_smartReInit();
    
    // Init memory for the strings
    ttyd::memory::SmartAllocationData *ErrorHandlerStringsMemory = 
        ttyd::memory::smartAlloc(sizeof(ErrorHandlerStrings), 
        ttyd::memory::SmartAllocationGroup::kNone);
    
    // Set up a temporary local variable to use for getting the strings memory
    ErrorHandlerStrings *ErrorHandler = reinterpret_cast<ErrorHandlerStrings *>(ErrorHandlerStringsMemory->pMemory);
    
    // Get the OS error
    if (error == 3)
    {
        // ISI error
        ErrorHandler->OSError = "---- OS_ERROR_ISI ----";
    }
    else if (error < 3)
    {
        // DSI error
        ErrorHandler->OSError = "---- OS_ERROR_DSI ----";
    }
    else if (error == 6)
    {
        // Program error
        ErrorHandler->OSError = "---- OS_ERROR_PROGRAM ----";
    }
    
    // Get the context address
    sprintf(ErrorHandler->ContextAddress, 
        "---- Context 0x%08" PRIX32 " ----", 
        reinterpret_cast<uint32_t>(context));
    
    // Get the general purpose register values
    uint32_t GeneralRegistersSize = sizeof(ErrorHandler->GeneralRegisterValues) / 
        sizeof(ErrorHandler->GeneralRegisterValues[0]);
    
    for (uint32_t i = 0; i < GeneralRegistersSize; i++)
    {
        sprintf(ErrorHandler->GeneralRegisterValues[i],
            "0x%08" PRIX32,
            context->gpr[i]);
    }
    
    // Additional register names
    static const char *AdditionalRegisterNames[] = 
    {
        "LR",
        "SRR0",
        "DSISR",
        "CR",
        "SRR1",
        "DAR",
    };
    
    // Get the additional register values
    // Get the LR value
    uint32_t j = 0;
    sprintf(ErrorHandler->AdditionalRegisterValues[j++],
        "0x%08" PRIX32,
        context->lr);
    
    // Get the SRR0 value
    sprintf(ErrorHandler->AdditionalRegisterValues[j++],
        "0x%08" PRIX32,
        context->srr0);
    
    // Get the DSISR value
    sprintf(ErrorHandler->AdditionalRegisterValues[j++],
        "0x%08" PRIX32,
        dsisr);
    
    // Get the CR value
    sprintf(ErrorHandler->AdditionalRegisterValues[j++],
        "0x%08" PRIX32,
        context->cr);
    
    // Get the SRR1 value
    sprintf(ErrorHandler->AdditionalRegisterValues[j++],
        "0x%08" PRIX32,
        context->srr1);
    
    // Get the DAR value
    sprintf(ErrorHandler->AdditionalRegisterValues[j++],
        "0x%08" PRIX32,
        dar);
    
    // Address, Back Chains, and LR Saves names
    static const char *AddressHeaderNames[] = 
    {
        "Address",
        "Back Chain",
        "LR Save",
    };
    
    // Get the addresses, back chains, and LR saves
    // Loop through the addresses and fill in the address, back chain, and LR save fields
    uint32_t *CurrentContext = reinterpret_cast<uint32_t *>(context->gpr[1]);
    uint32_t AddressCount = 1; // Will be used to determine how many lines to draw later
    
    for (uint32_t i = 0; CurrentContext && 
        (reinterpret_cast<uint32_t>(CurrentContext) != 0xFFFFFFFF) && (i++ < MAX_LEVELS);
        CurrentContext = reinterpret_cast<uint32_t *>(*CurrentContext))
    {
        // Get the address
        sprintf(ErrorHandler->AddressList[i], 
            "0x%08" PRIX32, 
            reinterpret_cast<uint32_t>(CurrentContext));
        
        // Get the back chain
        sprintf(ErrorHandler->AddressList[i + MAX_LEVELS], 
            "0x%08" PRIX32, 
            CurrentContext[0]);
        
        // Get the LR save
        sprintf(ErrorHandler->AddressList[(i + MAX_LEVELS) * 2], 
            "0x%08" PRIX32, 
            CurrentContext[1]);
        
        AddressCount++;
    }
    
    // Get the instruction where the error occured, as well as the invalid address
    const char *Format = "Instruction at 0x%" PRIX32 "\n(read from SRR0) attempted to access\ninvalid address 0x%" PRIX32 " (read from DAR)";
    sprintf(ErrorHandler->InstructionAndAddress, 
        Format, 
        context->srr0, 
        dar);
    
    // Fill FPU context
    gc::OSContext::L_OSFillFPUContext(context);
    
    // Get the float register values
    uint32_t FloatRegistersSize = sizeof(ErrorHandler->FloatRegisterValues) / 
        sizeof(ErrorHandler->FloatRegisterValues[0]);
    
    for (uint32_t i = 0; i < FloatRegistersSize; i++)
    {
        uint64_t *Address = reinterpret_cast<uint64_t *>(&context->fpr[i]);
        sprintf(ErrorHandler->FloatRegisterValues[i],
            "0x%016" PRIX64,
            *Address);
    }
    
    // Get the paired singles
    uint32_t PairedSinglesSize = sizeof(ErrorHandler->PairedSinglesValues) / 
        sizeof(ErrorHandler->PairedSinglesValues[0]);
    
    // Only get them if they are enabled
    bool PairedSinglesEnabled = asmErrorHandlerCheckPairedSinglesEnabled();
    if (PairedSinglesEnabled)
    {
        for (uint32_t i = 0; i < PairedSinglesSize; i++)
        {
            uint64_t *Address = reinterpret_cast<uint64_t *>(&context->psf[i]);
            sprintf(ErrorHandler->PairedSinglesValues[i],
                "0x%016" PRIX64,
                *Address);
        }
    }
    
    // Disable audio
    ttyd::pmario_sound::psndExit();
    
    // Allow interrupts
    asmErrorHandlerInterrupts();
    
    // Init memory for the font data
    ttyd::memory::SmartAllocationData *FontDataMemory;
    
    uint32_t CurrentFontEncode = gc::OSFont::OSGetFontEncode();
    if (CurrentFontEncode == 0)
    {
        // ANSI
        FontDataMemory = ttyd::memory::smartAlloc(0x20120, 
            ttyd::memory::SmartAllocationGroup::kNone);
    }
    else
    {
        // SJIS
        FontDataMemory = ttyd::memory::smartAlloc(0x120F00, 
            ttyd::memory::SmartAllocationGroup::kNone);
    }
    
    // Init the font data
    gc::OSFont::L_OSInitFont(reinterpret_cast<gc::OSFont::OSFontHeader *>(FontDataMemory->pMemory));
    
    // Init the DEMO pad
    gc::DEMOPad::DEMOPadInit();
    
    // Various other init stuff
    // Clear EFB
    uint32_t CopyClearColor = 0x000000FF;
    gc::gx::GXSetCopyClear(
        reinterpret_cast<uint8_t *>(&CopyClearColor), 
        0x00FFFFFF);
    
    gc::gx::GXCopyDisp(
        gc::DEMOInit::L_DEMOGetCurrentBuffer(), 
        true);
    
    // Set the visible screen region
    gc::gx::GXSetViewport(0.f, 0.f, 608.f, 480.f, 0.f, 1.f);
    gc::gx::GXSetScissor(0, 0, 608, 480);
    
    // Set matrices up to screen coordinates system
    gc::mtx::mtx44 MtxProjection;
    gc::mtx::C_MTXOrtho(
        MtxProjection, 
        0.f, 
        608.f, 
        0.f, 
        480.f, 
        0.f, 
        1.f);
    
    gc::gx::GXSetProjection(
        MtxProjection, 
        gc::gx::GXProjectionType::GX_ORTHOGRAPHIC);
    
    // Set the pixel processing mode
    gc::gx::GXSetZMode(
        true, 
        gc::gx::GXCompare::GX_LEQUAL, 
        true);
    
    // Set the TEV parameters to replace the color
    gc::gx::GXSetNumChans(0);
    gc::gx::GXSetNumTevStages(1);
    
    gc::gx::GXSetTevOp(
        gc::gx::GXTevStageID::GX_TEVSTAGE0, 
        gc::gx::GXTevMode::GX_REPLACE);
    
    gc::gx::GXSetTevOrder(
        gc::gx::GXTevStageID::GX_TEVSTAGE0, 
        gc::gx::GXTexCoordID::GX_TEXCOORD0, 
        gc::gx::GXTexMapID::GX_TEXMAP0, 
        gc::gx::GXChannelID::GX_COLOR_NULL);
    
    // Set the number of texture coordinates
    gc::gx::GXSetNumTexGens(1);
    
    // Specify specific texgen options
    gc::gx::GXSetTexCoordGen2(
        gc::gx::GXTexCoordID::GX_TEXCOORD0, 
        gc::gx::GXTexGenType::GX_TG_MTX2x4, 
        gc::gx::GXTexGenSrc::GX_TG_TEX0, 
        gc::gx::GXTexMtx::GX_TEXMTX0, 
        false, 
        125);
    
    // Translucent mode
    gc::gx::GXSetBlendMode(
        gc::gx::GXBlendMode::GX_BM_BLEND, 
        gc::gx::GXBlendFactor::GX_BL_ONE, 
        gc::gx::GXBlendFactor::GX_BL_ONE, 
        gc::gx::GXLogicOperation::GX_LO_CLEAR);
    
    // Set up vertex descriptors
    gc::gx::GXClearVtxDesc();
    
    gc::gx::GXSetVtxDesc(
        gc::gx::GXAttribute::GX_VA_POS, 
        gc::gx::GXAttributeType::GX_DIRECT);
    
    gc::gx::GXSetVtxDesc(
        gc::gx::GXAttribute::GX_VA_TEX0, 
        gc::gx::GXAttributeType::GX_DIRECT);
    
    gc::gx::GXSetVtxAttrFmt(
        gc::gx::GXVtxFmt::GX_VTXFMT0, 
        gc::gx::GXAttribute::GX_VA_POS, 
        gc::gx::GXComponentContents::GX_POS_XYZ, 
        gc::gx::GXComponentType::GX_S16, 
        0);
    
    gc::gx::GXSetVtxAttrFmt(
        gc::gx::GXVtxFmt::GX_VTXFMT0, 
        gc::gx::GXAttribute::GX_VA_TEX0, 
        gc::gx::GXComponentContents::GX_POS_XYZ, 
        gc::gx::GXComponentType::GX_S16, 
        0);
    
    // Set up an auto function for drawing each string
    auto drawString = [=](int32_t textPosX, int32_t textPosY, const char *text, float fontScale)
    {
        const int32_t OriginalTextPosX = textPosX;
        int32_t PosYIncrementAmount = 24;
        
        gc::OSFont::OSFontHeader *tempFontData = reinterpret_cast<gc::OSFont::OSFontHeader *>(FontDataMemory->pMemory);
        
        while (text[0] != '\0')
        {
            if (text[0] == '\n')
            {
                // Go to the next line
                text++;
                textPosX = OriginalTextPosX;
                textPosY += PosYIncrementAmount;
            }
            else
            {
                void *Image;
                int32_t ImagePosX;
                int32_t ImagePosY;
                int32_t ImageWidth;
                
                text = gc::OSFont::L_OSGetFontTexture(
                    &text[0], 
                    &Image, 
                    &ImagePosX, 
                    &ImagePosY, 
                    &ImageWidth);
                
                // Set up and load the texture object
                uint16_t SheetWidth = tempFontData->sheetWidth;
                uint16_t SheetHeight = tempFontData->sheetHeight;
                
                gc::gx::GXTexObj TexObj;
                gc::gx::GXInitTexObj(
                    &TexObj, 
                    Image, 
                    SheetWidth, 
                    SheetHeight, 
                    static_cast<gc::gx::GXTexFmt>(tempFontData->sheetFormat), 
                    gc::gx::GXTexWrapMode::GX_CLAMP, 
                    gc::gx::GXTexWrapMode::GX_CLAMP, 
                    false);
                
                gc::gx::GXLoadTexObj(
                    &TexObj, 
                    gc::gx::GXTexMapID::GX_TEXMAP0);
                
                gc::mtx::mtx34 MtxTextImage;
                gc::mtx::PSMTXScale(
                    MtxTextImage, 
                    1.f / static_cast<float>(SheetWidth), 
                    1.f / static_cast<float>(SheetHeight), 
                    1.f);
                
                gc::gx::GXLoadTexMtxImm(
                    MtxTextImage, 
                    gc::gx::GXTexMtx::GX_TEXMTX0, 
                    gc::gx::GXTexMtxType::GX_MTX2x4);
                
                // Set up the matrix to use for the text
                gc::mtx::mtx34 MtxFontSize;
                gc::mtx::PSMTXScale(
                    MtxFontSize, 
                    fontScale, 
                    fontScale, 
                    1.f);
                
                gc::gx::GXLoadPosMtxImm(
                    MtxFontSize, 
                    gc::gx::GXPosNormMtx::GX_PNMTX0);
                
                gc::gx::GXSetCurrentMtx(gc::gx::GXPosNormMtx::GX_PNMTX0);
                
                // Start the drawing process
                gc::gx::GXBegin(
                    gc::gx::GXPrimitive::GX_QUADS, 
                    gc::gx::GXVtxFmt::GX_VTXFMT0, 
                    4);
                
                // Get the positions for the images and text
                uint16_t CellWidth = tempFontData->cellWidth;
                uint16_t CellHeight = tempFontData->cellHeight;
                
                int16_t ImagePosLeft = static_cast<int16_t>(ImagePosX);
                int16_t ImagePosRight = static_cast<int16_t>(ImagePosLeft + CellWidth);
                int16_t ImagePosTop = static_cast<int16_t>(ImagePosY);
                int16_t ImagePosBottom = static_cast<int16_t>(ImagePosTop + CellHeight);
                
                int16_t TextPosLeft = static_cast<int16_t>(textPosX);
                int16_t TextPosRight = static_cast<int16_t>(TextPosLeft + CellWidth);
                int16_t TextPosTop = static_cast<int16_t>(textPosY);
                int16_t TextPosBottom = static_cast<int16_t>(TextPosTop + CellHeight);
                
                // Draw the images and text
                gc::ppc::writeGatherPipe.s16 = TextPosLeft;
                gc::ppc::writeGatherPipe.s16 = TextPosTop;
                gc::ppc::writeGatherPipe.s16 = 0;
                
                gc::ppc::writeGatherPipe.s16 = ImagePosLeft;
                gc::ppc::writeGatherPipe.s16 = ImagePosTop;
                gc::ppc::writeGatherPipe.s16 = TextPosRight;
                gc::ppc::writeGatherPipe.s16 = TextPosTop;
                gc::ppc::writeGatherPipe.s16 = 0;
                
                gc::ppc::writeGatherPipe.s16 = ImagePosRight;
                gc::ppc::writeGatherPipe.s16 = ImagePosTop;
                gc::ppc::writeGatherPipe.s16 = TextPosRight;
                gc::ppc::writeGatherPipe.s16 = TextPosBottom;
                gc::ppc::writeGatherPipe.s16 = 0;
                
                gc::ppc::writeGatherPipe.s16 = ImagePosRight;
                gc::ppc::writeGatherPipe.s16 = ImagePosBottom;
                gc::ppc::writeGatherPipe.s16 = TextPosLeft;
                gc::ppc::writeGatherPipe.s16 = TextPosBottom;
                gc::ppc::writeGatherPipe.s16 = 0;
                
                gc::ppc::writeGatherPipe.s16 = ImagePosLeft;
                gc::ppc::writeGatherPipe.s16 = ImagePosBottom;
                
                textPosX += ImageWidth;
            }
        }
    };
    
    // Reset the temporary local variable to use for getting the strings memory
    ErrorHandler = reinterpret_cast<ErrorHandlerStrings *>(ErrorHandlerStringsMemory->pMemory);
    
    int32_t PosX = 50;
    int32_t PosY = 50;
    uint32_t PageNumber = 0;
    float FontScale = 0.65f;
    
    // Main draw loop
    while (1)
    {
        uint32_t FirstRetraceCount = gc::vi::VIGetRetraceCount();
        
        const int32_t RegisterValueIncrement = 40;
        const int32_t AddressesIncrement = 130;
        const int32_t PosYIncrementAmount = 24;
        
        // Get the current button inputs
        ttyd::system::makeKey();
        
        // Check to see if the text should be moved
        // Allow for both a left/right and an up/down at the same time
        if (checkButtonComboEveryFrame(PAD_DPAD_LEFT))
        {
            PosX -= 3;
        }
        else if (checkButtonComboEveryFrame(PAD_DPAD_RIGHT))
        {
            PosX += 3;
        }
        
        if (checkButtonComboEveryFrame(PAD_DPAD_DOWN))
        {
            PosY -= 5;
        }
        else if (checkButtonComboEveryFrame(PAD_DPAD_UP))
        {
            PosY += 5;
        }
        
        // Check to see if the font size should be changed
        if (checkButtonCombo(PAD_A))
        {
            FontScale += 0.05f;
        }
        else if (checkButtonCombo(PAD_B))
        {
            FontScale -= 0.05f;
        }
        
        // Check to see if the page number should be changed
        if (checkButtonCombo(PAD_X))
        {
            // Increment
            if (PageNumber < 2)
            {
                PageNumber++;
            }
            else
            {
                PageNumber = 0;
            }
        }
        else if (checkButtonCombo(PAD_Y))
        {
            // Decrement
            if (static_cast<int32_t>(PageNumber) <= 0)
            {
                PageNumber = 2;
            }
            else
            {
                PageNumber--;
            }
        }
        
        int32_t NewPosX = PosX;
        int32_t NewPosY = PosY;
        
        int32_t tempPosX;
        int32_t tempPosY;
        
        // Draw the help text
        const char *Text = "Press/hold the D-Pad to move the text\nPress X to go to the next page\nPress Y to go to the previous page\nPress A to zoom in\nPress B to zoom out";
        drawString(NewPosX, NewPosY, Text, FontScale);
        
        // Draw the page number and the version
#ifdef TTYD_JP
        const char *Format = "Page %" PRIu32 "\n\nJPN";
#elif defined TTYD_US
        const char *Format = "Page %" PRIu32 "\n\nUSA";
#elif defined TTYD_EU
        const char *Format = "Page %" PRIu32 "\n\nEUR";
#endif
        
        char MiscBuffer[16];
        sprintf(MiscBuffer, Format, PageNumber + 1);
        
        drawString(NewPosX + 400, NewPosY, MiscBuffer, FontScale);
        NewPosY += PosYIncrementAmount * 6;
        
        if (PageNumber == 0)
        {
            // Draw the register OSError if it is valid
            const char *OSError = ErrorHandler->OSError;
            if (OSError)
            {
                drawString(NewPosX, NewPosY, OSError, FontScale);
                NewPosY += PosYIncrementAmount;
            }
            
            // Draw the context address
            drawString(NewPosX, NewPosY, ErrorHandler->ContextAddress, FontScale);
            NewPosY += PosYIncrementAmount;
            
            // Draw the general purpose registers and values in 3 columns
            tempPosX = NewPosX;
            tempPosY = NewPosY;
            
            uint32_t TotalRowsZeroIndexed = (GeneralRegistersSize - 1) / 3;
            uint32_t Counter = 0;
            
            for (uint32_t i = 0; i < GeneralRegistersSize; i++)
            {
                // Draw the general purpose register names
                sprintf(MiscBuffer, "r%" PRIu32, i);
                drawString(tempPosX, tempPosY, MiscBuffer, FontScale);
                
                // Draw the general purpose register values
                drawString(tempPosX + RegisterValueIncrement, tempPosY, ErrorHandler->GeneralRegisterValues[i], FontScale);
                
                if (Counter >= TotalRowsZeroIndexed)
                {
                    // Go to the next column
                    tempPosX += 170;
                    tempPosY = NewPosY;
                    Counter = 0;
                }
                else
                {
                    tempPosY += PosYIncrementAmount;
                    Counter++;
                }
            }
            NewPosY += (PosYIncrementAmount * TotalRowsZeroIndexed) + (PosYIncrementAmount * 2);
            
            // Draw the additional registers and values in 2 columns
            uint32_t SecondRowAdjustment = 0;
            uint32_t AdditionalRegisterNamesSize = sizeof(AdditionalRegisterNames) / sizeof(AdditionalRegisterNames[0]);
            TotalRowsZeroIndexed = (AdditionalRegisterNamesSize - 1) / 2;
            
            Counter = 0;
            tempPosX = NewPosX;
            tempPosY = NewPosY;
            
            for (uint32_t i = 0; i < AdditionalRegisterNamesSize; i++)
            {
                // Draw the additional register names
                drawString(tempPosX, tempPosY, AdditionalRegisterNames[i], FontScale);
                
                // Draw the additional register values
                drawString(tempPosX + 65 - SecondRowAdjustment, tempPosY, 
                    ErrorHandler->AdditionalRegisterValues[i], FontScale);
                
                if (Counter >= TotalRowsZeroIndexed)
                {
                    // Go to the next column
                    tempPosX += 190;
                    tempPosY = NewPosY;
                    Counter = 0;
                    
                    // Decrement the second row
                    if (SecondRowAdjustment == 0)
                    {
                        SecondRowAdjustment += 10;
                    }
                }
                else
                {
                    tempPosY += PosYIncrementAmount;
                    Counter++;
                }
            }
            NewPosY += (PosYIncrementAmount * TotalRowsZeroIndexed) + (PosYIncrementAmount * 2);
            
            // Draw the names for the addresses, back chains, and LR saves in 3 columns
            tempPosX = NewPosX;
            
            uint32_t AddressHeaderNamesSize = sizeof(AddressHeaderNames) / sizeof(AddressHeaderNames[0]);
            for (uint32_t i = 0; i < AddressHeaderNamesSize; i++)
            {
                drawString(tempPosX, NewPosY, AddressHeaderNames[i], FontScale);
                tempPosX += AddressesIncrement;
            }
            
            // Draw the addresses, back chains, and LR saves in 3 columns
            tempPosY = NewPosY;
            
            for (uint32_t i = 0; i < AddressCount; i++)
            {
                // Draw the addresses
                drawString(NewPosX, tempPosY, ErrorHandler->AddressList[i], FontScale);
                
                // Draw the back chains
                drawString(NewPosX + AddressesIncrement, tempPosY, 
                    ErrorHandler->AddressList[i + MAX_LEVELS], FontScale);
                
                // Draw the LR saves
                drawString(NewPosX + (AddressesIncrement * 2), tempPosY, 
                    ErrorHandler->AddressList[(i + MAX_LEVELS) * 2], FontScale);
                
                tempPosY += PosYIncrementAmount;
            }
            NewPosY += (AddressCount * PosYIncrementAmount) + PosYIncrementAmount;
            
            // Draw the instruction where the error occured, as well as the invalid address
            drawString(NewPosX, NewPosY, ErrorHandler->InstructionAndAddress, FontScale);
        }
        else if (PageNumber == 1)
        {
            // Draw the float registers and values in 2 columns
            tempPosX = NewPosX;
            tempPosY = NewPosY;
            
            uint32_t TotalRowsZeroIndexed = (FloatRegistersSize - 1) / 2;
            uint32_t Counter = 0;
            
            for (uint32_t i = 0; i < FloatRegistersSize; i++)
            {
                // Draw the float register names
                sprintf(MiscBuffer, "f%" PRIu32, i);
                drawString(tempPosX, tempPosY, MiscBuffer, FontScale);
                
                // Draw the float register values
                drawString(tempPosX + RegisterValueIncrement, tempPosY, 
                    ErrorHandler->FloatRegisterValues[i], FontScale);
                
                if (Counter >= TotalRowsZeroIndexed)
                {
                    // Go to the next column
                    tempPosX += 290;
                    tempPosY = NewPosY;
                    Counter = 0;
                }
                else
                {
                    tempPosY += PosYIncrementAmount;
                    Counter++;
                }
            }
        }
        else // if (PageNumber >= 2)
        {
            // Draw the paired singles
            // Only draw them if they are enabled
            if (PairedSinglesEnabled)
            {
                tempPosX = NewPosX;
                tempPosY = NewPosY;
                
                uint32_t TotalRowsZeroIndexed = (PairedSinglesSize - 1) / 2;
                uint32_t Counter = 0;
                
                for (uint32_t i = 0; i < PairedSinglesSize; i++)
                {
                    // Draw the paired singles names
                    sprintf(MiscBuffer, "p%" PRIu32, i);
                    drawString(tempPosX, tempPosY, MiscBuffer, FontScale);
                    
                    // Draw the paired singles values
                    drawString(tempPosX + RegisterValueIncrement + 5, tempPosY, 
                        ErrorHandler->PairedSinglesValues[i], FontScale);
                    
                    if (Counter >= TotalRowsZeroIndexed)
                    {
                        // Go to the next column
                        tempPosX += 290;
                        tempPosY = NewPosY;
                        Counter = 0;
                    }
                    else
                    {
                        tempPosY += PosYIncrementAmount;
                        Counter++;
                    }
                }
            }
        }
        
        // Various post-drawing stuff
        gc::gx::GXSetZMode(
            true, 
            gc::gx::GXCompare::GX_LEQUAL, 
            true);
        
        gc::gx::GXSetColorUpdate(true);
        
        gc::gx::GXSetAlphaCompare(
            gc::gx::GXCompare::GX_ALWAYS, 
            0, 
            gc::gx::GXAlphaOperation::GX_AOP_OR, 
            gc::gx::GXCompare::GX_ALWAYS, 
            0);
        
        gc::gx::GXSetAlphaUpdate(true);
        
        gc::gx::GXCopyDisp(
            gc::DEMOInit::DemoCurrentBuffer, 
            true);
        
        gc::vi::VISetNextFrameBuffer(gc::DEMOInit::DemoCurrentBuffer);
        gc::vi::VISetBlack(false);
        gc::vi::VIFlush();
        
        // Wait for retrace
        uint32_t CurrentRetraceCount;
        do
        {
            CurrentRetraceCount = gc::vi::VIGetRetraceCount();
        }
        while (FirstRetraceCount == CurrentRetraceCount);
        
        // Set the new DEMO buffer
        void *DemoFrameBuffer1 = gc::DEMOInit::DemoFrameBuffer1;
        if (gc::DEMOInit::DemoCurrentBuffer == DemoFrameBuffer1)
        {
            gc::DEMOInit::DemoCurrentBuffer = gc::DEMOInit::DemoFrameBuffer2;
        }
        else
        {
            gc::DEMOInit::DemoCurrentBuffer = DemoFrameBuffer1;
        }
    }
}

}