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
    gCheatsMenuTextTurnOnOff,
    cheatsMenuForceNpcItemDropToggleFlag,

    "Change Item By Id",
    cheatsMenuForceNpcItemDropSetItem,
};

const MenuFunctions gCheatsMenuForceNpcItemDropFuncs = {
    gCheatsMenuForceNpcItemDropOptions,
    cheatsMenuForceNpcItemDropControls,
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

void cheatsMenuForceNpcItemDropControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CheatsMenuForceNpcItemDrop::CHEATS_FORCE_NPC_ITEM_DROP_FLAG_CURRENTLY_SELECTING_ID))
    {
        gCheatsMenu->getValueEditor()->controls(button);
        return;
    }

    // Use the default controls
    controlsBasicMenuLayout(menuPtr, button);
}

void CheatsMenu::drawForceNpcItemDropInfo() const
{
    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    const float scale = this->scale;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    // Draw the current cheat with its flag
    drawText(gCheatsMenuInitOptions[this->selectedCheat].name, posX, posY, scale, getColorWhite(0xFF));

    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;
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

    const float iconAdjustment = ICON_SIZE_FLOAT * scale;
    const float iconPosX = posX + (iconAdjustment / 2.f);
    const float iconPosY = posY - (iconAdjustment + (7.f * scale));
    drawItemIconWithText(iconPosX, iconPosY, iconScale, scale, 0.f, item, getColorWhite(0xFF));
}

void cheatsMenuForceNpcItemDropDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    // Draw the info for the Force NPC Item Drop cheat
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawForceNpcItemDropInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditor();
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

void cheatsMenuCancelForceNpcItemDropSetNewItem()
{
    gCheatsMenu->getValueEditor()->stopDrawing();
    gMenu->clearFlag(CheatsMenuForceNpcItemDrop::CHEATS_FORCE_NPC_ITEM_DROP_FLAG_CURRENTLY_SELECTING_ID);
}

void cheatsMenuForceNpcItemDropSetNewItem(const ValueType *valuePtr)
{
    gCheats->getForceNpcItemDropCheatPtr()->setItemDrop(static_cast<ItemId>(valuePtr->s32));

    // Close the value editor
    cheatsMenuCancelForceNpcItemDropSetNewItem();
}

void cheatsMenuForceNpcItemDropSetItem(Menu *menuPtr)
{
    // Bring up the window for selecting an id
    menuPtr->setFlag(CheatsMenuForceNpcItemDrop::CHEATS_FORCE_NPC_ITEM_DROP_FLAG_CURRENTLY_SELECTING_ID);

    // Initialize the value editor
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditor();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::CHEATS_CHANGE_DROPPED_ITEM);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_ITEM_ICON_AND_TEXT);

    const int32_t minValue = static_cast<int32_t>(ItemId::ITEM_GOLD_BAR);
    const int32_t maxValue = static_cast<int32_t>(ItemId::ITEM_SUPER_CHARGE_P);
    const int32_t currentValue = static_cast<int32_t>(gCheats->getForceNpcItemDropCheatPtr()->getItemDrop());

    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(&currentValue,
                         &minValue,
                         &maxValue,
                         rootWindowPtr,
                         flags,
                         VariableType::s16,
                         rootWindowPtr->getAlpha(),
                         cheatsMenuPtr->getScale());

    valueEditorPtr->startDrawing(cheatsMenuForceNpcItemDropSetNewItem, cheatsMenuCancelForceNpcItemDropSetNewItem);
}
