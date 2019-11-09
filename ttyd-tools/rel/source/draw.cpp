#include "draw.h"
#include "global.h"
#include "commonfunctions.h"
#include "menufunctions.h"
#include "memorywatch.h"
#include "mod.h"
#include "assembly.h"

#include <gc/gx.h>
#include <gc/ppc.h>
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
#include <ttyd/itemdrv.h>
#include <ttyd/battle_ac.h>
#include <ttyd/battle_unit.h>
#include <ttyd/battle_disp.h>

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
		reinterpret_cast<void (*)()>(user)();
	}, reinterpret_cast<void *>(func));
}

void drawFunctionOn2DLayer(void (*func)())
{
	ttyd::dispdrv::dispEntry(ttyd::dispdrv::CameraId::k2d, 2, 0.f, 
		[](ttyd::dispdrv::CameraId cameraId, void *user)
	{
		reinterpret_cast<void (*)()>(user)();
	}, reinterpret_cast<void *>(func));
}

void drawMenuWindow()
{
	uint32_t color 		= 0x000000F4;
	// uint32_t color 	= 0x444444F4;
	int32_t x 			= -245;
	int32_t y 			= 190;
	int32_t width 		= 490;
	int32_t height 		= 375;
	int32_t curve 		= 0;
	
	drawWindow(color, x, y, width, height, curve);
}

void drawWindow(uint32_t color, int32_t x, int32_t y, int32_t width, int32_t height, int32_t curve)
{
	uint8_t *NewColor 	= reinterpret_cast<uint8_t *>(&color);
	float NewX 			= static_cast<float>(x);
	float NewY 			= static_cast<float>(y);
	float NewWidth 		= static_cast<float>(width);
	float NewHeight 	= static_cast<float>(height);
	float NewCurve 		= static_cast<float>(curve);
	
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
	const int16_t DiamondStarIconValue 		= 407;
	const int16_t EmeraldStarIconValue 		= 408;
	const int16_t GarnetStarIconValue 		= 409;
	const int16_t GoldStarIconValue 		= 410;
	const int16_t CrystalStarIconValue 		= 411;
	const int16_t RubyStarIconValue 		= 412;
	const int16_t SapphireStarIconValue 	= 413;
	#else
	const int16_t DiamondStarIconValue 		= 415;
	const int16_t EmeraldStarIconValue 		= 416;
	const int16_t GarnetStarIconValue 		= 417;
	const int16_t GoldStarIconValue 		= 418;
	const int16_t CrystalStarIconValue 		= 419;
	const int16_t RubyStarIconValue 		= 420;
	const int16_t SapphireStarIconValue 	= 421;
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

void drawStringMultiline(float x, float y, const char *text, float scale)
{
	// Get the line increment amount
	// Round the value down
	float increment = (scale * 25) + 5;
	int32_t tempIncrementValue = static_cast<int32_t>(increment);
	increment = static_cast<float>(tempIncrementValue);
	
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
		y -= increment;
	}
	while (index != 0);
}

void drawText(const char *text, int32_t x, int32_t y, uint8_t alpha, uint32_t color, float scale)
{
	ttyd::fontmgr::FontDrawStart_alpha(alpha);
	ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
	ttyd::fontmgr::FontDrawEdge();
	ttyd::fontmgr::FontDrawScale(scale);
	
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
		drawStringMultiline(NewX, NewY, text, scale);
	}
	else
	{
		// The text has one line
		ttyd::fontmgr::FontDrawString(NewX, NewY, text);
	}
}

uint16_t getMessageWidth(const char *text, float scale)
{
	uint16_t LineLength = ttyd::fontmgr::FontGetMessageWidth(text);
	float Width = static_cast<float>(LineLength + 15) * (scale + 0.05);
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
	int32_t WindowHeight 	= (AdditionalLines * 20) + 44;
	int32_t WindowPosX 		= textPosX - 15;
	int32_t WindowPosY 		= textPosY + 13;
	
	// Draw the window
	drawWindow(windowColor, WindowPosX, WindowPosY, windowWidth, WindowHeight, windowCurve);
	
	// Draw the text
	drawText(text, textPosX, textPosY, alpha, textColor, textScale);
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
	uint32_t tempCurrentMenu 		= MenuVar.CurrentMenu;
	uint32_t tempSelectedOption 	= MenuVar.SelectedOption;
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t IndexStart 			= currentPage * maxOptionsPerPage;
	
	uint8_t Alpha 		= 0xFF;
	int32_t PosX 		= -232;
	// int32_t PosY 	= 180;
	float Scale 		= 0.6;
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
		drawText(CurrentLine, PosX, posY, Alpha, Color, Scale);
		posY -= 20;
	}
}

void drawMultipleColumnsVertical(int32_t posX, int32_t posY, uint32_t currentMenuOption, 
	uint32_t currentPage, uint32_t size, uint32_t maxOptionsPerPage, 
		uint32_t maxOptionsPerRow, bool showCurrentOption, 
			uint32_t posXIncrementAmount, const char **lines)
{
	uint32_t IndexStart 				= currentPage * maxOptionsPerPage;
	
	uint32_t TotalRowsPerPage 			= 1 + ((maxOptionsPerPage - 1) / maxOptionsPerRow); // Round up
	uint32_t TotalColumns 				= 1 + ((maxOptionsPerPage - 1) / TotalRowsPerPage); // Round up
	uint32_t ColumnSplitAmount 			= 1 + ((maxOptionsPerPage - 1) / TotalColumns); // Round up
	
	uint8_t Alpha 						= 0xFF;
	float Scale 						= 0.6;
	int32_t tempPosY 					= posY;
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
			drawText(CurrentLine, posX, posY, Alpha, Color, Scale);
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
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (!tempArray)
	{
		return;
	}
	
	uint32_t tempAddress 	= tempArray[0];
	uint32_t tempSize 		= tempArray[1];
	
	uint8_t Alpha 					= 0xFF;
	float TextScale 				= 0.6;
	float IconScale					= 0.6;
	int32_t PosX 					= -90;
	int32_t PosY 					= 125;
	
	const int32_t SetPosXText 		= PosX + 15;
	const int32_t SetPosYText 		= PosY + 20;
	const int32_t AdjustPosX 		= PosX + 260;
	const int32_t AdjustPosYIcon 	= PosY;
	const int32_t AdjustPosYText 	= SetPosYText;
	
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	IconPosition[IconPositionX] 	= 217;
	
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
	for (uint32_t i = 0; i < 2; i++)
	{
		// Draw the left and right columns
		drawItemIconsColumn(i * MultiplyAmount, IncrementAmount, tempSize, 
			PosX, PosY, tempAddress, IconScale);
		
		// Adjust position for right column
		PosX += AdjustPosX;
		PosY = AdjustPosYIcon;
	}
	
	// Adjust the initial position of the text
	PosX = SetPosXText;
	PosY = SetPosYText;
	
	// Draw the text
	for (uint32_t i = 0; i < 2; i++)
	{
		// Draw the left and right columns
		drawItemTextColumn(i * MultiplyAmount, IncrementAmount, tempSize, 
			PosX, PosY, tempAddress, Alpha, TextScale);
		
		// Adjust position for right column
		PosX += AdjustPosX;
		PosY = AdjustPosYText;
	}
	
	// Draw the L and/or R buttons if appropriate
	if (tempCurrentMenu == INVENTORY_STANDARD)
	{
		return;
	}
	
	float L_and_R_buttons_scale = 0.5;
	
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

void drawItemIconsColumn(uint32_t indexStart, uint32_t indexIncrement, uint32_t size, 
	int32_t posX, int32_t posY, uint32_t address, float scale)
{
	uint32_t MaxIconsPerColumn 		= 10;
	uint32_t MaxIconsPerPage 		= 20;
	uint32_t newIndexStart 			= indexStart + (MenuVar.CurrentPage * MaxIconsPerPage);
	uint32_t IndexCounter 			= newIndexStart;
	float tempScale;
	
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	IconPosition[IconPositionX] 	= posX;
	
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
			tempScale = 0.5;
		}
		else if (CurrentItem == ttyd::item_data::Item::MagicalMapBigger)
		{
			// Decrease the size of the bigger magical map
			tempScale = 0.4;
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

void drawItemTextColumn(uint32_t indexStart, uint32_t indexIncrement, uint32_t size, 
	int32_t posX, int32_t posY, uint32_t address, uint8_t alpha, float scale)
{
	uint32_t MaxIconsPerColumn 		= 10;
	uint32_t MaxIconsPerPage 		= 20;
	uint32_t tempCurrentPage 		= MenuVar.CurrentPage;
	uint32_t tempSelectedOption 	= MenuVar.SelectedOption;
	uint32_t newIndexStart 			= indexStart + (tempCurrentPage * MaxIconsPerPage);
	uint32_t IndexCounter 			= newIndexStart;
	
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
		drawText(ItemName, posX, posY, alpha, TextColor, scale);
		posY -= 30;
		IndexCounter += indexIncrement;
	}
}

void drawMarioSpecialMovesOptions()
{
	// Check for button inputs
	marioSpecialMovesButtonControls();
	
	// Draw the window
	uint32_t Color 		= 0x151515F6;
	int32_t PosX 		= -120;
	int32_t PosY 		= 169;
	int32_t Width 		= 243;
	int32_t Height 		= 332;
	int32_t Curve 		= 20;
	
	drawWindow(Color, PosX, PosY, Width, Height, Curve);
	
	// Draw the help text
	const char *HelpText = "Press A to turn on/off\nPress B to cancel";
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	Color = 0xFFFFFFFF;
	PosX += 30;
	PosY -= 25;
	
	#ifdef TTYD_JP
	PosX -= 1;
	#endif
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
	// Set the values for the text to use
	int32_t NewPosY = PosY - 54;
	PosX += 25;
	PosY = NewPosY;
	
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	IconPosition[IconPositionX] 	= PosX - 15;
	IconPosition[IconPositionY] 	= PosY - 18;
	
	// Draw the icons and main text
	uint32_t tempSecondaryMenuOption = MenuVar.SecondaryMenuOption;
	int16_t SpecialMoveIcon;
	
	for (uint32_t i = 0; i < 8; i++)
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
		
		// Draw the main text
		bool CurrentOptionCheck = tempSecondaryMenuOption == i;
		Color = getSelectedTextColor(CurrentOptionCheck);
		
		drawText(MarioStatsSpecialMovesOptions[i], PosX, PosY, Alpha, Color, Scale);
		
		IconPosition[IconPositionY] -= 30;
		PosY -= 30;
	}
	
	// Draw the on/off text
	uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
	int16_t SpecialMovesBits = *reinterpret_cast<int16_t *>(PouchPtr + 0x8C);
	PosX += 130;
	PosY = NewPosY;
	
	for (uint32_t i = 0; i < 8; i++)
	{
		// Check if the current special move is on or off
		bool SpecialMoveOnOrOff = SpecialMovesBits & (1 << i);
		const char *CurrentSpecialMove;
		
		getOnOffTextAndColor(SpecialMoveOnOrOff, &CurrentSpecialMove, &Color);
		
		drawText(CurrentSpecialMove, PosX, PosY, Alpha, Color, Scale);
		PosY -= 30;
	}
}

