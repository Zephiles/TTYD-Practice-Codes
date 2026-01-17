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

static void controls(Menu *menuPtr, MenuButtonInput button);
static void draw(CameraId cameraId, void *user);
static void exit();

static void selectedOptionUseAnalogStickInMenus(Menu *menuPtr);
static void selectedOptionChangeMemoryCardSlot(Menu *menuPtr);
static void selectedOptionChangeWindowColor(Menu *menuPtr);
static void selectedOptionDeleteSettingsFile(Menu *menuPtr);
static void selectedOptionLoadSettings(Menu *menuPtr);
static void selectedOptionSaveSettings(Menu *menuPtr);

static const char *gFileNotFound = "The Settings file was not found on the memory card.";
static const char *gMemoryCardInUse = "The memory card is currently already in use.";
static const char *gInvalidVersionText = "The Settings file's version does not support\nthis version of the Practice Codes";

SettingsMenu *gSettingsMenu = nullptr;

static const MenuOption gOptions[] = {
    "Use Analog Stick In Menus",
    selectedOptionUseAnalogStickInMenus,

    "Change Memory Card Slot",
    selectedOptionChangeMemoryCardSlot,

    "Change Window Color",
    selectedOptionChangeWindowColor,

    "Delete Settings File",
    selectedOptionDeleteSettingsFile,

    "Load Settings",
    selectedOptionLoadSettings,

    "Save Settings",
    selectedOptionSaveSettings,
};

static const MenuFunctions gFuncs = {
    gOptions,
    controls,
    draw,
    exit,
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
    constexpr uint32_t totalOptions = sizeof(gOptions) / sizeof(MenuOption);
    enterNextMenu(&gFuncs, totalOptions);
}

static void controls(Menu *menuPtr, MenuButtonInput button)
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

static void drawSettingsMenuInfo()
{
    // Get the text position for the top-left of the window two lines under the main text
    const uint32_t totalOptions = gMenu->getTotalOptions();
    constexpr float scale = MENU_SCALE;

    float tempPosX;
    float tempPosY;
    gRootWindow->getTextPosXYUnderMainText(nullptr, WindowAlignment::TOP_LEFT, totalOptions, 2, scale, &tempPosX, &tempPosY);

    // Draw whether or not the analog stick is being used in the menus
    const char *yesNoText;
    if (gMod->saveFlagIsSet(ModSaveFlag::MOD_SAVE_FLAG_USE_ANALOG_STICK_IN_MENUS))
    {
        yesNoText = "Yes";
    }
    else
    {
        yesNoText = "No";
    }

    char buf[32];
    constexpr uint32_t bufSize = sizeof(buf);
    snprintf(buf, bufSize, "Use Analog Stick In Menus\n%s", yesNoText);

    // Retrieve posX and posY as separate variables to avoid repeatedly loading them from the stack when using them
    float posX = tempPosX;
    float posY = tempPosY;
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));

    constexpr float lineDecrement = LINE_HEIGHT_FLOAT * scale;
    posY -= (lineDecrement * 3.f);

    // Draw the memory card slot being used
    const SettingsMenu *settingsMenuPtr = gSettingsMenu;
    char cardSlot;

    if (settingsMenuPtr->getMemoryCardSlot() == CARD_SLOT_A)
    {
        cardSlot = 'A';
    }
    else
    {
        cardSlot = 'B';
    }

    snprintf(buf, bufSize, "Memory Card Slot\nSlot %c", cardSlot);
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
    posY -= (lineDecrement * 3.f);

    // Draw the current window color
    snprintf(buf, bufSize, "Window Color\n0x%08" PRIX32, settingsMenuPtr->getRootWindowBackupColor());
    drawText(buf, posX, posY, scale, getColorWhite(0xFF));
}

