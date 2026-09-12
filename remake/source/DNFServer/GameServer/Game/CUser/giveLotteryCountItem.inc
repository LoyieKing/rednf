// ORIG T 0x0869154a sz0x664：抽奖计数道具发放（事件币/道具/邮件补偿）。
// [R16a15 2026-09-12 落修] 3 处形态对齐（ORIG 直证，四闸门全过）：
//   ① `slot = -1;` 死写照抄（ORIG 0x086917b2 `movl $0xffffffff,-0x2c(%ebp)`）；
//   ② 循环体每轮重发 `add $0x3a0`（ORIG 0x08691646/0x08691662/0x0869195d ×3）
//      ⇒ 删 `std::vector<int>& src` 引用缓存，改第二个指针槽 `stackable`
//      （ORIG -0x34，`mov -0x38(%ebp),%eax; mov %eax,-0x34(%ebp)` 纯拷贝）；
//   ③ `put_byte((int)(signed char)rewards.size())`（ORIG 0x086919a9 `movsbl %al`）。
//   A/B 四数 809/1728/5/58/586 逐值不变；帧 `sub $0x1fc`=ORIG；35 个槽逐槽
//   引用计数与 ORIG 全等；符号集 comm-3=0。残余：`shr $0x1f` ↔ `cmpl $0; jns`
//   与 `sete/je` 极性族（N-lowering，语义等价）。
// 历史：b28d-lane5 曾判 N82/P112 槽号分配封顶 | ORIG 0x0869154a | _ZN5CUser20giveLotteryCountItemEib
void CUser::giveLotteryCountItem(int itemNo, bool match)
{
    // 2026-09-10（G3CUser-B 复验）：ORIG 0x086915fa `cmpl $0x0,-0x38(%ebp); je`
    //   （item==0 返回）与 0x08691623 `xor $1; test %al,%al; jne`（!verifyStackable…
    //   返回）为**两条独立守卫且各有 ORIG 直证** ⇒ 我方同形，不可删；
    //   本 .inc 具名量（item/src/i/it/slot/msg/str/rewards/packet）均有 ORIG 槽对应，
    //   残余属跨版本 codegen（N 族），判 N。
    if (getCurCharacR() == 0)
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0xb45f,
            "giveLotteryCountItem curchar = null id(%s) item_no(%u) match(%d)",
            NumberToString(get_acc_id(), 0), itemNo, match ? 1 : 0);
        return;
    }

    CStackableItem* item =
        (CStackableItem*)G_CDataManager()->find_item(itemNo);
    if (item == 0)
        return;
    if (!verifyStackableItemProcess((StackableAction::Type)0x36, item))
        return;

    std::vector<Inven_Item> rewards;
    // [R16a15 2026-09-12 §22 循环不变式/P183 删除方向] ORIG 0x0869163a
    //   `mov -0x38(%ebp),%eax; mov %eax,-0x34(%ebp)`（item → stackable 的**纯指针
    //   拷贝**，两槽并存），循环体每轮重发 `mov -0x34(%ebp),%edx; add $0x3a0,%edx`
    //   （全函数 `add $0x3a0` ×3：operator[](int&) 两处 + size() 一处，实测
    //   0x08691646/0x08691662/0x0869195d）；我方旧形把 +0x3a0 提前并入 -0x34
    //   缓存（`std::vector<int>& src = item->m_vec3a0;`）⇒ add 仅 1 次、
    //   且 -0x34 内容由 base 变 base+0x3a0。m_vec3a0 为**非 const** vector<int>
    //   （ORIG 调 _ZNSt6vectorIiSaIiEEixEj）。
    // 2026-09-10（C05② 类型旋钮）：ORIG 0x0869196a 为 `cmp -0x30(%ebp),%eax; setg`
    //   （内存直比 + 有符号）⇒ 索引局部必须是有符号 int（size_t 会让 4.4 先载寄存器
    //   并退化为 setb 无符号序）。
    CStackableItem* stackable = item;   // ORIG -0x34（具名局部，P0 槽往返）
    for (int i = 0; i < (int)stackable->m_vec3a0.size(); )
    {
        Inven_Item it;                              // 默认构造后按 ORIG 填 8 字节
        // 2026-09-11（Lane2，字段错位轴修正 + P183 去具名局部）：ORIG 0x08691672/
        //   0x08691696 把两次 `operator[]` 结果直接 store 到 -0x98/-0x93
        //   （it 基址 -0x9a ⇒ **+0x02 / +0x07** = Inven_Item::m_addInfo@+0x02 /
        //   m_addInfo2@+0x07，Inven_Item.h:269/280），无具名 int 中间量。
        //   我方旧形写 it+0/+4 ⇒ (a) 两字段全错位（写到 m_field0 与 m_addInfo 高位，
        //   读回的是 C1Ev 初值 ⇒ 事件币分支与邮件分支取值全错 = 真语义 bug）；
        //   (b) 多占 itemNo/itemCnt 两槽（帧 0x20c vs ORIG 0x1fc）+ 4 条 store/load。
        it.m_addInfo = (unsigned int)stackable->m_vec3a0[i++];
        it.m_addInfo2 = stackable->m_vec3a0[i++];
        rewards.push_back(it);

        // ORIG 0x086916b1 `mov -0x98(%ebp),%eax; cmp $0x1,%eax; jne`
        //   + 0x086916c0 `mov -0x93(%ebp),%eax; test %eax,%eax; js`。
        if ((int)it.m_addInfo == 1 && it.m_addInfo2 >= 0)   // 事件币分支
        {
            // ORIG 0x086916d4..0x08691715：GetEventCoin() **独立调用两次**
            //（Set 实参一次、EventCoinAdd 实参一次；O0 无 CSE，P48）——
            // 旧版缓存 now 局部少一次调用（p167 miss=2）。
            // 实参直读 it.m_addInfo2（ORIG 0x086916e1 `mov -0x93(%ebp),%edx; lea (%eax,%edx,1),%ebx`）。
            getCurCharacInvenW()->SetEventCoin(
                getCurCharacInvenR()->GetEventCoin() + (unsigned int)it.m_addInfo2);
            m_historyLog.EventCoinAdd(
                getCurCharacInvenR()->GetEventCoin(), it.m_addInfo2, (eCoinAddReason)2);
            SaveMoneyCoin();
            SendCoin();
            continue;
        }

        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, it, it.m_addInfo2);

        // ORIG 0x08691782 `mov -0x98(%ebp),%eax; test; jne`（m_addInfo!=0 短路）
        //   → 0x0869178c `lea -0x9a(%ebp),%eax; call get_add_info`。
        if (!(it.m_addInfo != 0 || it.get_add_info() != 0))
            continue;

        // [R16a15 2026-09-12 §18 子轴A·ORIG 死写照抄] ORIG 0x086917b2
        //   `movl $0xffffffff,-0x2c(%ebp)`：唯一覆盖点 0x0869186a =
        //   insertItemIntoInventory 返回值，两个读点（0x0869186d 符号位判定 /
        //   0x08691938 slot 使用）均在其后 ⇒ 不可观察，但按「照抄 ORIG」补回；
        //   声明须留在本 if 体首（ORIG 该 store 紧随 0x086917ac `je` 之后）。
        int slot = -1;
        slot = getCurCharacInvenW()->insertItemIntoInventory(
            it, (eItemAddReason)0xd, true, true);
        if (slot < 0)
        {
            char msg[256];
            // [R5a16 ⑤ 高危#17 多守卫] ORIG 0x08691883..0x0869189c 实证：
            //   `findString(4,"game_server_msg_104",0)` 结果**直接**作为 strncpy 第 2 实参
            //   （`call findString; mov %eax,0x4(%esp)`），无判空、无默认串；我方旧形
            //   `str ? str : ""` 多 4 条 + 多一个空串引用（字符串多重集 OURS-only={""}×1）。
            //   照抄 ORIG（其 findString 返回 0 时 strncpy(NULL) 为 ORIG 自身缺陷）。
            strncpy(msg, g_scriptStringManager_.findString(
                       4, "game_server_msg_104", (bool*)0), 0xff);
            // 2026-09-12（R6a08 §高#17）：ORIG 0x086918b4 `call strncpy@plt` 之后
            //   紧接 0x086918b9 `mov 0x8(%ebp),%eax`（无补 NUL 写、无 movb $0x0,-0x9b）
            //   ⇒ 我方旧形多写 msg[0xff]='\0'（NEW 0x08488bd9 `movb $0x0,-0x9b(%ebp)`）。
            //   按用户裁定「完全复刻 ORIG 逻辑（含缺陷）」删除该防御性写入。
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                getCurCharacName(), it, 0, (unsigned int)getCurCharacNo(),
                msg, (int)strlen(msg), 0, GetServerGroup(), false, false);
        }
        else
        {
            SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1d0);
    // [R16a15 2026-09-12] ORIG 0x086919a9 `movsbl %al,%eax`（size() 低字节按
    //   有符号扩展后传入；put_byte 内部按 (unsigned char) 截断 ⇒ 低字节不变）。
    packet.put_byte((int)(signed char)rewards.size());
    // ORIG 0x086919??：BOOST_FOREACH 展开（contain/begin/end/deref/next/done +
    //   auto_any_base::cvb ×3；元素 Inven_Item& 直取 +2/+7 字段）。
    BOOST_FOREACH(Inven_Item& ri, rewards)
    {
        packet.put_int(ri.m_addInfo);     // +2
        packet.put_int(ri.m_addInfo2);    // +7
    }
    packet.finalize(true);
    Send(packet);
}