// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N89/N93附）→ 试=新口径 n1_v 431/423 gap=+8 blocks=60；缺口全在 ORIG 独有段（0x086c15b5/0x086c18cd 两处 `cmpl $0xffffffff,槽; sete %al; test %al,%al; je` 物化 ×2 + bool 归并 5 条 + 实参装载 4/5 条差），4.4.6 后端复编更差（431/422，gap +9）⇒ 维持。
unsigned int WongWork::CWebEvent::RecvInfoFromDB(CUser* user, unsigned int itemId,
                                                 unsigned int count, unsigned int eventIdx,
                                                 unsigned int extra)
{
    // [b28d-solo-W6_SecMiscDeps 判词 · 语义完成 / byte 层 N 封顶（N82/N89/N93附）]
    // ORIG 0x086c121e：§6.2 三件套 —— ①gap 8（431/423）；②p167 调用点差集
    // **CLEAR 52=52**（修前 MISSING operator*×2/operator<<(int)×2/G_CDataManager×1，
    // EXTRA operator<<(uint)×2/CGenUniqueNo::C1×1/~CStreamGuard×1/getItemList×1）；
    // ③残差为帧/槽号整体平移 0x20（N82）+ 返回值常量岛物化（N93附）+ STL 内联差
    // （N89）。本轮共修 4 处真 bug（见各分支注释）：
    //   ①0x086c1491 equip 分支 `jbe` 无符号比较（P196，旧 `(int)count>1` 生 jle）；
    //   ②0x086c14ec `_GetATIAvatarAbility` 实参来源 = 形参 itemId（非 slot 槽）；
    //   ③ProcessSpecialItem 结果内联进 if（P0：ORIG 无 bool 槽，旧 `const bool ok`
    //     多发 `mov %al,-N; cmpb $0,-N` 两步）；
    //   ④`return codeStr == 0;` 改早返回 `return 1/0`（复现 ORIG 两臂 `mov $1/%ebx`
    //     常量岛，消 sete+movzbl 归一化）。
    // [M2FreeNext15A · 2026-09-11 复核] 已按新范式重核：p167 差集仍 CLEAR
    // （MISSING/EXTRA 全为 C1/C2·D1/D2 同址别名，P93 良性）；槽集合差 = 我方
    // 浅区多 0x20、深区整体下移（vptr 临时槽 ORIG -0xec → 我方 -0x10c），帧
    // ORIG 0x13c / our 0x15c；char countStr/uniqueNoBuf/agencyName 未对齐槽
    // （-0x55/-0xbc/-0xf9…）说明我方在浅区多占 3 字节对象且数组清零形为逐字节。
    // P339（全头部声明）不宜套用：数组 `= {0}` 提头会提前发射清零（改可观察
    // 行为/指令序），非 POD 对象（guard/Inven_Item/vector/paramObj）禁提（P331）。
    // ⇒ 维持 N82/N89/N93附 封顶。
    // [N1A 2026-09-11 复核 · 维持封顶] gap +8 的缺口全在 ORIG 独有段：
    // ①0x086c15b5 / 0x086c18cd 两处 `cmpl $0xffffffff,槽; sete %al; test %al,%al;
    //   je`（各较我方 `cmpl; jne` 多 2 条）——判据库 asm_translation_patterns.md
    //   l.1441-1443「4.4-O0 槽比恒出 cmp/jne 直跳、唯 call 结果寄存器直比出 sete；
    //   源形不可复现」⇒ 禁改内联比较；
    // ②ORIG[346:351] `test %esi,%esi; jne; mov $0x0,%esi; jmp; mov $0x1,%esi`
    //   （bool 归并 5 条，ORIG 用寄存器承载 bVar12）与 ③实参装载 4/5 条差同族。
    // 反证：4.4.6 后端复编实测更差（431/422，gap +9）⇒ 非编译器轴，属 N 族封顶。
    // ORIG T 0x086c121e (size 0x6f1)：
    if (itemId == 1)
    {
        // ---- 活动币入账分支（asm +0x16..+0x97）----
        // [b28d-solo-W6_SecMiscDeps] P0/P52：ORIG prev/now 无槽（%ebx/%eax 直传
        // SetEventCoin/EventCoinAdd），旧具名局部多 6 条槽往返。
        user->getCurCharacInvenW()->SetEventCoin(
            user->getCurCharacInvenR()->GetEventCoin() + count);
        user->m_historyLog.EventCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), (int)count,
            (eCoinAddReason)2);
        user->SaveMoneyCoin();
        user->SendCoin();
        return 1;
    }
    if (itemId == 999)
    {
        // ---- IPG 计费分支（asm +0xa9..+0x207）----
        char countStr[0xc];
        // 真 bug 修正：ORIG 086c12c7 读 0x10(%ebp)=**count**（非 extra）作 %d。
        OS_API::snprintf(countStr, sizeof(countStr), "%d", (int)count);
        // P259：Acquire 结果内联进构造实参（ORIG 无 stream 局部槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("WebEvent.cpp", 0x47), false);
        // P103/P132：ORIG 调 operator*()->operator<<(int) 链（非 CStreamGuard::operator<<
        // 转发；p167 差集 MISSING operator*×2/operator<<(int)×2，EXTRA operator<<(uint)×2）。
        guard.operator*()->operator<<((int)eventIdx);
        guard.operator*()->operator<<((int)extra);
        char uniqueNoBuf[0x20];
        const char* codeStr = _GetEventIPGCode((int)eventIdx);
        // ORIG 单析构出口（086c141a 被两路径汇入）；GenUniqueNo 为 static（p167
        // EXTRA CGenUniqueNo::C1 证实旧 `CGenUniqueNo generator;` 多余）。
        if (codeStr == 0)
        {
            GlobalData::s_stream_pool->Free(guard);   // asm +0x156
            return 1;                     // asm +0x15b `mov $0x1,%ebx`
        }
        GlobalData::s_pIPGHelper->IPGInput(user, codeStr, countStr,
                               CGenUniqueNo::GenUniqueNo(uniqueNoBuf),
                               _GetEventString((int)eventIdx),
                               user->getCurCharacName(), guard.Get());
        return 0;                         // asm +0x1da `mov $0x0,%ebx`
    }

    // ---- 物品发放分支（asm +0x20c..+0x6ef）----
    // ORIG 无 mgr 具名槽：两次独立 `call G_CDataManager`（086c142d / 086c1647）。
    CItem* item = G_CDataManager()->find_item((int)itemId);
    if (item == 0)
    {
        return 0;
    }
    Inven_Item slot;
    item->make_item(slot);
    // 块序照 ORIG：`cmp $1,%al; je <equip 块>` —— 非 equip 直落（P222 正向包裹）。
    if (slot.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
    {
        slot.m_addInfo2 = (int)count;     // ←item+0x07（asm -0xde 槽）
    }
    else
    {
        // asm +0x273..+0x29b：equip 类且 count>1 才回写数量到 item+0x07。
        // [b28d-solo-W6_SecMiscDeps 真 bug 修正 · P196] ORIG 086c1491
        // `cmpl $0x1,0x10(%ebp); jbe` 是**无符号**序（count 为 unsigned 形参）；
        // 旧 `(int)count > 1` 生有符号 `jle`，count ≥ 0x80000000 时语义分叉
        // （ORIG 写回数量、旧码不写）⇒ 按 P196 改无符号比较。
        if (count > 1)
        {
            slot.m_addInfo2 = (int)count;
        }
    }
    slot.m_addInfo = (int)itemId;         // ←item+0x02（asm -0xe3 槽）

    int spaceCode = 0xd;
    if (eventIdx == 0x186a1)
    {
        spaceCode = 0xe;
    }
    else if (eventIdx == 0x30d46)
    {
        spaceCode = 0x14;
    }

    if (item->IsAvatarItem())
    {
        // ---- Avatar 分支（asm +0x2ce..+0x3cf）----
        // 真 bug 修正：ORIG AddAvatarItem 实参为 (itemId,0,false,ability,-1,agency,2,false,0)
        //（086c158e `mov %esi,0x10(%esp)`=char c、086c1586 `movl $0xffffffff,0x14(%esp)`=char d）。
        // [b28d-solo-W6_SecMiscDeps · 实参来源判据] ORIG 086c14ec 读 `0xc(%ebp)`
        // = 形参 itemId（非 slot.m_addInfo 槽 -0xe3）；AddAvatarItem 第一参才用
        // slot.m_addInfo（086c1554 `mov -0xe3(%ebp),%eax`）⇒ 两处来源照抄 ORIG。
        const int ability = _GetATIAvatarAbility((int)itemId);
        char agencyName[0x14] = {0};   // ORIG 5×movl $0 清零（P214③）
        OS_API::snprintf(agencyName, 0x14, "E_%s",
                         _GetEventString((int)eventIdx));
        CInventory* invW = user->getCurCharacInvenW();
        int avatarSlot = -1;   // ORIG `movl $0xffffffff,-0x28`（P198① 预清两句式）
        avatarSlot = invW->AddAvatarItem((int)slot.m_addInfo, 0, false,
                                         (char)(signed char)ability, (char)-1,
                                         agencyName,
                                         (eAvatarItemAddReason)2, false, 0);
        if (avatarSlot == -1)
        {
            return 0;
        }
        user->SendUpdateItemList(CUser::eSendTarget(1), ENUM_ITEMSPACE(1),
                                 avatarSlot);
        return 1;
    }

    int outType = 0;
    int outValue = 0;
    bool flag = false;
    if (GlobalData::s_pCeraShop->IsSpecialItem((int)slot.m_addInfo, outType, outValue, flag))
    {
        // ---- 商城特殊物品分支（asm +0x417..+0x5d1）----
        // 真 bug 修正：ORIG 第 3 参（count 形参）= GetRestrictCode 结果(-0x24)，
        // 第 4 参（int& restrictCode）= 预清 0 的独立输出局部(-0x84)；旧码把
        // GetRestrictCode 结果直接当 restrictCode& 传、count 当第 3 参，两处错位。
        int outRestrict = 0;   // ORIG 086c1635 `movl $0x0,-0x84` 预清
        const int restrictCode =
            G_CDataManager()->m_itemList->GetRestrictCode((int)slot.m_addInfo);
        const long curSec = (long)GlobalData::s_systemTime_.getCurSec();
        std::vector<Inven_Item> attached;
        CCeraShop::stCeraShopItemParam_t paramObj;
        // [b28d-solo-W6_SecMiscDeps · P0 槽对应律] ORIG 086c16fd 直接
        // `test %al,%al; je`（无 bool 槽）⇒ 原码把调用内联进 if 条件；
        // 具名 `const bool ok` 使 4.4 多发 `mov %al,-0x19; cmpb $0,-0x19` 两步。
        if (GlobalData::s_pCeraShop->ProcessSpecialItem(
                user, (int)slot.m_addInfo, restrictCode, outRestrict, curSec,
                true, attached, paramObj, '\0', '\0', false, 1))
        {
            if (outRestrict != 0)
                return 0;   // ORIG 086c170b：ebx=0 经共享析构出口返回 0
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                GlobalData::s_pCeraShop->SaveFeaturedIdx(user, restrictCode,
                                             (int)curSec, 0);
            }
        }
        return 1;   // asm +0x55e：ProcessSpecialItem 失败仍 esi=1 → 成功出口
    }

    // ---- 普通背包插入分支（asm +0x5d6..+0x6ea）----
    // [R17a13] ORIG 0x086c17f4 `movl $0xffffffff,-0x1c(%ebp)` 先于 call ⇒ 原码有
    //   `= -1` 预置（P171 死写入照抄；我方旧形缺该 store，hints CONST O-only -1×1）。
    int newSlot = -1;
    CInventory* invW = user->getCurCharacInvenW();
    newSlot = invW->insertItemIntoInventory(
        slot, (eItemAddReason)spaceCode, true, false);
    if (newSlot == -1)
    {
        return 0;
    }
    user->SendUpdateItemList(CUser::eSendTarget(1), ENUM_ITEMSPACE(0),
                             newSlot);
    return 1;
}