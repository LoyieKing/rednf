int AvatarFixedHiddenOptionServer::GetSkillIndex(ENUM_RARITY rarity)
{
    int total = GetRandomInput(rarity);
    if (total == -1)
        return -1;
    int randIdx = get_rand_int(total);
    return GetSkillIndex(rarity, randIdx);
}