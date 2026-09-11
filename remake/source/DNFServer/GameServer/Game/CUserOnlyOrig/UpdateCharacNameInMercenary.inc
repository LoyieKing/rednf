// ---------------------------------------------------------------------------
// 28. CUser::UpdateCharacNameInMercenary @0x084ec5e6 W(报告缺失,objdump 直转)
//     lea this+0x6ef6c → UserMercenaryInfoMgr::UpdateCharacName(uint,string const&)
//     (成员声明 NEEDS_DECL,ORIG _ZN20UserMercenaryInfoMgr16UpdateCharacNameEjRKSs)
// ---------------------------------------------------------------------------
void CUser::UpdateCharacNameInMercenary(unsigned int characNo, const std::string& name)
{
    m_mercenaryMgr.UpdateCharacName(characNo, name);
}