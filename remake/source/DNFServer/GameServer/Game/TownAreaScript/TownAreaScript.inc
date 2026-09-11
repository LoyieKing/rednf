// [b28-L14] ORIG C1Ev 0x86d1d9a 只构造 m_mapName/m_rects/m_field18 后调 clear()
// （无标量清零）；C1ERKS_/aSERKS_ 为 W 隐式合成（P74），用户定义已删。
TownAreaScript::TownAreaScript()
{
    clear();
}