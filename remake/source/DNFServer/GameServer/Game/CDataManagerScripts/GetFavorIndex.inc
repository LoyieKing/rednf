ENUM_NPC_RELATION_EVENT_FAVOR_KIND CNPCRelationEventManager::GetFavorIndex(
    const char* name)
{
    if (std::strncmp("[good]", name, 10) == 0)
        return ENUM_NPC_RELATION_FAVOR_GOOD;
    if (std::strncmp("[soso]", name, 10) == 0)
        return ENUM_NPC_RELATION_FAVOR_SOSO;
    if (std::strncmp("[bad]", name, 10) == 0)
        return static_cast<ENUM_NPC_RELATION_EVENT_FAVOR_KIND>(2);
    return static_cast<ENUM_NPC_RELATION_EVENT_FAVOR_KIND>(3);
