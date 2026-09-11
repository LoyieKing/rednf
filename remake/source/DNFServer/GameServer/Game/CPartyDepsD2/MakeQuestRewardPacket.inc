// ---------------------------------------------------------------------------
// CParty::MakeQuestRewardPacket —— ORIG T 0x085bd7ae（报告
//   MakeQuestRewardPacket.md）：任务清图奖励物品包（20 任务槽 × 清图物品表）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 0→-1（295 vs 296），p167 40/40 CLEAR。本轮真修
// （三处 p167 真缺口）：①循环内取任务由 `getCurCharacQuestR`（18 字）改
//   `getCurCharacQuestRefR`（21 字，ORIG `_ZNK5CUser21getCurCharacQuestRefREv`）；
// ②去 dungeonIdx 缓存：ORIG 0x085bd878/0x085bd8e3 两次独立 call
//   get_dungeon_index（P183）；③`itemList[i]`→`itemList.at(i)`（ORIG 调
//   `_ZNSt6vectorISt4pairIiiESaIS1_EE2atEj`，P175；补发 at 实例，AE +2）。
// 既有修复保留：needCount 头部预清、itemList/pairValue 分步声明+拷贝赋值
// （N86 反向）、quest==0 死块照抄（P184）、m_itemList 成员直访。
// [b28d-solo-CPartyDepsD2²] **真语义修复 ×3**（ORIG 直证；旧写为字段错位的
// 常量化误译，线上包内容错误）：
//  ①put_short：ORIG 0x085bdaa7 `movzwl -0x7e(%ebp)` = item+0x0b m_fieldb
//    （endurance，Inven_Item.h:281）⇒ 旧写 `put_short(0)` 恒发 0；
//  ②分支条件：ORIG 0x085bdb07 `movzbl -0x88(%ebp); cmp $0x1,%al` = item+0x01
//    m_field1（ITEM_TYPE）== 1 ⇒ 旧写 `(item.m_addInfo >> 16) == 1` 读 +0x04 半字；
//  ③UseLuckPoint 第 2 实参：ORIG 0x085bdb4e `movzbl -0x88; movzbl %al,%eax`
//    = `(int)item.m_field1` ⇒ 旧写 `(int)(m_addInfo >> 16)`（同错位）。
// 读数：295→296（gap 0→-1），仍 DIFF——残差仅 1 条 `mov %eax,%edx`（4.1 把
// get_rarity 结果挪到 %edx 再读 m_field1，4.4 直接落 %edx，N7 族）与 1 个尾部
// `jmp+nop` 岛（4 个早退岛 vs 我方 5 个）。
// 残余：4.4 槽基恒 ebp-8 起 vs ORIG 大局部顶到 ebp ⇒ 全帧槽号 +8 平移
// （ORIG -0x89/-0x44/-0x4c vs 我方 -0x91/-0x4c/-0x54）；N95 定论（double
// framePad 反致下移、int pad/声明序不可上移）。语义/调用集齐，byte 层封顶。
void CParty::MakeQuestRewardPacket(PacketGuard* packet, CUser* user)
{
    int count = 0;
    int needCount = 0;   // ORIG 0x085bd7c0 头部预清（-0x24）
    int countIndex = packet->get_index();
    packet->put_byte(0);
    for (int q = 0; q < 0x14; ++q)
    {
        // [b28d-solo-CPartyDepsD2] P183：ORIG 0x085bd7f4 每轮重调
        // user->getCurCharacQuestR()（-0x1c 槽），无循环外缓存。
        // ORIG 0x085bd8xx 用 getCurCharacQuestRefR（21 字 mangled，与
        // checkMoveMapHackCnt 的 18 字 getCurCharacQuestR 不同重载）。
        UserQuest* questRef = user->getCurCharacQuestRefR();
        if (questRef->m_quest[q] == 0 || questRef->m_trigger[q] == 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[q]);
        if (quest == 0)
            continue;
        // +0x75da = UserQuest.m_authenData(+0x75d8) + 2 = m_characClass（CUser.h:1220）
        // P183：ORIG 0x085bd878/0x085bd8e3 两次独立 call get_dungeon_index
        //（无缓存槽，各自直传实参）。
        // [b28f-CTS P200] ORIG 0x085bd86x 为 `movswl %ax,%ebx`（短整表达式作
        //   普通实参直传、2 操作数扩展到 callee-saved）；原码无具名 short 局部
        //   —— 具名形退化为 `mov %ax,-0x16(%ebp); movswl -0x16(%ebp),%ebx`
        //   并多占 4 字节帧槽（全函数槽号整体深 4）。内联直传复现 ORIG。
        if (!quest->check_clear_item(bf(this).get_dungeon_index(),
                                     questRef->m_authenData.m_characClass))
            continue;
        needCount = questRef->m_quest[q];
        if (needCount <= 0)
            continue;
        // ORIG 0x085bd8b8/-0x4c 循环外默认构造 + 拷贝赋值（N86 反向）；
        // 0x085bd8ce quest==0 死块照抄（P184）。
        std::vector<std::pair<int, int> > itemList;
        std::pair<int, int> pairValue;
        if (quest == 0)
            continue;
        itemList = quest->get_clear_itemlist(bf(this).get_dungeon_index());
        // [b28f-CTS 轴B] ORIG 循环尾为 `cmp -0x..(%ebp),%eax; setg`（**有符号**序，
        //   见 0x085bd9xx）⇒ 原码循环变量是 int；`size_t i < size()` 让 4.4 产
        //   无符号序 `seta`（符号性分叉）。改 int + 显式 `(int)` 收窄复现 ORIG
        //   （P196 前置已核：ORIG 是 setg 而非 seta）。at() 实参 32 位下零转换开销。
        for (int i = 0; i < (int)itemList.size(); ++i)
        {
            Inven_Item item;
            item.reset();
            // ORIG 0x085bd972 单次 vector::at（P175：ORIG 走 at 非 operator[]）
            // 后 pair 拷贝到 -0x4c/-0x48。
            pairValue = itemList.at(i);
            item.m_addInfo = pairValue.first;
            // ORIG 0x085bd98e `mov 0xc(%eax),%eax` 直接成员访问（无 getItemList call）
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_1, item, pairValue.second);
            int slot = (int)user->getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)0xb, true, true);
            if (slot < 0)
                continue;
            packet->put_short(slot);
            packet->put_int(item.m_addInfo);
            packet->put_int(item.get_add_info());
            // [b28d-solo-CPartyDepsD2²] ORIG 0x085bdaa7 `movzwl -0x7e(%ebp)`
            // = item+0x0b m_fieldb（endurance，Inven_Item.h:281）⇒ 旧写
            // `put_short(0)` 是常量化误译（真语义 bug，线上包内容恒 0）。
            packet->put_short(item.m_fieldb);
            packet->put_byte((int)item.m_amp.getAbilityType());
            packet->put_short((int)item.m_amp.getAbilityValue());
            ++count;
            // [b28d-solo-CPartyDepsD2²] ORIG 0x085bdb07 `movzbl -0x88(%ebp);
            // cmp $0x1,%al` = item+0x01 m_field1（ITEM_TYPE）== 1；旧写
            // `(item.m_addInfo >> 16)` 读的是 +0x04 半字（字段错位）。
            if (item.m_field1 == 1)
            {
                CItem* card = G_CDataManager()->find_item(item.m_addInfo);
                if (card != 0)
                    // ORIG 0x085bdb4e `movzbl -0x88(%ebp),%eax; movzbl %al,%eax`
                    // ⇒ 第 2 实参同为 item+0x01 m_field1（(int) 扩展），旧写
                    // `(m_addInfo >> 16)` 读 +0x04 半字（真语义 bug）。
                    CLuckPoint::UseLuckPoint(user, (int)item.m_field1,
                                             card->get_rarity(),
                                             card->get_grade());
            }
        }
    }
    if (count != 0)
        packet->put_byte(countIndex, count);
}