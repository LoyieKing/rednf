int CMailBox::RemoveMail(unsigned int mailIndex)
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
        // [b27 封顶] 唯一残差 = begin() 返回槽：ORIG -0x2c 临时槽→拷贝 jt(-0x1c)
        // 两步（N86：4.1 无 copy-elision），4.4 直构进 jt 槽(-0x20) ⇒ jt 槽号
        // 连锁平移。无源码解（直构即 4.4 语义）。语义/循环体/出口全等。
        bool stillExists = false;
        for (MailMap::iterator jt = m_mailMap.begin(); jt != m_mailMap.end(); ++jt)
        {
            if (jt->second->GetLetterId() == mail->GetLetterId())
                stillExists = true;
        }
        if (!stillExists)
            m_loadedLetterCount += 1;
    }

    mail->ResetPackage();
    m_mailPool.Free(mail);
    return 1;
