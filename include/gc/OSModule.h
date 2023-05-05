#ifndef GC_OSMODULE_H
#define GC_OSMODULE_H

#include <cstdint>

struct OSModuleImpSection
{
    int32_t moduleId;
    uint32_t offset;
} __attribute__((__packed__));

struct OSModuleInfo
{
    uint32_t id;
    OSModuleInfo *next;
    OSModuleInfo *prev;
    uint32_t numSections;
    uint32_t sectionInfoOffset;
    uint32_t nameOffset;
    uint32_t nameSize;
    uint32_t version;
    uint32_t bssSize;
    uint32_t relOffset;
    uint32_t impOffset;
    uint32_t impSize;
    uint8_t prologSection;
    uint8_t epilogSection;
    uint8_t unresolvedSection;
    uint8_t bssSection;
    uint32_t prologFuncOffset;
    uint32_t epilogFuncOffset;
    uint32_t unresolvedFuncOffset;
    uint32_t moduleAlignment;
    uint32_t bssAlignment;
    uint32_t fixSize;
} __attribute__((__packed__));

static_assert(sizeof(OSModuleImpSection) == 0x8);
static_assert(sizeof(OSModuleInfo) == 0x4C);

extern "C"
{
    // OSNotifyLink
    // OSNotifyUnlink
    // Relocate
    // Link
    bool OSLink(OSModuleInfo *module, void *bss);
    // Undo
    // OSUnlink
    // __OSModuleInit
}

#endif
