bool CUserPremium::IsRestrictedPremium(int type)  // static（ORIG 0x086af7c4 无 this）
{
    return (type >= 60000 && type <= 69999);
}