void drawFollowersOptions()
{
	// Check for button inputs
	followersOptionsButtonControls();
	
	// Draw the window
	uint32_t Color 		= 0x151515F6;
	int32_t PosX 		= -108;
	int32_t PosY 		= 110;
	int32_t Width 		= 213;
	int32_t Height 		= 198;
	int32_t Curve 		= 20;
	
	drawWindow(Color, PosX, PosY, Width, Height, Curve);
	
	// Draw the help text
	const char *HelpText = "Press A to confirm\nPress B to cancel";
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	Color = 0xFFFFFFFF;
	PosX += 30;
	PosY -= 25;
	
	#ifdef TTYD_JP
	PosX -= 1;
	#endif
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
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
		
		drawText(StatsFollowerOptionsLines[i], PosX, PosY, Alpha, Color, Scale);
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
	
	uint32_t Color 					= 0xFFFFFFFF;
	uint8_t Alpha 					= 0xFF;
	float TextScale 				= 0.6;
	float IconScale					= 0.6;
	int32_t TextPosX 				= -208;
	int32_t TextPosY 				= 175;
	int32_t ValuesPosX 				= TextPosX + 105;
	int32_t IconPosX 				= TextPosX - 15;
	int32_t IconPosY 				= TextPosY - 20;
	
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	IconPosition[IconPositionX] 	= IconPosX;
	IconPosition[IconPositionY] 	= IconPosY;
	
	uint32_t tempStatsMarioOptionsLinesSize = StatsMarioOptionsLinesSize;
	uint32_t TotalRowsPerPage 		= 1 + ((tempStatsMarioOptionsLinesSize - 1) / 2); // Round up
	uint32_t TotalColumns 			= 1 + ((tempStatsMarioOptionsLinesSize - 1) / TotalRowsPerPage); // Round up
	uint32_t ColumnSplitAmount 		= 1 + ((tempStatsMarioOptionsLinesSize - 1) / TotalColumns); // Round up
	
	int32_t PosX = TextPosX;
	int32_t PosY = TextPosY;
	
	// Draw the main text and icons
	char *tempDisplayBuffer = DisplayBuffer;
	int32_t IconPosYIncrement;
	uint32_t SelectionTextColor;
	Counter = 0;
	
	const uint32_t BeforeStarPowerSlot = 11;
	
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
			
			// Draw the icon
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
			
			// Draw the text
			bool CurrentOptionCheck = (MenuVar.CurrentMenuOption == i);
			SelectionTextColor = getSelectedTextColor(CurrentOptionCheck);
			
			drawText(StatsMarioOptionsLines[i], PosX, PosY, Alpha, SelectionTextColor, TextScale);
			
			// Draw the values
			if (i != BeforeStarPowerSlot) // Special moves -- drawn separately
			{
				sprintf(tempDisplayBuffer,
					"%" PRId32,
					MarioStatsArray[Counter]);
				
				drawText(tempDisplayBuffer, ValuesPosX, PosY, Alpha, Color, TextScale);
				Counter++;
			}
			
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
	float SpecialMovesScale = 0.37;
	int16_t SpecialMoveIcon;
	IconPosition[IconPositionX] = 133;
	IconPosition[IconPositionY] = 101;
	
	int16_t SpecialMovesBits = *reinterpret_cast<int16_t *>(PouchPtr + 0x8C);
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
	
	uint8_t Alpha = 0xFF;
	int32_t PosX = -120;
	int32_t PosY = 180;
	float Scale = 0.6;
	uint32_t Color;
	
	// Draw the main text
	uint32_t tempStatsPartnerOptionsLinesSize = StatsPartnerOptionsLinesSize;
	for (uint32_t i = 0; i < tempStatsPartnerOptionsLinesSize; i++)
	{
		bool CurrentOptionCheck = (tempCurrentMenuOption == i) && (tempSelectedOption > 0);
		Color = getSelectedTextColor(CurrentOptionCheck);
		
		drawText(StatsPartnerOptionsLines[i], PosX, PosY, Alpha, Color, Scale);
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
		drawText(String, PosX, PosY, Alpha, Color, Scale);
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
	drawText(OptionToDrawText, PosX, PosY, Alpha, Color, Scale);
	
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
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
	}
	
	// Draw the on or off text
	bool PartnerEnabled = *reinterpret_cast<bool *>(PartnerEnabledAddress + 0x1);
	const char *String;
	
	getOnOffTextAndColor(PartnerEnabled, &String, &Color);
	drawText(String, PosX, PosY, Alpha, Color, Scale);
	
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
		
		const uint32_t ColorIdGreen 	= 0;
		const uint32_t ColorIdRed 		= 1;
		const uint32_t ColorIdBlue 		= 2;
		const uint32_t ColorIdOrange 	= 3;
		const uint32_t ColorIdPink 		= 4;
		const uint32_t ColorIdBlack 	= 5;
		// const uint32_t ColorIdWhite 	= 6;
		
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
		drawText(YoshiColorString, PosX, PosY, Alpha, YoshiColorText, Scale);
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
		drawText(String, PosX, PosY, Alpha, Color, Scale);
	}
}

