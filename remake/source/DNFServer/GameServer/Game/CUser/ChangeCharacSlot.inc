int CUser::ChangeCharacSlot(unsigned int a, unsigned int b)
{
    if (a < m_characList.size())
    {
        if (b < m_characList.size())
        {
            if (a < b)
            {
                // b28d-solo-CUser p167：ORIG 0x08651b7a 的 `begin()+b+1` 是**两次**
                //   operator+（迭代器 +b 再 +1；我方 `begin()+(b+1)` 仅一次 ⇒ MISSING ×2）。
                m_characList.insert(m_characList.begin() + b + 1, m_characList[a]);
                m_characList.erase(m_characList.begin() + a);
            }
            else
            {
                m_characList.insert(m_characList.begin() + b, m_characList[a]);
                m_characList.erase(m_characList.begin() + a + 1);
            }
            m_characterView.enableSaveCharacView();
            return 0;
        }
    }
    return 0;
