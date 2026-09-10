void CParty::GetRoutingDiceMemberList(std::vector<std::pair<int, int> >& list,
                                      CItem* item)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085ba294：
    //   P226 语句表达式复现 && 短路物化岛（ORIG 每条件 je 共享 false 岛 +
    //   mov $1/mov $0/test/je，且零具名 bool 槽）；
    //   P61 具名 characNo 左值 ⇒ make_pair<int&,int&> 直构 sret（ORIG -0x24/-0x18/-0x0c
    //   三枚 per-loop 左值，无额外 pair 拷贝构造）；
    //   P183 去 user 缓存（ORIG 每处重算 m_member[i].m_pUser）。
    if (item->IsRoutingItem())                  // ORIG 直接解引用 item（无判空）
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (({ _checkValidUser(i) &&
                   m_routingData.GetMemberRoutingState(i) == 1 &&
                   item->IsPriorityJob(
                       m_member[i].m_pUser->get_charac_job(),
                       (int)(signed char)m_member[i].m_pUser->getCurCharacGrowType()); }))
            {
                // P192：ORIG mangled 为 pair<int,int>::pair<int&,unsigned int>
                // （converting ctor）⇒ value_type(a,b) 直构，非 make_pair。
                list.push_back(std::pair<int, int>(
                    i, m_member[i].m_pUser->getCurCharacNo()));
            }
        }
    }

    if (list.empty())
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (({ _checkValidUser(i) &&
                   m_routingData.GetMemberRoutingState(i) == 1 &&
                   item->check_job_type(
                       (char)m_member[i].m_pUser->get_charac_job()); }))
            {
                list.push_back(std::pair<int, int>(
                    i, m_member[i].m_pUser->getCurCharacNo()));
            }
        }
    }

    if (list.empty())
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (({ _checkValidUser(i) &&
                   m_routingData.GetMemberRoutingState(i) == 1; }))
            {
                list.push_back(std::pair<int, int>(
                    i, m_member[i].m_pUser->getCurCharacNo()));
            }
        }
    }
