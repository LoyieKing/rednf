// df_game_r Game/ EventAvatarListScript 实现。
// ORIG：C1 W 0x8372e10（set 构造 + clear()）；clear W 0x8372e52（set::clear）。
#include "EventAvatarListScript.h"

EventAvatarListScript::EventAvatarListScript() : m_avatars()
{
    clear();
}

EventAvatarListScript::~EventAvatarListScript() {}

void EventAvatarListScript::clear()
{
    m_avatars.clear();
}

bool EventAvatarListScript::isfindEventAvatar(int avatar_id)
{
    if (m_avatars.empty())
        return false;
    std::set<int>::const_iterator it = m_avatars.find(avatar_id);
    if (it != m_avatars.end())
        return true;
    return false;
}
