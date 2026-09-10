bool CUsedFatigueGiveItem::getIsCharacEvent(bool& out) const
{
    // ORIG 0x81107ea：m_script == 0 直接返回 false；否则按 bool 读
    // m_charcEventFlag（+0x14 字节）后返回 true。
    // [b28d S12] P211 档②：ORIG 尾 store RHS-first（`movzbl 0x14(%eax),%edx;
    // mov 0xc(%ebp),%eax; mov %dl,(%eax)`）。旧 B1..B5 探针只试了 uchar/char 源
    // （与 bool 目标视为同型 ⇒ 恒 LHS-first）。S12 探针 g1..g4 定：源字段 bool +
    // 目标 bool 直存 ⇒ 4.4 复现 RHS-first 且无 setne（g2 形逐条全等）。
    // InGameEventScripts.h m_charcEventFlag uchar→bool（同宽同偏移）。
    if (m_script1c == 0)
        return false;
    out = static_cast<const UsedFatigueGiveItemScript*>(m_script1c)->m_charcEventFlag;
    return true;
