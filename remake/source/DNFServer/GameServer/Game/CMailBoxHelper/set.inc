void SIG_AVATAR_INSERT::set(int a1, unsigned int a2, int a3, int expire, bool bSet,
                            char c, const char* agency, int a7, unsigned long a8)
{
    // [2026-09-09 本波] 46→44 insn 归因修正：旧实现经本地 AvatarInsertV
    // 视图 cast 访问（`reinterpret_cast<V*>(this)->m_fieldN = x`），4.4 对此
    // 恒 base(this) 先载 = LHS-first 且折掉窄化中转，故残留 2 条
    // `mov %eax,%edx`（旧判词归 N76/N18a 属误判）。ORIG 0x0850d3c8/0x850d436
    // 是**完整类型成员直访问**形（value-first + `mov %eax,%edx` 窄化中转），
    // 探针 /tmp/probe_cmh/p1.cpp（viaView vs direct）实证两形差异 ⇒ 改用
    // 权威头 SigTypes_A.h 的具名字段直访问，逐条复现 ORIG（P235 同族）。
    // 语义同前波真 bug 修复：+0x28←a8(第 9 参)、+0x2c←a7(第 8 参低字节)。
    m_characNo = a1;
    m_itId = a2;
    m_slot = (unsigned char)a3;
    m_period = expire;
    m_obtainFrom = bSet;    // P136 bool 直存（unsigned char 字段）
    m_abilityNo = c;
    if (agency != 0)
        std::strncpy(m_agency, agency, 0x18);
    m_a7 = (int)a8;
    m_a8 = (unsigned char)a7;
