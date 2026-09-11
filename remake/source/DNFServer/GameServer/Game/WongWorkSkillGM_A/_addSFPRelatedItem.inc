// ============================================================================
// ORIG T 0x0860a918 _addSFPRelatedItem(int*,int*,int,int)
// 0x4b4 → *a += count；0x4b5 → *b += count；其余无操作。
// ============================================================================
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
}