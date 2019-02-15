#include "mod.h"
#include "draw.h"
#include "global.h"
#include "items.h"
#include "menufunctions.h"
#include "commonfunctions.h"

#include <ttyd/dispdrv.h>
#include <ttyd/windowdrv.h>
#include <ttyd/icondrv.h>
#include <ttyd/string.h>
#include <ttyd/fontmgr.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/system.h>
#include <ttyd/mario.h>
#include <ttyd/w_atan2.h>

#include <cstdio>

namespace mod {

void drawFunctionOnDebugLayer(void (*func)())
{
	ttyd::dispdrv::dispEntry(ttyd::dispdrv::DisplayLayer::kDebug3d, 0, 
		[](ttyd::dispdrv::DisplayLayer layerId, void *user)
	{
		reinterpret_cast<void (*)()>(user)();
	}, reinterpret_cast<void *>(func));
}

void drawFunctionOn2DLayer(void (*func)())
{
	ttyd::dispdrv::dispEntry(ttyd::dispdrv::DisplayLayer::k2d, 0, 
		[](ttyd::dispdrv::DisplayLayer layerId, void *user)
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
	ttyd::windowdrv::windowDispGX_Waku_col(0, reinterpret_cast<uint8_t *>(&color), x, y, width, height, curve);
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
	ItemData *item_db = ItemDataTable;
	int16_t iconNum;
	
	#ifdef TTYD_JP
	const int16_t diamondStarIconValue 		= 407;
	const int16_t emeraldStarIconValue 		= 408;
	const int16_t garnetStarIconValue 		= 409;
	const int16_t goldStarIconValue 		= 410;
	const int16_t crystalStarIconValue 		= 411;
	const int16_t rubyStarIconValue 		= 412;
	const int16_t sapphireStarIconValue 	= 413;
	#else
	const int16_t diamondStarIconValue 		= 415;
	const int16_t emeraldStarIconValue 		= 416;
	const int16_t garnetStarIconValue 		= 417;
	const int16_t goldStarIconValue 		= 418;
	const int16_t crystalStarIconValue 		= 419;
	const int16_t rubyStarIconValue 		= 420;
	const int16_t sapphireStarIconValue 	= 421;
	#endif
	
	// Get different icons for the crystal stars
	switch (itemNum)
	{
		case WeddingRing:
		{
			// Use the larger icon; the smaller one is used by default
			iconNum = item_db[itemNum].icon_id - 1;
			break;
		}
		case DiamondStar:
		{
			iconNum = diamondStarIconValue;
			break;
		}
		case EmeraldStar:
		{
			iconNum = emeraldStarIconValue;
			break;
		}
		case GoldStar:
		{
			iconNum = goldStarIconValue;
			break;
		}
		case RubyStar:
		{
			iconNum = rubyStarIconValue;
			break;
		}
		case SapphireStar:
		{
			iconNum = sapphireStarIconValue;
			break;
		}
		case GarnetStar:
		{
			iconNum = garnetStarIconValue;
			break;
		}
		case CrystalStar:
		{
			iconNum = crystalStarIconValue;
			break;
		}
		default:
		{
			iconNum = item_db[itemNum].icon_id;
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
	int32_t textSize = ttyd::string::strlen(text);
	char tempText[textSize + 1];
	ttyd::string::strcpy(tempText, text);
	
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

void startDrawString(uint8_t alpha, uint32_t color, float scale)
{
	ttyd::fontmgr::FontDrawStart_alpha(alpha);
	ttyd::fontmgr::FontDrawColor(reinterpret_cast<uint8_t *>(&color));
	ttyd::fontmgr::FontDrawEdge();
	ttyd::fontmgr::FontDrawScale(scale);
}

void drawText(const char *text, int32_t x, int32_t y, uint8_t alpha, uint32_t color, float scale)
{
	startDrawString(alpha, color, scale);
	bool MultipleLines = false;
	
	uint32_t i = 0;
	bool LoopDone = false;
	while (!LoopDone)
	{
		if (text[i] == '\n')
		{
			MultipleLines 	= true;
			LoopDone 		= true;
		}
		else if (text[i] == '\0')
		{
			LoopDone = true;
		}
		i++;
	}
	
	float NewX = static_cast<float>(x);
	float NewY = static_cast<float>(y);
	
	if (MultipleLines)
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
	uint32_t tempTotalMenuOptions = Menu[CurrentMenu].TotalMenuOptions;
	uint32_t tempCurrentPage = 0;
	
	drawSingleColumn(tempTotalMenuOptions, tempCurrentPage);
}

void drawSingleColumn(uint32_t maxOptionsPerPage, uint32_t currentPage)
{
	uint32_t tempCurrentMenu 		= CurrentMenu;
	uint32_t tempSelectedOption 	= SelectedOption;
	uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t IndexStart 			= currentPage * maxOptionsPerPage;
	
	uint8_t Alpha 	= 0xFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 180;
	float Scale 	= 0.6;
	uint32_t Color;
	
	for (uint32_t i = IndexStart; i < (IndexStart + maxOptionsPerPage); i++)
	{
		if (i >= tempTotalMenuOptions)
		{
			// Reached the end of the array, so exit
			return;
		}
		
		if (tempSelectedOption == 0)
		{
			if (CurrentMenuOption == i)
			{
				Color = 0x5B59DEFF;
			}
			else
			{
				Color = 0xFFFFFFFF;
			}
		}
		else
		{
			if (tempSelectedOption == i)
			{
				Color = 0x5B59DEFF;
			}
			else
			{
				Color = 0xFFFFFFFF;
			}
		}
		
		const char *CurrentLine = Menu[tempCurrentMenu].Line[i];
		drawText(CurrentLine, PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
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
			
			if ((currentMenuOption == i) && showCurrentOption)
			{
				Color = 0x5B59DEFF;
			}
			else
			{
				Color = 0xFFFFFFFF;
			}
			
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
	uint32_t tempCurrentMenu = MenuSelectedOption;
	uint32_t tempCurrentPage = CurrentPage;
	
	uint32_t Address_and_Size[2];
	uint32_t *tempArray		= getPouchAddressAndSize(Address_and_Size);
	
	if (tempArray == nullptr)
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
	if (tempCurrentMenu != INVENTORY_STANDARD)
	{
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
}

void drawItemIconsColumn(uint32_t indexStart, uint32_t indexIncrement, uint32_t size, 
	int32_t posX, int32_t posY, uint32_t address, float scale)
{
	uint32_t MaxIconsPerColumn 		= 10;
	uint32_t MaxIconsPerPage 		= 20;
	uint32_t newIndexStart 			= indexStart + (CurrentPage * MaxIconsPerPage);
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
		else if ((CurrentItem >= PaperModeCurse) && 
			(CurrentItem <= UltraHammer))
		{
			// Decrease the size of these icons
			tempScale = 0.5;
		}
		else if (CurrentItem == MagicalMapBigger)
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
	uint32_t tempCurrentPage 		= CurrentPage;
	uint32_t tempSelectedOption 	= SelectedOption;
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
		
		if ((tempSelectedOption >= DUPLICATE) && 
			(tempSelectedOption <= DELETE) && 
				(FunctionReturnCode >= 0) && 
					(CurrentMenuOption == IndexCounter))
		{
			TextColor = 0x5B59DEFF;
		}
		else
		{
			TextColor = 0xFFFFFFFF;
		}
		
		int16_t CurrentItem = *reinterpret_cast<int16_t *>(address + (IndexCounter * 0x2));
		if (CurrentItem == 0)
		{
			// Reached the end of the array, so exit
			return;
		}
		
		const char *ItemName = getItemName(CurrentItem);
		drawText(ItemName, posX, posY, alpha, TextColor, scale);
		posY -= 30;
		IndexCounter += indexIncrement;
	}
}

void iconViewer()
{
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	IconPosition[IconPositionX] 	= 0;
	IconPosition[IconPositionY] 	= 0;
	
	static int16_t tempValue = 0;
	
	if (checkButtonCombo(PAD_DPAD_LEFT))
	{
		tempValue--;
	}
	else if (checkButtonCombo(PAD_DPAD_RIGHT))
	{
		tempValue++;
	}
	
	drawIcon(IconPosition, tempValue, 0.6);
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"%d",
		tempValue);
	
	drawText(tempDisplayBuffer, 0, 60, 0xFF, 0xFFFFFFFF, 0.8);
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
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
	// Set the values for the text to use
	PosX += 25;
	PosY = 90;
	
	// Set up array to use for displaying icons
	int32_t IconPosition[3];
	const int32_t IconPositionX 	= 0;
	const int32_t IconPositionY 	= 1;
	IconPosition[2] 				= 0;
	IconPosition[IconPositionX] 	= PosX - 15;
	IconPosition[IconPositionY] 	= PosY - 18;
	
	// Draw the icons and main text
	int16_t SpecialMoveIcon;
	
	for (uint32_t i = 0; i < 8; i++)
	{
		// Draw the icon
		if (i == 0)
		{
			// Set the icon for Sweet Treat
			SpecialMoveIcon = MagicalMap;
		}
		else
		{
			SpecialMoveIcon = (DiamondStar + i) - 1;
		}
		
		drawIconFromItem(IconPosition, SpecialMoveIcon, Scale);
		
		// Draw the main text
		if (SecondaryMenuOption == i)
		{
			Color = 0x5B59DEFF;
		}
		else
		{
			Color = 0xFFFFFFFF;
		}
		
		drawText(MarioStatsSpecialMovesOptions[i], PosX, PosY, Alpha, Color, Scale);
		
		IconPosition[IconPositionY] -= 30;
		PosY -= 30;
	}
	
	// Draw the on/off text
	uint32_t PouchPtr = reinterpret_cast<uint32_t>(ttyd::mario_pouch::pouchGetPtr());
	int16_t SpecialMovesBits = *reinterpret_cast<int16_t *>(PouchPtr + 0x8C);
	PosX += 130;
	PosY = 90;
	
	for (uint32_t i = 0; i < 8; i++)
	{
		// Check if the current special move is on or off
		const char *CurrentSpecialMove;
		if (SpecialMovesBits & (1 << i))
		{
			CurrentSpecialMove = "On";
			Color = 0x1BBE23FF;
		}
		else
		{
			CurrentSpecialMove = "Off";
			Color = 0xFF1414FF;
		}
		
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
	
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
	PosX += 45;
	PosY -= 54;
	
	// Draw the main text
	uint32_t tempStatsFollowerOptionsLinesSize = StatsFollowerOptionsLinesSize;
	for (uint32_t i = 0; i < tempStatsFollowerOptionsLinesSize; i++)
	{
		if (SecondaryMenuOption == i)
		{
			Color = 0x5B59DEFF;
		}
		else
		{
			Color = 0xFFFFFFFF;
		}
		
		drawText(StatsFollowerOptionsLines[i], PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
	}
}

void drawMarioStats()
{
	// Create array for each stat to go in
	int32_t MarioStatsArray[16];
	clearMemory(MarioStatsArray, sizeof(MarioStatsArray));
	
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
	
	uint32_t PiantaParlorPtr = *reinterpret_cast<uint32_t *>(PiantaParlorAddressesStart);
	MarioStatsArray[14] = *reinterpret_cast<int32_t *>(PiantaParlorPtr + 0x4); // Piantas Stored
	MarioStatsArray[15] = *reinterpret_cast<int32_t *>(PiantaParlorPtr + 0x8); // Current Piantas
	
	uint32_t Color 					= 0xFFFFFFFF;
	uint8_t Alpha 					= 0xFF;
	float TextScale 				= 0.6;
	float IconScale					= 0.6;
	int32_t TextPosX 				= -208;
	int32_t TextPosY 				= 120;
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
			if (i == 11) // 1 before Star Power
			{
				// Move the icons up
				IconPosYIncrement = -5;
			}
			else if (i == 13) // 1 after Max Star Power
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
			if ((CurrentMenuOption == i) && (SelectedOption > 0))
			{
				SelectionTextColor = 0x5B59DEFF;
			}
			else
			{
				SelectionTextColor = 0xFFFFFFFF;
			}
			drawText(StatsMarioOptionsLines[i], PosX, PosY, Alpha, SelectionTextColor, TextScale);
			
			// Draw the values
			if (i != 11) // Special moves -- drawn separately
			{
				sprintf(tempDisplayBuffer,
					"%ld",
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
	IconPosition[IconPositionY] = 46;
	
	int16_t SpecialMovesBits = *reinterpret_cast<int16_t *>(PouchPtr + 0x8C);
	for (uint32_t i = 0; i < 8; i++)
	{
		if (SpecialMovesBits & (1 << i))
		{
			if (i == 0)
			{
				// Set the icon for Sweet Treat
				SpecialMoveIcon = MagicalMap;
			}
			else
			{
				SpecialMoveIcon = (DiamondStar + i) - 1;
			}
			
			drawIconFromItem(IconPosition, SpecialMoveIcon, SpecialMovesScale);
		}
		
		IconPosition[IconPositionX] += 13;
	}
}

void drawPartnerStats()
{
	uint32_t tempCurrentMenuOption = CurrentMenuOption;
	uint32_t tempSelectedOption = SelectedOption;
	
	// Only display if a partner is selected
	if (tempSelectedOption == 0)
	{
		if (tempCurrentMenuOption == 0)
		{
			return;
		}
	}
	
	// Create array for each stat to go in
	int16_t PartnerStats[3];
	clearMemory(PartnerStats, sizeof(PartnerStats));
	
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
	uint32_t tempStatsPartnerOptionsLinesSize = StatsPartnerOptionsLinesSize - 2;
	for (uint32_t i = 0; i < tempStatsPartnerOptionsLinesSize; i++)
	{
		if ((tempCurrentMenuOption == i) && (tempSelectedOption > 0))
		{
			Color = 0x5B59DEFF;
		}
		else
		{
			Color = 0xFFFFFFFF;
		}
		
		drawText(StatsPartnerOptionsLines[i], PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
	}
	
	// Check if the current partner out is the selected one
	uint8_t OptionToDraw;
	if (checkIfPartnerOutSelected())
	{
		OptionToDraw = REMOVE; 
	}
	else
	{
		OptionToDraw = BRING_OUT;
	}
	
	// Draw the option
	if ((static_cast<uint8_t>(tempCurrentMenuOption) == 
		(BRING_OUT - 1)) && (tempSelectedOption > 0))
	{
		Color = 0x5B59DEFF;
	}
	else
	{
		Color = 0xFFFFFFFF;
	}
	
	drawText(StatsPartnerOptionsLines[OptionToDraw - 1], PosX, PosY, Alpha, Color, Scale);
	
	PosX = -20;
	PosY = 180;
	Color = 0xFFFFFFFF;
	
	// Draw the HP, Max HP, and Level
	char *tempDisplayBuffer = DisplayBuffer;
	for (uint32_t i = 0; i < (tempStatsPartnerOptionsLinesSize - 1); i++)
	{
		sprintf(tempDisplayBuffer,
			"%d",
			PartnerStats[i]);
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
	}
	
	// Draw the on or off text
	bool PartnerEnabled = *reinterpret_cast<bool *>(PartnerEnabledAddress + 0x1);
	const char *String;
	
	if (PartnerEnabled)
	{
		String = "On";
		Color = 0x1BBE23FF;
	}
	else
	{
		String = "Off";
		Color = 0xFF1414FF;
	}
	
	drawText(String, PosX, PosY, Alpha, Color, Scale);
	
	// Check if a partner is out or not
	uint32_t PartnerPointer = reinterpret_cast<uint32_t>(getPartnerPointer());
	if (PartnerPointer == 0)
	{
		// Draw text explaining that no partner is currently out
		const char *String = "No partner is currently out";
		Color = 0xFFFFFFFF;
		PosX = -120;
		PosY -= 60;
		drawText(String, PosX, PosY, Alpha, Color, Scale);
	}
}

void drawBattlesActorStats()
{
	// Get the starting address for the current actor
	uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuSelectedOption));
	if (ActorAddress == 0)
	{
		return;
	}
	
	// Create array for each stat to go in
	int16_t ActorStats[4];
	clearMemory(ActorStats, sizeof(ActorStats));
	
	// Add each stat to the array
	ActorStats[0] = *reinterpret_cast<int16_t *>(ActorAddress + 0x10C); // HP
	ActorStats[1] = *reinterpret_cast<int16_t *>(ActorAddress + 0x108); // Max HP
	ActorStats[2] = *reinterpret_cast<int16_t *>(ActorAddress + 0x112); // FP
	ActorStats[3] = *reinterpret_cast<int16_t *>(ActorAddress + 0x10E); // Max FP
	
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	int32_t PosX = -50;
	int32_t PosY = 180;
	float Scale = 0.6;
	
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
	char *tempDisplayBuffer = DisplayBuffer;
	for (uint32_t i = 0; i < (tempBattlesCurrentActorStatsSize); i++)
	{
		sprintf(tempDisplayBuffer,
			"%d",
			ActorStats[i]);
		
		drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
	}
}

void drawCurrentFollowerOut()
{
	// Draw the text for the current follower out
	uint32_t FollowerPointer = reinterpret_cast<uint32_t>(getFollowerPointer());
	const char *String;
	
	if (FollowerPointer == 0)
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
	int32_t PosY = 100;
	float Scale = 0.6;
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
}

void drawBattlesActorsList()
{
	uint32_t tempCurrentPage 		= CurrentPage;
	uint32_t TotalMenuOptions 		= 62; // Excluding System
	uint32_t MaxOptionsPerPage 		= 13;
	uint32_t IndexStart 			= tempCurrentPage * MaxOptionsPerPage;
	
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = 120;
	float Scale = 0.6;
	uint32_t Color;
	
	// Draw the current page
	drawPageNumber(PosX, PosY, tempCurrentPage);
	PosY -= 40;
	
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
		
		if (CurrentActorPointer == 0)
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
		
		if ((CurrentMenuOption == i) && 
			(SelectedOption != 0))
		{
			Color = 0x5B59DEFF;
		}
		else
		{
			if (SlotIsEmpty)
			{
				Color = 0x4B4B4BFF;
			}
			else
			{
				Color = 0xFFFFFFFF;
			}
		}
		
		drawText(CurrentActorString, PosX, PosY, Alpha, Color, Scale);
		PosY -= 20;
	}
}

void drawBattlesStatusesList()
{
	// Get the starting address for the current actor
	uint32_t ActorAddress = reinterpret_cast<uint32_t>(getActorPointer(MenuSelectedOption));
	if (ActorAddress == 0)
	{
		return;
	}
	
	uint32_t MaxOptionsPerPage 		= 12;
	uint32_t IndexStart 			= CurrentPage * MaxOptionsPerPage;
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
	if (IndexStart >= 1) // Sleep flags
	{
		DisplayValueCounter++;
		
		if (IndexStart >= 18) // Can't use items
		{
			DisplayValueCounter++;
			
			if (IndexStart >= 20) // Explosion turns left
			{
				DisplayValueCounter++;
			}
		}
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
		
		if (CurrentMenuOption == i)
		{
			// Draw a window for the current line
			uint32_t WindowColor = 0x151515E0;
			int32_t WindowPosX = -245;
			int32_t WindowPosY = 185 - (DrawWindowCounter * 30);
			int32_t Width = 363;
			int32_t Height = 35;
			int32_t Curve = 0;
			drawWindow(WindowColor, WindowPosX, WindowPosY, Width, Height, Curve);
			
			// Set the main text color to blue
			Color = 0x5B59DEFF;
		}
		else
		{
			Color = 0xFFFFFFFF;
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
		sprintf(tempDisplayBuffer,
			"%d",
			CurrentValue);
		
		Color = 0xFFFFFFFF;
		drawText(tempDisplayBuffer, TextPosX + 290, TextPosY, Alpha, Color, TextScale);
		
		IconPosition[IconPositionY] -= 30;
		TextPosY -= 30;
		DrawWindowCounter++;
	}
}

void drawErrorWindow(const char *text, int32_t textPosX, int32_t windowWidth)
{
	// int32_t TextPosX 		= -200;
	int32_t TextPosY 		= 60;
	uint8_t Alpha  			= 0xFF;
	uint32_t TextColor 		= 0xFFFFFFFF;
	float Scale 			= 0.6;
	// int32_t WindowWidth 	= 415;
	uint32_t WindowColor 	= 0x151515F4;
	int32_t WindowCurve 	= 20;
	
	drawTextWithWindow(text, textPosX, TextPosY, Alpha, TextColor, 
		Scale, windowWidth, WindowColor, WindowCurve);
}

void drawErrorMessage(const char *line)
{
	uint32_t tempTimer = Timer;
	if ((FunctionReturnCode < 0) && (tempTimer > 0))
	{
		if (checkForClosingErrorMessage())
		{
			SelectedOption = 0;
			return;
		}
		
		int32_t TextPosX 			= -130;
		// int32_t WindowWidth 		= 305;
		uint16_t LineLength 		= ttyd::fontmgr::FontGetMessageWidth(line);
		int32_t WindowWidth 		= 80 + (LineLength >> 1) - (LineLength % 10);
		drawErrorWindow(line, TextPosX, WindowWidth);
	}
	else
	{
		resetAndCloseSecondaryMenu();
	}
}

void drawPartnerFollowerMessage(const char *message)
{
	uint32_t tempTimer = Timer;
	if ((FunctionReturnCode < 0) && (tempTimer > 0))
	{
		if (checkForClosingErrorMessage())
		{
			return;
		}
		
		// Print error text if currently trying to spawn a partner/follower when not able to
		int32_t PosX 			= -172;
		int32_t WindowWidth 	= 375;
		drawErrorWindow(message, PosX, WindowWidth);
	}
}

void drawPartnerErrorMessage()
{
	// Print error text if currently trying to spawn a partner when not able to
	const char *CurrentLine = "To spawn a partner, you must have a file\nloaded and not be in a battle nor a\nscreen transition.";
	drawPartnerFollowerMessage(CurrentLine);
}

void drawFollowersErrorMessage()
{
	// Print error text if currently trying to spawn a follower when not able to
	const char *CurrentLine = "To spawn a follower, you must have a file\nloaded and not be in a battle nor a\nscreen transition.";
	drawPartnerFollowerMessage(CurrentLine);
}

void drawNotInBattleErrorMessage()
{
	uint32_t tempTimer = Timer;
	if ((FunctionReturnCode < 0) && (tempTimer > 0))
	{
		if (checkForClosingErrorMessage())
		{
			return;
		}
		
		// Print error text if currently trying to warp when not able to
		const char *CurrentLine = "You must be in a battle to use the Battles menu.";
		int32_t PosX 			= -205;
		int32_t WindowWidth 	= 440;
		drawErrorWindow(CurrentLine, PosX, WindowWidth);
	}
}

void drawWarpsErrorMessage()
{
	uint32_t tempTimer = Timer;
	if ((FunctionReturnCode < 0) && (tempTimer > 0))
	{
		if (checkForClosingErrorMessage())
		{
			return;
		}
		
		// Print error text if currently trying to warp when not able to
		const char *CurrentLine = "To warp, you must have a file loaded and not\nbe in a battle nor a screen transition.";
		int32_t PosX 			= -195;
		int32_t WindowWidth 	= 415;
		drawErrorWindow(CurrentLine, PosX, WindowWidth);
	}
}

void drawConfirmationWindow(const char *message)
{
	// Manage the D-Pad
	uint32_t ButtonInputTrg = ttyd::system::keyGetButtonTrg(0);
	uint32_t tempSecondaryMenuOption = SecondaryMenuOption;
	
	if (ButtonInputTrg & PAD_DPAD_DOWN)
	{
		if (tempSecondaryMenuOption == 1)
		{
			SecondaryMenuOption = 0;
		}
		else
		{
			SecondaryMenuOption = 1;
		}
	}
	else if (ButtonInputTrg & PAD_DPAD_UP)
	{
		if (tempSecondaryMenuOption == 0)
		{
			SecondaryMenuOption = 1;
		}
		else
		{
			SecondaryMenuOption = 0;
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
	
	drawText(message, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the yes/no options
	PosX += 155;
	PosY -= 110;
	Scale = 0.8;
	const char *Option;
	
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
		
		if (tempSecondaryMenuOption == i)
		{
			Color = 0x5B59DEFF;
		}
		else
		{
			Color = 0xFFFFFFFF;
		}
		
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

void drawSingleLineFromStringAndValue(int32_t posX, int32_t posY, 
	const char *line, int32_t value)
{
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"%s%ld",
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
		if (ReturnCode == 0x1000)
		{
			// There are no numbers to display, so exit
			return;
		}
	}
	
	// Draw the window
	uint32_t color 	= 0x151515E0;
	int32_t PosX 	= -189;
	int32_t y 		= 105;
	int32_t width 	= 377;
	int32_t curve 	= 10;
	int32_t height;
	
	if (changingItem)
	{
		height = 208;
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
	
	adjustMenuItemBounds(0, currentMenu); // Adjust the current value if necessary
	
	int32_t tempMenuSecondaryValue = MenuSecondaryValue;
	y -= 100;
	
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
		
		if ((tempMenuSecondaryValue >= PaperModeCurse) && 
			(tempMenuSecondaryValue <= UltraHammer))
		{
			// Decrease the size of these icons
			IconScale = 0.5;
		}
		else if (tempMenuSecondaryValue == MagicalMapBigger)
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
	
	// Check if the number is negative
	bool NumberIsNegative = false;
	if (tempMenuSecondaryValue < 0)
	{
		// Convert the number to positive
		NumberIsNegative = true;
		tempMenuSecondaryValue = -tempMenuSecondaryValue;
	}
	
	// Get the amount of numbers to draw
	uint32_t AmountOfNumbers = getHighestAdjustableValueDigit(currentMenu);
	
	// Set up array for each digit of the number
	uint8_t AdjustableValue[AmountOfNumbers];
	
	// Get the values for the array
	for (uint32_t i = 0; i < AmountOfNumbers; i++)
	{
		AdjustableValue[AmountOfNumbers - i - 1] = tempMenuSecondaryValue % 10;
		tempMenuSecondaryValue /= 10;
	}
	
	/*AdjustableValue[0] = tempMenuSecondaryValue / 100;
	AdjustableValue[1] = (tempMenuSecondaryValue % 100) / 10;
	AdjustableValue[2] = (tempMenuSecondaryValue % 100) % 10;*/
	
	x 					+= 166;
	y 					-= 30;
	scale 				= 0.9;
	
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
		drawText("-", (x - 10), y, alpha, color, scale);
	}
	
	// Draw each digit of the number
	for (uint32_t i = 0; i < AmountOfNumbers; i++)
	{
		if (SecondaryMenuOption == i)
		{
			color = 0x5B59DEFF;
		}
		else
		{
			color = 0xFFFFFFFF;
		}
		
		char *tempDisplayBuffer = DisplayBuffer;
		sprintf(tempDisplayBuffer,
			"%d",
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
	int32_t Address_and_Size[2];
	int32_t *tempArray 			= getUpperAndLowerBounds(Address_and_Size, MenuSelectedOption);
	
	if (tempArray == nullptr)
	{
		return;
	}
	
	int32_t LowerBound 			= tempArray[0];
	int32_t UpperBound 			= tempArray[1];
	
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
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the window for the current menu option
	uint32_t tempCurrentMenuOption = SecondaryMenuOption;
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
		if ((i >= PaperModeCurse) && (i <= UltraHammer))
		{
			// Decrease the size of these icons
			Scale = 0.5;
		}
		else if (i == MagicalMapBigger)
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
		drawErrorMessage(CurrentLine);
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
		drawErrorMessage(CurrentLine);
	}
}

void drawPageNumber(int32_t posX, int32_t posY, uint32_t currentPage)
{
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha 	= 0xFF;
	float Scale = 0.6;
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"Page %ld",
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
	if (tempBool)
	{
		StringValue = "On";
		Color = 0x1BBE23FF;
	}
	else
	{
		StringValue = "Off";
		Color = 0xFF1414FF;
	}
	
	posY -= 20;
	drawText(StringValue, PosX, posY, Alpha, Color, Scale);
}

void drawCheatsBool(int32_t posY)
{
	uint32_t tempMenuSelectedOption = MenuSelectedOption;
	
	bool CheatActive = Cheat[tempMenuSelectedOption].Active;
	const char *CurrentLine = CheatsLines[tempMenuSelectedOption + 1];
	
	drawBoolOnOrOff(CheatActive, CurrentLine, posY);
}

void drawButtonCombo(uint16_t buttonCombo, int32_t posY, const char *description)
{
	uint8_t Alpha 	= 0xFF;
	uint32_t Color 	= 0xFFFFFFFF;
	int32_t PosX 	= -232;
	float Scale 	= 0.6;
	
	// Draw the text for telling what the button combo is
	Color = 0xFFFFFFFF;
	drawText(description, PosX, posY, Alpha, Color, Scale);
	
	// Draw the button combo
	uint8_t tempButtonArray[14]; // Extra spot for 0 at the end of the array
	clearMemory(tempButtonArray, sizeof(tempButtonArray));
	uint8_t *ButtonArray = getButtonsPressed(tempButtonArray, buttonCombo);
	
	// uint8_t *CurrentButtonComboArray = nullptr;
	// uint8_t *ButtonArray = getButtonsPressed(CurrentButtonComboArray);
	
	char ButtonString[256];
	clearMemory(ButtonString, sizeof(ButtonString));
	char *tempButtonString = createButtonStringArray(ButtonString, ButtonArray);
	
	posY -= 20;
	drawText(tempButtonString, PosX, posY, Alpha, Color, Scale);
	// delete[] (ButtonArray);
	// ButtonArray = nullptr;
}

void drawChangeButtonCombo(uint16_t &currentButtonCombo)
{
	// Check for if this menu should be closed or not
	uint32_t ButtonInput = ttyd::system::keyGetButton(0);
	if (incrementCheatsBButtonCounter(ButtonInput))
	{
		// Close this menu
		Timer 						= 0;
		SelectedOption 				= 0;
		ChangingCheatButtonCombo 	= false;
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
	PosX 					+= 60;
	PosY 					-= 30;
	Color 					= 0xFFFFFFFF;
	const char *HelpText 	= "Hold the button(s) for 3 seconds to set the\nnew button combo\n\nPress B three times in succession to cancel";
	drawText(HelpText, PosX, PosY, Alpha, Color, Scale);
	
	// Draw the timer
	// Get the proper FPS for the timer
	uint32_t FPS = *reinterpret_cast<uint32_t *>(
		*reinterpret_cast<uint32_t *>(GlobalWorkPointer) + 0x4);
    
	uint32_t tempTimer = Timer;
    uint32_t second = (tempTimer / FPS) % 60;
    uint32_t frame = tempTimer % FPS;
	char *tempDisplayBuffer = DisplayBuffer;
	
	sprintf(tempDisplayBuffer,
		"Time Left: %.2ld.%.2ld",
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
	uint16_t NewButtonCombo = static_cast<uint16_t>(ButtonInput);
	uint8_t *tempButtonArray = CheatsDisplayButtons.CheatsCurrentButtonsHeld;
	
	// Clear the memory, so that the previous results do not interfere with the new results
	clearMemory(tempButtonArray, (14 * sizeof(uint8_t)));
	getButtonsPressed(tempButtonArray, NewButtonCombo);
	
	char ButtonString[256];
	clearMemory(ButtonString, sizeof(ButtonString));
	char *tempButtonString = createButtonStringArray(ButtonString, 
		CheatsDisplayButtons.CheatsCurrentButtonsHeld);
	
	PosY -= 20;
	drawText(tempButtonString, PosX, PosY, Alpha, Color, Scale);
	
	// Decrement the timer and check to see if it's at 0
	if (cheatsManageTimer(ButtonInput))
	{
		// The timer reached 0, so set the currently held buttons to the current button combo for the cheat
		// Make sure the button combo isn't L + Start
		if (NewButtonCombo != (PAD_L | PAD_START))
		{
			currentButtonCombo = NewButtonCombo;
		}
		
		// Close this menu
		SelectedOption 				= 0;
		ChangingCheatButtonCombo 	= false;
		clearMemory(CheatsDisplayButtons.CheatsPreviousButtonsHeld, (14 * sizeof(uint8_t)));
		clearMemory(CheatsDisplayButtons.CheatsCurrentButtonsHeld, (14 * sizeof(uint8_t)));
	}
}

void drawCheatsForcedDropItem()
{
	uint8_t Alpha 	= 0xFF;
	uint32_t Color 	= 0xFFFFFFFF;
	int32_t PosX 	= -232;
	int32_t PosY 	= 10;
	float Scale 	= 0.6;
	
	// Draw the text for showing what the current item is
	const char *CurrentLine = "Current Item/Badge";
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
	
	int16_t tempForcedNPCItemDrop = ForcedNPCItemDrop;
	drawIconFromItem(IconPosition, tempForcedNPCItemDrop, Scale);
	
	// Draw the text for the item
	PosX += 17;
	PosY += 18;
	const char *ItemName = getItemName(tempForcedNPCItemDrop);
	
	drawText(ItemName, PosX, PosY, Alpha, Color, Scale);
}

void drawCheatsClearArea()
{
	// Draw the main text
	uint32_t tempCheatsForceItemDropAreasSize 	= CheatsForceItemDropAreasSize;
	const char **tempCheatsForceItemDropAreas 	= CheatsForceItemDropAreas;
	
	int32_t PosX 								= -232;
	int32_t PosY 								= 100;
	uint32_t Size 								= tempCheatsForceItemDropAreasSize;
	uint32_t MaxOptionsPerPage 					= tempCheatsForceItemDropAreasSize;
	uint32_t MaxOptionsPerRow 					= 4;
	uint32_t PosXIncrementAmount 				= 50;
	uint32_t tempPage 							= 0;
	uint32_t tempSecondaryMenuOption 			= SecondaryMenuOption;
	
	bool CurrentlySelectingOption;
	if (SelectedOption == SELECT_AREA)
	{
		CurrentlySelectingOption = true;
	}
	else
	{
		CurrentlySelectingOption = false;
	}
	
	drawMultipleColumnsVertical(PosX, PosY, tempSecondaryMenuOption, tempPage, 
		Size, MaxOptionsPerPage, MaxOptionsPerRow, CurrentlySelectingOption, 
			PosXIncrementAmount, tempCheatsForceItemDropAreas);
	
	// Draw the description for the current option
	if (CurrentlySelectingOption)
	{
		PosY -= 140;
		drawSingleLineFromArray(PosX, PosY, tempSecondaryMenuOption, 
			CheatsForceItemDropAreasFullNames);
	}
	
	// Draw the current area selected
	PosX = 0;
	PosY = 160;
	const char *String = "Current Area: ";
	drawSingleLineFromStringAndArray(PosX, PosY, MenuSecondaryValue, 
		String, tempCheatsForceItemDropAreas);
}

void drawWarpsOptions()
{
	// Draw the main text
	uint32_t tempWarpDestinationsSize 			= WarpDestinationsSize;
	const char **tempWarpDestinations 			= WarpDestinations;
	
	int32_t PosX 								= -232;
	int32_t PosY 								= 120;
	uint32_t Size 								= tempWarpDestinationsSize;
	uint32_t MaxOptionsPerPage 					= tempWarpDestinationsSize;
	uint32_t MaxOptionsPerRow 					= 4;
	uint32_t PosXIncrementAmount 				= 100;
	uint32_t tempPage 							= 0;
	uint32_t tempCurrentMenuOption 				= CurrentMenuOption;
	
	bool CurrentlySelectingOption;
	if (SelectedOption == SELECT_WARP)
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

void drawOnScreenTimerButtonCombos()
{
	int32_t PosY = -20;
	uint32_t tempOnScreenTimerOptionsSize = OnScreenTimerOptionsSize;
	
	for (uint32_t i = 0; i < tempOnScreenTimerOptionsSize; i++)
	{
		drawButtonCombo(OnScreenTimer.ButtonCombo[i], PosY, OnScreenTimerOptions[i]);
		PosY -= 60;
	}
}

void drawSequenceInPauseMenu()
{
	uint32_t Color 				= 0xFFFFFFFF;
	uint8_t Alpha 				= 0xFF;
	int32_t PosX 				= 83;
	int32_t PosY 				= 47;
	float Scale 				= 0.8;
	
	uint32_t SequencePosition 	= getSequencePosition();
	char *tempDisplayBuffer 	= DisplayBuffer;
	
	sprintf(tempDisplayBuffer,
		"Seq: %lu",
		SequencePosition);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, Color, Scale);
}

void drawOnScreenTimer()
{
	// Get the proper FPS for the timer
	uint32_t FPS = *reinterpret_cast<uint32_t *>(
		*reinterpret_cast<uint32_t *>(GlobalWorkPointer) + 0x4);
	
	uint32_t tempTimer = OnScreenTimer.MainTimer;
	uint32_t hour = tempTimer / 3600 / FPS;
	uint32_t minute = (tempTimer / 60 / FPS) % 60;
	uint32_t second = (tempTimer / FPS) % 60;
	uint32_t frame = tempTimer % FPS;
	
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"%.2ld:%.2ld:%.2ld.%.2ld",
		hour,
		minute,
		second,
		frame);
	
	uint32_t TextColor 	= 0xFFFFFFFF;
	uint8_t Alpha 		= 0xFF;
	int32_t PosX 		= 105;
	int32_t PosY 		= -160;
	float Scale 		= 0.8;
	
	// Move the timer up if Mario's Coordinates are currently displayed
	if (Displays[MARIO_COORDINATES] || 
		Displays[PALACE_SKIP] || 
			Displays[YOSHI_SKIP])
	{
		PosY += 20;
	}
	
	#ifdef TTYD_JP
	PosX 	+= 5;
	PosY 	+= 2;
	Scale 	+= 0.05;
	#endif
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
	
	if (!OnScreenTimer.TimerPaused)
	{
		OnScreenTimer.MainTimer++;
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

void drawJumpStorageDetails()
{
	uint32_t TextColor 	= 0xFFFFFFFF;
	uint8_t Alpha 		= 0xFF;
	int32_t PosX 		= 110;
	int32_t PosY 		= 120;
	float Scale 		= 0.75;
	
	ttyd::mario::Player *player = ttyd::mario::marioGetPtr();
	char *tempDisplayBuffer = DisplayBuffer;
	
	sprintf(tempDisplayBuffer,
		"JS: %ld\nSpdY: %.2f",
		(player->flags3 & (1 << 16)) >> 16, // Get only the 16 bit
		player->wJumpVelocityY);
	
	drawText(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, Scale);
}

void drawButtonInputs()
{
	uint32_t Color = 0xFFFFFFFF;
	uint8_t Alpha = 0xFF;
	int32_t PosX = -232;
	int32_t PosY = -160;
	float Scale = 0.75;
	
	startDrawString(Alpha, Color, Scale);
	uint32_t ButtonInput = ttyd::system::keyGetButton(0);
	char *tempDisplayBuffer = DisplayBuffer;
	
	uint32_t Counter = 0;
	for (int i = 0; i < 13; i++)
	{
		if (i == 7)
		{
			// Skip unused input value
			i++;
		}
		
		if (ButtonInput & (1 << i))
		{
			sprintf(tempDisplayBuffer,
				"%c",
				ButtonInputDisplay[Counter]);
			
			ttyd::fontmgr::FontDrawString(PosX, PosY, tempDisplayBuffer);
		}
		
		PosX += 20;
		Counter++;
	}
}

void drawYoshiSkipDetails()
{
	double StickX = static_cast<double>(ttyd::system::keyGetStickX(0));
	if (StickX > 127)
	{
		StickX -= 256;
	}
	
	double StickY = static_cast<double>(ttyd::system::keyGetStickY(0));
	if (StickY > 127)
	{
		StickY -= 256;
	}
	
	double PI = 3.14159265358979323846;
	double StickAngle = (ttyd::w_atan2::atan2(StickX, StickY)) * (180 / PI);
	
	if (StickAngle < 0)
	{
		StickAngle += 360;
	}
	
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
	
	// Draw the text
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"YST: %lu\nStickAngle: %.2f",
		YoshiSkip.MainTimer,
		StickAngle);
	
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
	
	if (PartnerPointer != 0)
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
	uint32_t NPCAddresses = *reinterpret_cast<uint32_t *>(NPCAddressesStart);
	uint32_t tempNPCAddresses = NPCAddresses + 0x340; // NPC 2
	float PhantomEmberPosY;
	
	// Check if NPC 2 is active, followed by NPC 1, and then default to 0 if neither is active
	bool FoundPhantomEmber = false;
	for (int32_t i = 0; i < 2; i++)
	{
		uint32_t NPCActive = *reinterpret_cast<uint32_t *>(tempNPCAddresses);
		if (NPCActive &= (1 << 0)) // Check if 0 bit is active
		{
			PhantomEmberPosY = *reinterpret_cast<float *>(
				tempNPCAddresses + 0x90);
			
			FoundPhantomEmber = true;
			break;
		}
		tempNPCAddresses -= 0x340;
	}
	
	if (!FoundPhantomEmber)
	{
		// Neither NPC is active, so set PhantomEmberPosY to 0
		PhantomEmberPosY = 0;
	}
	
	// Get Field Item Timer
	uint32_t FieldItemsAddress = *reinterpret_cast<uint32_t *>(FieldItemsAddressesStart);
	bool FoundItem = false;
	int32_t ItemTimer;
	
	for (int i = 0; i < 32; i++)
	{
		uint32_t Coin = 121;
		uint32_t Heart = 123;
		uint32_t Flower = 124;
		
		uint32_t tempItem;
		uint16_t tempItemAction;
		uint16_t tempBit;
		
		tempItem = *reinterpret_cast<uint32_t *>(FieldItemsAddress + 0x4);
		if ((tempItem == 0) || (tempItem == Coin) || (tempItem == Heart) || (tempItem == Flower))
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
	// uint32_t WindowColor = 0x000000C0;
	uint8_t Alpha 			= 0xFF;
	int32_t PosX 			= -232;
	int32_t PosY 			= -70;
	float Scale 			= 0.75;
	// int32_t WindowWidth 	= 440;
	// float WindowCurve 	= 0;
	
	// Move the text up if the input display is active
	if (Displays[BUTTON_INPUT_DISPLAY])
	{
		PosY += 20;
	}
	
	// Draw the text
	char *tempDisplayBuffer = DisplayBuffer;
	sprintf(tempDisplayBuffer,
		"PST: %lu\nItemTimer: %ld\nPhaEmY: %.2f\nParY: %.2f",
		PalaceSkip.MainTimer,
		ItemTimer,
		PhantomEmberPosY,
		PartnerPosY);
	
	// uint16_t LineLength = ttyd::fontmgr::FontGetMessageWidth(tempDisplayBuffer);
	// int32_t WindowWidth = 245 + (LineLength >> 1) - (LineLength % 10);
	
	/*drawTextWithWindow(tempDisplayBuffer, PosX, PosY, Alpha, TextColor, 
		Scale, WindowWidth, WindowColor, WindowCurve);*/
	
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
	PosX 					+= 113;
	PosY 					-= 14;
	
	const char *String = "Practice Codes v3.0.1\nCreated by Zephiles";
	drawText(String, PosX, PosY, Alpha, TextColor, Scale);
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

}