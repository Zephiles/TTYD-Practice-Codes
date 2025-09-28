#include "mod.h"
#include "classes/followerSelector.h"
#include "classes/optionSelector.h"
#include "classes/window.h"
#include "classes/menu.h"
#include "ttyd/party.h"

#include <cstdint>

const char *gFollowersOptions[TOTAL_FOLLOWERS] = {
    "Goombella",
    "Koops",
    "Flurrie",
    "Yoshi",
    "Vivian",
    "Bobbery",
    "Ms. Mowz",
    "Egg",
    "Flavio",
    "Punio",
    "Frankly",
    "Craw",
};

const PartyMembers gFollowersOptionsId[TOTAL_FOLLOWERS] = {
    PartyMembers::kGoombellaFollower,
    PartyMembers::kKoopsFollower,
    PartyMembers::kFlurrieFollower,
    PartyMembers::kYoshiFollower,
    PartyMembers::kVivianFollower,
    PartyMembers::kBobberyFollower,
    PartyMembers::kMsMowzFollower,
    PartyMembers::kEgg,
    PartyMembers::kFlavio,
    PartyMembers::kPunio,
    PartyMembers::kFrankly,
    PartyMembers::kCraw,
};

void FollowerSelector::init(const Window *parentWindow)
{
    this->init(parentWindow, 0xFF);
}

void FollowerSelector::init(const Window *parentWindow, uint8_t windowAlpha)
{
    this->OptionSelector::init(gHelpTextAConfirmBCancel, gFollowersOptions, TOTAL_FOLLOWERS, 2, parentWindow, windowAlpha, 0.f);
}

void FollowerSelector::stopDrawing()
{
    this->OptionSelector::setClassPtr(nullptr);
    this->OptionSelector::stopDrawing();
}

/**
 * Callback function for when an option is selected. Calls the `selectFunc` variable function from the follower selector.
 *
 * @param classPtr Pointer to the current follower selector.
 * @param currentIndex The index of the option that was selected.
 *
 * @relatesalso FollowerSelector
 */
static void followerSelectorSelectedFollower(uint32_t currentIndex, void *classPtr)
{
    const FollowerSelector *followerSelectorPtr = convertToFollowerSelectorPtr(classPtr);

    // Make sure followerSelectorPtr is set
    if (!followerSelectorPtr)
    {
        return;
    }

    // Run the function for when a follower is selected
    const FollowerSelectorSelectFunc func = followerSelectorPtr->getSelectFunc();
    if (func)
    {
        return func(gFollowersOptionsId[currentIndex]);
    }
}

void FollowerSelector::startDrawing(FollowerSelectorSelectFunc selectedFunc, FollowerSelectorCancelFunc cancelFunc)
{
    this->OptionSelector::setClassPtr(this);
    this->selectFunc = selectedFunc;

    // The `classPtr` param is not needed for `cancelFunc`, so it can just converted to the necessary type to save a bit of
    // memory.
    this->OptionSelector::startDrawing(followerSelectorSelectedFollower, convertToCancelFunc(cancelFunc));
}
