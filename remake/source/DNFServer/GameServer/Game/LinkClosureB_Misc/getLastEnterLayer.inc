// ---------------------------------------------------------------------------
// TOD_UserState::getLastEnterLayer —— ORIG T 0x08643942 (const, sret)
// ORIG 汇编：add $0x14 → TOD_Layer(TOD_Layer const&) 拷贝 ⇒ 读 +0x14
//（m_todayLayer，TOD_Layer 仅 ushort 字段，拷贝构造与 ushort 构造等价）。
// 头内字段已具名（DataManagerInitDepends.h TOD_UserState）。
// ---------------------------------------------------------------------------
TOD_Layer TOD_UserState::getLastEnterLayer() const
{
    return m_data.m_todayLayer;
}