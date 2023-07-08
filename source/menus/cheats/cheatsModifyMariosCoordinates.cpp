#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "classes/window.h"
#include "classes/valueEditor.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "misc/utils.h"
#include "ttyd/mario.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>
#include <cmath>

const MenuOption gCheatsMenuModifyMariosCoordinatesOptions[] = {
    "Modify X Coordinate",
    cheatsMenuModifyMariosCoordinatesSetCoordinate,

    "Modify Y Coordinate",
    cheatsMenuModifyMariosCoordinatesSetCoordinate,

    "Modify Z Coordinate",
    cheatsMenuModifyMariosCoordinatesSetCoordinate,

    "Modify As Hex",
    cheatsMenuModifyMariosCoordinatesToggleFlag,
};

const MenuFunctions gCheatsMenuModifyMariosCoordinatesFuncs = {
    gCheatsMenuModifyMariosCoordinatesOptions,
    cheatsMenuModifyMariosCoordinatesControls,
    cheatsMenuModifyMariosCoordinatesDraw,
    nullptr, // Exit function not needed
};

void cheatsMenuModifyMariosCoordinatesInit(Menu *menuPtr)
{
    (void)menuPtr;

    constexpr uint32_t totalOptions = sizeof(gCheatsMenuModifyMariosCoordinatesOptions) / sizeof(MenuOption);
    enterNextMenu(&gCheatsMenuModifyMariosCoordinatesFuncs, totalOptions);
}

void cheatsMenuModifyMariosCoordinatesControls(Menu *menuPtr, MenuButtonInput button)
{
    // If the button combo editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(
            CheatsMenuModifyMariosCoordinates::CHEATS_MODIFY_MARIOS_COORDINATES_FLAG_CURRENTLY_MODIFYING_COORDINATE))
    {
        gCheatsMenu->getValueEditor()->controls(button);
        return;
    }

    // Use the default controls
    controlsBasicMenuLayout(menuPtr, button);
}

void CheatsMenu::drawModifyMariosCoordinatesInfo() const
{
    // Get the text position for the top-left of the window
    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXY(nullptr, WindowAlignment::TOP_LEFT, scale, &tempPosX, &tempPosY);

    // Get the text position a bit to the right of the last option
    float width;
    const float scale = this->scale;
    getTextWidthHeight("Modify As Hex", scale, &width, nullptr);

    const uint32_t totalOptions = gMenu->getTotalOptions();
    const float lineDecrement = LINE_HEIGHT_FLOAT * scale;

    // Set up the base text position to be two lines under the main text
    const float posXBase = tempPosX;
    float posYBase = tempPosY - (intToFloat(totalOptions + 1) * lineDecrement);

    const float hexTextPosX = posXBase + width + (20.f * scale);
    const float hexTextPosY = posYBase + (lineDecrement * 2.f);

    // Draw the flag for whether the coordinates are being modified as hex or not
    Cheats *cheatsPtr = gCheats;
    const char *string;
    uint32_t color;

    getOnOffTextAndColor(cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_MARIO_COORDINATES_MODIFY_AS_HEX),
                         &string,
                         &color,
                         0xFF);

    drawText(string, hexTextPosX, hexTextPosY, scale, color);

    // Draw Mario's coordinates
    Player *playerPtr = marioGetPtr();
    float *coordinatesPtr = reinterpret_cast<float *>(&playerPtr->playerPosition.x);

    char coordinateLetter = 'X';
    const char *coordinateText;
    char coordinateBuf[64];
    char mainBuf[sizeof(coordinateBuf) + 32];
    ValueType value;

    for (uint32_t i = 0; i < 3; i++, coordinateLetter++)
    {
        const float currentCoordinate = coordinatesPtr[i];
        value.f32 = currentCoordinate;

        // Make sure the value will not go offscreen
        if ((currentCoordinate >= MIN_FLOAT_VALUE) && (currentCoordinate <= MAX_FLOAT_VAlUE))
        {
            // Value will not go offscreen
            snprintf(coordinateBuf, sizeof(coordinateBuf), "%.6f", currentCoordinate);
            coordinateText = coordinateBuf;
        }
        else
        {
            // Value will go offscreen
            coordinateText = "Too Big";
        }

        // Get the actual text to draw
        snprintf(mainBuf, sizeof(mainBuf), "%c = %s, 0x%08" PRIX32, coordinateLetter, coordinateText, value.u32);
        drawText(mainBuf, posXBase, posYBase, scale, getColorWhite(0xFF));
        posYBase -= lineDecrement;
    }
}

void cheatsMenuModifyMariosCoordinatesDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    drawBasicMenuLayout(cameraId, user);

    // Draw the info for the Modify Mario's Coordinates cheat
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawModifyMariosCoordinatesInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditor();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the error window if applicable
    ErrorWindow *errorWindowPtr = cheatsMenuPtr->getErrorWindow();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void cheatsMenuModifyMariosCoordinatesToggleFlag(Menu *menuPtr)
{
    (void)menuPtr;

    cheatsMenuToggleEnabledFlag(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_MARIO_COORDINATES_MODIFY_AS_HEX);
}

void cheatsMenuCancelModifyMariosCoordinatesSetNewCoordinate()
{
    gCheatsMenu->getValueEditor()->stopDrawing();
    gMenu->clearFlag(CheatsMenuModifyMariosCoordinates::CHEATS_MODIFY_MARIOS_COORDINATES_FLAG_CURRENTLY_MODIFYING_COORDINATE);
}

void cheatsMenuModifyMariosCoordinatesSetNewCoordinate(const ValueType *valuePtr)
{
    Player *playerPtr = marioGetPtr();
    float *coordinatesPtr = reinterpret_cast<float *>(&playerPtr->playerPosition.x);

    coordinatesPtr[gMenu->getCurrentIndex()] = valuePtr->f32;

    // Close the value editor
    cheatsMenuCancelModifyMariosCoordinatesSetNewCoordinate();
}

void cheatsMenuModifyMariosCoordinatesSetCoordinate(Menu *menuPtr)
{
    // If the value is not being modified as hex, then make sure the number is one that can easily be edited
    Player *playerPtr = marioGetPtr();
    float *coordinatesPtr = reinterpret_cast<float *>(&playerPtr->playerPosition.x);

    const uint32_t currentIndex = menuPtr->getCurrentIndex();
    const Window *rootWindowPtr = gRootWindow;
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    bool modifyAsHex = true;

    if (!gCheats->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_MARIO_COORDINATES_MODIFY_AS_HEX))
    {
        modifyAsHex = false;

        const float currentValue = coordinatesPtr[currentIndex];
        switch (classifyFloat(currentValue))
        {
            case FP_ZERO:
            case FP_NORMAL:
            {
                // Make sure the value will not go offscreen
                if ((currentValue >= MIN_FLOAT_VALUE) && (currentValue <= MAX_FLOAT_VAlUE))
                {
                    // Value will not go offscreen
                    break;
                }

                [[fallthrough]];
            }
            default:
            {
                // Initialize the error window
                ErrorWindow *errorWindowPtr = cheatsMenuPtr->getErrorWindow();

                errorWindowPtr->setScale(cheatsMenuPtr->getScale());
                errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
                errorWindowPtr->setText("This value must be modified as hex.");

                errorWindowPtr->setTimer(3000);
                errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
                return;
            }
        }
    }

    // Bring up the window for selecting an id
    menuPtr->setFlag(CheatsMenuModifyMariosCoordinates::CHEATS_MODIFY_MARIOS_COORDINATES_FLAG_CURRENTLY_MODIFYING_COORDINATE);

    // Initialize the value editor
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditor();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::VALUE_IS_SIGNED);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Y_SET_MAX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_BUTTON_Z_SET_MIN);

    if (modifyAsHex)
    {
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
    }

    valueEditorPtr->init(&coordinatesPtr[currentIndex],
                         nullptr,
                         nullptr,
                         rootWindowPtr,
                         flags,
                         VariableType::f32,
                         rootWindowPtr->getAlpha(),
                         cheatsMenuPtr->getScale());

    valueEditorPtr->startDrawing(cheatsMenuModifyMariosCoordinatesSetNewCoordinate,
                                 cheatsMenuCancelModifyMariosCoordinatesSetNewCoordinate);
}
