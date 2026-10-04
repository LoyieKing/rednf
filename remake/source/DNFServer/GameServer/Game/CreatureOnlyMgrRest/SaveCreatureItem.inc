// ORIG _ZN13user_creature12CCreatureMgr16SaveCreatureItemEv T 0x0833bbbe
// （SaveCreatureItem.md，条目寻址以汇编 lea 复核）：
// CDBMsgSender(0x89) 取 SIG_CREATURE_ITEM_LIST(0x4078)；count@+0；
// 遍历宠物：IsSave() 为真 → SaveCreatureItem(item 区 @cnt*0x3d+4（Inven_Item 数组）、
// detail 区 @cnt*0x38+0x21a0、m_user)，detail.m_uid(+4) 非 0 则 count++；
// count==0 → Cancel 否则 Send。
// [b28d-solo-CreatureOnlyMgrRest + L2Free4to15.S3] verdict=DIFF（语义完整，
// p167 16/16 CLEAR）：真虚调用（entry->IsSave / entry->SaveCreatureItem，
// ORIG 0x0833bc9b/bcb1）+ P171（删 entry 判空，ORIG 直接解引用）+ P183 count
// 每处重读 + P28 `it++`（ppEi）。
// [S3 收敛] 三处地址算式改「整型加法」形 `(int)list + cnt*K [+C]`
// （ORIG 0x0833bc6a..bc70 / bcc1..bce6 均 `add -0x10(%ebp),%reg` 内存加、
// `lea 0x21a0(%edx),%eax` 分步；char*+偏移形产 107 ndiff 且带 push %edi）。
// 尾部 Cancel/Send 改 `if(count!=0) Send else Cancel` 复现 ORIG
// 0x0833bd7d `test;jne` 前跳 then=Send 直落（原 `==0` 形产反向布局）。
// 读数：our 165→161（gap −9→−5）、ndiff 107→35、帧 0x4c→0x40、push %edi 消。
// 残余 = 4.4 临时寄存器重分配（detail 结果 eax→edx 搬运、cnt*0x38 经 %esi
// 中转、`+1` 用 add 非 ORIG `lea 0x1(%eax)`）⇒ N7/N18a 族封顶。
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
    list->m_count = 0;

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
                (Inven_Item*)(list->m_preRows + list->m_count * 0x3d),
                (SIG_CREATURE_ITEM_DETAIL_INFO*)((char*)list + list->m_count * 0x38 + 0x21a0),
                m_user);
            if (list->m_rows[list->m_count].m_uiId != 0)
                list->m_count = list->m_count + 1;
        }
    }

    if (list->m_count != 0)
        sender.Send();
    else
        sender.Cancel();
    return true;
}