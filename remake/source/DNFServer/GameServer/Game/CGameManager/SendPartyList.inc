// ============================================================================
// 列表发送
// ============================================================================
void CGameManager::SendPartyList(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=384 our=380 gap=4；ndiff 614——迭代器/
    // PacketGuard/局部槽组分区差）。ORIG 0x08296792。
    // b28d-solo-CGameManager 判词：语义缺口已修（P167 70=70 CLEAR——4 处三元
    // put_byte 改 if/else 双调用点、getDemensionInoutValue 改 getCurCharacR()->
    // m_demensionInout[j] 直读、dim 去具名槽）；本波 P11 类型旋钮（type→unsigned
    // char、member!=0 正向包裹）ndiff 82→45；余 386 vs 384 为 if/else 臂序
    // （4.4 恒 then 后置）+ char/uchar 读扩展 + N82 槽组分区（guard -0x50 vs -0x4c）。
    PacketGuard guard;
    guard.put_header(0, 9);
    int count = 0;
    int index = guard.get_index();
    guard.put_short((int)m_partyMap.size());
    unsigned char type = 0;  // b28d-solo：ORIG 读 type 用 movzbl（P11 unsigned 化）
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.begin();
    while (it != m_partyMap.end())
    {
        CParty* party = it->second;
        if (party != 0 && !party->IsAutoCreated())
        {
            count = count + 1;
            guard.put_short(party->GetPartyIndex());
            guard.put_byte((int)type);
            if (type == 0 || type == 1)
            {
                // T1 直证：ORIG 0x082968a1 `call getTitleIndex; movzbl %al,%eax`
                // ⇒ 低字节按 unsigned 扩展（原 `& 0xff` 在 char 返回下产
                // movsbl+and，P321-T1 照抄扩展形）。
                guard.put_byte((unsigned char)party->getTitleIndex());
                if (party->getTitleIndex() == 0)
                {
                    guard.put_int((int)strlen(party->getTitle()));
                    guard.put_str(party->getTitle(), (int)strlen(party->getTitle()));
                }
                // b28d-solo-CGameManager: ORIG 0x08296927 段为 if/else 两独立
                // put_byte 调用点（+0x1a7/+0x1bc）；三元写法 4.4 汇合单点（P167 -1）。
                if (party->IsReturnUserParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
                guard.put_byte(party->getUserMax() & 0xff);
                guard.put_short(party->getDungIndex() & 0xffff);
                guard.put_byte(party->getDungDiffi() & 0xff);
                if (party->IsEventCharacParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
            }
            if (type == 0 || type == 2)
            {
                int managerIdx = 0;
                for (int i = 0; i < 4; ++i)
                {
                    CUser* member = party->get_user(i);
                    // b28d-solo：ORIG 为 member!=0 直落、null 段后置（P222 块序翻正）
                    if (member != 0)
                    {
                        guard.put_short(member->get_unique_id() & 0xffff);
                        if (party->getManager() == member)
                            managerIdx = i;
                        char sex = member->getSex();  // ORIG -0x16 用 movsbl/cmpb$0xff（P11 signed）
                        if (sex == -1)
                            sex = 1;
                        guard.put_byte((int)sex);
                    }
                    else
                    {
                        guard.put_short(0xffff);
                        guard.put_byte(0xff);
                    }
                }
                if (party->IsReturnUserParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
                guard.put_byte(managerIdx);
                // T1 直证：ORIG 0x08296b11 `movzbl %al,%eax`（无 and）⇒ (unsigned char) cast。
                guard.put_byte((unsigned char)party->is_quick_party());
                if (party->IsEventCharacParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
            }
            if (type == 0 || type == 1 || type == 2)
            {
                // T1 直证：ORIG 0x08296b88 读该局部用 `movzbl -0x15(%ebp),%eax`
                // ⇒ unsigned char（原 char 生 movsbl，P321-T1 照抄宽度/扩展）。
                unsigned char matching = G_GameWorld()->is_dungeon_tag_matching_channel("[impossible]");
                guard.put_byte((int)matching);
                if (matching != 0)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        CUser* member = party->get_user(i);
                        // ORIG 0x08296bca `je <put_byte(0) 块>`（then 后置、else 直落）
                        // ⇒ P272 块序交换：条件取反 + 两块互换。
                        if (member != 0)
                        {
                            guard.put_byte(6);
                            for (int j = 0; j < 6; ++j)
                            {
                                // ORIG +0x456..+0x477：dim 无具名槽（get_dimensionInout
                                // 结果 movsbl 后直压 put_byte 实参，P0 表达式直传）。
                                guard.put_byte((int)(char)((CDataManager*)G_CDataManager())->get_dimensionInout(j));
                                // ORIG +0x47c..+0x49f：getCurCharacR() 后直读
                                // +0xeb9+j 字节（movzbl+movsbl），非 getDemensionInoutValue
                                // 调用（P167 EXTRA/MISSING 实证）。
                                guard.put_byte((int)(signed char)member->getCurCharacR()->m_demensionInout[j]);
                            }
                        }
                        else
                        {
                            guard.put_byte(0);
                        }
                    }
                }
            }
        }
        it++;
    }
    guard.put_short(index, count);
    guard.finalize(true);
    user->Send(guard);
}