// ---- IsLimitedSpace（ORIG T 0x85c4ff2，报告 private_store__CPrivateStore/IsLimitedSpace.md 全文）----
// 开店起点 m_pos 与目标 pos 区域(a/b)一致，且目标坐标落在
// [m_field2-0x50, m_field2+0x50) × [m_field4-0x32, m_field4+0x32) 矩形内。
// ORIG 为 && 短路链 + 单出口（mov $1/jmp/mov $0），按 P56 写 if 分支。
bool CPrivateStore::IsLimitedSpace(PrivateStorePos const& pos)
{
    if (m_pos.m_a == pos.m_a && m_pos.m_b == pos.m_b &&
        (int)(short)m_pos.m_field4 - 0x32 < (int)(short)pos.m_field4 &&
        (int)(short)m_pos.m_field4 + 0x32 > (int)(short)pos.m_field4 &&
        (int)(short)m_pos.m_field2 - 0x50 < (int)(short)pos.m_field2 &&
        (int)(short)m_pos.m_field2 + 0x50 > (int)(short)pos.m_field2)
        return true;
    return false;
}