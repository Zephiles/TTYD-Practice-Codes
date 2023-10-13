#ifndef CLASSES_FOLLOWERSELECTOR_h
#define CLASSES_FOLLOWERSELECTOR_h

#include "classes/optionSelector.h"
#include "classes/window.h"
#include "ttyd/party.h"

#include <cstdint>

// Need to define some external variables/functions first, as the class will use them
class FollowerSelector;
extern FollowerSelector *gFollowerSelector;

void followerSelectorSelectedFollower(uint32_t currentIndex);

// Called when the player presses A to select a follower
typedef void (*FollowerSelectorSelectFunc)(PartyMembers followerSelected);

typedef void (*FollowerSelectorCancelFunc)(); // Called when the player presses B to cancel selecting a follower

class FollowerSelector: private OptionSelector
{
   public:
    FollowerSelector() {}
    ~FollowerSelector() {}

    // Sets windowAlpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t windowAlpha);

    bool shouldDraw() const { return this->OptionSelector::shouldDraw(); }

    void stopDrawing()
    {
        gFollowerSelector = nullptr;
        this->OptionSelector::stopDrawing();
    }

    void startDrawing(FollowerSelectorSelectFunc selectedFunc, FollowerSelectorCancelFunc cancelFunc)
    {
        gFollowerSelector = this;
        this->selectFunc = selectedFunc;

        this->OptionSelector::startDrawing(followerSelectorSelectedFollower, cancelFunc);
    }

    void controls(MenuButtonInput button) { this->OptionSelector::controls(button); }
    void draw() const { this->OptionSelector::draw(); }

    FollowerSelectorSelectFunc getSelectFunc() const { return this->selectFunc; }

   private:
    FollowerSelectorSelectFunc selectFunc;
};

extern const char *gFollowersOptions[TOTAL_FOLLOWERS];
extern const PartyMembers gFollowersOptionsId[TOTAL_FOLLOWERS]; // Intended to be used alongside gFollowersOptions

#endif