void drawPartnerChangeYoshiColorOptions()
{
	// Check for button inputs
	partnerChangeYoshiColorButtonControls();
	
	// Draw the window
	uint32_t Color 		= 0x151515F6;
	int32_t PosX 		= -106;
	int32_t PosY 		= 123;
	int32_t Width 		= 212;
	int32_t Height 		= 239;
	int32_t Curve 		= 20;
	
	drawWindow(Color, PosX, PosY, Width, Height, Curve);
	
	// Draw the help text
	const char *HelpText = "Press A to confirm\nPress B to cancel";
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	Color = 0xFFFFFFFF;
	PosX += 30;
	PosY -= 25;
	
	#ifdef TTYD_JP
	PosX -= 1;
	#endif
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
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
		
		drawText(StatsYoshiColorOptionsLines[i], PosX, PosY, Alpha, Color, Scale);
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
	uint8_t Alpha = 0xFF;
	int32_t NamePosX = -232;
	int32_t NamePosY = 20;
	float Scale = 0.6;
	
	uint32_t CurrentActor = *reinterpret_cast<uint32_t *>(ActorAddress + 0x8);
	char *tempDisplayBuffer = DisplayBuffer;
	
	sprintf(tempDisplayBuffer,
		"Selected Actor\n%s",
		BattlesActorsLines[CurrentActor - 1]);
	
	drawText(tempDisplayBuffer, NamePosX, NamePosY, Alpha, Color, Scale);
	
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
		drawText(BattlesCurrentActorStats[i], PosX, PosY, Alpha, Color, Scale);
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
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
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
	const uint32_t MarioId 		= 222;
	const uint32_t GoombellaId 	= 224;
	const uint32_t MowzId 		= 230;
	
	if ((CurrentActorId == MarioId) || 
		((CurrentActorId >= GoombellaId) && 
			(CurrentActorId <= MowzId)))
	{
		return;
	}*/
	
	uint8_t Alpha 	= 0xFF;
	uint32_t Color 	= 0xFFFFFFFF;
	int32_t PosX 	= -50;
	int32_t PosY 	= 80;
	float Scale 	= 0.6;
	
	// Draw the text for showing what the current item is
	const char *CurrentLine = "Current Held Item";
	drawText(CurrentLine, PosX, PosY, Alpha, Color, Scale);
	
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
		const int32_t IconPositionX 	= 0;
		const int32_t IconPositionY 	= 1;
		IconPosition[IconPositionX] 	= PosX;
		IconPosition[IconPositionY] 	= PosY;
		IconPosition[2] 				= 0;
		
		drawIconFromItem(IconPosition, tempitem, Scale);
		
		// Draw the text for the item
		PosX += 17;
		PosY += 18;
		const char *ItemName = getItemName(tempitem);
		
		drawText(ItemName, PosX, PosY, Alpha, Color, Scale);
	}
	else
	{
		PosY -= 20;
		
		drawText("None", PosX, PosY, Alpha, Color, Scale);
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
	
	uint8_t Alpha = 0xFF;
	uint32_t Color = 0xFFFFFFFF;
	int32_t PosX = -232;
	int32_t PosY = 120;
	float Scale = 0.6;
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
}

void drawMemoryWatches()
{
	// Make sure there's at least one watch to draw
	if (!MemoryWatch[0].Address)
	{
		return;
	}
	
	uint32_t tempCurrentPage 		= MenuVar.CurrentPage;
	int32_t TotalMenuOptions 		= sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
	int32_t MaxOptionsPerPage 		= 10;
	int32_t IndexStart 				= tempCurrentPage * MaxOptionsPerPage;
	
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = 80;
	float Scale = 0.6;
	
	const int32_t TypeOffset 		= 150;
	const int32_t ValueOffset 		= TypeOffset + 90;
	
	// Draw the current page
	drawPageNumber(PosX, PosY, tempCurrentPage);
	PosY -= 40;
	
	// Draw the headers for the values
	drawText("Address", PosX, PosY, Alpha, Color, Scale);
	drawText("Display", PosX + TypeOffset, PosY, Alpha, Color, Scale);
	drawText("Value", PosX + ValueOffset, PosY, Alpha, Color, Scale);
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
		drawText(OnOffText, PosX + TypeOffset, PosY, Alpha, Color, Scale);
		
		// Set the color of the address text
		bool CurrentOptionCheck = (MenuVar.CurrentMenuOption == i) && (MenuVar.SelectedOption > 0);
		Color = getSelectedTextColor(CurrentOptionCheck);
		
		// Draw the current address
		drawText(getAddressString(i), PosX, PosY, Alpha, Color, Scale);
		
		// Draw the value
		Color = 0xFFFFFFFF;
		drawText(getValueString(i), PosX + ValueOffset, PosY, Alpha, Color, Scale);
		
		PosY -= 20;
	}
}

void drawMemoryModifyList()
{
	uint32_t tempSelectedOption 		= MenuVar.SelectedOption;
	uint32_t tempCurrentMenuOption 		= MenuVar.CurrentMenuOption;
	uint32_t tempMenuSelectedOption 	= MenuVar.MenuSelectedOption;
	uint32_t TotalOptions 				= MemoryModifyLinesSize;
	
	uint8_t Alpha 	= 0xFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 180;
	float Scale 	= 0.6;
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
		drawText(MemoryModifyLines[i], PosX, PosY, Alpha, Color, Scale);
		
		// Reset the color back to white
		Color = 0xFFFFFFFF;
		
		// Draw the value for the text
		switch (i + 1)
		{
			case CHANGE_ADDRESS:
			{
				// Draw the current address
				drawText(getAddressString(tempMenuSelectedOption), PosX + PosX_Offset, PosY, Alpha, Color, Scale);
				break;
			}
			case CHANGE_TYPE:
			{
				// Draw the current type
				drawText(MemoryTypeLines[MemoryWatch[tempMenuSelectedOption].Type], 
					PosX + PosX_Offset, PosY, Alpha, Color, Scale);
				break;
			}
			case SHOW_AS_HEX:
			{
				// Draw the Yes/No text for whether the watch is shown as hex or not
				const char *YesNoText;
				getYesNoTextAndColor(MemoryWatch[tempMenuSelectedOption].ShowAsHex, &YesNoText, &Color);
				drawText(YesNoText, PosX + PosX_Offset, PosY, Alpha, Color, Scale);
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
				
				drawText(tempDisplayBuffer, PosX + PosX_Offset, PosY, Alpha, Color, Scale);
				break;
			}
			case DISPLAY_OUT_OF_MENU:
			{
				// Draw the On/Off text for whether the watch is displayed or not
				const char *OnOffText;
				getOnOffTextAndColor(MemoryWatch[tempMenuSelectedOption].Display, &OnOffText, &Color);
				drawText(OnOffText, PosX + PosX_Offset, PosY, Alpha, Color, Scale);
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
	
	drawText("Value", PosX, PosY, Alpha, Color, Scale);
	drawText(getValueString(tempMenuSelectedOption), PosX + PosX_Offset, PosY, Alpha, Color, Scale);
}

void drawMemoryTypeList()
{
	// Check for button inputs
	memoryAddressTypeButtonControls();
	
	// Draw the window
	uint32_t Color 		= 0x151515F6;
	int32_t PosX 		= -110;
	int32_t PosY 		= 169;
	int32_t Width 		= 212;
	int32_t Height 		= 332;
	int32_t Curve 		= 20;
	
	drawWindow(Color, PosX, PosY, Width, Height, Curve);
	
	// Draw the help text
	const char *HelpText = "Press A to confirm\nPress B to cancel";
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	Color = 0xFFFFFFFF;
	PosX += 30;
	PosY -= 25;
	
	#ifdef TTYD_JP
	PosX -= 1;
	#endif
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
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
		
		drawText(MemoryTypeLines[i], PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
	}
}

void drawMemoryChangeWatchPosition()
{
	// Check for button inputs
	memoryChangeWatchPositionButtonControls();
	
	uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
	
	// Draw the current watch
	int32_t PosX 		= MemoryWatchPosition.PosX;
	int32_t PosY 		= MemoryWatchPosition.PosY;
	uint32_t TextColor 	= 0xFFFFFFFF;
	uint8_t Alpha 		= 0xFF;
	float Scale 		= 0.75;
	
	drawText(getValueString(tempMenuSelectedOption), PosX, PosY, Alpha, TextColor, Scale);
	
	// Don't draw the window and text if Y is being held
	if (checkButtonComboEveryFrame(PAD_Y))
	{
		return;
	}
	
	// Draw the help window
	int32_t Curve 			= 20;
	uint32_t WindowColor 	= 0x151515F6;
	int32_t TextPosX 		= -135;
	int32_t TextPosY 		= 140;
	Scale = 0.6;
	
	const char *HelpText = "Press/Hold the D-Pad directions\nto move the watch\n\nHold Y to hide this window\n\nPress A to confirm\n\nPress B to cancel";
	int32_t Width = static_cast<int32_t>(getMessageWidth(HelpText, Scale));
	
	drawTextWithWindow(HelpText, TextPosX, TextPosY, Alpha, TextColor, 
		Scale, Width, WindowColor, Curve);
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
	uint32_t tempMenuSelectedOption 	= MenuVar.MenuSelectedOption;
	uint32_t tempCurrentMenuOption 		= MenuVar.CurrentMenuOption;
	uint32_t tempSelectedOption 		= MenuVar.SelectedOption;
	
	uint8_t Alpha 	= 0xFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 100;
	float Scale 	= 0.6;
	uint32_t Color;
	
	const int32_t PosX_Offset_Position 		= 110;
	const int32_t PosX_Address_Position 	= 250;
	
	// Get the color for the address text
	bool CurrentOptionCheck = (tempSelectedOption != 0) && (tempCurrentMenuOption == 0);
	Color = getSelectedTextColor(CurrentOptionCheck);
	
	if (CurrentOptionCheck)
	{
		// Draw a window for the current line
		drawMemoryWatchChangeAddressListWindow(PosY);
	}
	
	// Draw the address
	drawText("Address", PosX, PosY, Alpha, Color, Scale);
	
	Color = 0xFFFFFFFF;
	drawText(getAddressStringNoLetterP(tempMenuSelectedOption), 
		PosX + PosX_Address_Position, PosY, Alpha, Color, Scale);
	
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
		}
		
		// Draw the text for the current level
		sprintf(tempDisplayBuffer,
			"Level %" PRIu32,
			i);
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		
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
		
		drawText(tempDisplayBuffer, PosX + PosX_Offset_Position, PosY, Alpha, Color, Scale);
		
		// Draw the address pointed to by the current level
		drawText(getAddressStringFromOffsets(tempMenuSelectedOption, i), 
			PosX + PosX_Address_Position, PosY, Alpha, Color, Scale);
		
		PosY -= 20;
	}
	
	// Draw the final value
	Color = 0xFFFFFFFF;
	PosY -= 20;
	
	drawText("Final Value", PosX, PosY, Alpha, Color, Scale);
	drawText(getValueString(tempMenuSelectedOption), PosX + PosX_Offset_Position, PosY, Alpha, Color, Scale);
}

