#include "EventAvatarListScript.h"
EventAvatarListScript::EventAvatarListScript() : m_avatars() {}
EventAvatarListScript::~EventAvatarListScript() {}
bool EventAvatarListScript::isfindEventAvatar(int avatar_id) const
{
    return !m_avatars.empty() && m_avatars.find(avatar_id) != m_avatars.end();
}
