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

typedef void (*FollowerSelectorCancelFunc)();
typedef void (*FollowerSelectorErrorFunc)();

class FollowerSelector
{
   public:
    FollowerSelector() {}
    ~FollowerSelector() {}

    // Sets the alpha to 0xFF
    void init(const Window *parentWindow, float scale);

    void init(const Window *parentWindow, float scale, uint8_t alpha);

    bool shouldDraw() const { return this->enabled; }
    void stopDrawing() { this->enabled = false; }

    void startDrawing(FollowerSelectorCancelFunc cancelFunc, FollowerSelectorErrorFunc errorFunc)
    {
        this->cancelFunc = cancelFunc;
        this->errorFunc = errorFunc;
        this->enabled = true;
    }

    void controls(MenuButtonInput button);
    void draw() const;

   private:
    Window window;
    float scale;

    FollowerSelectorCancelFunc cancelFunc; // Called when the player presses B to cancel selecting a follower
    FollowerSelectorErrorFunc errorFunc;   // Called when the player tries to bring out a follower while not being able to
    MenuAutoIncrement autoIncrement;

    bool enabled;         // Whether this window is enabled/drawn or not
    uint8_t currentIndex; // Current cursor position
    uint8_t alpha;
};

extern const char *followersOptions[TOTAL_FOLLOWERS];
extern const PartyMembers followersOptionsId[TOTAL_FOLLOWERS]; // Intended to be used alongside followersOptions

#endif