#ifndef MEMORY_EDITOR_H
#define MEMORY_EDITOR_H

#include "cxx.h"
#include "classes/valueEditor.h"
#include "classes/menu.h"
#include "misc/utils.h"

#include <cstdint>

#define MEMORY_EDITOR_BYTES_PER_ROW 13
#define MEMORY_EDITOR_TOTAL_ROWS 13
#define MEMORY_EDITOR_DIGITS_PER_ROW (MEMORY_EDITOR_BYTES_PER_ROW * 2)
#define MEMORY_EDITOR_MAX_NUM_BYTES_DISPLAYED (MEMORY_EDITOR_BYTES_PER_ROW * MEMORY_EDITOR_TOTAL_ROWS)
#define MEMORY_EDITOR_MAX_NUM_DIGITS_DISPLAYED (MEMORY_EDITOR_DIGITS_PER_ROW * MEMORY_EDITOR_TOTAL_ROWS)

enum class MemoryEditorState : uint8_t
{
    MEMORY_EDITOR_STATE_SELECTING_HEADER = 0,
    MEMORY_EDITOR_STATE_ENTERED_EDITOR,
    MEMORY_EDITOR_STATE_SELECTED_BYTES_TO_EDIT,
    MEMORY_EDITOR_STATE_EDITING_BYTES,
};

enum MemoryEditorOptions
{
    MEMORY_EDITOR_OPTION_CHANGE_ADDRESS = 0,
    MEMORY_EDITOR_OPTION_CHANGE_BYTES_SIZE,
    MEMORY_EDITOR_OPTION_ENTER_EDITOR,

    MEMORY_EDITOR_OPTION_MAX_VALUE, // Don't use this directly other than for defines
};

#define MEMORY_EDITOR_HEADER_TOTAL_OPTIONS MemoryEditorOptions::MEMORY_EDITOR_OPTION_MAX_VALUE

// Enabled flags will be saved to the settings file
enum MemoryEditorEnabledFlag
{
    MEMORY_EDITOR_ENABLED_FLAG_MEMORY_EDITOR_ENABLED = 0,
    MEMORY_EDITOR_ENABLED_FLAG_CLEAR_CACHE,
    MEMORY_EDITOR_ENABLED_FLAG_SET_SYSTEM_LEVEL,
    MEMORY_EDITOR_ENABLED_FLAG_DISABLE_PAUSE_MENU,
    MEMORY_EDITOR_ENABLED_FLAG_ENABLE_VERITCAL_LINES,
    MEMORY_EDITOR_ENABLED_FLAG_ENABLE_HORIZONTAL_LINES,

    MEMORY_EDITOR_ENABLED_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_MEMORY_EDITOR_ENABLED_FLAGS MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_MAX_VALUE
#define MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE intCeil(TOTAL_MEMORY_EDITOR_ENABLED_FLAGS, sizeof(uint32_t) * 8)

// Subtract one to exclude EDITOR_ENABLED
#define MEMORY_EDITOR_TOTAL_SPECIAL_FLAGS (TOTAL_MEMORY_EDITOR_ENABLED_FLAGS - 1)

// Make sure `gMemoryEditorSpecialFlagsArray` has the same amount of entries as `MEMORY_EDITOR_TOTAL_SPECIAL_FLAGS`
extern const uint8_t gMemoryEditorSpecialFlagsArray[MEMORY_EDITOR_TOTAL_SPECIAL_FLAGS];

class MemoryEditor
{
   public:
    MemoryEditor();
    ~MemoryEditor() {}

    const uint32_t *getEnabledFlagsPtr() const { return this->enabledFlags; }
    uint8_t *getCurrentAddressPtr() const { return this->currentAddress; }
    void setCurrentAddress(uint32_t addressRaw) { this->currentAddress = reinterpret_cast<uint8_t *>(addressRaw); }
    void setCurrentAddress(uint8_t *address) { this->currentAddress = address; }

