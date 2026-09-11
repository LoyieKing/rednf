// ----------------------------------------------------------------------------
// 6. ORIG T 0x081b4262 sz 0x4b。empty→1；size>=index（jae 无符号，含等）→1；
// size<index→0（汇编实证：empty 分支经 eax=0→je→返回 1，仅 size<index
// 落到返回 0）。ORIG 边界 index<=size 与调用点 FindIndexByCharacNo
// 命中下标恒 <size 相容。
// ----------------------------------------------------------------------------
bool UserMercenaryInfoMgr::CheckCharacIndex(unsigned char index) const
{
    // ORIG 全形复现（同 CheckCompetitionPeriodIndex g2 形）：empty() 直判
    // test/jne；idx 拓宽后置（size call 后 movzbl -0xc,%edx；cmp %edx,%eax；jae）；
    // 尾 !invalid choose（探针 /tmp/probe_l8j3.o g2 同构实证）。
    if (m_characs.empty() || m_characs.size() < static_cast<unsigned int>(index))
        return false;
    return true;
}