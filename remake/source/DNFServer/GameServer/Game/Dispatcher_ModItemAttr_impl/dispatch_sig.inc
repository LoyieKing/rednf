// b28d-36（修法 P48/P282 + N82 封顶，ORIG 0x08200b08 T size 0xcfc）：
// P282 候选已核——ORIG vptr+0xc 虚调 4 处（0x08200f22/0x08200f39/0x082011b9/
// 0x082012d9），我方旧实现仅 3 处（itype 具名缓存把 0x08200f22+0x08200f39 合并为
// 单次）⇒ 改两次独立 `citem->GetItemType()` 短路比较后 vcall 4/4 与 ORIG 同槽
// 同形。三件套：①p167 调用点差集 102=102 CLEAR；②帧尺寸同 0x1ac；③gap 17
// 全为 4.4 槽复用/声明序（N82）导致的槽号平移（358 行 %ebp 槽差），无 MISSING/
// EXTRA 调用。语义完成，byte 层 N82 封顶。
// [N96 清绑 2026-09-09，本函数 4 处绑定一次清完]
// 删除：`register int ret asm("ebx")` / `register bool bad asm("al")` /
//       `register char* idStr asm("ebx")` / `register int st asm("esi")`
//       （P0 限定② + N96 全域禁绑；P248 尾条：任何情形不得恢复）。
// 还原（按 P0 双推律：ORIG 侧 ret 走 %ebx、bad 走 %al、idStr 走 %ebx、st 走
// %esi，四者均无栈槽）：
//   · ret  → 具名栈槽局部 `int ret;`（ORIG 有统一出口 `mov %ebx,%eax`，原码确有
//            统一返回局部，故取具名槽形而非逐点 return 常量）；
//   · bad  → 具名栈槽局部 `bool bad = (A || B); if (bad)`，4.4 产同构双岛物化
//            （`mov $1,%eax / mov $0,%eax / mov %al,-0x45(%ebp) / cmpb` 对比 ORIG
//            的 `mov $1,%eax / mov $0,%eax / test %al,%al`）；
//   · idStr/st → 右值直传 `tr(fmt, user->get_state(), NumberToString(...))`，4.4
//            产 `call NumberToString; mov %eax,%ebx; call get_state;
//            mov %ebx,0xc(%esp); mov %eax,0x8(%esp)`，与 ORIG 同序同槽位
//            （ORIG 多一条 `mov %eax,%esi` 中转）。
// 残差 = N96+N18a 封顶：bad 的物化结果 4.4 必落栈槽（ORIG 停在 %al），ret 每次
// 返回多 1 条槽读（`movl $0,-0x4c(%ebp); mov -0x4c(%ebp),%ebx` 对比 ORIG
// `mov $0,%ebx`），绑定外不可达。
// 读数（并列）：清绑前 verdict=DIFF orig829/our812 gap17，四数
//   [identical=2 ae=2 合计4 near=0 diff=2]；清绑后 verdict=DIFF orig829/our816
//   gap13，四数 [identical=2 ae=2 合计4 near=0 diff=2]（不变）。
int Dispatcher_ModItemAttr::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ========================================================================
    // ORIG 0x08200b08（报告 docs/class_func_reports/Dispatcher_ModItemAttr/
    // dispatch_sig.md + objdump /tmp/mia.txt 逐条核实）。
    // 该函数是 **单个 try + 两个 catch 子句** 结构，共 19 处抛出：
    //   · 14 处 __cxa_throw(&ENUM_ERROR::typeinfo@0x08b75f40)，载荷
    //     4/4/0x13/4/0x13/0x11/0xd/0x13/0x13/0x13/0x13/0x11/0x16/0x12
    //     （地址 0x08200e4f/e88/ebf/f0d/f77、0x082010af/11a4/11ec/12b3/130c/
    //      134d/13fb/1443/14f9）；
    //   · 5 处 __cxa_throw(&typeinfo(int)@0x0936b1e0)，载荷 0/-1/-2/-2/-3
    //     （0x08200bd6/0d18/126e/13cf/14ab）。
    // catch 子句：@0x08201880 → SendCmdErrorPacket(0x54,(unsigned char)e)；
    //             @0x082018c8 → `cmpl $0x0; jns`：e<0 才 LineFunc(0x9706,...,0,0)，
    //                            e>=0 置 %ebx=0。
    // 旧实现把全部抛出点内联成 `SendCmdErrorPacket(0x54,N); return 0;`（EXTRA
    // 13 次该调用、缺 19+19 次 allocate/throw 与 2 catch），语义近似但结构与
    // ORIG 完全不符（gap 173），现按 ORIG 恢复。为免整函数重排，try 内保持原缩进。
    // ========================================================================
    // [N96 清绑 2026-09-09] 原 `register int ret asm("ebx")` 已删（P0 限定②/N96 全域
    // 禁绑；P248 不得恢复）。ORIG 出口统一 `mov %ebx,%eax`（0x0820192b）+ 各 return
    // 前 `mov $0,%ebx` ⇒ 原码确有统一返回局部；按 P0 还原为具名栈槽局部。残差见判词。
    int ret;
    try {
    // ---- 1. 状态检查 ----
    // ORIG 0x08200b1a..0x08200b41 实证是 **正向 || 物化双岛**，不是普通短路 if：
    //   `call get_state; cmp $0x3; jne L1` / `call getCurCharacR; test; jne L2`
    //   L1: mov $1,%eax ; jmp L3        L2: mov $0,%eax
    //   L3: test %al,%al ; je 0x08200bdb
    // ⇒ ORIG 先物化 bad 到 %al 再单点判跳（技巧40 家族的正向形，与 batch-10 的
    //   `!(A && B)` 三条直分支形不同，不可混用）。
    // 岛内：NumberToString 结果直落 %ebx（0x08200b62 `mov %eax,%ebx`）、第二次
    // get_state 结果直落 %esi（0x08200b6f `mov %eax,%esi`），跨 cMyTrace 两 call
    // 存活（技巧2）。三者均无栈槽 ⇒ 按 P0 双推律原码无 bad/idStr/st 具名局部。
    // [N96 清绑 2026-09-09] 已删 `register bool bad asm("al")` /
    // `register char* idStr asm("ebx")` / `register int st asm("esi")`（P0/N96 全域
    // 禁绑，P248 不得恢复）；bad 还原为具名栈槽局部（保 ORIG 双岛物化结构），
    // idStr/st 还原为右值直传。
    // [N1Free91to105 2026-09-11] 试删具名 `bool bad`（P0：ORIG 三者均无栈槽）：
    // 具名版实测多发 `mov %al,-0x45(%ebp); cmpb $0x0,-0x45(%ebp)`（2 条）+ 帧
    // 次生 -0x45/-0x70 平移。改表达式直判后按 ORIG 双岛物化结构核对。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        // [N1Free91to105 2026-09-11] P227 全表达式临时链：ORIG 0x08200b62/0x08200b6f
        // 把 `NumberToString` 与第二次 `get_state` 的结果直落 %ebx/%esi（跨 cMyTrace
        // 构造 call 保活），构造 call 在实参之后 ⇒ 原码是**单语句** `cMyTrace(...)(...)`，
        // 不是「具名 tr + 调用」两语句（具名版实测构造 call 提前，与 ORIG 逆序）。
        cMyTrace(__PRETTY_FUNCTION__,
                 0x95db, 5)("Dispatcher_ModItemAttr => User State : %d, m_id : %s",
                            user->get_state(),
                            NumberToString(user->get_acc_id(), 0));
        throw 0;                        // ORIG 0x08200bd6 throw int(0)
    }

    // ---- 2. 服务限制 (RestrictCategory=1, v=0xf) ----
    if (ARAD::Singleton<ServiceRestrictManager>::Get()
            ->isRestricted(user, (RestrictCategory::Enum)1, 0xf))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0xd1);
        ret = 0;
        return ret;
    }

    // ---- 3. 安全保护 CSecu_ProtectionField::Check(0x26) ----
    unsigned int secuRet = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x26);
    if (secuRet != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, (unsigned char)secuRet);
        ret = 0;
        return ret;
    }

    // ---- 4. 交易中则直接忽略 ----
    if (user->CheckInTrade())
    {
        ret = 0;
        return ret;
    }

    // ---- 5. 解析包 ----
    unsigned short equipSlot = 0;
    unsigned long itemIdx = 0;
    unsigned short matSlot = 0;
    if (!packet.get_short(equipSlot) ||
        !packet.get_int(itemIdx) ||
        !packet.get_short(matSlot))
    {
        throw -1;                       // ORIG 0x08200d18 throw int(-1)
    }

    // ---- 6. 取目标装备槽项目并校验 ----
    CInventory* inven = user->getCurCharacInvenW();
    // ORIG callee 差集实证：本函数有 2 次 Inven_Item::Inven_Item()（默认构造），
    // 即「先默认构造、再赋值」两语句形（copy-init `Inven_Item t = GetInvenSlot()`
    // 不产生默认 ctor 调用）。
    Inven_Item target;
    target = inven->GetInvenSlot((INVEN_TYPE)1, (int)equipSlot);
    if (target.isEmpty())
    {
        throw (ENUM_ERROR)4;            // ORIG 0x08200e4f
    }
    if ((unsigned int)(unsigned short)target.m_addInfo != (unsigned int)itemIdx)
    {
        throw (ENUM_ERROR)4;            // ORIG 0x08200e88
    }
    if (target.m_itemType != 1)
    {
        throw (ENUM_ERROR)0x13;         // ORIG 0x08200ebf
    }

    CItem* citem = G_CDataManager()->find_item((int)itemIdx);
    if (citem == 0)
    {
        throw (ENUM_ERROR)4;            // ORIG 0x08200f0d
    }
    {
        // b28d-36（P48/P282，ORIG 0x08200f22/+0x431）：ORIG 两次独立虚调 vptr+0xc
        // （GetItemType），首次结果只用于 `cmp $0x9; jle`，短路不成立才第二次
        // `cmp $0x15; jg`；旧实现具名 itype 缓存成单次调用（vcall 计数 4→3）。
        if (citem->GetItemType() < 10 || citem->GetItemType() > 0x15)
        {
            throw (ENUM_ERROR)0x13;     // ORIG 0x08200f77
        }
    }

    // ---- 7. 取材料槽项目 ----
    Inven_Item material;
    material = inven->GetInvenSlot((INVEN_TYPE)1, (int)matSlot);
    if (material.isEmpty())
    {
        throw (ENUM_ERROR)0x11;         // ORIG 0x082010af
    }

    // ---- 8. 装备槽加锁检查 ----
    if (user->CheckItemLock(1, (int)equipSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, 0xd5);
        return 0;
    }

    // ---- 9. 按材料 item idx 选择再铸模式并计算消耗 cost ----
    int resultType = 2;          // local_36：默认「一次性(oneshot)」模式
    int cost = 1;                // local_34：消耗数量（cast 前初值 1）
    const int matIdx = material.m_addInfo;
    if (matIdx == 0xe || matIdx == 0x292090f1)
    {
        // 封印再铸（reroll）：材料=封印卷，逐次加封印次数
        resultType = 1;
        if (target.GetReSealCount() > 6)
        {
            throw (ENUM_ERROR)0xd;      // ORIG 0x082011a4
        }
        if (citem->GetItemType() == 0xb)
        {
            throw (ENUM_ERROR)0x13;     // ORIG 0x082011ec
        }
        cost = G_CDataManager()->m_resealTable.get_reseal_cost(
                   (unsigned char)citem->get_rarity(),
                   (unsigned char)citem->get_grade(),
                   target.GetReSealCount());
        if (cost < 0)
        {
            throw -2;                   // ORIG 0x0820126e throw int(-2)
        }
        if (citem->GetAttachType() != 3)
        {
            throw (ENUM_ERROR)0x13;     // ORIG 0x082012b3
        }
    }
    else if (matIdx == 0x28978e)
    {
        // 一次性再铸（oneshot）
        resultType = 1;
        if (citem->GetItemType() == 0xb)
        {
            throw (ENUM_ERROR)0x13;     // ORIG 0x0820130c
        }
        if (citem->GetAttachType() != 3)
        {
            throw (ENUM_ERROR)0x13;     // ORIG 0x0820134d
        }
        cost = G_CDataManager()->m_resealTable.get_oneshot_reseal_cost(
                   (unsigned char)citem->get_rarity(),
                   (unsigned char)citem->get_grade(),
                   target.GetReSealCount());
        if (cost < 0)
        {
            throw -2;                   // ORIG 0x082013cf throw int(-2)
        }
    }
    else if (matIdx != 0x28d14e && matIdx != 0xf &&
             matIdx != 0x381 && matIdx != 0x292090f2)
    {
        throw (ENUM_ERROR)0x11;         // ORIG 0x082013fb
    }
    // 其余 (0x28d14e / 0xf / 0x381 / 0x292090f2) 保持 resultType=2（一次性）

    // ---- 10. 校验材料数量并扣除 ----
    if (material.get_add_info() < cost)
    {
        throw (ENUM_ERROR)0x16;         // ORIG 0x08201443
    }
    if (!inven->delete_item((INVEN_TYPE)1, (int)matSlot, cost,
                            (eItemDelReason)3, true))
    {
        throw -3;                       // ORIG 0x082014ab throw int(-3)
    }

    // ---- 11. 修改装备并落库 ----
    if (resultType == 1)
    {
        // 封印再铸：标记已用 + 封印次数 +1（上限 7）
        if (target.m_seal != 0)
        {
            throw (ENUM_ERROR)0x12;     // ORIG 0x082014f9
        }
        target.m_seal = 1;
        target.SetReSealCount(
            (unsigned char)std::min<int>(target.GetReSealCount() + 1, 7));
    }
    else
    {
        // 一次性再铸：随机重置附加信息
        target.set_add_info(get_rand_int(0));
    }
    inven->update_item((INVEN_TYPE)1, (int)equipSlot, target);

    // ---- 12. 回包（修改结果） ----
    // ORIG 是该类真实成员 _ZN22Dispatcher_ModItemAttr11_SendResultEP5CUserttt
    // （T 0x08201938），匿名 namespace 自由函数 sSendResult 属 §4.1 发明符号。
    _SendResult(user, (unsigned short)resultType,
                (unsigned short)matSlot, (unsigned short)equipSlot);

    // ---- 13. 会员返还材料（GetReturnItemRate(matIdx) / insertItemIntoInventory） ----
    int rate = 0;
    rate = user->GetPremiumInfo()->GetReturnItemRate(matIdx);
    if (rate != 0)
    {
        int returnQty = (cost * rate) / 100;
        if (returnQty > 0)
        {
            Inven_Item ret = material;
            // ORIG 此处无第三次 Inven_Item::set_add_info（callee 差集：我方多 1 次），
            // 即直接字段写而非 setter。
            ret.m_addInfo = (unsigned int)returnQty;
            int reslot = inven->insertItemIntoInventory(ret, (eItemAddReason)0xe,
                                                        true, false);
            if (reslot > -1)
                user->SendUpdateItemList(CUser::eSendTarget_1,
                                         (ENUM_ITEMSPACE)0, reslot);
        }
    }
    ret = 0;
    return ret;
    }
    // ---- ORIG catch 子句一 @0x08201880：ENUM_ERROR → 0x54 错误包（载荷低字节）----
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x54, (unsigned char)e);
        ret = 0;
        return ret;
    }
    // ---- ORIG catch 子句二 @0x082018c8：`cmpl $0x0,-0x1c(%ebp); jns` ----
    // e<0 才 LineFunc(0x9706, pretty, 0, 0)（第 3/4 参均为 0，非 e）；e>=0 置 0。
    catch (int e)
    {
        if (e < 0)
            ret = LineFunc(0x9706, __PRETTY_FUNCTION__, 0, 0);
        else
            ret = 0;
        return ret;
    }
}