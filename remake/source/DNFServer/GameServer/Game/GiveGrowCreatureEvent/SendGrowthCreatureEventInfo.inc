void GiveGrowCreatureEvent::SendGrowthCreatureEventInfo(CUser& user)  // 0x081b6b70
{
    PacketGuard pkt;
    pkt.put_header(0, 0x1c0);

    int now = GlobalData::s_systemTime_.getCurSec();
    int job = user.get_charac_job();
    std::vector<GrowthCreatureEvent::eventCharacInfo>* mine =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
            user.getGrowthCreatureEventdata());

    // P198/P0：ORIG -0x10 有槽往返 ⇒ 具名 jobCount（声明+赋值分离）。
    int jobCount;
    jobCount = G_CDataManager()->getGiveGrowCreatureEventJobCount(now);
    pkt.put_byte(jobCount);

    // P13/P28：ORIG begin/end 各求值一次存具名局部、增量段后置 `jt++`（ppEi）；
    // 无 script 具名局部（直用 G_CDataManager()->m_createCreatureGift）；无 mapKey
    // 缓存局部（每用点重读 `jt->first`，P157/P183）。
    // P167/P96：ORIG map 迭代器用非 const `_Rb_tree_iterator` + `->`（ptEv），
    // vector 迭代器用 `(*it).`（deEv）——两者实例化不同成员。
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator jt =
        G_CDataManager()->m_createCreatureGift.m_eventJobMap.begin();
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator jtEnd =
        G_CDataManager()->m_createCreatureGift.m_eventJobMap.end();
    while (jt != jtEnd)
    {
        if (jt->second.m_minLevel < now && now < jt->second.m_maxLevel)
        {
            pkt.put_byte(jt->first);
            // ORIG：!empty() 走内层循环、else 空分支。
            if (!mine->empty())
            {
                std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator kt =
                    mine->begin();
                std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator ktEnd =
                    mine->end();
                while (kt != ktEnd)
                {
                    if ((*kt).m_job == jt->first)
                    {
                        int nameLen = static_cast<int>(strlen((*kt).m_name));
                        // ORIG：then=nameLen==0（put_byte(1) 直落 kt++）、else=nameLen!=0
                        // （put_byte(2)/put_int/put_str 后 break 跳外层）⇒ if/else 块序。
                        if (nameLen == 0)
                        {
                            pkt.put_byte(1);                // nameLen==0：继续内层循环
                        }
                        else
                        {
                            pkt.put_byte(2);
                            pkt.put_int(nameLen);
                            pkt.put_str((*kt).m_name, nameLen);
                            break;                      // ORIG：nameLen!=0 跳出内层
                        }
                    }
                    kt++;
                }
            }
            else
            {
                if (jt->first == job)
                    pkt.put_byte(1);
                else
                    pkt.put_byte(0);
            }
        }
        jt++;
    }

    pkt.finalize(true);
    user.Send(pkt);
