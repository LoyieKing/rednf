int getSkillChecksum(int skill, int level, int flag)
{
    if (skill > 0x95)
    {
        return 0;
    }
    int result = 0;
    result = (level + flag) * (skill * 10) / 10;
    return result;
