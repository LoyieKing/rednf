bool ChattingEmoticon_byLevel::isUsable(const CUser& user) const
{
    // ORIG 0x80e5e4e：call get_charac_level 先行（结果留 %eax），随后重读
    // this->m_level 入 %edx，`cmp %edx,%eax; setge` ⇒ 左操作数为调用结果。
    return user.get_charac_level() >= m_level;
