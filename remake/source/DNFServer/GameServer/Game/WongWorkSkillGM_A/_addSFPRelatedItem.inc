void CSkillChanger::_addSFPRelatedItem(int* a, int* b, int itemIdx, int count)
{
    if (itemIdx == 0x4b4)
    {
        *a = *a + count;
    }
    else if (itemIdx == 0x4b5)
    {
        *b = *b + count;
    }