void drawBattlesActorsList()
{
	uint32_t tempCurrentPage 		= MenuVar.CurrentPage;
	uint32_t TotalMenuOptions 		= 62; // Excluding System
	uint32_t MaxOptionsPerPage 		= 18;
	uint32_t IndexStart 			= tempCurrentPage * MaxOptionsPerPage;
	
	uint8_t Alpha 	= 0xFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 180;
	float Scale 	= 0.6;
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
		
		drawText(CurrentActorString, PosX, PosY, Alpha, Color, Scale);
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
	
	uint32_t MaxOptionsPerPage 		= 12;
	uint32_t IndexStart 			= MenuVar.CurrentPage * MaxOptionsPerPage;
	uint32_t TotalOptions 			= BattlesStatusesLinesSize;
	
	uint8_t Alpha 					= 0xFF;
	float TextScale 				= 0.6;
	float IconScale					= 0.6;
	int32_t TextPosX 				= -210;
	int32_t TextPosY 				= 175;
	int32_t IconPosX 				= TextPosX - 15;
	int32_t IconPosY 				= TextPosY - 20;
	
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	IconPosition[IconPositionX] 	= IconPosX;
	IconPosition[IconPositionY] 	= IconPosY;
	
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
	char *tempDisplayBuffer = DisplayBuffer;
	uint32_t Color;
	
	uint32_t DrawWindowCounter = 0;
	for (uint32_t i = IndexStart; i < (IndexStart + MaxOptionsPerPage); i++)
	{
		if (i >= TotalOptions)
		{
			// Reached the end of the array, so exit
			return;
		}
		
		bool CurrentOptionCheck = MenuVar.CurrentMenuOption == i;
		Color = getSelectedTextColor(CurrentOptionCheck);
		
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
		
		// Draw the main text
		drawText(BattlesStatusesLines[i], TextPosX, TextPosY, Alpha, Color, TextScale);
		
		// Draw the icon
		drawIcon(IconPosition, BattlesStatusesIcons[i], IconScale);
		
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
		
		drawText(TextToDraw, TextPosX + 290, TextPosY, Alpha, Color, TextScale);
		
		IconPosition[IconPositionY] -= 30;
		TextPosY -= 30;
		DrawWindowCounter++;
	}
}

void drawErrorWindow(const char *message, int32_t textPosX, int32_t textPosY, int32_t windowWidth)
{
	// int32_t TextPosX 	= -200;
	// int32_t TextPosY 	= 60;
	uint8_t Alpha 			= 0xFF;
	uint32_t TextColor 		= 0xFFFFFFFF;
	float Scale 			= 0.6;
	// int32_t WindowWidth 	= 415;
	uint32_t WindowColor 	= 0x151515F4;
	int32_t WindowCurve 	= 20;
	
	drawTextWithWindow(message, textPosX, textPosY, Alpha, TextColor, 
		Scale, windowWidth, WindowColor, WindowCurve);
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
		
		// int32_t TextPosX 	= -130;
		// int32_t TextPosY 	= 60;
		float Scale 			= 0.6;
		int32_t WindowWidth 	= static_cast<int32_t>(getMessageWidth(message, Scale));
		
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
	int32_t TextPosX 		= -205;
	int32_t TextPosY 		= 0;
	int32_t WindowWidth 	= 440;
	
	#ifdef TTYD_JP
	TextPosX += 6;
	WindowWidth -= 13;
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
	int32_t TextPosX 		= -195;
	int32_t WindowWidth 	= 415;
	
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
	uint32_t Color 		= 0x151515F4;
	int32_t PosX 		= -245;
	int32_t PosY 		= 110;
	int32_t Width 		= 490;
	int32_t Height 		= 220;
	int32_t Curve 		= 0;
	
	drawWindow(Color, PosX, PosY, Width, Height, Curve);
	
	// Draw the confirmation message
	Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	PosX += 70;
	PosY -= 40;
	
	#ifdef TTYD_JP
	PosX += 7;
	#endif
	
	drawText(message, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the yes/no options
	PosX += 155;
	PosY -= 110;
	Scale = 0.8;
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
		
		drawText(Option, PosX, PosY, Alpha, Color, Scale);
		PosY -= 23;
	}
}

void drawSingleLineFromArray(int32_t posX, int32_t posY, 
	int32_t currentMenuOption, const char **line)
{
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	const char *CurrentLine = line[currentMenuOption];
	
	drawText(CurrentLine, posX, posY, Alpha, Color, Scale);
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
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	
	drawText(tempDisplayBuffer, posX, posY, Alpha, Color, Scale);
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
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	
	drawText(tempDisplayBuffer, posX, posY, Alpha, Color, Scale);
}

void drawAdjustableValueSpawnItem()
{
	drawAdjustableValue(true, SPAWN_ITEM_MENU_VALUE);
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
	uint32_t color 	= 0x151515E0;
	int32_t PosX 	= -189;
	int32_t y 		= 105;
	int32_t width 	= 377;
	int32_t curve 	= 10;
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
	uint8_t alpha 		= 0xFF;
	float scale 		= 0.6;
	int32_t x 			= PosX + 15;
	y 					-= 13;
	color 				= 0xFFFFFFFF;
	const char *HelpText = "Press D-Pad Up/Down to adjust the value\nPress D-Pad Left/Right to change digits\nPress Y to set the value to max\nPress Z to set the value to min\nPress A to confirm\nPress B to cancel";
	drawText(HelpText, x, y, alpha, color, scale);
	
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
		const int32_t IconPositionX 	= 0;
		const int32_t IconPositionY 	= 1;
		IconPosition[IconPositionX] 	= IconPosX;
		IconPosition[IconPositionY] 	= y;
		IconPosition[2] 				= 0;
		
		float IconScale = 0.6;
		
		if ((tempMenuSecondaryValue >= ttyd::item_data::Item::PaperModeCurse) && 
			(tempMenuSecondaryValue <= ttyd::item_data::Item::UltraHammer))
		{
			// Decrease the size of these icons
			IconScale = 0.5;
		}
		else if (tempMenuSecondaryValue == ttyd::item_data::Item::MagicalMapBigger)
		{
			// Decrease the size of the bigger magical map
			IconScale = 0.4;
		}
		
		int16_t CurrentItem = static_cast<int16_t>(tempMenuSecondaryValue);
		drawIconFromItem(IconPosition, CurrentItem, IconScale);
		
		// Draw the text for the item
		color = 0xFFFFFFFF;
		IconPosX += 17;
		y += 18;
		const char *ItemName = getItemName(CurrentItem);
		
		drawText(ItemName, IconPosX, y, alpha, color, scale);
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
		drawText(tempDisplayBuffer, x + 80, NamesPosY, alpha, color, scale);
		
		const char *String = "Stage\nEvent";
		drawText(String, x, NamesPosY, alpha, color, scale);
		
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
		
		drawText(tempDisplayBuffer, x, y - 35, alpha, color, scale);
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
	scale = 1;
	x -= 1;
	y += 3;
	#else
	scale = 0.9;
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
		
		drawText("-", (x - NegativeSignOffset), y, alpha, color, scale);
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

		drawText(tempDisplayBuffer, x, y, alpha, color, scale);
		x += 17;
	}
}

