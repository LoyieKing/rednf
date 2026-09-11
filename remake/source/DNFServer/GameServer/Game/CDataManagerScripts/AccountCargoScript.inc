// ORIG C1 0x8372480：仅构造 +0x08 map 后调 clear()（+0x1c call；+0/+4 由 clear 内清零）。
AccountCargoScript::AccountCargoScript()
    : m_upgradeMap()
{
    clear();
}