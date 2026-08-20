#ifndef DNF_GAME_CQUICK_PARTY_REWARD_MANAGER_H_
#define DNF_GAME_CQUICK_PARTY_REWARD_MANAGER_H_

namespace QuickParty
{
class CQuickPartyRewardManager
{
public:
    CQuickPartyRewardManager();
    ~CQuickPartyRewardManager();

private:
    char m_reserved[0x228];
};
}

#endif
