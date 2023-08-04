#ifndef CLASSES_FOLLOWERSELECTOR_h
#define CLASSES_FOLLOWERSELECTOR_h

#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/party.h"

#include <cstdint>

#define TOTAL_FOLLOWERS_PER_COLUMN (TOTAL_FOLLOWERS / 2)
#define TOTAL_FOLLOWERS_PER_ROW (TOTAL_FOLLOWERS / TOTAL_FOLLOWERS_PER_COLUMN)

#define TOTAL_FOLLOWERS_PER_COLUMN_FLOAT (TOTAL_FOLLOWERS_FLOAT / 2.f)
#define TOTAL_FOLLOWERS_PER_ROW_FLOAT (TOTAL_FOLLOWERS_FLOAT / TOTAL_FOLLOWERS_PER_COLUMN_FLOAT)

typedef void (*FollowerSelectorSelectFunc)(PartyMembers selectedFollower);
typedef void (*FollowerSelectorCancelFunc)();

class FollowerSelector
{
   public:
    FollowerSelector() {}
    ~FollowerSelector() {}

    // Sets the alpha to 0xFF
    void init(const Window *parentWindow);

    void init(const Window *parentWindow, uint8_t alpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(FollowerSelectorSelectFunc selectedFunc, FollowerSelectorCancelFunc cancelFunc)
    {
        this->selectFunc = selectedFunc;
        this->cancelFunc = cancelFunc;
        this->enabled = true;
    }

    void controls(MenuButtonInput button);
    void draw() const;

   private:
    Window window;

    FollowerSelectorSelectFunc selectFunc; // Called when the player presses A to select a follower
    FollowerSelectorCancelFunc cancelFunc; // Called when the player presses B to cancel selecting a follower
    MenuAutoIncrement autoIncrement;

    bool enabled;         // Whether this window is enabled/drawn or not
    uint8_t currentIndex; // Current cursor position
    uint8_t alpha;
};

extern const char *gFollowersOptions[TOTAL_FOLLOWERS];
extern const PartyMembers gFollowersOptionsId[TOTAL_FOLLOWERS]; // Intended to be used alongside gFollowersOptions

#endif