// ORIG T 0x086437ec：+8/+0xc/+0x10 清零 → resetLayer(+0x14) → 清
// lastEnterLayer(+0x16)/lastEnterTime(+0x18)/lastClearTime(+0x1c)。
void TOD_UserState::_reset()
{
    m_data.m_characNo = 0;
    m_data.m_firstEnterTime = 0;
    m_data.m_enterCount = 0;
    // ORIG 以 TOD_Layer(this+0x14)::resetLayer 调用（即 today 层视图重置），
    // 随后立即清 +0x16；此序与汇编一致。
    m_data.m_todayLayer.resetLayer();
    m_data.m_lastEnterLayer = 0;
    m_data.m_lastEnterTime = 0;
    m_data.m_lastClearTime = 0;
}