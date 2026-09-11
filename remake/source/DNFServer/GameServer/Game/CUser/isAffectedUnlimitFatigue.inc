// ORIG 0x08694a54：movzbl 0x85c(%eax),%eax 直读 +0x85c（overEquipFlag byte）。
bool WongWork::CUserPremium::isAffectedUnlimitFatigue() const
{
    return m_overEquipFlag;
}