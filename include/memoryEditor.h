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

    // The code assumes that the next five flags are all in this exact order
    MEMORY_EDITOR_ENABLED_FLAG_CLEAR_CACHE,
    MEMORY_EDITOR_ENABLED_FLAG_SET_SYSTEM_LEVEL,
    MEMORY_EDITOR_ENABLED_FLAG_DISABLE_PAUSE_MENU,
    MEMORY_EDITOR_ENABLED_FLAG_ENABLE_VERITCAL_LINES,
    MEMORY_EDITOR_ENABLED_FLAG_ENABLE_HORIZONTAL_LINES,

    MEMORY_EDITOR_ENABLED_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_MEMORY_EDITOR_ENABLED_FLAGS MemoryEditorEnabledFlag::MEMORY_EDITOR_ENABLED_FLAG_MAX_VALUE
#define MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE intCeil(TOTAL_MEMORY_EDITOR_ENABLED_FLAGS, sizeof(uint32_t) * 8)

enum MemoryEditorMiscFlag
{
    MEMORY_EDITOR_MISC_FLAG_MEMORY_EDITOR_OPENED = 0,
    MEMORY_EDITOR_MISC_FLAG_ENTER_EDITOR,
    MEMORY_EDITOR_MISC_FLAG_SELECTED_BYTES_TO_EDIT,
    MEMORY_EDITOR_MISC_FLAG_EDITING_BYTES,

    MEMORY_EDITOR_MISC_FLAG_MAX_VALUE, // Don't use this directly other than for defines
};

#define TOTAL_MEMORY_EDITOR_MISC_FLAGS MemoryEditorMiscFlag::MEMORY_EDITOR_MISC_FLAG_MAX_VALUE
#define MEMORY_EDITOR_MISC_FLAGS_ARRAY_SIZE intCeil(TOTAL_MEMORY_EDITOR_MISC_FLAGS, sizeof(uint32_t) * 8)

class MemoryEditor
{
   public:
    MemoryEditor();
    ~MemoryEditor() {}

    bool enabledFlagIsSet(uint32_t enabledFlag) const;
    void setEnabledFlag(uint32_t enabledFlag);
    void clearEnabledFlag(uint32_t enabledFlag);
    bool toggleEnabledFlag(uint32_t enabledFlag);

    bool miscFlagIsSet(uint32_t miscFlag) const;
    void setMiscFlag(uint32_t miscFlag);
    void clearMiscFlag(uint32_t miscFlag);
    bool toggleMiscFlag(uint32_t miscFlag);

    void setCurrentAddress(uint32_t addressRaw) { this->currentAddress = reinterpret_cast<uint8_t *>(addressRaw); }

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

    uint32_t getButtonCombo() const { return this->buttonCombo; }
    void setButtonCombo(uint32_t combo) { this->buttonCombo = static_cast<uint16_t>(combo); }

    bool currentlyChangingAddress() const
    {
        return this->headerIndex == MemoryEditorOptions::MEMORY_EDITOR_OPTION_CHANGE_ADDRESS;
    }

    void setNumBytesBeingEdited(uint32_t num) { this->numBytesBeingEdited = static_cast<uint8_t>(num); }

    void dpadControls(MenuButtonInput button);
    void controls(MenuButtonInput button);

    void draw() const;

   private:
    bool checkIfAddressInHoverRange(const uint8_t *address, uint32_t currentDigit) const;
    void adjustDigitValueFromAddress(uint32_t currentDigit, int32_t adjustAmount);

    uint32_t enabledFlags[MEMORY_EDITOR_ENABLED_FLAGS_ARRAY_SIZE];
    uint32_t miscFlags[MEMORY_EDITOR_MISC_FLAGS_ARRAY_SIZE];

    uint8_t *currentAddress; // Initial address that values are being retrieved from inside the editor

    uint8_t *bytesBeingEdited; // Pointer to the bytes that are currently being edited. The size is determined by
                               // numBytesBeingEdited.

    ValueEditor *valueEditor; // For changing currentAddress and numBytesBeingEdited

    MenuAutoIncrement autoIncrement;
    uint16_t buttonCombo; // For opening/closing the memory editor

    uint16_t editorSelectedIndex; // For navigating through the bytes that have been selected to edit
    uint8_t editorIndex;          // For navigating through the bytes, but have not selected any to edit yet
    uint8_t headerIndex;

    uint8_t numBytesBeingEdited; // Number of bytes that are currently being edited
};

extern MemoryEditor *gMemoryEditor;

bool memoryEditorIsOpen();
void handleMemoryEditor();

#endif
