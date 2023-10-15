#include "menuUtils.h"
#include "cxx.h"
#include "drawText.h"
#include "mod.h"
#include "settings.h"
#include "gc/card.h"
#include "classes/window.h"
#include "menus/settingsMenu.h"
#include "menus/rootMenu.h"
#include "ttyd/camdrv.h"

#include <cstdint>
#include <cstdio>
#include <cinttypes>

const char *gSettingsFileNotFoundText = "The Settings file was not found on the memory card.";
const char *gSettingsMemoryCardInUseText = "The memory card is currently already in use.";

const char *gSettingsFileInvalidVersionText =
    "The Settings file's version does not support\nthis version of the Practice Codes";

SettingsMenu *gSettingsMenu = nullptr;

const MenuOption gSettingssMenuInitOptions[] = {
    "Change Memory Card Slot",
    settingsMenuToggleMemoryCardSlot,

    "Change Window Color",
    settingsMenuSelectedChangeWindowColor,

    "Delete Settings File",
    settingsMenuSelectedDeleteSettingsFile,

    "Load Settings",
    settingsMenuSelectedLoadSettingsFile,

    "Save Settings",
    settingsMenuSelectedSaveSettingsFile,
};

const MenuFunctions gSettingssMenuInitFuncs = {
    gSettingssMenuInitOptions,
    settingsMenuInitControls,
    settingsMenuInitDraw,
    settingsMenuInitExit,
};

void settingsMenuInit(Menu *menuPtr)
{
    (void)menuPtr;

    // Failsafe: Make sure memory isn't already allocated for gSettingsMenu
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    if (settingsMenuPtr)
    {
        delete settingsMenuPtr;
    }

    settingsMenuPtr = new SettingsMenu;
    gSettingsMenu = settingsMenuPtr;

    // Backup the current window color
    settingsMenuPtr->setRootWindowBackupColor(gRootWindow->getColor());

    // Enter the menu
    constexpr uint32_t totalOptions = sizeof(gSettingssMenuInitOptions) / sizeof(MenuOption);
    enterNextMenu(&gSettingssMenuInitFuncs, totalOptions);
}

void settingsMenuInitControls(Menu *menuPtr, MenuButtonInput button)
{
    SettingsMenu *settingsMenuPtr = gSettingsMenu;

    // If the confirmation window is open, then handle the controls for that
    ConfirmationWindow *confirmationWindowPtr = settingsMenuPtr->getConfirmationWindowPtr();
    if (confirmationWindowPtr->shouldDraw())
    {
        confirmationWindowPtr->controls(button);
        return;
    }

    // If the value editor is open, then handle the controls for that
    ValueEditor *valueEditorPtr = settingsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        valueEditorPtr->controls(button);
        return;
    }

    // Use the default controls
    basicMenuLayoutControls(menuPtr, button);
}

void SettingsMenu::drawSettingsMenuInfo() const
{
    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Draw the memory card slot being used
    char cardSlot;
    if (this->memoryCardSlot == CARD_SLOT_A)
    {
        cardSlot = 'A';
    }
    else
    {
        cardSlot = 'B';
    }

    char buf[32];
    constexpr uint32_t butSize = sizeof(buf);
    snprintf(buf, butSize, "Memory Card Slot\nSlot %c", cardSlot);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (lineDecrement * 3.f);

    // Draw the current window color
    snprintf(buf, butSize, "Window Color\n0x%08" PRIX32, this->rootWindowBackupColor);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

void settingsMenuInitDraw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the settings menu
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    settingsMenuPtr->drawSettingsMenuInfo();

    // Draw the confirmation window if applicable
    ConfirmationWindow *confirmationWindowPtr = settingsMenuPtr->getConfirmationWindowPtr();
    if (confirmationWindowPtr->shouldDraw())
    {
        confirmationWindowPtr->draw();
    }

    // Draw the value editor if applicable
    ValueEditor *valueEditorPtr = settingsMenuPtr->getValueEditorPtr();
    if (valueEditorPtr->shouldDraw())
    {
        // Update gRootWindow's color and the value editor's window alpha based on the value editor's current value
        ValueType value;
        if (valueEditorPtr->getValueFromString(&value))
        {
            const uint32_t color = value.u32;
            gRootWindow->setColor(color);
            valueEditorPtr->setWindowAlpha(color & 0xFF);
        }

        valueEditorPtr->draw();
    }

    // Draw an error message if applicable
    ErrorWindow *errorWindowPtr = settingsMenuPtr->getErrorWindowPtr();
    if (errorWindowPtr->shouldDraw())
    {
        errorWindowPtr->draw();
    }
}