void drawMemoryWatchAdjustableValue(uint32_t currentMenu)
{
	uint32_t tempMenuSelectedOption = MenuVar.MenuSelectedOption;
	
	// Check for button inputs
	uint32_t ReturnCode = adjustWatchValueControls(tempMenuSelectedOption);
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
	uint32_t color 	= 0x151515E0;
	int32_t PosX 	= -189;
	int32_t y 		= 105;
	int32_t width 	= 377;
	int32_t curve 	= 10;
	int32_t height 	= 148;
	
	#ifdef TTYD_JP
	PosX += 4;
	width -= 6;
	#endif
	
	drawWindow(color, PosX, y, width, height, curve);
	
	// Draw the help text
	uint8_t alpha 		= 0xFF;
	float scale 		= 0.6;
	int32_t x 			= PosX + 15;
	y 					-= 13;
	color 				= 0xFFFFFFFF;
	const char *HelpText = "Press D-Pad Up/Down to adjust the value\nPress D-Pad Left/Right to change digits\nPress A to confirm\nPress B to cancel";
	drawText(HelpText, x, y, alpha, color, scale);
	
	uint32_t tempMemoryWatchSecondaryValue = MenuVar.MemoryWatchSecondaryValue;
	int32_t tempMemoryWatchSecondaryValueSigned;
	y -= 100;
	
	// Check if the number is negative and get the amount of numbers to draw
	bool NumberIsNegative = false;
	uint32_t AmountOfNumbers;
	
	switch (currentMenu)
	{
		case MEMORY_CHANGE_ADDRESS:
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
					tempMemoryWatchSecondaryValueSigned = static_cast<int32_t>(tempMemoryWatchSecondaryValue);
					if (tempMemoryWatchSecondaryValueSigned < 0)
					{
						NumberIsNegative = true;
					}
				}
			}
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
	scale = 1;
	x -= 1;
	y += 2;
	#else
	scale = 0.9;
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
		tempMemoryWatchSecondaryValueSigned = -tempMemoryWatchSecondaryValueSigned;
		
		// Get the values for the array
		for (uint32_t i = 0; i < AmountOfNumbers; i++)
		{
			AdjustableValue[AmountOfNumbers - i - 1] = tempMemoryWatchSecondaryValueSigned % 0x10;
			tempMemoryWatchSecondaryValueSigned /= 0x10;
		}
		
		// Draw the negative sign
		int32_t NegativeSignOffset = 40;
		
		#ifdef TTYD_JP
		NegativeSignOffset += 2;
		#endif
		
		drawText("-", (x - NegativeSignOffset), y, alpha, color, scale);
	}
	else
	{
		// Get the values for the array
		for (uint32_t i = 0; i < AmountOfNumbers; i++)
		{
			AdjustableValue[AmountOfNumbers - i - 1] = tempMemoryWatchSecondaryValue % 0x10;
			tempMemoryWatchSecondaryValue /= 0x10;
		}
	}
	
	// Draw the 0x
	drawText("0x", (x - 30), y, alpha, color, scale);
	
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

		drawText(tempDisplayBuffer, x, y, alpha, color, scale);
		x += 17;
	}
}

void drawAddByIconMain(uint32_t currentMenu)
{
	// Check for button inputs
	addByIconButtonControls(currentMenu);
	
	// Draw the main window
	int32_t UpperAndLowerBounds[2];
	getUpperAndLowerBounds(UpperAndLowerBounds, MenuVar.MenuSelectedOption);
	
	int32_t LowerBound 			= UpperAndLowerBounds[0];
	int32_t UpperBound 			= UpperAndLowerBounds[1];
	
	uint32_t MaxIconsPerRow 	= 16;
	uint32_t IconSpaceOccupied 	= 30;
	
	uint32_t Color 				= 0x151515E0;
	// uint32_t Color 			= 0x000000E0;
	int32_t PosX 				= -245;
	int32_t PosY 				= 160;
	int32_t Width 				= 490;
	int32_t TotalIcons 			= UpperBound - LowerBound + 1;
	int32_t TotalLines 			= 1 + ((TotalIcons - 1) / MaxIconsPerRow); // Round up;
	int32_t Height 				= (TotalLines * 34) + 40;
	int32_t Curve 				= 0;
	
	drawWindow(Color, PosX, PosY, Width, Height, Curve);
	
	// Draw the help text
	uint8_t Alpha 			= 0xFF;
	float Scale 			= 0.6;
	PosX 					= -75;
	PosY 					= 145;
	Color 					= 0xFFFFFFFF;
	const char *HelpText 	= "Press A to confirm\nPress B to cancel";
	
	#ifdef TTYD_JP
	PosX -= 2;
	#endif
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the window for the current menu option
	uint32_t tempCurrentMenuOption = MenuVar.SecondaryMenuOption;
	Color 	= 0x5B59DEC0;
	PosY 	= 96 - ((tempCurrentMenuOption / MaxIconsPerRow) * IconSpaceOccupied);
	Width 	= 29;
	Height 	= 29;
	
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
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	
	int32_t tempIconPositionX = -225;
	int32_t tempIconPositionY = 70;
	IconPosition[IconPositionY] = tempIconPositionY;
	
	// Draw all of the badges for the current menu
	uint32_t Counter = 0;
	for (int32_t i = LowerBound; i <= UpperBound; i++)
	{
		if ((i >= ttyd::item_data::Item::PaperModeCurse) && 
			(i <= ttyd::item_data::Item::UltraHammer))
		{
			// Decrease the size of these icons
			Scale = 0.5;
		}
		else if (i == ttyd::item_data::Item::MagicalMapBigger)
		{
			// Decrease the size of the bigger magical map
			Scale = 0.4;
		}
		else
		{
			Scale = 0.6;
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
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	
	drawText(VersionNumber, posX, posY, Alpha, Color, Scale);
}

void drawPageNumber(int32_t posX, int32_t posY, uint32_t currentPage)
{
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha 	= 0xFF;
	float Scale = 0.6;
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"Page %" PRIu32,
		currentPage + 1);
	
	drawText(tempDisplayBuffer, posX, posY, Alpha, Color, Scale);
}

void drawBoolOnOrOff(bool tempBool, const char *currentLine, int32_t posY)
{
	uint8_t Alpha 	= 0xFF;
	uint32_t Color 	= 0xFFFFFFFF;
	int32_t PosX 	= -232;
	float Scale 	= 0.6;
	
	// Draw the current option
	drawText(currentLine, PosX, posY, Alpha, Color, Scale);
	
	// Draw the bool value
	const char *StringValue;
	getOnOffTextAndColor(tempBool, &StringValue, &Color);
	
	posY -= 20;
	drawText(StringValue, PosX, posY, Alpha, Color, Scale);
}

void drawCheatsChangeSequence()
{
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = 140;
	float Scale = 0.6;
	
	uint32_t SequencePosition = getSequencePosition();
	
	// Draw the text for showing what the current Sequence value is
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"Current Value: %" PRIu32,
		SequencePosition);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
	
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
	drawText(tempDisplayBuffer, PosX + 80, PosY, Alpha, Color, Scale);
	
	const char *String = "Stage\nEvent";
	drawText(String, PosX, PosY, Alpha, Color, Scale);
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
	uint8_t Alpha 	= 0xFF;
	uint32_t Color 	= 0xFFFFFFFF;
	int32_t PosX 	= -232;
	float Scale 	= 0.6;
	
	// Draw the text for telling what the button combo is
	Color = 0xFFFFFFFF;
	drawText(description, PosX, posY, Alpha, Color, Scale);
	
	// Draw the button combo
	uint8_t ButtonArray[14]; // Extra spot for 0 at the end of the array
	clearMemory(ButtonArray, sizeof(ButtonArray));
	getButtonsPressed(ButtonArray, buttonCombo);
	
	// uint8_t *ButtonArray = nullptr;
	// getButtonsPressed(ButtonArray);
	
	char ButtonString[128];
	createButtonStringArray(ButtonArray, ButtonString, sizeof(ButtonString));
	
	posY -= 20;
	drawText(ButtonString, PosX, posY, Alpha, Color, Scale);
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
		MenuVar.Timer 						= 0;
		MenuVar.SelectedOption 				= 0;
		MenuVar.ChangingCheatButtonCombo 	= false;
		clearMemory(CheatsDisplayButtons.CheatsPreviousButtonsHeld, (14 * sizeof(uint8_t)));
		clearMemory(CheatsDisplayButtons.CheatsCurrentButtonsHeld, (14 * sizeof(uint8_t)));
		return;
	}
	
	// Draw the window
	uint32_t Color 			= 0x151515F0;
	int32_t PosX 			= -245;
	int32_t PosY 			= 130;
	int32_t Width 			= 490;
	int32_t Height 			= 255;
	int32_t Curve 			= 0;
	drawWindow(Color, PosX, PosY, Width, Height, Curve);
	
	// Draw the help text
	uint8_t Alpha 			= 0xFF;
	float Scale 			= 0.6;
	PosX 					+= 65;
	PosY 					-= 30;
	Color 					= 0xFFFFFFFF;
	const char *HelpText 	= "Hold the button(s) for 3 seconds to set the\nnew button combo\n\nPress B three times in succession to cancel";
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
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
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the text for telling what buttons are currently held
	Color = 0xFFFFFFFF;
	const char *CurrentLine = "Currently held buttons";
	PosY -= 40;
	drawText(CurrentLine, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the currently held buttons
	uint8_t *tempButtonArray = CheatsDisplayButtons.CheatsCurrentButtonsHeld;
	
	// Clear the memory, so that the previous results do not interfere with the new results
	clearMemory(tempButtonArray, (14 * sizeof(uint8_t)));
	getButtonsPressed(tempButtonArray, ButtonInput);
	
	char ButtonString[128];
	createButtonStringArray(tempButtonArray, ButtonString, sizeof(ButtonString));
	
	PosY -= 20;
	drawText(ButtonString, PosX, PosY, Alpha, Color, Scale);
	
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
	MenuVar.SelectedOption 				= 0;
	MenuVar.ChangingCheatButtonCombo 	= false;
	clearMemory(CheatsDisplayButtons.CheatsPreviousButtonsHeld, (14 * sizeof(uint8_t)));
	clearMemory(CheatsDisplayButtons.CheatsCurrentButtonsHeld, (14 * sizeof(uint8_t)));
}

