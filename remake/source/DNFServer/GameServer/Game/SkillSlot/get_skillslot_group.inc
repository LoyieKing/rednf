int SkillSlot::get_skillslot_group(int slot) const
{
    int group = 6;
    if (slot < 6)
    {
        group = -1;
    }
    else if (slot < 0x36)
    {
        group = 0;
    }
    else if (slot < 0x66)
    {
        group = 1;
    }
    else if (slot < 0x96)
    {
        group = 2;
    }
    else if (slot < 0xc6)
    {
        group = 3;
    }
    else if (slot < 0xcc)
    {
        group = -1;
    }
    if (group == 2 && 0x89 < slot)
    {
        group = 4;
    }
    return group;
}