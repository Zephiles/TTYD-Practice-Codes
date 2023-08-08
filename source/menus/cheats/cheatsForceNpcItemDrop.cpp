#include "menuUtils.h"
#include "drawText.h"
#include "drawIcon.h"
#include "cheats.h"
#include "mod.h"
#include "classes/window.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"
#include "ttyd/item_data.h"

#include <cstdint>

const MenuOption gCheatsMenuForceNpcItemDropOptions[] = {
    "Turn On/Off",
    cheatsMenuForceNpcItemDropToggleFlag,

    "Change Item By Id",
    cheatsMenuForceNpcItemDropSetItem,
};

const MenuFunctions gCheatsMenuForceNpcItemDropFuncs = {
    gCheatsMenuForceNpcItemDropOptions,
    cheatsMenuDefaultControlsWithValueEditor,
    cheatsMenuForceNpcItemDropDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuForceNpcItemDropInit(Menu *menuPtr)
{
    // Backup the selected cheat
    gCheatsMenu->setSelectedCheat(menuPtr->getCurrentIndex());

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuForceNpcItemDropOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuForceNpcItemDropFuncs, totalOptions);
}

void CheatsMenu::drawForceNpcItemDropInfo() const
{
    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the current cheat with its flag
    drawText(gCheatsMenuInitOptions[this->selectedCheat].name, posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= lineDecrement;

    Cheats *cheatsPtr = gCheats;
    const char *string;
    uint32_t color;

    getOnOffTextAndColor(cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FORCE_NPC_ITEM_DROP),
                         &string,
                         &color,
                         0xFF);

    drawText(string, posX, posY, scale, color);
    posY -= (lineDecrement * 2.f);

    // Draw the current item
    drawText("Current Item", posX, posY, scale, getColorWhite(0xFF));
    posY -= lineDecrement;

    // Several items need to have their icon scales adjusted
    const ItemId item = cheatsPtr->getForceNpcItemDropCheatPtr()->getItemDrop();
    const float iconScale = adjustItemIconScale(item, scale);

    constexpr float iconAdjustment = ICON_SIZE_FLOAT * scale;
    const float iconPosX = posX + (iconAdjustment / 2.f);
    const float iconPosY = posY - (iconAdjustment + (7.f * scale));
    drawItemIconWithText(iconPosX, iconPosY, iconScale, scale, 0.f, item, getColorWhite(0xFF));
}

void cheatsMenuForceNpcItemDropDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Force NPC Item Drop cheat
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawForceNpcItemDropInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }
}

void cheatsMenuForceNpcItemDropToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_FORCE_NPC_ITEM_DROP);
}

void cheatsMenuForceNpcItemDropSetNewItem(const ValueType *valuePtr)
{
    gCheats->getForceNpcItemDropCheatPtr()->setItemDrop(static_cast<ItemId>(valuePtr->s32));

    // Close the value editor
    cheatsMenuValueEditorCancelSetValue();
}

void cheatsMenuForceNpcItemDropSetItem(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the value editor
    constexpr uint32_t minValue = static_cast<int32_t>(ItemId::ITEM_GOLD_BAR);
    constexpr uint32_t maxValue = static_cast<int32_t>(ItemId::ITEM_SUPER_CHARGE_P);
    const uint32_t currentValue = static_cast<uint32_t>(gCheats->getForceNpcItemDropCheatPtr()->getItemDrop());

    ValueEditor *valueEditorPtr = gCheatsMenu->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::CHEATS_CHANGE_DROPPED_ITEM);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);

    cheatsMenuInitValueEditor(currentValue,
                              minValue,
                              maxValue,
                              flags,
                              VariableType::s16,
                              true,
                              cheatsMenuForceNpcItemDropSetNewItem);
}
