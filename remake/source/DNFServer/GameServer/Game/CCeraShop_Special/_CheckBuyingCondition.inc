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
// [CCeraShop_Special 2026-09-11 本轮 大改] ndiff 297→74（gap +7→+1，our 559→565）：
// ORIG 的 category 归一化与 case 4/0xc/0xd 段全部是 **switch** 形（eax 承接 switch
// 值 + 二分 case 树 + 集中叶块），旧的嵌套 if-else/`||` 链被逐段转写：
//   ① category 段（ORIG 0x083240a1..0x083241d0，首比较 `cmp $0x349,%eax; ja` +
//      `cmp $0x347,%eax; jae` 对合并节点 [0x347,0x349]；case 集合见下）→ switch(itemIdx)；
//   ② case 4 容量表（ORIG 0x08324281..，`cmp $0x3e; jb` 的隐含 case 0x3d）→ switch(itemIdx)；
//   ③ case 0xc 的 serverGroup（ORIG 0x08324386 `cmp $0xb; je`/`$0xc`/`$0x5` + 三独立
//      返回块）→ switch(grp)；④ case 0xd 的 itemIdx 0x2897c8/0x2897c9 → switch(itemIdx)；
//   ⑤ case 3 时间窗（ORIG 0x0832422e..0x08324259 布尔值物化 al + `test;je`）→ 语句
//      表达式包住 `time >= start && time < end`；⑥ case 8 极性改 `<= ? 0 : 1` 的 if 形。
// 负结论（勿重跑）：删 citem register（Nd 97）／`volatile citem`（91）／具名 field10
// （193）／去 `grp` 具名（110）／case 0xb 改 if 形（gap +1→−4）—— 全部反劣已回滚。
bool CCeraShop::_CheckBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                                     unsigned int b, unsigned short c, unsigned char d,
                                     unsigned short e)
{
    // local_20：归一化 category。ORIG 先按 itemIdx 特例置 1/3，再按区间映射。
    int category = 0;

    // [CeraShop12Run 2026-09-11 负结论] 试把 case 6 窗口比较改用与 case 3
    // 同一组函数级 static（ORIG 0x083244b5/0x083244bf 直证）+ 删除 case 3 块内
    // 声明：实测 our 576→580（+4 反劣，四数不变）⇒ 已回滚（声明仍在 case 3 块内）。
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
        // [H9MgrItemLoginCera.CCeraShop 回滚] 本轮删 register 实测 ndiff 318→421
        // 反劣（读 -0x18 具名槽虽与 ORIG 同形，但连带帧/槽体系劣化）⇒ 复原。
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
            // [H9MgrItemLoginCera.CCeraShop 回滚] 具名 selField10 实测 ndiff 反劣 ⇒ 复原内联。
            // [CCeraShop_Special 2026-09-11 本轮重试] switch 化后仍反劣（165→193）
            // ⇒ 维持内联形。
            // [R19-07 ④ 真语义差] ORIG 0x08324069..0x08324087 **无条件**求值
            //   v = ((CEquipItem*)citem)->getAvatarTypeSelect()->at(e).m_field10
            //   （该段不在任何 jcc 的支配域内：唯一后继判断是 **之后**的
            //   0x0832408a `cmpw $0x4,-0x6c(%ebp)` / 0x0832408f `je 83240a1`，
            //   且该 je 只支配 0x08324091/95 的对 v 比较与返回 0 块）；
            //   0x08324087 `mov %eax,-0x14(%ebp)` 是具名局部槽（P0 槽往返）。
            //   旧短路形（`c != 4 && ...at(e)...`）在 c==4 时**跳过**
            //   getAvatarTypeSelect()/at(e)：`at()` 是 vector::at ⇒ 越界必抛
            //   std::out_of_range，ORIG 无越界保护也求值 ⇒ 我方跳过 = 行为分叉
            //   （异常/调用副作用可观察）⇒ 改为先求值再判。
            //   （历史注释中「具名 selField10 ndiff 反劣」仅形态口径，不改本条
            //   语义结论。）
            const int selField10 =
                ((const std::vector<stAvatarTypeSelect_t>*)
                     (((CEquipItem*)citem)->getAvatarTypeSelect()))->at(e).m_field10;
            if (c != 4 && selField10 != 0)
                return 0;
        }
    }

    // 归一化 category（itemIdx 区间/特例映射）。
    // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x083240a1..0x083241d0 是
    // **switch(itemIdx) 的 case 树**，不是嵌套 if-else：
    //   0x083240a1 `mov 0x14(%ebp),%eax` 单次载入后整链复用 eax；
    //   0x083240a4 `cmp $0x349,%eax; ja` + 0x083240af `cmp $0x347,%eax; jae`
    //     正对合并后的 case 节点 [0x347,0x349]（16 个节点中位）；
    //   0x083240ba/bf、0x083240f9/fe、0x08324104 `sub $0x45`、0x08324137/3c
    //     等全部为 case 节点区间判断；
    //   尾部 0x08324193..0x083241c9 为集中 case 块 `movl $imm,-0x1c(%ebp); jmp`。
    // 探针（4.4.7，/tmp/psw.cpp）以 case 集合复现出逐条同形的树 ⇒ 源形为
    // switch。旧嵌套 if-else 链（gap +7 / ndiff 297）的寄存器复用、
    // jcc 极性与共享 case 块差由此而来。
    // case 集合按旧链语义逐条转写（0x2897c9 与 0x2897c8 同为 0xd；
    // 0x28995d/0x28995e/0x2899ab 同为 0x10）。
    switch (itemIdx)
    {
    case 0x32:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x28978b:
    case 0x28978c:
        category = 4;
        break;

    case 0x33:
    case 0x28978a:
        category = 5;
        break;

    case 0x3f:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
        category = 6;
        break;

    case 0x347:
    case 0x348:
    case 0x349:
    case 0xbe:
    case 0xbf:
    case 0xc0:
    case 0x2e4:
    case 0x335:
    case 0x1ee2:
    case 0x3a0:
    case 0x2896cb:
        category = 0xb;
        break;

    case 0x28978f:
        category = 0xc;
        break;

    case 0x2897c8:
    case 0x2897c9:
        category = 0xd;
        break;

    case 0x28995d:
    case 0x28995e:
    case 0x2899ab:
        category = 0x10;
        break;

    default:
        break;
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
        // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x0832422e..0x08324259 为
        // 单一布尔值物化：`mov &eventStartTime,%edx; cmp %edx,%eax; jl L0` +
        // `mov &eventEndTime,%edx; cmp %edx,%eax; jge L0` + `mov $1,%eax; jmp L1;
        // L0: mov $0,%eax; L1: test %al,%al; je <break>` ⇒ 旧「两个独立 break」
        // 形（setle/setg 各一发）应改写为语句表达式（值物化到 al 再 test）。
        if (({ user->getCurCharacCreateTime() >= eventStartTime &&
               user->getCurCharacCreateTime() < eventEndTime; }))
            return 1;
        break;
    }

    case 4:
    {
        // 仓库容量精确匹配（0x8323f86 汇编 case4 容量表）。
        // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x08324281 起同样是
        // switch(itemIdx) 的 case 树：`mov 0x14(%ebp),%eax` 后
        // `cmp $0x3c; je` / `cmp $0x3c; ja` / `cmp $0x39; je` / `cmp $0x39; ja`
        // / `cmp $0x32; je` / `cmp $0x3a; je` / `cmp $0x3b; je` / `cmp $0x3e; je`
        // / `cmp $0x3e; jb`(0x3d) / `cmp $0x28978b; je` / `cmp $0x28978c; je`
        // ⇒ 旧嵌套 if-else 链按 case 集合（0x3d 为隐含 case）转写。
        const int cap = user->getCurCharacCargoR()->GetCapacity();
        switch (itemIdx)
        {
        case 0x32:
            if (cap == 8)
                return 1;
            break;

        case 0x39:
            if (cap == 0x18)
                return 1;
            break;

        case 0x3a:
            if (cap == 0x28)
                return 1;
            break;

        case 0x3b:
            if (cap == 0x38)
                return 1;
            break;

        case 0x3c:
            if (cap == 0x48)
                return 1;
            break;

        case 0x3d:
            if (cap == 0x58)
                return 1;
            break;

        case 0x3e:
            if (cap == 0x68)
                return 1;
            break;

        case 0x28978b:
            if (cap == 0x78)
                return 1;
            break;

        case 0x28978c:
            if (cap == 0x88)
                return 1;
            break;

        default:
            break;
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
        // [CCeraShop_Special 2026-09-11 本轮重试] 去具名 bool（内联到 if）实测
        // ndiff 89→125 反劣（虽然 ORIG 0x08324462/0x0832447d 为 `cmp;jne` 零槽形）
        // ⇒ 维持具名 bool（连带寄存器/对齐体系）。
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
        // [CeraShop12Run 2026-09-11 负结论] ORIG 0x083244b5/0x083244bf 确有
        // `movl $0x46a7b9d0,&eventStartTime; movl $0x46d5de50,&eventEndTime`
        // （与 case 3 同一组符号 0x0943bec8/0x0943becc），但把 case 6 改用该
        // static 形实测 our 576→580（+4 反劣）⇒ 维持立即数形。
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
        // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x0832452f: `cmp $0xe,%eax;
        // setle %al; test %al,%al; je <L1>; mov $0x0,%eax; jmp; L1: mov $0x1,%eax`
        // ——三元 `?:` 在 4.4 只出反向 `setg`（无 setle/test 物化）；改 if/else
        // 语句形。P59/P222 极性：条件按 `<=` 书写。
        if (user->get_charac_level() <= 0xe)
            return 0;
        return 1;

    case 0xb:
        // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x0832455c: `cmp $<A>,%eax;
        // sete %al; test %al,%al; je <T>; mov $0x1,%eax`；实测 if 语句形把
        // our 560→570（gap +1→−4）、ndiff 83→82（仅 −1）⇒ 回滚为三元
        // （4.4 三元给 `sete %al; jmp`，条数少）。
        return user->GetCurCharacSkillTreeIndex() == -1 ? 1 : 0;

    case 0xc:
    {
        // ORIG 0x8323f86 case0xc：G_CEnvironment 两次（+0x1a8 serverType /
        // +0x378 serverGroup 各自取全局，P48 禁缓存）。
        // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x08324386 起 serverGroup
        // 的比较是 **switch(grp)**：`cmp $0xb,%eax; je` / `cmp $0xc,%eax; je`
        // / `cmp $0x5,%eax; je` + 三个独立 `mov $0x0,%eax; jmp` 叶块
        // （0x0832439c/0x083243a4/0x083243ac，块序 0xb→0xc→5 = 书写序）
        // ⇒ 旧 `||` 链转写为 switch（每 case 独立 return 0）。
        if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
            return 0;
        // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x08324386/0x0832438b 为
        // `call; mov 0x378(%eax),%eax; cmp $0xb,%eax`（零槽）；但实测去具名
        // grp（switch 直接求值）ndiff 74→110 反劣 ⇒ 保留具名槽。
        const int grp = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        switch (grp)
        {
        case 0xb:
            return 0;
        case 0xc:
            return 0;
        case 5:
            return 0;
        default:
            break;
        }
        if (user->getCharacSlotLimit() < 0x15)
            return 1;
        break;
    }

    case 0xd:
    {
        const int cap = user->getCurCharacInvenR()->get_inventory_capacity();
        // [CCeraShop_Special 2026-09-11 本轮] ORIG 0x083245a1 起为
        // **switch(itemIdx)**：`mov 0x14(%ebp),%eax` 后 `cmp $0x2897c8,%eax; je`
        // / `cmp $0x2897c9,%eax; je` + 两个独立叶块（0x083245b5/0x083245c1）
        // ⇒ 旧 if/else-if 链转写为 switch（case 体仍各自判 cap）。
        switch (itemIdx)
        {
        case 0x2897c8:
            if (cap == 0)
                return 1;
            break;
        case 0x2897c9:
            if (cap == 8)
                return 1;
            break;
        default:
            break;
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