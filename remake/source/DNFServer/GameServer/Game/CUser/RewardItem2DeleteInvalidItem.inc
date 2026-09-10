void CUser::RewardItem2DeleteInvalidItem(
    const std::string& name, const std::vector<std::pair<int, int> >& list)
{
    static const char* defalt_string = "";   // ORIG 局部静态 defalt_string(0x93ace50)

    // ORIG 0x086931db `test %al,%al; jne 尾` 与 0x086931f5 `sete/test; jne 尾`
    //   为两条独立早返（非 || 合并）。
    if (list.empty())
        return;
    if (getCurCharacR() == 0)
        return;

    // ORIG 0x086931fb..0x08693210 四条 movl $0 预清（-0x34/-0x30/-0x38/-0x3c），
    //   全部声明在 vector C1(0x08693217) 之前（P198③ 两句式）。
    CItem* itemDef = 0;
    CItem* rewardDef = 0;
    int rewardIdx = 0;
    int rewardCnt = 0;
    std::vector<Inven_Item> items;
    // ORIG 0x08693222..0x0869322d：`call size; mov %eax,-0x2c` 循环外缓存一次
    //   （P13；循环条件 0x08693396 `cmp -0x2c,%eax; setb`）。
    unsigned int listCnt = (unsigned int)list.size();
    for (unsigned int i = 0; i < listCnt; ++i)
    {
        m_historyLog.DeleteInvaildItem(name, list[i].first, list[i].second);
        itemDef = G_CDataManager()->find_item(list[i].first);
        if (itemDef == 0)
            continue;
        // ORIG 0x086932f0 `xor $1,%eax; test %al; jne 循环尾` ⇒ callee bool + if(!...)
        //   （P292/P248档①）；0x08693300 `test %eax,%eax; jle 循环尾` 独立早返。
        if (!G_CDataManager()->m_deleteInvalidItem.GetRewardItemInfo(
                list[i].first, rewardIdx, rewardCnt))
            continue;
        if (rewardCnt <= 0)
            continue;
        rewardDef = G_CDataManager()->find_item(rewardIdx);
        if (rewardDef == 0)
            continue;
        Inven_Item add;
        add.m_addInfo = rewardIdx;   // +0x02 先写奖励物品索引
        rewardDef->make_item(add);   // ORIG vtable 槽 2 填充默认字段
        if (rewardDef->is_stackable())
            add.m_addInfo2 = rewardCnt;   // +0x07 堆叠数量
        items.push_back(add);
    }
    if (items.empty())
        return;

    std::string str0;
    std::string str1;
    // b28d-solo-CUser p167：ORIG 0x0868b9xx 内联 rep stos（我方 EXTRA memset ×2）。
    char sender[0x1e] = {0};
    char body[0x100] = {0};
    G_CDataManager()->m_deleteInvalidItem.GetMailInfo(str0, str1);
    // ORIG 0x08693509..0x08693549：三目结果直接作 strncpy 实参，无 s0/s1 具名局部
    //   （具名会多 `mov %eax,-N` + 回读两条，P0/P329）。
    strncpy(sender,
            str0.empty()
                ? defalt_string
                : g_scriptStringManager_.findString(4, toTString(str0), 0),
            0x1d);
    strncpy(body,
            str1.empty()
                ? defalt_string
                : g_scriptStringManager_.findString(4, toTString(str1), 0),
            0xff);

    unsigned int count = 0;
    Inven_Item batch[10];
    // ORIG 0x0869357c..0x08693587：`call size; mov %eax,-0x24` 同样循环外缓存。
    unsigned int itemCnt = (unsigned int)items.size();
    for (unsigned int i = 0; i < itemCnt; ++i)
    {
        batch[count].setCopy(items[i]);
        count++;
        // ORIG 0x086935c7 `jne 0x485(nop); body; jmp 0x486; nop` = 4.1 if-join 岛
        //   （P223 探针 fC 形：条件假 goto + 体尾 goto 连续双标签）。
        if (count != 10)
            goto batchSkip;
        count = 0;
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, batch, 10u, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false);
        goto batchJoin;
        batchSkip:;
        batchJoin:;
    }
    if (count != 0)
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, batch, count, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false);
    }
