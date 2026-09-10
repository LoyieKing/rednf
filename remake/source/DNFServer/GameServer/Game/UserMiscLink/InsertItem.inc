void LevelupSupportEventManger::InsertItem(CUser* user, int eventIdx, ENUM_ERROR& err)
{
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：218/217、p167 CLEAR(28=28)；
    // 残差=①4.4 整数提升致 `movzwl %ax,%eax; sar $0xf,%eax`（ORIG 16 位 `shr $0xf,%ax`）；
    // ②N7 间接 call 目标 %eax vs %edx。slot<0 判定以 (unsigned short)(~slot)>>15 复现
    // ORIG 的 not/shr 形（cmpw/js 直比差 3 条）。
    // ORIG 0x081473cc：P157 每次重取
    // user->m_levelupEvents[eventIdx]（无 info 缓存，operator[] 两处独立调用），
    // items->at(i) 每字段重调（m_itemIndex/m_itemCount 各一次）。
    user->m_levelupEvents[eventIdx].m_state = 2;

    const std::vector<LevelupSupportSciprt::ItemInfo>* items =
        ARAD::Singleton<LevelupSupportEventManger>::Get()->GetItemInfoVector(
            (int)user->m_levelupEvents[eventIdx].m_level);
    if (items == 0)
    {
        err = (ENUM_ERROR)0x15;
        return;
    }

    // ORIG 0x0814743b：size() 缓存于 -0x1c，循环尾 `addl $1; cmp; setb; test; jne`
    // （while 形，非 4.4 for 的直跳形）。
    std::size_t i = 0;
    std::size_t n = items->size();
    while (i < n)
    {
        CItem* pItem = G_CDataManager()->find_item(items->at(i).m_itemIndex);
        if (pItem == 0)
        {
            err = (ENUM_ERROR)0x15;
            ARAD::PRINT_ERROR::find_item(
                (char*)" LevelupSupportEventManger::InsertItem", user,
                items->at(i).m_itemIndex);
            return;
        }

        Inven_Item item;
        // ORIG：make_item 前先置 m_addInfo(+0x02) = get_index()（0x081474c9）
        item.m_addInfo = pItem->get_index();
        pItem->make_item(item);
        // ORIG 0x0814753c：`movzwl 0x4(%eax),%eax; cwtl` ⇒ 原码为 (short) 截断后转 int
        item.set_add_info((int)(short)items->at(i).m_itemCount);

        // ORIG 0x0814752b：返回值截断为 short 存 -0x12（`mov %ax,-0x12`）。
        short slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0xd, true, true);
        // ORIG 0x081475ec：slot<0 跳后置 mail 块（`not/shr $0xf/test/je`），
        // slot>=0 直落 SendUpdateItem ⇒ 正极性 if/else 交换块序（P59/P222）。
        // ORIG 0x081475ec：`movzwl -0x12,%eax; not %eax; shr $0xf,%ax; test %al,%al; je`
        // ⇒ 原码为 (~slot) >> 15 后按 unsigned short 取值（非 cmpw/js 直比形）。
        // [D04 探针 /tmp/pm/probe2.cpp k1、probe3.cpp p1..p5] 只有
        // `(unsigned short)((unsigned)(~slot) >> 15)` 产 4 条形 `movswl; not %eax;
        // shr $0xf,%eax; test %ax,%ax`（其余五形恒插 `movzwl %ax,%eax` 窄化中转 = +1 条）。
        // 残余 = 4.1 的 16 位窄化（`movzwl` 装载 / `shr %ax` / `test %al,%al`）vs 4.4 恒 32 位
        // 提升（`movswl` / `shr %eax` / `test %ax,%ax`）⇒ 结构性 N（同 N206/整数提升族）。
        if ((unsigned short)((unsigned)(~slot) >> 15) != 0)
        {
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
        }
        else
        {
            // 背包不足 → 邮件补发（game_server_msg_241 sender / 242 title）。
            const char* sender = g_scriptStringManager_.findString(
                4, "game_server_msg_241", 0);
            const char* title = g_scriptStringManager_.findString(
                4, "game_server_msg_242", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                sender, item, 0,
                (unsigned int)user->getCurCharacNo(), title,
                (int)std::strlen(title), 0xf,
                user->GetServerGroup(), false, false);
        }
        ++i;
    }

    // ORIG 0x08147622：再次 operator[] 取 m_level（无 info 缓存）。
    DB_SaveEventLevelupSupport::makeRequest((int)user->GetUID(),
                                            (int)user->getCurCharacNo(),
                                            (int)user->m_levelupEvents[eventIdx].m_level, 2);
    LevelupSupportEventManger::SendCharacState(user, 0);
