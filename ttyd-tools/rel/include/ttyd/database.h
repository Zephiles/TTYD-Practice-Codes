#pragma once

#include <cstdint>

namespace ttyd::database {

struct DatabaseDefinition
{
    const char *name;
    int32_t id;
} __attribute__((__packed__));

static_assert(sizeof(DatabaseDefinition) == 0x8);

extern "C" {

void setupDataLoad(const char *mapName);
int32_t setupDataCheck();
void setupDataBase(const char *areaName, const char *mapName);

}

}