void Area::send_group(PacketGuard& packet, CBelong* belong)
{
    // [b28d-solo-Area] ORIG 086c2e1e 调 vptr+0（CBelong 唯一虚函数）且返回
    // bool 直判（test %al,%al; je，无 setne ⇒ P292）；权威声明点=CSchoolMgr.h
    // 的 `virtual bool IsRight(CUser*)`（本文件原局部 class CBelong 属重复
    // 定义+发明符号 IsBelong，已删）。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    while (it != m_UsersInArea.end())
    {
        user = it->second;
        if (user == 0 || user->get_state() <= 2)
        {
            it++;
            continue;
        }
        if (belong->IsRight(user))
            user->Send(packet);
        it++;
    }
