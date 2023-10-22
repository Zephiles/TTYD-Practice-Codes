#include "mod.h"
#include "menuUtils.h"
#include "drawText.h"
#include "classes/yoshiColorSelector.h"
#include "menus/statsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/party.h"
#include "ttyd/mario_pouch.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

static void mainControls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void selectedOptionSelectPartner(Menu *menuPtr);

static const PartyMembers gPartnersIds[] = {
    PartyMembers::kGoombella,
    PartyMembers::kKoops,
    PartyMembers::kFlurrie,
    PartyMembers::kYoshi,
    PartyMembers::kVivian,
    PartyMembers::kBobbery,
    PartyMembers::kMsMowz,
};

static const uint32_t gYoshiColors[] = {
    0x29AD21FF, // Green
    0xE8385AFF, // Red
    0x8E7DDFFF, // Blue
    0xF47529FF, // Orange
    0xF58BB6FF, // Pink
    0x635D63FF, // Black
    0xC6E1ECFF, // White
};

static const char *gStatsStrings[] = {
    "HP",
    "Max HP",
    "Rank",
    "Toggle",
};

static const char *gYoshiStatsStrings[] = {
    "Color",
    "Name",
};

static const MenuOption gOptions[] = {
    "Goombella",
    selectedOptionSelectPartner,

    "Koops",
    selectedOptionSelectPartner,

    "Flurrie",
    selectedOptionSelectPartner,

    "Yoshi",
    selectedOptionSelectPartner,

    "Vivian",
    selectedOptionSelectPartner,

    "Bobbery",
    selectedOptionSelectPartner,

    "Ms. Mowz",
    selectedOptionSelectPartner,
};

static const MenuFunctions gFuncs = {
    gOptions,
    mainControls,
    draw,
    nullptr, // Exit function not needed
};

void statsMenuPartnersInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void selectedOptionSelectPartner(Menu *menuPtr)
{
    // Set the flag for a partner being selected
    menuPtr->setFlag(StatsFlagPartner::STATS_FLAG_PARTNER_SELECTED_PARTNER);

    // Reset currentIndex
    gStatsMenu->setCurrentIndex(0);
}

static uint32_t getTotalOptions(uint32_t currentIndex)
{
    uint32_t totalOptions = StatsPartnersCurrentPartnerOption::STATS_PARTNER_BRING_OUT_OR_REMOVE + 1;
    if (STATS_PARTNERS_DRAWING_YOSHI_STATS(currentIndex))
    {
        totalOptions += 2; // Add 2 for Yoshi's color and name
    }
    return totalOptions;
}

static void verifyMenuAndCurrentIndexes()
{
    Menu *menuPtr = gMenu;
    uint32_t menuCurrentIndex = menuPtr->getCurrentIndex();

    // Make sure the menu current index is valid
    if (menuCurrentIndex >= menuPtr->getTotalOptions())
    {
        // Reset the menu current index to 0
        menuCurrentIndex = 0;
        menuPtr->setCurrentIndex(menuCurrentIndex);
    }

    // Make sure the current index is valid
    StatsMenu *statsMenuPtr = gStatsMenu;
    const uint32_t currentIndex = statsMenuPtr->getCurrentIndex();
    const uint32_t totalOptions = getTotalOptions(menuCurrentIndex);

    if (currentIndex >= totalOptions)
    {
        // Reset the current index to 0
        statsMenuPtr->setCurrentIndex(0);
    }
}

static PouchPartyData *getCurrentPartnerData()
{
    // Make sure both of the current indexes are valid
    verifyMenuAndCurrentIndexes();

    const uint32_t currentIndex = gMenu->getCurrentIndex();
    return &pouchGetPtr()->partyData[static_cast<uint32_t>(gPartnersIds[currentIndex])];
}

