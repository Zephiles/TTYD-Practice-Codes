#include "menuUtils.h"
#include "drawText.h"
#include "cheats.h"
#include "mod.h"
#include "classes/window.h"
#include "classes/valueEditor.h"
#include "menus/cheatsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/mario.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

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
    // If the value editor is open, then handle the controls for that
    if (menuPtr->flagIsSet(CHEATS_MENU_USING_VALUE_EDITOR_FLAG))
    {
        gCheatsMenu->getValueEditorPtr()->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void CheatsMenu::drawModifyMariosCoordinatesInfo() const
{
    // Get the text position a bit to the right of the last option
    float width;
    constexpr float scale = MENU_SCALE;
    getTextWidthHeight("Modify As Hex", scale, &width, nullptr);

    // Get the text position for the top-left of the window two lines under the main text
    float tempPosX;
    float tempPosY;
    const uint32_t totalOptions = gMenu->getTotalOptions();
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    const float posX = tempPosX;
    float posY = tempPosY;

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    const float hexTextPosX = posX + width + (20.f * scale);
    const float hexTextPosY = posY + (lineDecrement * 2.f);

    // Draw the flag for whether the coordinates are being modified as hex or not
    Cheats *cheatsPtr = gCheats;
    const char *string;
    uint32_t color;

    getYesNoTextAndColor(cheatsPtr->enabledFlagIsSet(CheatsEnabledFlag::CHEATS_ENABLED_FLAG_MARIO_COORDINATES_MODIFY_AS_HEX),
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
        drawText(mainBuf, posX, posY, scale, getColorWhite(0xFF));
        posY -= lineDecrement;
    }
}

void cheatsMenuModifyMariosCoordinatesDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the Modify Mario's Coordinates cheat
    CheatsMenu *cheatsMenuPtr = gCheatsMenu;
    cheatsMenuPtr->drawModifyMariosCoordinatesInfo();

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->draw();
    }

    // Draw the error window if applicable
    ErrorWindow *errorWindowPtr = cheatsMenuPtr->getErrorWindowPtr();
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

void cheatsMenuModifyMariosCoordinatesSetNewCoordinate(const ValueType *valuePtr)
{
    Player *playerPtr = marioGetPtr();
    float *coordinatesPtr = reinterpret_cast<float *>(&playerPtr->playerPosition.x);

    coordinatesPtr[gMenu->getCurrentIndex()] = valuePtr->f32;

    // Close the value editor
    cheatsMenuValueEditorCancelSetValue();
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
        // Make sure that the current value can be worked with, and that it will not go offscreen
        const float currentValue = coordinatesPtr[currentIndex];
        if (floatCanBeWorkedWith(currentValue) && (currentValue >= MIN_FLOAT_VALUE) && (currentValue <= MAX_FLOAT_VAlUE))
        {
            // Value will not go offscreen
            modifyAsHex = false;
        }
        else
        {
            // Initialize the error window
            ErrorWindow *errorWindowPtr = cheatsMenuPtr->getErrorWindowPtr();

            errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
            errorWindowPtr->setText("This value must be modified as hex.");

            errorWindowPtr->setTimer(3000);
            errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
            return;
        }
    }

    // Initialize the value editor
    ValueEditor *valueEditorPtr = cheatsMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::VALUE_IS_SIGNED);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);

    if (modifyAsHex)
    {
        flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
    }

    // Dirty trick to pass in the float as a u32
    ValueType value;
    value.f32 = coordinatesPtr[currentIndex];

    cheatsMenuInitValueEditor(menuPtr,
                              value.u32,
                              0,
                              0,
                              flags,
                              VariableType::f32,
                              false,
                              cheatsMenuModifyMariosCoordinatesSetNewCoordinate);
}
