#include "mod.h"
#include "classes/followerSelector.h"
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
    "Gus",
};

// Intended to be used alongside gFollowersOptions
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
    PartyMembers::kGus,
};

static FollowerSelector *gFollowerSelector = nullptr;

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
    gFollowerSelector = nullptr;
    this->OptionSelector::stopDrawing();
}

static void followerSelectorSelectedFollower(uint32_t currentIndex)
{
    // Make sure gFollowerSelector is set
    const FollowerSelector *followerSelectorPtr = gFollowerSelector;
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
    gFollowerSelector = this;
    this->selectFunc = selectedFunc;

    this->OptionSelector::startDrawing(followerSelectorSelectedFollower, cancelFunc);
}
