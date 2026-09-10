// ============================================================================
// _CheckBuyingCondition —— ORIG 0x08323f86 (T)
// 商城购买前置条件分发（uint/ulong 重载，被 checkBuyingCondition 与
// _CheckBuyingCondition(CCeraShopGoods*) 共用）。按 itemIdx 归一化 category
// 后分派 0x01/_CheckConditionPutonAvatar、0x03（创角时间窗）、0x04（仓库容量）、
// 0x05（背包容量+造物）、0x06（事件商品存在性）、0x08（等级）、0x0b（技能树）、
// 0x0c（角色槽/服务器环境）、0x0d（背包容量精确）、0x10（转职/可堆叠限定）。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08323f86 orig/ours=566/563 ndiff=530。
// 巨件帧/槽平移（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82/N89] 566/561 ndiff 357→351；
// P251/P286 switch(a) 复现无槽多路比较链（ORIG 0x0832482a `mov a,%eax; cmp; je`）；
// P167 CLEAR 52/52；余帧 0xc4/0xd4 + 槽平移封 N82。
bool CCeraShop::_CheckBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                                     unsigned int b, unsigned short c, unsigned char d,
                                     unsigned short e)
{
    // local_20：归一化 category。ORIG 先按 itemIdx 特例置 1/3，再按区间映射。
    int category = 0;

    // P251/P286：ORIG 0x0832482a `mov 0x10(%ebp),%eax; cmp $0x1adbd,%eax; je;
    // cmp $0x1c13c,%eax; je` 为无槽多路比较链 ⇒ 源形是 switch(a)，非 if-else。
    switch (a)
    {
    case 0x1adbd:
        category = 1;
        break;
    case 0x1c13c:
        category = 3;
        break;
    default:
        break;
    }

    // 真 bug 修复（①判据 + 报告 Ghidra C 648 行）：ORIG `if (param_3 != 1)`
    // 即持有**第 3 形参 itemIdx（ulong）**，旧码误用第 4 形参 b（uint）。
    if (itemIdx != 1)
    {
        // [G10 实验·P231/P52] ORIG 0x0832400x 无 citem 栈槽（浅区 store 集
        // ORIG-only 0x14/0x57；我方 0x20/0x24/0x28 三槽）⇒ 值以 callee-saved
        // 跨 IsAvatarItem 虚调保活。纯 register（非 asm 绑定，P0 合规）试形。
        register CItem* citem = G_CDataManager()->find_item((int)itemIdx);
        if (citem == 0)
            return 0;
        if (citem->IsAvatarItem())  // vtable +0x10（IsAvatarItem）
        {
            // ORIG 0x08324019：`e == 0 || getAvatarTypeSelect()->size() <= e` 短路，
            // 结果零槽物化（mov$1/mov$0/test），且 e==0 时跳过后一调用
            // ⇒ P226 语句表达式（含具名 bool 会落槽并多一对 store/load）。
            if (({ e == 0 ||
                   ((const std::vector<stAvatarTypeSelect_t>*)(((CEquipItem*)citem)
                        ->getAvatarTypeSelect()))->size() <= e; }))
                return 0;
            if (e != c)
                return 0;
            // 实测：帧 0xd4→0xc4（register citem）→0xc0；OURS-only 槽由 5 个降为 0；
            // 报告 diff 481/472→439/430；TU 四数 176/319/3/11/104 不变 ⇒ 保留。
            // [G10 实验·P0/P183/P157] ORIG 浅区 store 集只多 1 槽（0x14），我方多
            // 3 槽（0x20/0x24/0x28 = sel 指针 / selItem 引用 / slotCount）⇒ 三点全
            // 内联为单表达式（ORIG 每点重算 getAvatarTypeSelect()/at()，无 CSE）。
            if (c != 4 &&
                ((const std::vector<stAvatarTypeSelect_t>*)
                     (((CEquipItem*)citem)->getAvatarTypeSelect()))->at(e).m_field10 != 0)
                return 0;
        }
    }

    // 归一化 category（itemIdx 区间/特例映射，见报告汇编 0x0239..0x0241）。
    if (itemIdx < 0x34a)
    {
        if (itemIdx > 0x346)
            category = 0xb;
        else if (itemIdx < 0x44)
        {
            if (itemIdx < 0x3f)
            {
                if (itemIdx == 0x33)
                    category = 5;
                else if (itemIdx < 0x34)
                {
                    if (itemIdx == 0x32)
                        category = 4;
                }
                else if (itemIdx > 0x38)
                    category = 4;
            }
            else
                category = 6;
        }
        else if (itemIdx < 0xc1)
        {
            if (itemIdx < 0xbe)
            {
                if (itemIdx - 0x45 < 5)
                    category = 6;
            }
            else
                category = 0xb;
        }
        else if (itemIdx == 0x2e4 || itemIdx == 0x335)
            category = 0xb;
    }
    else
    {
        if (itemIdx > 0x28978c)
        {
            if (itemIdx < 0x2897ca)
            {
                if (itemIdx < 0x2897c8)
                {
                    if (itemIdx == 0x28978f)
                        category = 0xc;
                }
                else
                    category = 0xd;
            }
            else if (itemIdx > 0x28995c &&
                     (itemIdx < 0x28995f || itemIdx == 0x2899ab))
                category = 0x10;
        }
        else
        {
            if (itemIdx > 0x28978a)
                category = 4;
            else if (itemIdx == 0x1ee2 || itemIdx == 0x3a0)
                category = 0xb;
            else if (itemIdx > 0x1ee3)
            {
                if (itemIdx == 0x2896cb)
                    category = 0xb;
                else if (itemIdx == 0x28978a)
                    category = 5;
            }
        }
    }

    if (category == 0)
        return 1;

    // case 逻辑直接内联（ORIG 0x8323f86 为单一函数，无 file-static helper；
    // 各 case 代码形态/调用序对齐报告反编译 C）。
    switch (category)
    {
    case 1:
        return _CheckConditionPutonAvatar(user);

    case 3:
    {
        // ORIG 0x0832420f/0x08324219：函数内 static（nm 证实 .bss：
        // _ZZN8WongWork9CCeraShop21_CheckBuyingConditionEP5CUserjmjthtE14eventStartTime
        // @0x0943bec8 / ...E12eventEndTime @0x0943becc）**每次调用无条件重写**常量
        // ⇒ 原码为「声明无初值 + 赋值语句」（带初值声明会折进 .data 且无代码）。
        // [L14 轴 B（a_S->U 命中）] ORIG 0x08324236/0x0832424b 直证为
        // `mov <A>,%edx; cmp %edx,%eax; jl/jge`（**有符号序**）；
        // `getCurCharacCreateTime()` 返回 long（CUserCharacInfo.h:846），
        // 与 `unsigned int` 比较按 C++ 规则转无符号 ⇒ 我方出 `setb/setae`。
        // 改 `int` 复现有符号序（常量 0x4589a450/0x45ae8e50 均为正，值不变）。
        // [AxisB3 属主复核 2026-09-10 ⇒ **保留**] 地址自校验通过：
        //   `nm -S --defined-only ORIG` 得本函数 [0x08323f86, +0x889)，清单
        //   orig_addr 0x08324236/0x0832424b 均落在区间内；两侧上下文同构
        //   （`mov &eventStartTime/%edx; cmp %edx,%eax; jl/jge` ↔ 我方
        //   `mov 0x943bec8,%edx; cmp %edx,%eax; setl` —— 符号性已对齐）。
        //   实测：our 585→583（gap −11→−9，向 ORIG 574 收敛）；TU 四数
        //   176/319/3/11/104（identical/ae/near/diff/missing）逐值不变 ⇒ 不劣 ⇒ 保留。
        static int eventStartTime;
        static int eventEndTime;
        eventStartTime = 0x4589a450;
        eventEndTime = 0x45ae8e50;
        // ORIG 调用两次 getCurCharacCreateTime 后比较 [start,end) 窗口。
        if (user->getCurCharacCreateTime() < eventStartTime)
            break;
        if (user->getCurCharacCreateTime() >= eventEndTime)
            break;
        return 1;
    }

    case 4:
    {
        // 仓库容量精确匹配（0x8323f86 汇编 case4 容量表）。
        const int cap = user->getCurCharacCargoR()->GetCapacity();
        if (itemIdx == 0x3c)
        {
            if (cap == 0x48)
                return 1;
        }
        else if (itemIdx < 0x3d)
        {
            if (itemIdx == 0x39)
            {
                if (cap == 0x18)
                    return 1;
            }
            else if (itemIdx < 0x3a)
            {
                if (itemIdx == 0x32 && cap == 8)
                    return 1;
            }
            else if (itemIdx == 0x3a)
            {
                if (cap == 0x28)
                    return 1;
            }
            else if (itemIdx == 0x3b && cap == 0x38)
            {
                return 1;
            }
        }
        else if (itemIdx == 0x3e)
        {
            if (cap == 0x68)
                return 1;
        }
        else if (itemIdx < 0x3e)
        {
            if (cap == 0x58)
                return 1;
        }
        else if (itemIdx == 0x28978b)
        {
            if (cap == 0x78)
                return 1;
        }
        else if (itemIdx == 0x28978c && cap == 0x88)
        {
            return 1;
        }
        break;
    }

    case 5:
    {
        // 背包/仓库均无该物时 create_item 后 tryInsert 成功返回 1。
        // ORIG 0x8323f86 case5：InvenR×2（check_exist / tryInsert 两侧共两次静态
        // 调用点）、CargoR×1、check_item_exist×2；create_item 直访 m_itemList(+0xc)，
        // tryInsert 走 getCurCharacInvenR（const 视图，无 W 版）。
        // InvenR→check；失败侧再 CargoR→check（P48：结果存 bVar1，非缓存指针）。
        bool invClear = (user->getCurCharacInvenR()->check_item_exist((int)itemIdx) == -1);
        if (invClear)
        {
            bool cargoClear =
                (user->getCurCharacCargoR()->check_item_exist((int)itemIdx) == -1);
            if (cargoClear)
            {
                Inven_Item item;
                item.m_addInfo = (int)itemIdx;   // ORIG uStack_5b = param_3
                G_CDataManager()->m_itemList->create_item(
                    (ENUM_ITEM_CREATE_TYPE)0, item, 1);
                if (user->getCurCharacInvenR()->tryInsertItemIntoInventory(item) >= 0)
                    return 1;
            }
        }
        break;
    }

    case 6:
    {
        // 事件窗口内要求背包/仓库均无 0x65d6..0x65da 任一。
        // ORIG 双 getCurCharacCreateTime（start/end 各自求值）+ 全展开
        // 0x65d6..0x65da × (InvenR+check | CargoR+check)（P48/P183 逐点重发禁缓存；
        // ORIG 静态全展开五组，非循环——调用点差集据此）。
        if (user->getCurCharacCreateTime() < 0x46a7b9d0)
            break;
        if (user->getCurCharacCreateTime() >= 0x46d5de50)
            break;
        // 逐 id 短路链：任一 Inven/Cargo 持有 0x65d6..0x65da 中物品即失败。
        if (user->getCurCharacInvenR()->check_item_exist(0x65d6) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d6) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65d7) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d7) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65d8) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d8) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65d9) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d9) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65da) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65da) == -1)
        {
            return 1;
        }
        break;
    }

    case 8:
        return user->get_charac_level() > 0xe ? 1 : 0;

    case 0xb:
        return user->GetCurCharacSkillTreeIndex() == -1 ? 1 : 0;

    case 0xc:
    {
        // ORIG 0x8323f86 case0xc：G_CEnvironment 两次（+0x1a8 serverType /
        // +0x378 serverGroup 各自取全局，P48 禁缓存）。
        if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
            return 0;
        const int grp = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        if (grp == 0xb || grp == 0xc || grp == 5)
            return 0;
        if (user->getCharacSlotLimit() < 0x15)
            return 1;
        break;
    }

    case 0xd:
    {
        const int cap = user->getCurCharacInvenR()->get_inventory_capacity();
        if (itemIdx == 0x2897c8)
        {
            if (cap == 0)
                return 1;
        }
        else if (itemIdx == 0x2897c9 && cap == 8)
        {
            return 1;
        }
        break;
    }

    case 0x10:
    {
        if (user->get_charac_job() == 9)
            return 0;
        if (user->get_charac_job() == 10)
            return 0;
        return user->verifyStackableItemActionPlace(
            StackableAction::TYPE_CLASS_CHANGE, (unsigned int)itemIdx);
    }
    }

    return 0;
}

