// ORIG 0x088b0170：return *(int*)(this+0x48) == itemIndex
bool ARAD::SCRIPT::AvatarFixedHiddenOption::isMaterialItemIndex(int itemIndex)
{
    if (m_materialItemIndex == itemIndex)
        return true;
    return false;
}