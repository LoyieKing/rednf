// ORIG W 0x085bfe1a _ZN19CPartyExpBonusEvent12getBonusRateEib
// 事件进行中 且 memberCount>1 且 !isHelpAbuse → m_field0c/100.0f，否则 0.0f。
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
float CPartyExpBonusEvent::getBonusRate(int memberCount, bool isHelpAbuse)
{
    // P0：ORIG 虚调结果直 test，无 bool 槽（右值内联）
    if (IsEventing((CUser*)0) && memberCount > 1 && !isHelpAbuse)
        return (float)m_field0c / 100.0f;
    return 0.0f;
}