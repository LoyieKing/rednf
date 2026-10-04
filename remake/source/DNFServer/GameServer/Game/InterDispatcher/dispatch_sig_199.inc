// [b28d-lane2] verdict=DIFF ndiff=41（ORIG 0x084dcf4a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：P227 全表达式临时链 + 字段 int
// 同型化（m_0/m_4 与 SetPowerInfo 形参同型 ⇒ 单段直落 callee-saved））。
int Inter_LoadPowerWarInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_LOAD_POWER_WAR_INFO* info = reinterpret_cast<SIG_LOAD_POWER_WAR_INFO*>(data);
    // P227：ORIG 在 ctor 前把三字段载入 %edi/%esi/%ebx 跨 ctor 保活 ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x483a, 0)(
        "Receive SIG_LOAD_POWER_WAR_INFO(Winner:%d, A:%d, B:%d)",
        (signed char)info->m_side,
        info->m_point,
        info->m_channelPoint);
    GlobalData::s_power_manager->SetPowerInfo(
        (char)info->m_side,
        info->m_point,
        info->m_channelPoint);
    return 0;
}