bool STEquipmentScript::artifact_info::isUsable() const  // 0x0898f0fe
{
    // [b28d-solo-STEquipmentScript] ORIG 形 = 具名 bool 局部 + `||` 短路链
    // （0x0898f0fe：逐条件 jne 共享 mov$1 块 → mov $0 岛 → 尾 `mov %al,-0x1(%ebp);
    // movzbl`，sub $0x10）。float 项必须写 `!(x == 0.0f)`（否定式）：4.4 才发
    // `fucompp; fnstsw; sahf; sete; setnp; and; xor $1; test; jne` 与 ORIG 逐条同形；
    // 写 `x != 0.0f` 则折成 `fxch; jne; jp` 直跳（探针 /tmp/b28d_s219/probe5.cpp c1 vs c2）。
    bool usable = (m_minLevel != 0) || (m_physAttack != 0) || (m_magAttack != 0) ||
                  !(m_mpRate == 0.0f) || !(m_chargeRate == 0.0f) ||
                  !(m_overChargeRate == 0.0f) || !(m_expRate == 0.0f) ||
                  !(m_physCrit == 0.0f) || !(m_magCrit == 0.0f) ||
                  !(m_stuck == 0.0f) || !(m_piercing == 0);
    return usable;
}