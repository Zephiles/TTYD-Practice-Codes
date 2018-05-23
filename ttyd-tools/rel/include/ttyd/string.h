#pragma once

namespace ttyd::string {
	
extern "C" {

// strstr
// strchr
// strncmp
int32_t strcmp(const char *String1, const char *String2);
// strcat
void strncpy(char *Destination, const char *SourceString, uint32_t AmountOfBytesToCopy);
void strcpy(char *Destination, const char *SourceString);
// strlen

}

}