static void draw(CameraId cameraId, void *user)
{
    // Draw the main window and text
    basicMenuLayoutDraw(cameraId, user);

    // Draw the info for the settings menu
    drawSettingsMenuInfo();

    // Draw the confirmation window if applicable
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
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

static void exit()
{
    // The player may have manually closed the menu via L + Start, so must restore the window color here in case the player
    // closed the menu while changing the window color
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    gRootWindow->setColor(settingsMenuPtr->getRootWindowBackupColor());

    delete settingsMenuPtr;
    gSettingsMenu = nullptr;
}

static void selectedOptionUseAnalogStickInMenus(Menu *menuPtr)
{
    (void)menuPtr;

    gMod->toggleSaveFlag(ModSaveFlag::MOD_SAVE_FLAG_USE_ANALOG_STICK_IN_MENUS);
}

static void selectedOptionChangeMemoryCardSlot(Menu *menuPtr)
{
    (void)menuPtr;

    gSettingsMenu->toggleMemoryCardSlot();
}

static void closeConfirmationWindow()
{
    gSettingsMenu->getConfirmationWindowPtr()->stopDrawing();
}

static void initConfirmationWindow(const char *helpTextPtr, ConfirmationWindowSelectedOptionFunc selectedOptionFunc)
{
    // Initialize the confirmation window
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    ConfirmationWindow *confirmationWindowPtr = settingsMenuPtr->getConfirmationWindowPtr();

    const Window *rootWindowPtr = gRootWindow;

    confirmationWindowPtr->init(rootWindowPtr, helpTextPtr, rootWindowPtr->getAlpha());
    confirmationWindowPtr->startDrawing(selectedOptionFunc);
}

static void sDeleteSettingsFile(bool selectedYes)
{
    if (selectedYes)
    {
        SettingsMenu *settingsMenuPtr = gSettingsMenu;
        switch (deleteSettingsFile(settingsMenuPtr->getMemoryCardSlot()))
        {
            case CARD_RESULT_CARD_IN_USE:
            {
                settingsMenuPtr->initErrorWindow(gMemoryCardInUse);
                break;
            }
            case CARD_RESULT_NOFILE:
            {
                settingsMenuPtr->initErrorWindow(gFileNotFound);
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
    closeConfirmationWindow();
}

static void selectedOptionDeleteSettingsFile(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText = "Are you sure you want to delete the\nSettings file?";
    initConfirmationWindow(helpText, sDeleteSettingsFile);
}

static void sLoadSettings(bool selectedYes)
{
    if (selectedYes)
    {
        SettingsMenu *settingsMenuPtr = gSettingsMenu;
        switch (loadSettings(settingsMenuPtr->getMemoryCardSlot()))
        {
            case CARD_RESULT_INVALID_SETTNGS_VERSION:
            {
                settingsMenuPtr->initErrorWindow(gInvalidVersionText);
                break;
            }
            case CARD_RESULT_CARD_IN_USE:
            {
                settingsMenuPtr->initErrorWindow(gMemoryCardInUse);
                break;
            }
            case CARD_RESULT_NOFILE:
            {
                settingsMenuPtr->initErrorWindow(gFileNotFound);
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
    closeConfirmationWindow();
}

static void selectedOptionLoadSettings(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText =
        "Are you sure you want to load the\nsettings from the Settings file? Your\ncurrent settings will be overwritten\nwith "
        "the settings from the Settings file.";

    initConfirmationWindow(helpText, sLoadSettings);
}

static void sSaveSettings(bool selectedYes)
{
    if (selectedYes)
    {
        SettingsMenu *settingsMenuPtr = gSettingsMenu;
        switch (saveSettings(settingsMenuPtr->getMemoryCardSlot()))
        {
            case CARD_RESULT_CARD_IN_USE:
            {
                settingsMenuPtr->initErrorWindow(gMemoryCardInUse);
                break;
            }
            case CARD_RESULT_NOFILE:
            {
                settingsMenuPtr->initErrorWindow(gFileNotFound);
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
    closeConfirmationWindow();
}

static void selectedOptionSaveSettings(Menu *menuPtr)
{
    (void)menuPtr;

    const char *helpText =
        "Are you sure you want to save your\ncurrent settings? The settings in the\nsettings file will be overwritten "
        "with\nyour current settings.";

    initConfirmationWindow(helpText, sSaveSettings);
}

static void cancelSetWindowColor()
{
    // Reset gRootWindow's color to the backup color
    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    gRootWindow->setColor(settingsMenuPtr->getRootWindowBackupColor());

    // Close the value editor
    settingsMenuPtr->getValueEditorPtr()->stopDrawing();
}

static void setWindowColor(const ValueType *valuePtr)
{
    // Update gRootWindow's color as well as the backup color
    const uint32_t color = valuePtr->u32;
    gRootWindow->setColor(color);

    SettingsMenu *settingsMenuPtr = gSettingsMenu;
    settingsMenuPtr->setRootWindowBackupColor(color);

    // Close the value editor
    settingsMenuPtr->getValueEditorPtr()->stopDrawing();
}

static void selectedOptionChangeWindowColor(Menu *menuPtr)
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
    valueEditorPtr->startDrawing(setWindowColor, cancelSetWindowColor);
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
