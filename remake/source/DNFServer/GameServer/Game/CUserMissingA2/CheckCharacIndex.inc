bool UserMercenaryInfoMgr::CheckCharacIndex(unsigned char index) const
{
    // ORIG 全形复现（同 CheckCompetitionPeriodIndex g2 形）：empty() 直判
    // test/jne；idx 拓宽后置（size call 后 movzbl -0xc,%edx；cmp %edx,%eax；jae）；
    // 尾 !invalid choose（探针 /tmp/probe_l8j3.o g2 同构实证）。
    if (m_characs.empty() || m_characs.size() < static_cast<unsigned int>(index))
        return false;
    return true;
