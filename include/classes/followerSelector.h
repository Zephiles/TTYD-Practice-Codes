#ifndef CLASSES_FOLLOWERSELECTOR_h
#define CLASSES_FOLLOWERSELECTOR_h

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/party.h"

#include <cstdint>

// Called when the player presses A to select a follower
typedef void (*FollowerSelectorSelectFunc)(PartyMembers followerSelected);

// Called when the player presses B to cancel selecting a follower
typedef void (*FollowerSelectorCancelFunc)();

class FollowerSelector: private OptionSelector
{
   public:
    FollowerSelector() {}
    ~FollowerSelector() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t windowAlpha);

    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }
    void stopDrawing();
    void startDrawing(FollowerSelectorSelectFunc selectedFunc, FollowerSelectorCancelFunc cancelFunc);

    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }
    void draw() const { this->OptionSelector::draw(); }

    FollowerSelectorSelectFunc getSelectFunc() const { return this->selectFunc; }

   private:
    FollowerSelectorSelectFunc selectFunc;
};

extern const char *gFollowersOptions[TOTAL_FOLLOWERS];
extern const PartyMembers gFollowersOptionsId[TOTAL_FOLLOWERS]; // Intended to be used alongside gFollowersOptions

#endif