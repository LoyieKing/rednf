bool CCreatureMgr::SaveCreatureItem()
{
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x89, m_user);
    SIG_CREATURE_ITEM_LIST* list =
        sender.GetInBuffer<SIG_CREATURE_ITEM_LIST>();
    if (list == 0)
    {
        // b28d-solo-CreatureOnlyMgrRest [P227 两形互试取优]：ORIG uid 在
        // ctor call 前 movzwl→%ebx 保活 ⇒ 全表达式临时链（具名形改求值序、
        // 槽同样深移 0x10，实测无收益）。残差帧/槽分配记 N82/P112。
        cMyTrace(__PRETTY_FUNCTION__, 0xd90, 5)(
            "cannot write db uid(%d)\n",
            m_user->get_unique_id() & 0xffff);
        return false;
    }

    memset(list, 0, sizeof(SIG_CREATURE_ITEM_LIST));   // 0x4078
    *(int*)(void*)list = 0;                            // count

    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); it++)
    {
        CCreatureItem* entry = it->second;
        // b28d-solo-CreatureOnlyMgrRest [§4.5 槽指针直调 + P183/P28]：ORIG
        // 0x0833bc9b 起 `mov (%eax),%eax; add $4; mov (%eax),%edx; …call *%edx`
        // = IsSave 虚槽；0x0833bcb1 `add $0xc` = SaveCreatureItem 虚槽；
        // count 每处重读（无 cnt/base 具名局部）；增量段调 ppEi ⇒ `it++`。
        // b28d-solo-CreatureOnlyMgrRest [P171/P140]：ORIG 0x0833bc95 起直接
        // `mov 0x4(%eax),%eax; mov %eax,-0xc; mov -0xc,%eax; mov (%eax),%eax`
        // 无 entry 判空（原码不写即删，补写即多 cmpl/je）。
        // b28d-Main·检查点14 CCreatureItem 多态化后改**真虚调用**：
        // ORIG 0x0833bc9b 槽 1（vptr+4）= IsSave、0x0833bcb1 槽 3（vptr+0xc）
        // = SaveCreatureItem。
        if (entry->IsSave())
        {
            entry->SaveCreatureItem(
                (Inven_Item*)(void*)((char*)(void*)list +
                                     (*(int*)(void*)list * 0x3d + 4)),
                (SIG_CREATURE_ITEM_DETAIL_INFO*)(void*)(
                    (char*)(void*)list + (*(int*)(void*)list * 0x38 + 0x21a0)),
                m_user);
            // detail.m_uid（详情区 +4）非 0 视为写入成功。
            if (*(int*)((char*)(void*)list +
                        *(int*)(void*)list * 0x38 + 0x21a4) != 0)
                *(int*)(void*)list = *(int*)(void*)list + 1;
        }
    }

    if (*(int*)(void*)list == 0)
        sender.Cancel();
    else
        sender.Send();
    return true;
