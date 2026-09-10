// df_game_r Game/ EventAvatarListScript 实现。
// ORIG：C1 W 0x8372e10（set 构造 + clear()）；clear W 0x8372e52（set::clear）。
#include "EventAvatarListScript.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 __ioinit；isfindEventAvatar() 前移为本 TU 首个
// 强外部符号 ⇒ GI 键名 `_GLOBAL__I__ZN21EventAvatarListScript17isfindEventAvatarEi`
// （ORIG 0x896239d）。零语义/布局改动。
#include <iostream>

bool EventAvatarListScript::isfindEventAvatar(int avatar_id)
{
    if (m_avatars.empty())
        return false;
    std::set<int>::const_iterator it = m_avatars.find(avatar_id);
    if (it != m_avatars.end())
        return true;
    return false;
}

EventAvatarListScript::EventAvatarListScript() : m_avatars()
{
    clear();
}

EventAvatarListScript::~EventAvatarListScript() {}

void EventAvatarListScript::clear()
{
    m_avatars.clear();
}