    ValueEditor *getValueEditorPtr() { return this->valueEditor; }

    void freeValueEditor()
    {
        ValueEditor *valueEditorPtr = this->valueEditor;
        if (valueEditorPtr)
        {
            delete valueEditorPtr;
            this->valueEditor = nullptr;
        }
    }

    Menu *getMenuPtr() { return this->menuPtr; }
    void setMenuPtr(Menu *menuPtr) { this->menuPtr = menuPtr; }

    uint32_t getButtonCombo() const { return this->buttonCombo; }
    void setButtonCombo(uint32_t combo) { this->buttonCombo = static_cast<uint16_t>(combo); }

    bool currentlyChangingAddress() const
    {
        return this->headerIndex == MemoryEditorOptions::MEMORY_EDITOR_OPTION_CHANGE_ADDRESS;
    }

    uint32_t getNumBytesBeingEdited() const { return this->numBytesBeingEdited; }
    void setNumBytesBeingEdited(uint32_t num) { this->numBytesBeingEdited = static_cast<uint8_t>(num); }

    bool memoryEditorIsDisplayed() const { return this->displayed; }
    void displayMemoryEditor() { this->displayed = true; }
    void hideMemoryEditor() { this->displayed = false; }

    void toggleMemoryEditorDisplayed()
    {
        const bool displayed = this->displayed;
        this->displayed = !displayed;
    }

    void controls(MenuButtonInput button);
    void draw() const;

    bool enabledFlagIsSet(uint32_t enabledFlag) const
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (enabledFlag >= maxFlags)
        {
            return false;
        }

        bool ret;
        FLAG_IS_SET(this->enabledFlags, enabledFlag, ret);
        return ret;
    }

    void setEnabledFlag(uint32_t enabledFlag)
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (enabledFlag >= maxFlags)
        {
            return;
        }

        SET_FLAG(this->enabledFlags, enabledFlag);
    }

    void clearEnabledFlag(uint32_t enabledFlag)
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (enabledFlag >= maxFlags)
        {
            return;
        }

        CLEAR_FLAG(this->enabledFlags, enabledFlag);
    }

    bool toggleEnabledFlag(uint32_t enabledFlag)
    {
        // Make sure the flag is valid
        constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;
        constexpr uint32_t maxFlags = MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE * bitsPerWord;

        if (enabledFlag >= maxFlags)
        {
            return false;
        }

        TOGGLE_FLAG(this->enabledFlags, enabledFlag);
        return this->enabledFlagIsSet(enabledFlag);
    }

   private:
    bool checkIfAddressInHoverRange(const uint8_t *address, uint32_t currentDigit) const;
    void adjustDigitValueFromAddress(uint32_t currentDigit, int32_t adjustAmount);
    void dpadControls(MenuButtonInput button);

    uint32_t enabledFlags[MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE];

    uint8_t *currentAddress; // Initial address that values are being retrieved from inside the editor

    uint8_t *bytesBeingEdited; // Pointer to the bytes that are currently being edited. The amount is determined by
                               // numBytesBeingEdited.

    ValueEditor *valueEditor; // For changing currentAddress and numBytesBeingEdited

    Menu *menuPtr; // Pointer used for handling the Settings menu

    MenuAutoIncrement autoIncrement;
    uint16_t buttonCombo; // For opening/closing the memory editor

    uint16_t editorSelectedIndex; // For navigating through the bytes that have been selected to edit
    uint8_t editorIndex;          // For navigating through the bytes, but have not selected any to edit yet
    uint8_t headerIndex;

    uint8_t numBytesBeingEdited; // Number of bytes that are currently being edited
    MemoryEditorState state;
    bool displayed; // Whether the memory editor is displayed or not
};

extern MemoryEditor *gMemoryEditor;

bool memoryEditorIsOpen();
void handleMemoryEditor();

#endif
