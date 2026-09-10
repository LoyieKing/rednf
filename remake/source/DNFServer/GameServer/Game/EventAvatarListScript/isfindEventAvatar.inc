bool EventAvatarListScript::isfindEventAvatar(int avatar_id)
{
    if (m_avatars.empty())
        return false;
    std::set<int>::const_iterator it = m_avatars.find(avatar_id);
    if (it != m_avatars.end())
        return true;
    return false;
