CUser* CDeathTower::_pickupItem(CUser* user, char* countBuf, int slot, const map_item& item, bool a5, char a6, const CItem* citem)
{
    // [b28d-solo-CDeathTower] verdict=DIFF（orig 309 / our 312，ndiff 147）：P167 CLEAR（17/17）。
    // 本轮修 P0/P47：isFromMap 改「初值 1 + 条件直写 movb」（复现 ORIG 0x08466d25 movb 直落，
    // 消三目 eax 中转）、去 rarity 具名槽（内联 get_rarity()>3 复现 setg 物化）、
    // isEpic 改 && 形；残余 = 帧 0xa0 vs 0x90 槽号整体平移 + 实参跨 call 落槽（N82/N95）。
    // ORIG T 0x08466cfe（Ghidra 反编译语义逐条转写；countBuf = 调用点 nameBuf[4] 计数）
    CUser* result = user;
    if (slot > 1)
    {
        // [b28d-solo-CDeathTower] ORIG 0x08466d25..0x08466d83 逐条：
        //   movb $0x1,-0x25；`movzbl m_count; xor $1; test; je` ⇒ m_count!=1 才写 0；
        //   `call get_rarity; cmp $3; setg; test; je`（P0：无 rarity 具名槽）⇒ >3 写 a5；
        //   `movzbl m_count; test; je` + `call isEpicRoutingItem; test; je`（P133 独立守卫）
        //   + bool 物化岛 ⇒ 两者皆真才写 a5。三目/具名 bool 形均产 eax 中转（+槽）。
        char isFromMap = 1;
        if (item.m_count != 1)
            isFromMap = 0;
        if (citem->get_rarity() > 3)
            isFromMap = a5;
        if (item.m_count != 0 && citem->isEpicRoutingItem())
            isFromMap = a5;
        // [判据①] ORIG 0x08466d7a（`cmpb $0,-0x25; je 尾`）与 0x08466d86（`call
        // check_item_routing; test %al,%al; je 尾`）为两条独立守卫、各自单条 je 直跳
        // 共享尾出口；4.4 对合成 `&&` 生 mov$1/mov$0 物化岛、对早退生 `jne next;
        // jmp epilogue` 双跳 ⇒ 用正向嵌套 if 复现两条单 je。
        if (isFromMap)
        {
            if (citem->check_item_routing())
            {
                result = NULL;
                int routedTurn = m_playData.getLastRoutedTurn() + 1;
                // [判据①] ORIG 0x08466d9a `mov -0x24(%ebp),%eax; cmp 0x14(%ebp),%eax;
                // jl`（routedTurn 为被减数）⇒ 原码写 `routedTurn >= slot`；旧形
                // `slot <= routedTurn` 在 4.4 生反方向 cmp（jg）且 jcc 落点/助记符不同。
                if (routedTurn >= slot)
                {
                    routedTurn = 0;
                }
                int local_24 = 0;
                // [判据⑩/P331] ORIG 槽序（深→浅）：result(-0x2c)/isFromMap(-0x25)/
                // routedTurn(-0x24)/local_24(-0x20)/**maxC(-0x1c)**/i#1(-0x18)/i#2(-0x14)
                // ⇒ maxC 是 int（`movl $0,-0x1c` 清零 + `cmp -0x1c,%eax` 32 位比较，
                // T1 直证）；声明位置按 ORIG 清零点（第二循环前）。
                // [判据①/T1 直证] ORIG 0x0846?? 该槽清零为 `movl $0x0,-0x1c`（4 字节）
                // 且比较为 `movsbl %al,%eax; cmp -0x1c(%ebp),%eax`（32 位内存比较）
                // ⇒ maxC 是 **int**（旧 char 使 4.4 落 1 字节槽 -0x19 + 8 位 cmpb）。
                for (int i = 0; i < 4; ++i)
                {
                    // [判据①] ORIG 循环体首 `movl $0x0,-0x14` ⇒ member 两段式
                    // （声明=0 + 独立赋值语句，P197③）。
                    CUser* member = 0;
                    member = m_party->get_user(i);
                    if (!(member != 0 && m_party->checkValidUser(i)))
                        continue;
                    if (!m_playData.isMemberAlive(i))
                        continue;

                    // [判据①/P0] ORIG 0x08466e5e `call getCurCharacInvenR; mov %eax,%edx`
                    // （结果在 %edx 跨 Inven_Item 实参复制保活，**无栈槽**；旧具名
                    // invenR 使 4.4 多发 `mov %eax,-0x14; mov -0x14,%eax` 一对 + 全帧 +4）
                    // ⇒ receiver 内联。
                    // [判据①] ORIG 0x08466e9c `cmp $0xffffffff,%eax; sete %al; test
                    // %al,%al; jne 循环尾`：ins 无具名槽（P0 直比 + sete 物化）⇒ continue。
                    if (member->getCurCharacInvenR()->tryInsertItemIntoInventory(item.m_item) == -1)
                        continue;

                    countBuf[i] += 1;
                    if (local_24 == routedTurn)
                    {
                        countBuf[i] += '(';
                        m_playData.setLastRoutedTurn(local_24);
                    }
                    // [判据①] ORIG 0x08466ee1..0x08466f8d：is_stackable/get_item_type/
                    // check_job_type 三守卫各 je 同一 mov$0 岛，无 bAdd 具名槽 ⇒ 原码是
                    // if(&&) 直包（探针复现同一 mov$1/mov$0/test/je 形态）。
                    if (!citem->is_stackable() && citem->GetItemType() == 10 &&
                        citem->check_job_type(member->get_charac_job()))
                    {
                        countBuf[i] += 0x1e;
                    }
                    countBuf[i] += get_rand_int(100 - countBuf[i]);
                    local_24 += 1;
                }
                int maxC = 0;
                for (int i = 0; i < 4; ++i)
                {
                    CUser* member = 0;
                    member = m_party->get_user(i);
                    if (!(member != 0 && m_party->checkValidUser(i)))
                        continue;
                    if (!m_playData.isMemberAlive(i))
                        continue;

                    if (maxC < countBuf[i])
                    {
                        maxC = countBuf[i];
                        result = member;
                    }
                    else if (countBuf[i] == maxC && countBuf[i] > 0)
                    {
                        // [判据①] ORIG 0x08467087 段单次地址物化
                        //（`mov -0x10; add countBuf; movzbl (%eax),%edx; sub $1,%edx;
                        // mov %dl,(%eax)`）；`-= 1` 双地址物化（+2 条）⇒ 用后置递减。
                        countBuf[i]--;
                    }
                }
            }
        }
    }
    return result;
}