void drawCheatsForcedDropItem()
{
	uint8_t Alpha 	= 0xFF;
	uint32_t Color 	= 0xFFFFFFFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 60;
	float Scale 	= 0.6;
	
	// Draw the text for showing what the current item is
	const char *CurrentLine = "Current Item";
	drawText(CurrentLine, PosX, PosY, Alpha, Color, Scale);
	
	PosX += 10;
	PosY -= 45;
	
	// Draw the current item icon
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[IconPositionX] 	= PosX;
	IconPosition[IconPositionY] 	= PosY;
	IconPosition[2] 				= 0;
	
	int16_t tempForcedNPCItemDrop = MenuVar.ForcedNPCItemDrop;
	drawIconFromItem(IconPosition, tempForcedNPCItemDrop, Scale);
	
	// Draw the text for the item
	PosX += 17;
	PosY += 18;
	const char *ItemName = getItemName(tempForcedNPCItemDrop);
	
	drawText(ItemName, PosX, PosY, Alpha, Color, Scale);
}

void drawCheatsResolveFades()
{
	// Draw the text explaining how to clear a fade
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = 180;
	float Scale = 0.6;
	
	const int32_t PosYStart = PosY;
	
	const char *ExplainText = "Select a fade to resolve, if applicable.";
	drawText(ExplainText, PosX, PosY, Alpha, Color, Scale);
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
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
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
		bool FadeIsActive = FadeEntry->flags & (1 << 0); // Check if 0 bit is active
		
		if (FadeIsActive)
		{
			sprintf(tempDisplayBuffer,
				"%" PRId32,
				static_cast<int32_t>(FadeEntry->fadeType));
			
			Color = 0xFFFFFFFF;
			drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		}
		else
		{
			const char *String = "None";
			Color = 0x4B4B4BFF;
			drawText(String, PosX, PosY, Alpha, Color, Scale);
		}
		
		PosY -= 20;
	}
}

void drawCheatsLockFlags()
{
	// Draw the Yes/No text for each option
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = 180;
	float Scale = 0.6;
	uint32_t Color;
	
	const char *String;
	
	uint32_t Size = sizeof(LockFlags.Region) / sizeof(LockFlags.Region[0]);
	for (uint32_t i = 0; i < Size; i++)
	{
		getYesNoTextAndColor(LockFlags.Region[i].MemoryRegionLocked, &String, &Color);
		drawText(String, PosX + 120, PosY, Alpha, Color, Scale);
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
	drawText(String, PosX, PosY, Alpha, Color, Scale);
	PosY -= 20;
	
	// Get the text for which area has its LSW flags locked
	char CurrentAreaLockedText[64];
	Color = getAreaText(CurrentAreaLockedText, lockFlagsMenuGetAreaLockedString(LSW), LockFlags.Region[LSW].MemoryRegionLocked);
	
	// Draw the text for which area has its flags locked
	drawText(CurrentAreaLockedText, PosX, PosY, Alpha, Color, Scale);
	PosY -= 40;
	
	// Draw the text indicating which area has its LSWF flags locked
	Color = 0xFFFFFFFF;
	String = "Current Area LSWF Flags Locked";
	drawText(String, PosX, PosY, Alpha, Color, Scale);
	PosY -= 20;
	
	// Get the text for which area has its LSWF flags locked
	Color = getAreaText(CurrentAreaLockedText, lockFlagsMenuGetAreaLockedString(LSWF), LockFlags.Region[LSWF].MemoryRegionLocked);
	
	// Draw the text for which area has its flags locked
	drawText(CurrentAreaLockedText, PosX, PosY, Alpha, Color, Scale);
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
	
	uint8_t Alpha 	= 0xFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 180;
	float Scale 	= 0.6;
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
			
			drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		}
		else
		{
			drawText(Line[TextIndex], PosX, PosY, Alpha, Color, Scale);
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
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		
		// Draw the values
		sprintf(tempDisplayBuffer,
			"%" PRIu32 "\n%" PRId32 "\n%" PRId32,
			FlagToSet,
			FlagValue,
			ManageFlags.ValueToSet);
		
		drawText(tempDisplayBuffer, PosX + 150, PosY, Alpha, Color, Scale);
	}
	else
	{
		// Draw the text descriptions
		sprintf(tempDisplayBuffer,
			"%s To Set\nCurrent Value",
			FlagText);
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		
		// Draw the values
		// Draw the flag being changed
		sprintf(tempDisplayBuffer,
			"%" PRIu32,
			FlagToSet);
		
		drawText(tempDisplayBuffer, PosX + 150, PosY, Alpha, Color, Scale);
		
		// Draw the On/Off text
		const char *TextToDraw;
		getOnOffTextAndColor(FlagValue, &TextToDraw, &Color);
		drawText(TextToDraw, PosX + 150, PosY - 20, Alpha, Color, Scale);
	}
}

void drawCheatsClearArea()
{
	// Draw the main text
	uint32_t tempCheatsClearAreaFlagsAreasSize 	= CheatsClearAreaFlagsAreasSize;
	const char **tempCheatsClearAreaFlagsAreas 	= CheatsClearAreaFlagsAreas;
	
	int32_t PosX 								= -232;
	int32_t PosY 								= 120;
	uint32_t Size 								= tempCheatsClearAreaFlagsAreasSize;
	uint32_t MaxOptionsPerPage 					= tempCheatsClearAreaFlagsAreasSize;
	uint32_t MaxOptionsPerRow 					= 4;
	uint32_t PosXIncrementAmount 				= 50;
	uint32_t tempPage 							= 0;
	uint32_t tempSecondaryMenuOption 			= MenuVar.SecondaryMenuOption;
	
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

void drawWarpsOptions()
{
	// Draw the main text
	uint32_t tempWarpDestinationsSize 			= WarpDestinationsSize;
	const char **tempWarpDestinations 			= WarpDestinations;
	
	int32_t PosX 								= -232;
	int32_t PosY 								= 100;
	uint32_t Size 								= tempWarpDestinationsSize;
	uint32_t MaxOptionsPerPage 					= tempWarpDestinationsSize;
	uint32_t MaxOptionsPerRow 					= 4;
	uint32_t PosXIncrementAmount 				= 100;
	uint32_t tempPage 							= 0;
	uint32_t tempCurrentMenuOption 				= MenuVar.CurrentMenuOption;
	
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
		PosY -= 160;
		drawSingleLineFromArray(PosX, PosY, 
			tempCurrentMenuOption, WarpDescriptions);
	}
}

void drawEventDetails(int32_t posX, int32_t posY, int32_t index)
{
	// Draw the labels for the details for the current event
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	float Scale = 0.6;
	
	const char *Details = "Stage\nEvent\nSequence\nPartner\nFollower\nMap\nLZ";
	drawText(Details, posX, posY, Alpha, Color, Scale);
	
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
	
	drawText(tempDisplayBuffer, posX, posY, Alpha, Color, Scale);
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
	
	drawText(PartnerString, posX, posY, Alpha, Color, Scale);
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
	
	drawText(FollowerString, posX, posY, Alpha, Color, Scale);
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
	
	drawText(Map, posX, posY, Alpha, Color, Scale);
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
	
	drawText(Bero, posX, posY, Alpha, Color, Scale);
}

void drawWarpByEventMenuDetails()
{
	// Draw the text explaining that warping with this menu will clear all game states
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = 180;
	float Scale = 0.6;
	
	const char *ExplainText = "Note: Warping via this menu will clear all game states.";
	drawText(ExplainText, PosX, PosY, Alpha, Color, Scale);
	PosY -= 60;
	
	// Draw the text for whether the standard inventory should be kept or not
	const char *String;
	getYesNoTextAndColor(WarpByEvent.ShouldKeepInventory, &String, &Color);
	
	int32_t tempPosX = PosX + 147;
	
	#ifdef TTYD_JP
	tempPosX -= 4;
	#endif
	
	drawText(String, tempPosX, PosY, Alpha, Color, Scale);
	PosY -= 20;
	
	// Draw the text for whether specific flags should be set or not
	getYesNoTextAndColor(WarpByEvent.ShouldSetFlags, &String, &Color);
	drawText(String, tempPosX, PosY, Alpha, Color, Scale);
	PosY -= 60;
	
	// Draw the details for the current event
	drawEventDetails(PosX, PosY, WarpByEvent.CurrentIndex);
}

