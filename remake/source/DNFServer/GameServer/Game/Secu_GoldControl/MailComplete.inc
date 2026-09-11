void Secu_GoldControl::MailComplete(bool flag)
{
    // ORIG 尾复制：m_mailGold=0 在两分支内各出现一次（jmp/nop 共享出口仅指 ret），
    // 共享尾写法会丢 if 分支的清零序，需按分支重复。
    if (flag)
    {
        SubImportMoney(m_mailGold);
        SubMoney(m_mailGold);
        m_mailGold = 0;
    }
    else
    {
        AddMoney(m_mailGold);
        m_mailGold = 0;
    }
}