// ORIG T 0x0867846e sz0xb7：遍历 m_list8cf80（list<int>）票表 → SIG_LOAD_TICKET。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867846e | _ZNK5CUser9GetTicketEP15SIG_LOAD_TICKET
void CUser::GetTicket(SIG_LOAD_TICKET* sig) const
{
    sig->m_field4 = 0;
    // b28d-lane5 复测：ORIG 0x0867846e——begin 直构 it@-0x10、end 直构端@-0x14 各一次，
    //   随后 **index 双初始化**（0x47/0x4e 两条 `movl $0x0,-0xc`＝声明式+for-init 式，
    //   P191）⇒ 旧 `it=m_list8cf80.end()` 每轮重取 + 单次 init 反向。
    std::list<int>::const_iterator it = m_list8cf80.begin();
    std::list<int>::const_iterator itEnd = m_list8cf80.end();
    int index = 0;
    for (index = 0; it != itEnd && index <= 99; ++it, ++index)
    {
        sig->m_tickets[index] = *it;
    }
    sig->m_count = index;
}