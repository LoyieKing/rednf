int Dispatcher_UseVendingMachine::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ========================================================================
    // ORIG 0x0821c2e6（报告 docs/class_func_reports/Dispatcher_UseVendingMachine/
    // dispatch_sig.md 全文已读：## 汇编 + ## 反编译 C 两段）。
    // 关键结构证据：
    //  · 出口统一 `mov %ebx,%eax`（0x0821d2b7），各 return 路径先写 %ebx ⇒
    //    技巧39 EH-ebx 返回缓存。
    //  · 0x0821c2f1 先 `call get_state; cmp $3; setne %al; test; je`，其后
    //    0x0821c310 才 `cmpl $0x0,0xc(%ebp); je` + CheckInTrade 合并 bool
    //    （旧实现把 user==0 放最前，顺序错）。
    //  · 9 处 __cxa_allocate_exception(4) + __cxa_throw(&ENUM_ERROR::typeinfo=
    //    0x8b75f40) —— 载荷值 1/1/4/0x16/0x16/0x16/0x11/0x16/0x16（旧实现退化成
    //    return 0，既少 ~150 行指令又丢错误码上报，属语义缺陷）。
    //  · 两处 catch handler（0x0821c75b try[56c..6c9] / 0x0821ce0e try[c8bb..cd3e]）
    //    同体：`mov (%eax),%eax` 取 ENUM_ERROR → `movzbl %al` →
    //    SendCmdErrorPacket(0xda, (unsigned char)e) → %ebx=0 → 公共出口。
    //  · 空位检查 ORIG 是 7 次独立 check_empty_count 嵌套 if（0x0821c8bb.. 逐段），
    //    不是循环。
    // ========================================================================
    // b28d-solo-Dispatcher_UseVendingMachine_impl [N82+N7]：ORIG 1009 insn / 我方 998
    //   （gap 11）；p167 调用点差集 CLEAR 112/112（0 缺 0 多）⇒ 语义完成、byte 层
    //   封顶（§6.2）。残差=全帧槽号平移（我方帧 0x210 vs ORIG 0x1c0，N82：4.1/4.4
    //   槽复用策略差）+ 7 处虚调用 vptr 链目标寄存器（N7，本 TU 未路由 446）+
    //   PacketGuard 段循环变量槽错位。
    //   本轮修真语义 bug：空位检查极性反置——旧 `else invFull = false;` 使
    //   check 失败时不抛 4；ORIG 0x0821c8c5 `jne FAIL`→FAIL `mov $1,%eax`→
    //   `test/je` 抛出 ⇒ 失败必须保持 invFull=true。
    // P0/N96：原 `register int ret asm("ebx")` 显式绑定已清除；纯 register
    // 存储类提示（P231）合法，用于复现 ORIG 的 %ebx 返回缓存形。
    register int ret;
    if ((bool)(user->get_state() != 3))
    {
        ret = 0;
        return ret;
    }
    if (user == 0 || user->CheckInTrade())
    {
        ret = 0;
        return ret;
    }
    // ORIG 0x0821c33f：三个输入局部在 user 检查之后才清零。
    unsigned int machineId = 0;
    unsigned int slotId    = 0;
    short        pos       = -1;
    if (!packet.get_int(machineId))
        return LineFunc(0xd029, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(slotId))
        return LineFunc(0xd02a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(pos))
        return LineFunc(0xd02b, __PRETTY_FUNCTION__, 0, 0);

    const stVendingMachine_t* vm = _getVendingMachine(machineId);
    if (vm == 0)
        return LineFunc(0xd02f, __PRETTY_FUNCTION__, 0, 0);
    const stVendingMachineInfo_t* info = _getVendingMachineInfo(vm, slotId);
    if (info == 0)
        return LineFunc(0xd033, __PRETTY_FUNCTION__, 0, 0);

    // ORIG 0x0821c4b2：三个只写不读的整型死局部（1/1/2）照抄（P171/P232）。
    int deadA = 1, deadB = 1, deadC = 2;
    (void)deadA; (void)deadB; (void)deadC;
    // 给/赏物品数组（ORIG 0x0821c520：数组默认构造 2 次 + 逐元素写 +0x02=-1，
    // 无 Inven_Item::reset 调用；local_163 生命周期覆盖两 try 之后的 _putItemIntoUser）。
    Inven_Item give[2];
    for (int slot = 0; slot < 2; ++slot)
        give[slot].m_addInfo = -1;
    // ORIG 0x0821c4c7：find_item 结果按 slot 索引保存的 2 元素指针数组（初值 0）。
    CItem* found[2] = {0, 0};
    // ORIG 有**两个**独立 catch handler：try1[0x0821c56c..0x0821c6c9]（给物品抽取，
    // catch @0x0821c75b）、try2[0x0821c8bb..0x0821cd3e]（空位检查 + 扣款，
    // catch @0x0821ce0e），两者同体。赏物品抽取（_getBonusItem）落在两 try 之间
    // （0x0821c7b6..），故不可合并成单个 try。
    try {
        for (int slot = 0; slot < 1; ++slot)
        {
            unsigned int retry = 0;                 // -0x30
            do
            {
                if (retry > 2)
                    throw (ENUM_ERROR)1;            // ORIG 0x0821c56c
                give[slot] = _getGiveItem(info);
                if (give[slot].m_addInfo == -1)
                    throw (ENUM_ERROR)1;            // ORIG 0x0821c672
                found[slot] = G_CDataManager()->find_item(give[slot].m_addInfo);
                register bool redo = false;
                if (found[slot] != 0 && found[slot]->GetItemType() == 0x17 &&
                    GlobalData::s_globalEffectManager_->getEffectNum() != 0)
                    redo = true;
                if (redo)
                {
                    give[slot].m_addInfo = -1;
                    give[slot].m_addInfo2 = 0;
                    ++retry;
                }
            } while (give[slot].m_addInfo == -1);
        }
    } catch (ENUM_ERROR e) {
        // ORIG catch handler 0x0821c75b（e 存 -0x2c）
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xda, (unsigned char)e);
        ret = 0;
        return ret;
    }
    // ORIG 0x0821c7b6：remainingMoney 在赏物品抽取前清零（声明在此处）。
    int remainingMoney = 0;
    for (int slot = 1; slot < 2; ++slot)
        give[slot] = _getBonusItem(info);

    try {
        // 空位检查：ORIG 逐背包区段 7 次 check_empty_count(zone, 2)（0x0821c8bb..），
        // 全部 ==1 才继续查货架 GetRemainCapacity(2, 8)。
        register bool invFull = true;
        // [b23S3C] ORIG 0x0821c8bb.. 七次 check 处为 `call; xor $1,%eax; test
        // %al,%al; jne <T>`（ORIG 0x0821c8c0/8ee/91c/94a/978/9a2/9cc 七处 xor
        // 实读；7 个 jne 共享同一失败落点）。嵌套 `if (check==1){…}` 恒
        // `test %al,%al; je`（无 xor）；`if(!c) goto` 虽出 xor+jne 但每个 goto
        // 多 `jmp;nop` 跳板（998→1019 实测劣化）。正解 = **空 then + else 承载
        // 后续**（e9 探针实证）：`xor;jne` 直跳、无跳板、每处 +1 条。
        if (!static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)1, 2)) { }
        else {
        if (!static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)2, 2)) { }
        else {
        if (!static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)3, 2)) { }
        else {
        if (!static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)10, 2)) { }
        else {
        if (!static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)9, 2)) { }
        else {
        if (!static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)5, 2)) { }
        else {
        if (!static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)6, 2)) { }
        else {
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->GetRemainCapacity(
                    (INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) > 1)
                invFull = false;
        }
        }
        }
        }
        }
        }
        }
        if (invFull)
            throw (ENUM_ERROR)4;                    // ORIG 0x0821ca32

        if (info->m_payType == 0)
        {
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->use_money(
                    info->m_payAmount, (eMoneySubReason)0x1c, 1) != 1)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821ca9e
            remainingMoney =
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->get_money();
        }
        else if (info->m_payType == 1)
        {
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()->getTotalCoin() <
                    (unsigned int)info->m_payAmount)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cb16
            for (unsigned int i = 0; i < (unsigned int)info->m_payAmount; ++i)
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->UseCoin(
                    (eCoinSubReason)6);
            remainingMoney =
                (int)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()->getTotalCoin();
        }
        else if (info->m_payType == 2)
        {
            if (static_cast<CUserCharacInfo*>(user)->GetWinPoint() <
                    (unsigned int)info->m_payAmount)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cbba
            user->UseWinPoint(info->m_payAmount, (eWPSubReason)4);
            remainingMoney = static_cast<CUserCharacInfo*>(user)->GetWinPoint();
        }
        else
        {
            Inven_Item payItem =
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                    ->GetInvenSlot((INVEN_TYPE)1, pos);
            if (payItem.m_addInfo != info->m_payType)
                throw (ENUM_ERROR)0x11;             // ORIG 0x0821cc59
            // [轴B] ORIG 0x0821cc6c 为有符号 `jge`（cmp %eax,%edx），而
            // m_fieldd 为 unsigned、m_payAmount 为 int ⇒ 4.4 默认产无符号
            // `jae`。按 ORIG 符号性显式转 int（P196：ORIG 该点有符号直证）。
            if ((int)payItem.m_extendInfo < info->m_payAmount)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cc95
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->delete_item(
                    (INVEN_TYPE)1, (int)pos, info->m_payAmount,
                    (eItemDelReason)0x1c, 1) != 1)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cd0a
            // ORIG 0x0821cd3a：第二次取槽结果整体赋回 payItem（逐字段拷贝），
            // 再从 payItem.m_extendInfo 取剩余数量。
            payItem = static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                          ->GetInvenSlot((INVEN_TYPE)1, pos);
            remainingMoney = payItem.m_extendInfo;
        }
    } catch (ENUM_ERROR e) {
        // ORIG catch handler 0x0821ce0e（e 存 -0x20）
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xda, (unsigned char)e);
        ret = 0;
        return ret;
    }

    GetInstanceVendingMachineStatistic()->incUseCount();   // ORIG 无空检查

    // 输出
    stPutItemResult_t result[2];
    for (int i = 0; i < 2; ++i)
        _putItemIntoUser(user, give[i].m_addInfo, give[i].m_addInfo2, result[i]);

    // ORIG 0x0821cee0 区：单元素循环 `for (i=0; i<1; ++i)` 内 ——
    // 失败时走 `vector::begin()` + `__normal_iterator::operator*` 取表首（不是
    // empty()/front()），再 _putItemIntoUser；随后按 (itemIdx != -1 && count != 0)
    // 触发 _notifyItemInfo。
    for (int i = 0; i < 1; ++i)
    {
        if (result[i].m_success != 1)
        {
            stVendingMachineOutput_t fallback = *info->m_vecGive.begin();
            _putItemIntoUser(user, fallback.m_itemId, fallback.m_count, result[i]);
        }
        if (result[i].m_itemIdx != 0xffffffffUL && result[i].m_count != 0)
            _notifyItemInfo(info, user, result[i]);
    }

    // 回包：header 0xda
    PacketGuard resp;
    resp.put_header(1, 0xda);
    resp.put_byte(1);
    resp.put_short(pos);
    resp.put_int(remainingMoney);
    // ORIG 0x0821d0xx：两段单元素循环（`for(i=0;i<1)` 与 `for(i=1;i<2)`）而非
    // 一个 2 次循环 —— 4.1 里这两组 call 各自展开，合并循环会少一次
    // put_item_idx/put_int。
    for (int i = 0; i < 1; ++i)
    {
        resp.put_item_idx(result[i].m_itemIdx);
        resp.put_int((int)result[i].m_count);
    }
    for (int i = 1; i < 2; ++i)
    {
        resp.put_item_idx(result[i].m_itemIdx);
        resp.put_int((int)result[i].m_count);
    }
    int idx = resp.get_index();
    int cnt = 0;
    resp.put_short(0);
    for (int i = 0; i < 2; ++i)
    {
        // ORIG 0x0821d135..0x0821d204（objdump 权威，result[0] 基址 -0xa8、步长 0x10）：
        //   +0x00 int  != -1      → m_itemIdx      (0x0821d146 cmp $0xffffffff)
        //   +0x0c WORD != 0xffff  → m_slot         (0x0821d162 movzwl 0xc;cmp $0xffff)
        //   +0x0e byte == 1       → m_success      (0x0821d181 movzbl 0xe;xor $1;test)
        //   0x0821d19c `sub $0x98` = **+0x08 m_result** → put_byte(m_result)
        //   0x0821d1d6 同 +0x08 → GetInvenTypeFromItemSpace(m_result) 作 MakeItemPacket
        //   第一实参（InvC10 实证该函数全 ORIG 仅一重载：
        //   0x084fc6bc (INVEN_TYPE,int,PacketGuard&) const）。
        if (result[i].m_itemIdx != 0xffffffffUL && result[i].m_slot != -1 &&
            result[i].m_success == 1)
        {
            resp.put_byte(result[i].m_result);                    // 0x0821d1ad
            const int invenType =
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)result[i].m_result);  // 0x0821d1e0
            static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                ->MakeItemPacket((INVEN_TYPE)invenType, (int)result[i].m_slot, resp);
            ++cnt;
        }
    }
    resp.put_short(idx, cnt);
    resp.finalize(true);
    user->Send(resp);
    APSystem::CUserProc::ClearActionAndSendtoUser(
        user, (APSystem::_ActionGroupIndex)0x27, 0, (ENUM_PACKETCLASS)0);
    return 0;
}