static void changeValue(const ValueType *valuePtr)
{
    const int32_t value = valuePtr->s32;
    PouchPartyData *partnerData = getCurrentPartnerData();
    const uint32_t currentIndex = gStatsMenu->getCurrentIndex();

    // Set the new value
    switch (currentIndex)
    {
        case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_CURRENT_HP:
        {
            partnerData->currentHp = static_cast<int16_t>(value);
            break;
        }
        case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_MAX_HP:
        {
            partnerData->maxHp = static_cast<int16_t>(value);

            // Reset the value for entering battles
            partnerData->baseMaxHp = static_cast<int16_t>(value);
            break;
        }
        case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_RANK:
        {
            partnerData->attackLevel = static_cast<int16_t>(value);

            // Set the rank for battles
            partnerData->techLevel = static_cast<int16_t>(value);

            // Set the rank used to verify the partner's stats when equipping/unequipping badges
            partnerData->hpLevel = static_cast<int16_t>(value);
            break;
        }
        default:
        {
            break;
        }
    }

    // The cache for the selected option will need to be cleared before a battle starts
    switch (currentIndex)
    {
        case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_CURRENT_HP:
        case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_MAX_HP:
        case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_RANK:
        {
            // Only set the flag for clearing the cache if not in a battle
            if (!checkForSpecificSeq(SeqIndex::kBattle))
            {
                gMod->setFlag(ModFlag::MOD_FLAG_CLEAR_PARTNER_STATS);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    // Close the value editor
    statsMenuCancelChangingValue();
}

static void setValueById(int32_t currentValue, int32_t minValue, int32_t maxValue)
{
    // Initialize the value editor
    StatsMenu *statsMenuPtr = gStatsMenu;
    ValueEditor *valueEditorPtr = statsMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr
        ->init(&currentValue, &minValue, &maxValue, rootWindowPtr, flags, VariableType::s16, rootWindowPtr->getAlpha());

    valueEditorPtr->startDrawing(changeValue, statsMenuCancelChangingValue);
}

static void cancelChangeYoshiName()
{
    gStatsMenu->getNameEditorPtr()->stopDrawing();
}

static void changeYoshiName(char *newNamePtr, uint32_t newNameSize)
{
    (void)newNamePtr;
    (void)newNameSize;

    // Close the name editor
    cancelChangeYoshiName();
}

static void cancelChangeYoshiColor()
{
    gStatsMenu->getYoshiColorSelectorPtr()->stopDrawing();
}

static void changeYoshiColor(uint32_t selectedColorId)
{
    // Set the new color
    pouchSetPartyColor(PartyMembers::kYoshi, selectedColorId);

    // The Partners menu in the pause menu will not visually update when Yoshi's color is changed, so manually reset the
    // Partners menu
    resetPauseMenuPartners();

    // Close the color selector
    cancelChangeYoshiColor();
}

static void bringOutOrRemoveFromOverworld(Menu *menuPtr)
{
    const PartyMembers currentPartnerOut = getCurrentPartnerOrFollowerOut(true);

    StatsMenu *statsMenuPtr = gStatsMenu;
    const uint32_t currentIndex = menuPtr->getCurrentIndex();
    const PartyMembers currentPartnerInMenu = gPartnersIds[currentIndex];

    if (currentPartnerOut == currentPartnerInMenu)
    {
        // Remove the partner from the overworld
        removePartnerFromOverworld();
    }
    else
    {
        // Make sure a partner can actually be spawned right now
        if (checkIfInGame())
        {
            // Spawn the partner
            spawnPartnerOrFollower(currentPartnerInMenu);
        }
        else
        {
            // Initialize the error window
            statsMenuPtr->initErrorWindow(true);
        }
    }
}

static void selectedPartnerDPadControls(MenuButtonInput button, uint8_t *currentIndexPtr, uint32_t menuCurrentIndex)
{
    const uint32_t totalOptions = getTotalOptions(menuCurrentIndex);
    menuControlsVertical(button, currentIndexPtr, nullptr, totalOptions, totalOptions, 1, true);
}

static void selectedPartnerControls(Menu *menuPtr, MenuButtonInput button)
{
    // Make sure the menu current index and the current index are valid
    verifyMenuAndCurrentIndexes();

    // The function for checking for auto-incrementing needs to run every frame to be handled correctly
    StatsMenu *statsMenuPtr = gStatsMenu;
    const bool autoIncrement = handleMenuAutoIncrement(statsMenuPtr->getAutoIncrementPtr());

    uint8_t *statsMenuCurrentIndexPtr = statsMenuPtr->getCurrentIndexPtr();
    const uint32_t menuCurrentIndex = menuPtr->getCurrentIndex();

    // Handle held button input if auto-incrementing should be done
    if (autoIncrement)
    {
        const MenuButtonInput buttonHeld = getMenuButtonInput(false);
        switch (buttonHeld)
        {
            case MenuButtonInput::DPAD_DOWN:
            case MenuButtonInput::DPAD_UP:
            {
                selectedPartnerDPadControls(buttonHeld, statsMenuCurrentIndexPtr, menuCurrentIndex);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    const uint32_t currentIndex = statsMenuPtr->getCurrentIndex();

    // Handle the button input pressed this frame
    switch (button)
    {
        case MenuButtonInput::DPAD_DOWN:
        case MenuButtonInput::DPAD_UP:
        {
            selectedPartnerDPadControls(button, statsMenuCurrentIndexPtr, menuCurrentIndex);
            break;
        }
        case MenuButtonInput::A:
        {
            bool handledYoshiOption = false;
            if (STATS_PARTNERS_DRAWING_YOSHI_STATS(menuCurrentIndex))
            {
                // Handle options specific to Yoshi
                switch (currentIndex)
                {
                    case StatsPartnersCurrentPartnerOptionYoshi::STATS_PARTNER_YOSHI_SET_COLOR:
                    {
                        // Initialize the color selector
                        const Window *rootWindowPtr = gRootWindow;
                        YoshiColorSelector *yoshiColorSelectorPtr = statsMenuPtr->getYoshiColorSelectorPtr();
                        yoshiColorSelectorPtr->init(rootWindowPtr, rootWindowPtr->getAlpha());

                        // Get the current index for the color selector
                        const uint32_t yoshiColorIndex = pouchGetPartyColor(PartyMembers::kYoshi);

                        // Make sure the index is valid
                        if (yoshiColorIndex < TOTAL_YOSHI_COLORS)
                        {
                            yoshiColorSelectorPtr->setCurrentIndex(yoshiColorIndex);
                        }

                        yoshiColorSelectorPtr->startDrawing(changeYoshiColor, cancelChangeYoshiColor);
                        handledYoshiOption = true;
                        break;
                    }
                    case StatsPartnersCurrentPartnerOptionYoshi::STATS_PARTNER_YOSHI_SET_NAME:
                    {
                        // Initialize the name editor
                        char *yoshiNamePtr = pouchGetPtr()->yoshiName;
                        const Window *rootWindowPtr = gRootWindow;
                        NameEditor *nameEditorPtr = statsMenuPtr->getNameEditorPtr();

                        nameEditorPtr->init(rootWindowPtr,
                                            yoshiNamePtr,
                                            yoshiNamePtr,
                                            sizeof(PouchData::yoshiName),
                                            true,
                                            rootWindowPtr->getAlpha());

                        nameEditorPtr->startDrawing(changeYoshiName, cancelChangeYoshiName);
                        handledYoshiOption = true;
                        break;
                    }
                    case StatsPartnersCurrentPartnerOptionYoshi::STATS_PARTNER_YOSHI_BRING_OUT_OR_REMOVE:
                    {
                        bringOutOrRemoveFromOverworld(menuPtr);
                        handledYoshiOption = true;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }

            // Handle options for all partners
            if (!handledYoshiOption)
            {
                PouchPartyData *partnerData = getCurrentPartnerData();
                switch (currentIndex)
                {
                    case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_CURRENT_HP:
                    {
                        const int32_t maxHp = partnerData->maxHp; // Make sure the current HP does not exceed the max HP
                        setValueById(partnerData->currentHp, 0, maxHp);
                        break;
                    }
                    case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_MAX_HP:
                    {
                        setValueById(partnerData->maxHp, 0, 999);
                        break;
                    }
                    case StatsPartnersCurrentPartnerOption::STATS_PARTNER_SET_RANK:
                    {
                        setValueById(partnerData->attackLevel, 0, 2);
                        break;
                    }
                    case StatsPartnersCurrentPartnerOption::STATS_PARTNER_TOGGLE_ENABLED_BOOL:
                    {
                        // Toggle the enabled bit
                        partnerData->flags ^= 1U;

                        // The Partners menu in the pause menu will not visually update when a partner is added/removed, so
                        // manually reset the Partners menu
                        resetPauseMenuPartners();
                        break;
                    }
                    case StatsPartnersCurrentPartnerOption::STATS_PARTNER_BRING_OUT_OR_REMOVE:
                    {
                        bringOutOrRemoveFromOverworld(menuPtr);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            break;
        }
        case MenuButtonInput::B:
        {
            // Go back to selecting a partner
            menuPtr->clearFlag(StatsFlagPartner::STATS_FLAG_PARTNER_SELECTED_PARTNER);
            break;
        }
        default:
        {
            break;
        }
    }
}

static void mainControls(Menu *menuPtr, MenuButtonInput button)
{
    StatsMenu *statsMenuPtr = gStatsMenu;

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = statsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // If the window for selecting a Yoshi color is open, then handle the controls for that
    YoshiColorSelector *yoshiColorSelectorPtr = statsMenuPtr->getYoshiColorSelectorPtr();
    if (yoshiColorSelectorPtr->shouldDraw())
    {
        yoshiColorSelectorPtr->controls(button);
        return;
    }

    // If the window for changing Yoshi's name is open, then handle the controls for that
    NameEditor *nameEditorPtr = statsMenuPtr->getNameEditorPtr();
    if (nameEditorPtr->shouldDraw())
    {
        nameEditorPtr->controls(button);
        return;
    }

    // If a partner was selected and neither of the previous windows are open, then handle the controls for the current partner
    if (menuPtr->flagIsSet(StatsFlagPartner::STATS_FLAG_PARTNER_SELECTED_PARTNER))
    {
        selectedPartnerControls(menuPtr, button);
        return;
    }

    // No flags are set, so use default controls
    basicMenuLayoutControls(menuPtr, button);
}

static void drawPartnerStats()
{
    auto getTextColor = [](bool anyFlagIsSet, bool currentOption)
    {
        if (!anyFlagIsSet)
        {
            return getColorWhite(0xFF);
        }

        return getCurrentOptionColor(currentOption, 0xFF);
    };

    // Make sure the menu current index and the current index are valid
    verifyMenuAndCurrentIndexes();

    // Set the text to be a bit to the right of the main text
    float tempPosX;
    float tempPosY;
    constexpr float scale = MENU_SCALE;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Get the width that is being used by the main text options
    float textWidth = getTextWidth("Goombella", scale);

    // Retrieve posXBase and posYBase as separate variables to avoid repeatedly loading them from the stack when using them
    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    constexpr float textIncrement = lineDecrement + (lineDecrement / 2.f) - LINE_HEIGHT_ADJUSTMENT_5(scale);
    const float posXBase = tempPosX + textWidth + textIncrement;
    const float posYBase = tempPosY;

    float posX = posXBase;
    float posY = posYBase;

    // Draw the first 4 options, as they will always be in the same order
    Menu *menuPtr = gMenu;
    const bool anyFlagIsSet = menuPtr->anyFlagIsSet();
    uint32_t menuCurrentIndex = menuPtr->getCurrentIndex();
    uint32_t currentIndex = gStatsMenu->getCurrentIndex();

    const char **statsStringsPtr = gStatsStrings;
    uint32_t counter = 0;
    uint32_t color;

    for (uint32_t i = 0; i < 4; i++, counter++)
    {
        color = getTextColor(anyFlagIsSet, currentIndex == counter);
        drawText(statsStringsPtr[i], posX, posY, scale, color);
        posY -= lineDecrement;
    }

    // If Yoshi's stats are being drawn, then draw the color and name strings
    const bool drawingYoshiStats = STATS_PARTNERS_DRAWING_YOSHI_STATS(menuCurrentIndex);
    if (drawingYoshiStats)
    {
        const char **yoshiStatsStringsPtr = gYoshiStatsStrings;
        for (uint32_t i = 0; i < 2; i++, counter++)
        {
            color = getTextColor(anyFlagIsSet, currentIndex == counter);
            drawText(yoshiStatsStringsPtr[i], posX, posY, scale, color);
            posY -= lineDecrement;
        }
    }

    // Draw the text for either bringing out the current partner or removing them from the overworld
    const PartyMembers currentPartnerOut = getCurrentPartnerOrFollowerOut(true);
    const PartyMembers currentPartnerInMenu = gPartnersIds[menuCurrentIndex];
    const char *optionText;

    if (currentPartnerOut == currentPartnerInMenu)
    {
        optionText = "Remove From Overworld";
    }
    else
    {
        optionText = "Bring Out";
    }

    color = getTextColor(anyFlagIsSet, currentIndex == counter);
    drawText(optionText, posX, posY, scale, color);

    // If no partner is currently out, then draw the text stating so
    if (currentPartnerOut == PartyMembers::kNone)
    {
        // Move two lines down
        posY -= lineDecrement * 2.f;

        drawText("No partner is currently out", posX, posY, scale, getColorWhite(0xFF));
    }

    // Draw the first 4 values, as they will always be in the same order
    // Get the HP, Max HP, and Rank first, as they can all be drawn in the same loop
    const PouchPartyData *partnerData = getCurrentPartnerData();
    int32_t partnerStats[3];

    partnerStats[0] = partnerData->currentHp;
    partnerStats[1] = partnerData->maxHp;
    partnerStats[2] = partnerData->attackLevel; // Rank

    // Set the values text to be a bit to the right of the previous text
    textWidth = getTextWidth("Bring Out", scale);
    posX = posXBase + textWidth + textIncrement;
    posY = posYBase;
    counter = 0;

    // Draw the first 3 values
    char buf[16];
    for (uint32_t i = 0; i < 3; i++, counter++)
    {
        snprintf(buf, sizeof(buf), "%" PRId32, partnerStats[i]);
        drawText(buf, posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;
    }

    // Draw the on or off text for Toggle
    const char *onOrOffText;
    getOnOffTextAndColor(partnerData->flags & 1U, &onOrOffText, &color, 0xFF);
    drawText(onOrOffText, posX, posY, scale, color);
    posY -= lineDecrement;

    // If Yoshi's stats are being drawn, then draw the color and name
    if (drawingYoshiStats)
    {
        // Draw the color
        uint32_t yoshiColorIndex = pouchGetPartyColor(PartyMembers::kYoshi);

        // Make sure the index is valid
        if (yoshiColorIndex >= TOTAL_YOSHI_COLORS)
        {
            yoshiColorIndex = 0;
        }

        const char *yoshiColorText = gYoshiColorsStrings[yoshiColorIndex];
        const uint32_t yoshiColor = gYoshiColors[yoshiColorIndex];

        drawText(yoshiColorText, posX, posY, scale, yoshiColor);
        posY -= lineDecrement;

        // Draw the name
        drawText(pouchGetYoshiName(), posX, posY, scale, getColorWhite(0xFF));
    }
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the partner's stats that the cursor is currently over
    drawPartnerStats();

    // Draw the value editor if applicable
    StatsMenu *statsMenuPtr = gStatsMenu;
    ValueEditor *valueEditorPtr = statsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the color selector if applicable
    YoshiColorSelector *yoshiColorSelectorPtr = statsMenuPtr->getYoshiColorSelectorPtr();
    if (yoshiColorSelectorPtr->shouldDraw())
    {
        yoshiColorSelectorPtr->draw();
    }

    // Draw the name editor if applicable
    NameEditor *nameEditorPtr = statsMenuPtr->getNameEditorPtr();
    if (nameEditorPtr->shouldDraw())
    {
        nameEditorPtr->draw();
    }

    // Draw the error message if applicable
    ErrorWindow *errorWindowPtr = statsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}
