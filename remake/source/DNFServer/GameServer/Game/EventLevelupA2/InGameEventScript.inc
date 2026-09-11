// ============ InGameEventScript C1/D1【报告缺失·objdump 转写】 ============
// C1 0x08985864：_ZNSsC1Ev(this)、_ZNSsC1Ev(this+4)，随后对两者 _ZNSs5clearEv。
// D1 0x08110e96：析构 string(this+4)、string(this)；无虚表写入（非多态）。
InGameEventScript::InGameEventScript()
    : m_field00(), m_field04()
{
    m_field00.clear();
    m_field04.clear();
}