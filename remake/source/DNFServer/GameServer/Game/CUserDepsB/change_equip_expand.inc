int CUser::change_equip_expand(int space, int slot, int dstSpace,
                               int dstSlot, int* outSlot)
{
    // b28d-solo-CUserDepsB: AE（189=189，本轮由 DIFF 翻正）——三条本轮定案：
    // ①P46 叶块排放序 = case **书写**顺序 ⇒ 外层 switch 必须写 3 → 0 → 0x12
    //   （原写 3 → 0x12 → 0，0x12 体整段前移 37 行；决策链 3/0x12/0 与书写序无关）；
    // ②P286 单 case switch 复现 `mov 0x14(%ebp),%eax; test %eax,%eax; jne`
    //   （if 形 4.4 恒折 `cmpl $0,mem`；两处 dstSpace 守卫均改
    //   `switch(dstSpace){case 0:{主体} default:return 1;}`）；
    // ③内层 default 直接 `return 1;` 复现 ORIG 0x0865f57e 独立出口。
    // P17/P208 外层/内层 switch 决策链 + ret 提为函数级 -0x18 槽照旧。ORIG 0x0865f382。
    int ret;                               // ORIG 各 case 共用 -0x18 槽（无初值）
    switch (space)
    {
    case 3:
    {
        if (slot <= 9 || slot > 0x15)      // cmpl $0x9 jle / cmpl $0x15 jle
            return 1;
        // P286：ORIG 0x0865f3bc 是 `mov 0x14(%ebp),%eax; test %eax,%eax; jne`
        // （形参载入+寄存器比较）；if 形在 4.4 恒折 `cmpl $0,mem` ⇒ 单 case
        // switch 复现载入形。
        switch (dstSpace)
        {
        case 0:
        {
            if (CheckInTrade())            // W 0x080da2fe
                return 0x13;
            // call 0x0865eed2：arg1=dstSpace(0x14(%ebp))、arg2=(short)dstSlot(cwtl)、
            // arg3=slot(0x10(%ebp))。
            ret = change_equip(dstSpace, static_cast<short>(dstSlot), slot);
            if (ret != 0)
                return ret;
            *outSlot = slot;               // mov 0x18(%ebp) → *0x1c(%ebp)
            return 0;
        }
        default:
            return 1;
        }
    }
    case 0:
    {
        // ORIG 0x0865f42a 段同为 switch 决策链（`mov 0x14(%ebp),%eax;
        // cmp $0x3,%eax; je; cmp $0x12,%eax; je; jmp default`）。
        switch (dstSpace)
        {
        case 3:
        {
            if (dstSlot <= 9 || dstSlot > 0x15)
                return 1;
            if (slot <= 2 || slot > 0x38)
                return 1;
            if (CheckInTrade())
                return 0x13;
            // call 0x0865eed2：arg1=space(0xc(%ebp))、arg2=(short)slot、arg3=dstSlot。
            ret = change_equip(space, static_cast<short>(slot), dstSlot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            // call 0x0865514c GetParty + 非空调 Reset_party_overlapped_drop_ratio。
            CParty* party = static_cast<CParty*>(GetParty());
            if (party != 0)
                party->Reset_party_overlapped_drop_ratio();
            return 0;
        }
        case 0x12:
        {
            if (CheckInTrade())            // 先交易校验（+0x166 处先于区间检查）
                return 0x13;
            if (slot <= 2 || slot > 0x38)
                return 1;
            CExpandEquipslot* expand =
                GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
            // call 0x08499a48：(expand, this, p1, p2, p3, p4)。
            ret = expand->Change_Equip(this, space, slot, dstSpace, dstSlot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            return 0;
        }
        default:
            return 1;      // ORIG 0x0865f57e 独立出口（mov$1;jmp epilogue）
        }
    }
    // ORIG 叶块排放序 = 3 → 0 → 0x12（P46：叶块顺序 = case 书写顺序；决策链
    // 顺序 3/0x12/0 与书写序无关）。原把 0x12 写在 0 前 ⇒ 0x12 体整段前移 37 行。
    case 0x12:
    {
        // 同 case 3：P286 单 case switch 复现 `mov 0x14(%ebp),%eax; test;
        // jne`（ORIG 0x0865f4e8 段）。
        switch (dstSpace)
        {
        case 0:
        {
            if (CheckInTrade())
                return 0x13;
            // GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9)：W 0x080dd584，
            // 字面量 9 由 movl $0x9 压栈。
            CExpandEquipslot* expand =
                GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
            // call 0x08499a48：(expand, this, p3, p4, p1, p2)。
            ret = expand->Change_Equip(this, dstSpace, dstSlot, space, slot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            return 0;
        }
        default:
            return 1;
        }
    }
    default:
        break;
    }
    return 1;
