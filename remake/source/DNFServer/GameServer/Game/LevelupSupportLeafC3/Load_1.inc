// ============================================================================
// LevelupSupport2ndEventManger::Load —— 报告 Load.md @0x081477e4。
// cMyTrace(pretty,0xb7,9,true,false) → 清脚本 map（manager+0x8 =
// m_script.m_stepItems）→ 返回 loadScript()==0。
// [b28d-N82] 同 EventManger::Load（0x08146e3c）：帧 0x?? 与迭代器槽方向差。
// ============================================================================
bool LevelupSupport2ndEventManger::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0xb7, 9, true, false);
    trace("\t- Loading Level Up Support 2nd Event script - ");

    m_script.m_stepItems.clear();
    if (m_script.loadScript())
        return false;
    return true;
}