void drawWarpIndexMapAndEntrance()
{
	uint32_t Color 	= 0xFFFFFFFF;
	uint8_t Alpha 	= 0xFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 80;
	float Scale 	= 0.6;
	
	const char *String = "Current Map\nCurrent Entrance\n\nNew Map\nNew Entrance Id";
	drawText(String, PosX, PosY, Alpha, Color, Scale);
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
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
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
	
	drawText(NextMap, PosX, PosY, Alpha, Color, Scale);
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
	
	drawText(NextBero, PosX, PosY, Alpha, Color, Scale);
}

void drawWarpIndexEntranceList()
{
	// Draw the window for the text
	uint32_t WindowColor 	= 0x000000E5;
	int32_t PosX 			= -245;
	int32_t PosY 			= 190;
	int32_t Width 			= 490;
	int32_t Height 			= 375;
	int32_t Curve 			= 0;
	
	drawWindow(WindowColor, PosX, PosY, Width, Height, Curve);
	
	// Draw the help text
	uint32_t Color 	= 0xFFFFFFFF;
	uint8_t Alpha 	= 0xFF;
	float Scale 	= 0.6;
	PosY -= 20;
	
	const char *HelpText = "Press B to close this window\nPress D-Pad Up/Down to adjust the page";
	drawText(HelpText, PosX + 72, PosY, Alpha, Color, Scale);
	PosX = -232;
	PosY -= 55;
	
	const char *String = "Current Map\nCurrent Entrance";
	drawText(String, PosX, PosY, Alpha, Color, Scale);
	
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
	
	drawText(NextMap, PosX + 170, PosY, Alpha, Color, Scale);
	
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
	
	drawText(NextBero, PosX + 170, PosY - 20, Alpha, Color, Scale);
	
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
		drawText(String, PosX, PosY, Alpha, Color, Scale);
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
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		
		// Draw the entrance name
		drawText(tempEntranceArray[i], PosX + 130, PosY, Alpha, Color, Scale);
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

void drawSequenceInPauseMenu()
{
	uint32_t Color 				= 0xFFFFFFFF;
	uint8_t Alpha 				= 0xFF;
	int32_t PosX 				= 43;
	int32_t PosY 				= 47;
	float Scale 				= 0.8;
	
	// Draw the sequence text
	const char *String = "Sequence";
	drawText(String, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the value
	uint32_t SequencePosition 	= getSequencePosition();
	char *tempDisplayBuffer 	= DisplayBuffer;
	
	sprintf(tempDisplayBuffer,
		"%" PRIu32,
		SequencePosition);
	
	// Adjust the position of the value based on the Sequence Position
	#ifdef TTYD_JP
	int32_t PosXIncrement = 186;
	#else
	int32_t PosXIncrement = 182;
	#endif
	
	if (SequencePosition >= 100)
	{
		PosXIncrement -= 26;
	}
	else if (SequencePosition >= 10)
	{
		PosXIncrement -= 13;
	}
	
	#ifdef TTYD_JP
	Scale += 0.05;
	#endif
	
	drawText(tempDisplayBuffer, PosX + PosXIncrement, PosY, Alpha, Color, Scale);
}

void drawOnScreenTimer()
{
	uint32_t TextColor 	= 0xFFFFFFFF;
	uint8_t Alpha 		= 0xFF;
	int32_t PosX 		= 105;
	int32_t PosY 		= -160;
	float Scale 		= 0.8;
	
	#ifdef TTYD_JP
	PosX 	+= 5;
	PosY 	+= 2;
	Scale 	+= 0.05;
	#endif
	
	// Move the timer up if Mario's Coordinates are currently displayed
	// Don't move up if the input display is active
	if (!Displays[BUTTON_INPUT_DISPLAY])
	{
		if (Displays[MARIO_COORDINATES] || 
			Displays[PALACE_SKIP] || 
			Displays[YOSHI_SKIP])
		{
			PosY += 20;
		}
	}
	
	char *CurrentTimeString = getTimeString(DisplayBuffer, OnScreenTimer.MainTimer);
	drawText(CurrentTimeString, PosX, PosY, Alpha, TextColor, Scale);
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
	
	uint32_t TextColor 	= 0xFFFFFFFF;
	uint8_t Alpha 		= 0xFF;
	int32_t PosX 		= 105;
	int32_t PosY 		= -160;
	float Scale 		= 0.8;
	
	#ifdef TTYD_JP
	PosX 	+= 5;
	PosY 	+= 2;
	Scale 	+= 0.05;
	#endif
	
	// Move the timer up if Mario's Coordinates are currently displayed
	// Don't move up if the input display is active
	if (!Displays[BUTTON_INPUT_DISPLAY])
	{
		if (Displays[MARIO_COORDINATES] || 
			Displays[PALACE_SKIP] || 
			Displays[YOSHI_SKIP])
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
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
	
	if (!FrameCounter.TimerPaused)
	{
		FrameCounter.MainTimer++;
	}
}

void drawMarioCoordinates()
{
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= -162;
	float Scale 			= 0.75;
	
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
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
}

void drawMarioSpeedXZ()
{
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= -162;
	float Scale 			= 0.75;
	
	// Move the text up if the input display is active
	if (Displays[BUTTON_INPUT_DISPLAY])
	{
		PosY += 20;
	}
	
	// Move the text up if the Mario coordinates display is active
	if (Displays[MARIO_COORDINATES] || 
		Displays[PALACE_SKIP] || 
		Displays[YOSHI_SKIP])
	{
		PosY += 20;
	}
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	char *tempDisplayBuffer = DisplayBuffer;
	
	sprintf(tempDisplayBuffer,
		"MarSpeedXZ: %.6f",
		player->wPlayerEffectiveSpeed);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
}

void drawJumpStorageDetails()
{
	uint32_t TextColor 	= 0xFFFFFFFF;
	uint8_t Alpha 		= 0xFF;
	int32_t PosX 		= 110;
	int32_t PosY 		= 120;
	float Scale 		= 0.75;
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	char *tempDisplayBuffer = DisplayBuffer;
	
	// Draw the main text
	sprintf(tempDisplayBuffer,
		"JS:\nSpdY: %.2f",
		player->wJumpVelocityY);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
	
	// Draw the On/Off Text
	bool JumpStorageFlag = (player->flags3 & (1 << 16)) >> 16; // Get only the 16 bit
	const char *TextToDraw;
	
	getOnOffTextAndColor(JumpStorageFlag, &TextToDraw, &TextColor);
	drawText(TextToDraw, PosX + 38, PosY, Alpha, TextColor, Scale);
}

void drawButtonInputs()
{
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = -160;
	float Scale = 0.75;
	
	uint32_t ButtonInput = ttyd::system::keyGetButton(0);
	char *tempDisplayBuffer = DisplayBuffer;
	
	const char *Format;
	#ifdef TTYD_JP
	Format = "%s";
	#else
	Format = "%c";
	#endif
	
	uint32_t Counter = 0;
	for (uint32_t i = 0; i < 13; i++)
	{
		if (i == 7)
		{
			// Skip unused input value
			i++;
		}
		
		if (ButtonInput & (1 << i))
		{
			sprintf(tempDisplayBuffer,
				Format,
				ButtonInputDisplay[Counter]);
			
			drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		}
		
		PosX += 20;
		Counter++;
	}
}

void drawStickAngle()
{
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= -162;
	float Scale 			= 0.75;
	
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
	
	// Move the text up if Mario's Coordinates are displayed, or if the Palace Skip display is active
	if (Displays[MARIO_COORDINATES] || Displays[PALACE_SKIP])
	{
		PosY += 20;
	}
	
	char StickAngleString[32];
	getStickAngleString(StickAngleString);
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"StickAngle: %s",
		StickAngleString);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
}

void drawMemoryWatchesOnOverworld()
{
	// Figure out which watches are going to be displayed
	bool tempHideMenu = MenuVar.HideMenu;
	uint32_t tempMenuSelectionOption = MenuVar.MenuSelectedOption;
	uint32_t Size = sizeof(MemoryWatch) / sizeof(MemoryWatch[0]);
	
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	float Scale = 0.75;
	
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
				// Draw the watch
				int32_t PosX = MemoryWatch[i].PosX;
				int32_t PosY = MemoryWatch[i].PosY;
				drawText(getValueString(i), PosX, PosY, Alpha, Color, Scale);
			}
		}
	}
}

