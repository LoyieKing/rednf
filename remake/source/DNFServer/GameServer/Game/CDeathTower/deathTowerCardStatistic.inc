void CDeathTower::deathTowerCardStatistic(int count, CUser* user, const Inven_Item* items)
{
    // [b28d-solo-CDeathTower] ORIG 0x08469683：get_index 结果直比（无 dungeonIdx 槽）；
    // 三元 false:true 复现 `cmp; sete; test; je; movb $0; jmp; movb $1`（P47/P225）。
    // [b28d-solo-CDeathTower] ORIG 0x08469683：`cmp $0x2af8; sete %al; test %al;
    // je; movb $0,-0x25; jmp; movb $1,-0x25` —— 三目形在 4.4 折 setne+槽；
    // 改 if/else 赋值复现 sete 岛（P47/P194）。
    // [b28d-solo-CDeathTower] ORIG 0x084696a4 `movsbl -0x25(%ebp),%ebx`（signed 扩展）
    // ⇒ 槽类型 char（bool 槽读恒 movzbl）。
    char isNotTowerOfDespair;
    if (m_dungeonMgr.getDungeon()->get_index() == 0x2af8)
        isNotTowerOfDespair = false;
    else
        isNotTowerOfDespair = true;
    int level = user->get_charac_level();

    for (int i = 0; i < count; ++i)
    {
        // [b28d-solo-CDeathTower] P183：ORIG 每使用点重算 items+i*0x3d
        //（`mov i,%eax; imul $0x3d; add items`），无 item 指针/ itemId 具名槽；
        // 条件为寄存器直测（test %eax,jne）而非槽物化。
        if (items[i].m_addInfo == 0)
        {
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0xb, user,
                (unsigned int)items[i].get_add_info());
            GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                (DEATHTOWER_VALUE_STATISTIC_FIELD)8, (char)isNotTowerOfDespair,
                (char)level, (unsigned int)items[i].get_add_info());
        }
        else
        {
            CItem* citem = G_CDataManager()->find_item(items[i].m_addInfo);
            // [b28d-solo-CDeathTower] ORIG 0x0846978X：price 预清与 GetSellItemPrice
            // 无条件执行（citem 判空在其后）；P184 死块 = 两个独立 `cmpl $0,-0x1c; je`。
            int price = 0;
            G_Store()->GetSellItemPrice(
                const_cast<Inven_Item&>(items[i]), (const CItem*)citem,
                (short)count, false, price);
            if (citem != 0)
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0xc, user, (unsigned int)price);
            if (citem != 0)
            {
                if (citem->GetItemType() == 3)   // ORIG vptr+0xc 虚调用 GetItemType==3
                {
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)2, (char)isNotTowerOfDespair,
                        (char)level, 1);
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)7, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)price);
                }
                else
                {
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)10, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)citem->get_rarity());
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)7, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)price);
                }
            }
        }
    }
