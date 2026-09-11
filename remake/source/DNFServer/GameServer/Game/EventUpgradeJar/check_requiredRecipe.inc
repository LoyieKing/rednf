// ORIG 0x080f4bb0。条件：item[+0x02]==0x28be7d 且 item[+0x07]>=1。
bool Event_Upgrade_Jar::check_requiredRecipe(const Inven_Item& item) const
{
    if (!(item.m_addInfo == 0x28be7d))
        return false;
    if (item.m_addInfo2 <= 0)
        return false;
    return true;
}