void settingsMenuInitExit()
{
    // The player may have manually closed the menu via L + Start, so must restore the window color here in case the player
    // closed the menu while changing the window color
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    gRootWindow->setColor(settingsMenuPtr->getRootWindowBackupColor());

    delete settingsMenuPtr;
    gSettingsMenu = nullptr;
}

void settingsMenuToggleMemoryCardSlot(Menu *menuPtr)
{
    (void)menuPtr;

    gSettingsMenu->toggleMemoryCardSlot();
}

void settingsMenuCloseConfirmationWindow()
{
    gSettingsMenu->getConfirmationWindowPtr()->stopDrawing();
}

void settingsMenuInitConfirmationWindow(const char *helpTextPtr, ConfirmationWindowSelectedOptionFunc selectedOptionFunc)
{
    // Initialize the confirmation window
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    ConfirmationWindow *confirmationWindowPtr = settingsMenuPtr->getConfirmationWindowPtr();

    const Window *rootWindowPtr = gRootWindow;

    confirmationWindowPtr->init(rootWindowPtr, helpTextPtr, rootWindowPtr->getAlpha());
    confirmationWindowPtr->startDrawing(selectedOptionFunc);
}

void settingsMenuDeleteSettingsFile(bool selectedYes)
{
    if (selectedYes)
    {
        SettingsMenu *settingsMenuPtr = gSettingsMenu;
        switch (deleteSettingsFile(settingsMenuPtr->getMemoryCardSlot()))
        {
            case CARD_RESULT_CARD_IN_USE:
            {
                settingsMenuPtr->initErrorWindow(gSettingsMemoryCardInUseText);
                break;
            }
            case CARD_RESULT_NOFILE:
            {
                settingsMenuPtr->initErrorWindow(gSettingsFileNotFoundText);
                break;
            }
            case CARD_RESULT_READY:
            {
                settingsMenuPtr->initErrorWindow("Delete Successful");
                break;
            }
            default:
            {
                settingsMenuPtr->initErrorWindow("Delete Failed");
                break;
            }
        }
    }

    // Close the confirmation window
    settingsMenuCloseConfirmationWindow();
}

void settingsMenuSelectedDeleteSettingsFile(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText = "Are you sure you want to delete the\nSettings file?";
    settingsMenuInitConfirmationWindow(helpText, settingsMenuDeleteSettingsFile);
}

void settingsMenuLoadSettingsFile(bool selectedYes)
{
    if (selectedYes)
    {
        SettingsMenu *settingsMenuPtr = gSettingsMenu;
        switch (loadSettings(settingsMenuPtr->getMemoryCardSlot()))
        {
            case CARD_RESULT_INVALID_SETTNGS_VERSION:
            {
                settingsMenuPtr->initErrorWindow(gSettingsFileInvalidVersionText);
                break;
            }
            case CARD_RESULT_CARD_IN_USE:
            {
                settingsMenuPtr->initErrorWindow(gSettingsMemoryCardInUseText);
                break;
            }
            case CARD_RESULT_NOFILE:
            {
                settingsMenuPtr->initErrorWindow(gSettingsFileNotFoundText);
                break;
            }
            case CARD_RESULT_READY:
            {
                // gRootWindow's color was updated by the settings, so the backup color must be updated as well
                settingsMenuPtr->setRootWindowBackupColor(gRootWindow->getColor());

                settingsMenuPtr->initErrorWindow("Load Successful");
                break;
            }
            default:
            {
                settingsMenuPtr->initErrorWindow("Load Failed");
                break;
            }
        }
    }

    // Close the confirmation window
    settingsMenuCloseConfirmationWindow();
}

