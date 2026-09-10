CMailBox::CMail* CMailBox::AddNewMail(const stAddNewMailInput& inputRaw)
{
    CMail* mail = m_mailPool.Acquire();
    if (mail == 0)
        return 0;

    mail->m_mailIndex      = SIG_OFF(unsigned int, &inputRaw, 0x00);
    mail->m_senderCharacNo = SIG_OFF(unsigned int, &inputRaw, 0x04);
    mail->m_package.m_packageMoney   = SIG_OFF(unsigned int, &inputRaw, 0x08);

    // ORIG：srcItem 分支逐字段展开拷贝；空时构造 Inven_Item() 后逐字段搬移。
    const Inven_Item* srcItem = SIG_OFF(const Inven_Item*, &inputRaw, 0x24);
    if (srcItem == 0)
        mail->m_package.m_item = Inven_Item();
    else
        mail->m_package.m_item = *srcItem;

    mail->m_period = (long)SIG_OFF(unsigned int, &inputRaw, 0x18);

    std::strncpy(mail->m_senderName, SIG_OFF(const char*, &inputRaw, 0x20), 0x1d);

    mail->m_packageType = SIG_OFF(int, &inputRaw, 0x14);
    // P292 全链 bool：ORIG 0x085520c4 `movzbl 0x1c(%edx),%edx; mov %dl,0x74(%eax)`
    // 单条直存（无 uchar→bool 归一化）⇒ 源类型同为 bool。
    mail->m_unlimit     = SIG_OFF(bool, &inputRaw, 0x1c);
    mail->m_letterId    = SIG_OFF(unsigned int, &inputRaw, 0x0c);

    m_mailMap.insert(std::make_pair(mail->m_mailIndex, mail));

    mail->m_remainSec = SIG_OFF(unsigned int, &inputRaw, 0x10);
    return mail;
