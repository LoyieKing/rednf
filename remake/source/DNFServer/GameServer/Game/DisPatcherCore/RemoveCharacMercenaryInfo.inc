// ORIG 0x0868e672 T _ZN5CUser25RemoveCharacMercenaryInfoEj：转调雇佣兵管理器
void CUser::RemoveCharacMercenaryInfo(unsigned int characNo)

{
    m_mercenaryMgr.RemoveCharac(characNo);     // UserMercenaryInfoMgr @ +0x6ef6c
}
