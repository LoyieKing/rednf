int TutorialData::getAvatarIndex(ENUM_EQUIPMENTTYPE type)
{
    std::map<int, int>::iterator it =
        m_avatarIndexMap.find(static_cast<int>(type));
    if (it != m_avatarIndexMap.end()) {
        return it->second;
    }
    return -1;
