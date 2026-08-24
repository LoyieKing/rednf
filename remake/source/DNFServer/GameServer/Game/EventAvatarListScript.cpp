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

bool EventAvatarListScript::isfindEventAvatar(int avatar_id) const
{
    return !m_avatars.empty() && m_avatars.find(avatar_id) != m_avatars.end();
}
