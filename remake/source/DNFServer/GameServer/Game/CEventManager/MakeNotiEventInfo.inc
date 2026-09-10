void CEventManager::MakeNotiEventInfo(PacketGuard& packet, CUser* user)
{
    // b28d-solo-CEventManager: [N7] 本轮修——两循环 `if (event == 0) continue;`
    // 改正向包裹 `if (event != 0) { if (event->IsEventing(user)) {…} }`（P296）
    // ⇒ 消 2 处 if 体尾 `jmp+nop` 中继岛（旧判「三形不消」作废：continue 岛随
    // 包裹消失），237→233（gap 0，§6.2）。ORIG 0x08115de2 实证——两循环各由
    // `if (GlobalData::s_event_manager != 0)` 包裹；`packet.put_short(activeCount)`
    // （非字面量 0）；尾部 if(!extra.empty())…else put_byte(0)；const_iterator +
    // `(*it).field`（ORIG 调 deEv 非 ptEv，P167 判据）；put_short 实参 (short) 截断。
    // 类型名 stEVENTADDINFO 已按 ORIG vector 实例 mangled 归位（原发明名 EventAddInfo
    // 违反 N114）。p167 30/30 CLEAR；4.4.7 口径 NEAR（残差=4 处虚调用目标寄存器
    // ORIG %edx/%ecx vs 4.4.7 恒 %eax，N7；P282 复核 +0x34=IsEventing/+0x1c=
    // GetAddInfo/+0x18=AppendInfo 槽序与头一致）；4.4.6 后端口径 AE（233=233，
    // 双后端读数见 /tmp/b28d_solo_CEventManager_result.tsv）。迭代器段
    // （begin/end 各带转换构造 + operator*/++/!=）与 ORIG 逐条同构。
    std::vector<stEVENTADDINFO> extra;
    packet.clear();
    packet.put_header(0, 0x6c);
    int countIndex = 0;
    int activeCount = 0;
    countIndex = packet.get_index();
    packet.put_short(activeCount);
    if (GlobalData::s_event_manager != 0)
    {
        for (int id = 0; id <= 0xa5; ++id)
        {
            CEventBase* event = GetRepeatEvent(id);
            if (event != 0)
            {
                if (event->IsEventing(user))
                {
                    packet.put_short(id);
                    ++activeCount;
                    // GetAddInfo 返回 stADDINFO（ORIG sret ABI，2026-08-28 OrigEventLeaf8f 修正）。
                    stADDINFO add = event->GetAddInfo();
                    if ((short)add.m_info0 >= 0)
                    {
                        stEVENTADDINFO item;
                        item.eventId = id;
                        item.info0 = add.m_info0;
                        item.info1 = add.m_info1;
                        extra.push_back(item);
                    }
                }
            }
        }
    }
    packet.put_short(countIndex, activeCount);
    if (GlobalData::s_event_manager != 0)
    {
        for (int id = 0; id <= 0xa5; ++id)
        {
            CEventBase* event = GetRepeatEvent(id);
            if (event != 0)
            {
                if (event->IsEventing(user))
                    event->AppendInfo(packet);
            }
        }
    }
    if (!extra.empty())
    {
        packet.put_byte((int)extra.size());
        // P13：ORIG begin()/end() 各求值一次（end 缓存 -0x48 槽），循环内不再重求。
        std::vector<stEVENTADDINFO>::const_iterator it = extra.begin();
        std::vector<stEVENTADDINFO>::const_iterator itEnd = extra.end();
        for (; it != itEnd; ++it)
        {
            packet.put_short((short)(*it).eventId);
            packet.put_short((short)(*it).info0);
            packet.put_short((short)(*it).info1);
        }
    }
    else
    {
        packet.put_byte(0);
    }
    packet.finalize(true);
