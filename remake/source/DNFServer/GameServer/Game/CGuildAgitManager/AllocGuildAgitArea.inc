void CGuildAgitArea::AllocGuildAgitArea(unsigned int guildKey)
{
    int areaIdx = GetArea();
    if (areaIdx >= 0)
    {
        SetAreaMapping(guildKey, areaIdx);
    }
