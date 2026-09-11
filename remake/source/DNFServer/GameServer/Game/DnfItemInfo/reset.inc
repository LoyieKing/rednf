// ORIG reset 0x84e9094：8 个标量字段逐条清零（movb/movl/movw 宽度照 ORIG），
// 随后 RandomOption::reset@+0x1d（0x80cb2d8）、UpgradeSeparateInfo::reset@+0x2b
// （0x80cb346）、ReservedCapacity::reset@+0x2c（0x80cb380）。
void DnfItemInfo::reset()
{
    m_seal = 0;           // movb $0,(%eax)
    m_itemId = 0;         // movl $0,0x1(%eax)
    m_attr = 0;           // movb $0,0x5(%eax)
    m_addInfo = 0;        // movl $0,0x6(%eax)
    m_endurance = 0;      // movw $0,0xa(%eax)
    m_extendInfo = 0;     // movl $0,0xc(%eax)
    m_abilityType = 0;    // movb $0,0x10(%eax)
    m_abilityValue = 0;   // movw $0,0x11(%eax)
    m_opt.reset();        // call RandomOption::reset
    m_up.reset();         // call UpgradeSeparateInfo::reset
    m_res.reset();        // call ReservedCapacity::reset
}