void drawYoshiSkipDetails()
{
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= -117;
	float Scale 			= 0.75;
	
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
	char StickAngleString[32];
	getStickAngleString(StickAngleString);
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"YST: %" PRIu32 "\nStickAngle: %s",
		YoshiSkip.MainTimer,
		StickAngleString);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
	
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
		PartnerPosY = 0;
	}
	
	// Get Phantom Ember Y coordinate
	const uint32_t NPCSlot2 = 1;
	uint32_t NPCAddress = reinterpret_cast<uint32_t>(getNPCFieldWorkPointer(NPCSlot2)); // NPC 2
	
	// Check if NPC 2 is active, followed by NPC 1, and then default to 0 if neither is active
	bool FoundPhantomEmber = false;
	float PhantomEmberPosY;
	
	for (int32_t i = 0; i < 2; i++)
	{
		uint32_t NPCActive = *reinterpret_cast<uint32_t *>(NPCAddress);
		if (NPCActive &= (1 << 0)) // Check if 0 bit is active
		{
			PhantomEmberPosY = *reinterpret_cast<float *>(NPCAddress + 0x90);
			FoundPhantomEmber = true;
			break;
		}
		NPCAddress -= 0x340;
	}
	
	if (!FoundPhantomEmber)
	{
		// Neither NPC is active, so set PhantomEmberPosY to 0
		PhantomEmberPosY = 0;
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
	
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= -70;
	float Scale 			= 0.75;
	
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
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
	
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

void drawBlimpTicketSkipDetails()
{
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= -95;
	float Scale 			= 0.75;
	
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
	char StickAngleString[32];
	getStickAngleString(StickAngleString);
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"URT: %" PRIu32 "\nSUT: %" PRIu32 "\nStickAngle: %s",
		BlimpTicketSkip.UpRightTimer,
		BlimpTicketSkip.StraightUpTimer,
		StickAngleString);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
	
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
	uint32_t SimplifierOffset 		= 0x305;
	uint32_t UnsimplifierOffset 	= 0x306;
	#elif defined TTYD_JP
	uint32_t SimplifierOffset 		= 0x301;
	uint32_t UnsimplifierOffset 	= 0x302;
	#elif defined TTYD_EU
	uint32_t SimplifierOffset 		= 0x309;
	uint32_t UnsimplifierOffset 	= 0x30A;
	#endif
	
	uint32_t MarioBattlePointer = reinterpret_cast<uint32_t>(getMarioBattlePointer());
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
	
	const uint32_t SuccessfulTiming 		= 1;
	const uint32_t PressedTooManyButtons 	= 2;
	const uint32_t PressedTooEarly 			= 3;
	const uint32_t CannotBeSuperguarded 	= 4;
	
	// Check to see which text should be displayed
	switch (TypeToDraw)
	{
		case SuccessfulTiming:
		{
			uint32_t CurrentDifficultyFrames;
			uint32_t FramePressed;
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
				"Pressed %c on frame %" PRIu32 " of %" PRIu32,
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
			uint32_t FramesEarly;
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
				"Pressed %c %" PRIu32 " frame%s early",
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
	
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint32_t WindowColor 	= 0x000000D0;
	uint8_t Alpha 			= 0xFF;
	int32_t TextPosX 		= -232;
	int32_t TextPosY 		= -105;
	// int32_t WindowWidth 	= 320;
	int32_t WindowCurve 	= 10;
	float Scale 			= 0.75;
	
	int32_t WindowWidth 	= static_cast<int32_t>(getMessageWidth(TextToDraw, Scale));
	
	drawTextWithWindow(TextToDraw, TextPosX, TextPosY, Alpha, TextColor, 
		Scale, WindowWidth, WindowColor, WindowCurve);
	
	DisplayActionCommands.DisplayTimer--;
}

void drawSettingsCurrentWork()
{
	const char *String;
	int32_t WindowWidth;
	
	switch (MenuSettings.ReturnCode)
	{
		case LOAD_FAILED:
		{
			String = "Load Failed";
			WindowWidth = 129;
			break;
		}
		case LOAD_FAILED_NO_FILE:
		{
			String = "Settings file not found";
			WindowWidth = 218;
			break;
		}
		case LOAD_SUCCESSFUL:
		{
			String = "Load Successful";
			WindowWidth = 162;
			break;
		}
		case SAVE_FAILED:
		{
			String = "Save Failed";
			WindowWidth = 129;
			break;
		}
		case SAVE_SUCCESSFUL:
		{
			String = "Save Successful";
			WindowWidth = 162;
			break;
		}
		default:
		{
			MenuVar.Timer = 0;
			return;
		}
	}
	
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint32_t WindowColor 	= 0x151515F4;
	uint8_t Alpha 			= 0xFF;
	int32_t TextPosX 		= -80;
	int32_t TextPosY 		= 0;
	int32_t WindowCurve 	= 10;
	float Scale 			= 0.6;
	
	drawTextWithWindow(String, TextPosX, TextPosY, Alpha, TextColor, 
		Scale, WindowWidth, WindowColor, WindowCurve);
	
	uint32_t tempTimer = MenuVar.Timer;
	MenuVar.Timer = tempTimer - 1;
}

void drawHeapArrayErrors()
{
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= 120;
	float Scale 			= 0.6;
	
	// Draw the text
	char *tempHeapBuffer = HeapBuffer;
	drawText(tempHeapBuffer, PosX, PosY, Alpha, TextColor, Scale);
	
	// Clear the heap buffer
	clearMemory(tempHeapBuffer, sizeof(HeapBuffer));
}

void drawTitleScreenInfo()
{
	// Draw the window for the text
	uint32_t WindowColor 	= 0x000000E5;
	int32_t PosX 			= -225;
	int32_t PosY 			= -16;
	int32_t WindowWidth 	= 450;
	int32_t WindowHeight 	= 70;
	int32_t Curve 			= 10;
	
	#ifdef TTYD_JP
	PosY += 30;
	WindowWidth -= 10;
	#endif
	
	drawWindow(WindowColor, PosX, PosY, WindowWidth, WindowHeight, Curve);
	
	// Draw the text
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	float Scale 			= 0.75;
	PosX 					+= 108;
	PosY 					-= 14;
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"Practice Codes %s\nCreated by Zephiles",
		VersionNumber);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
}

void drawFileSelectScreenInfo()
{
	// Draw the window for the text
	uint32_t WindowColor 	= 0x000000E5;
	int32_t PosX 			= -225;
	int32_t PosY 			= -20;
	int32_t WindowWidth 	= 450;
	int32_t WindowHeight 	= 102;
	int32_t Curve 			= 10;
	
	drawWindow(WindowColor, PosX, PosY, WindowWidth, WindowHeight, Curve);
	
	// Draw the text
	uint32_t TextColor 		= 0xFFFFFFFF;
	uint8_t Alpha 			= 0xFF;
	float Scale 			= 0.6;
	PosX 					+= 15;
	PosY 					-= 13;
	
	const char *String = "Press L + Start to open the menu\n\nFor more info, go to the main page:\nhttps://github.com/Zephiles/TTYD-Practice-Codes";
	
	drawText(String, PosX, PosY, Alpha, TextColor, Scale);
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
			DrawHitboxDirection = -1; // Draw down
		}
		else
		{
			// The current actor is not hanging from the ceiling
			DrawHitboxDirection = 1; // Draw up
		}
		
		// Get the variables used to calculate the position of the hitbox
		float ActorHitboxWidth 			= *reinterpret_cast<float *>(BattleUnitPtrUint + 0xE4);
		float ActorHitboxHeight 		= *reinterpret_cast<float *>(BattleUnitPtrUint + 0xE8);
		float ActorHitboxPosOffsetX 	= *reinterpret_cast<float *>(BattleUnitPtrUint + 0xD8);
		float ActorHitboxPosOffsetY 	= *reinterpret_cast<float *>(BattleUnitPtrUint + 0xDC);
		float ActorSizeScale 			= *reinterpret_cast<float *>(BattleUnitPtrUint + 0x114);
		
		// Get the position of the current actor
		float ActorPos[3];
		ttyd::battle_unit::BtlUnit_GetPos(BattleUnitPtr, &ActorPos[0], &ActorPos[1], &ActorPos[2]);
		
		// Get the X and Y coordinate starting positions
		float DrawHitboxPosXStart = (ActorHitboxPosOffsetX * ActorSizeScale) + ActorPos[0];
		float DrawHitboxPosYStart = (DrawHitboxDirection * ActorHitboxPosOffsetY * ActorSizeScale) + ActorPos[1];
		
		// Get the amount to adjust the starting positions by
		float DrawHitboxAdjustPosX = (ActorHitboxWidth * ActorSizeScale) * 0.5;
		float DrawHitboxAdjustPosY = (ActorHitboxHeight * ActorSizeScale) * 0.5;
		
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

}