void settingsMenuSelectedLoadSettingsFile(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText =
        "Are you sure you want to load the\nsettings from the Settings file? Your\ncurrent settings will be overwritten\nwith "
        "the settings from the Settings file.";

    settingsMenuInitConfirmationWindow(helpText, settingsMenuLoadSettingsFile);
}

void settingsMenuSaveSettingsFile(bool selectedYes)
{
    if (selectedYes)
    {
        SettingsMenu *settingsMenuPtr = gSettingsMenu;
        switch (saveSettings(settingsMenuPtr->getMemoryCardSlot()))
        {
            case CARD_RESULT_CARD_IN_USE:
            {
                settingsMenuPtr->initErrorWindow(gSettingsMemoryCardInUseText);
                break;
            }
            case CARD_RESULT_NOFILE:
            {
                settingsMenuPtr->initErrorWindow(gSettingsFileNotFoundText);
                break;
            }
            case CARD_RESULT_READY:
            {
                settingsMenuPtr->initErrorWindow("Save Successful");
                break;
            }
            default:
            {
                settingsMenuPtr->initErrorWindow("Save Failed");
                break;
            }
        }
    }

    // Close the confirmation window
    settingsMenuCloseConfirmationWindow();
}

void settingsMenuSelectedSaveSettingsFile(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText =
        "Are you sure you want to save your\ncurrent settings? The settings in the\nsettings file will be overwritten "
        "with\nyour current settings.";

    settingsMenuInitConfirmationWindow(helpText, settingsMenuSaveSettingsFile);
}

void settingsMenuCancelSetWindowColor()
{
    // Reset gRootWindow's color to the backup color
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    gRootWindow->setColor(settingsMenuPtr->getRootWindowBackupColor());

    // Close the value editor
    settingsMenuPtr->getValueEditorPtr()->stopDrawing();
}

void settingsMenuSetWindowColor(const ValueType *valuePtr)
{
    // Update gRootWindow's color as well as the backup color
    const uint32_t color = valuePtr->u32;
    gRootWindow->setColor(color);

    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    settingsMenuPtr->setRootWindowBackupColor(color);

    // Close the value editor
    settingsMenuPtr->getValueEditorPtr()->stopDrawing();
}

void settingsMenuSelectedChangeWindowColor(Menu *menuPtr)
{
    (void)menuPtr;

    // Initialize the value editor
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    ValueEditor *valueEditorPtr = settingsMenuPtr->getValueEditorPtr();

    uint32_t flags = 0;
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::HANDLE_AS_HEX);
    flags = valueEditorPtr->setFlag(flags, ValueEditorFlag::DRAW_DPAD_LEFT_RIGHT);

    const uint32_t currentValue = gRootWindow->getColor();
    const Window *rootWindowPtr = gRootWindow;

    valueEditorPtr->init(&currentValue, nullptr, nullptr, rootWindowPtr, flags, VariableType::u32, rootWindowPtr->getAlpha());
    valueEditorPtr->startDrawing(settingsMenuSetWindowColor, settingsMenuCancelSetWindowColor);
}

void SettingsMenu::initErrorWindow(const char *text)
{
    ErrorWindow *errorWindowPtr = &this->errorWindow;
    const Window *rootWindowPtr = gRootWindow;

    errorWindowPtr->setAlpha(rootWindowPtr->getAlpha());
    errorWindowPtr->setText(text);
    errorWindowPtr->setTimer(7000);
    errorWindowPtr->placeInWindow(rootWindowPtr, WindowAlignment::MIDDLE_CENTER);
}
