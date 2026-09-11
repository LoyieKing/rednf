// ORIG 0x08552138 (T)——docs/class_func_reports/WongWork__CMailBox/RemoveMail.md
int CMailBox::RemoveMail(unsigned int mailIndex)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    CMail* mail = 0;                       // ORIG movl $0x0 提前清零
    MailMap::iterator it;                  // ORIG 先默认构造再拷贝赋值
    it = m_mailMap.find(mailIndex);
    if (it == m_mailMap.end())
        return 0;

    mail = it->second;
    m_mailMap.erase(it);

    if (mail->GetLetterId() != 0)
    {
        // 若剩余邮件中不再有同 letterId 者 → 已加载计数 +1
        // ORIG 0x085521f0：begin() sret 复用 find 临时槽 -0x2c，随后
        // `mov -0x2c,%eax; mov %eax,-0x1c` 拷回 it（P50/P314 先声明后赋值形）
        // ⇒ 循环必须复用同一具名 it（赋值形），不能另声明 jt（直构进自身槽，
        // 少 2 条拷贝且槽号连锁平移）。
        bool stillExists = false;
        it = m_mailMap.begin();
        while (it != m_mailMap.end())
        {
            if (it->second->GetLetterId() == mail->GetLetterId())
                stillExists = true;
            ++it;
        }
        if (!stillExists)
            m_loadedLetterCount += 1;
    }

    mail->ResetPackage();
    m_mailPool.Free(mail);
    return 1;
}