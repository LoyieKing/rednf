// [b28d-L8] N82 | 帧差 0x120 vs 0x110 + 槽平移（ORIG 0x08555cxx）
// [b28d-solo-CMailBoxHelper 本波] 修复（463→462 insn，m_type 段全等）：
// ①P0/P183 删 `MailMsgSendV* m` 具名局部（ORIG 每处直读形参槽
//   `mov 0xc(%ebp),%eax`）⇒ 改宏 MSGV 直读；
// ②P0 补 `unsigned int fee = MSGV->m_money*5;`（ORIG 0x08555b6b
//   `mov %eax,-0xfc(%ebp)` 具名局部 + mul/shr 无符号除法）；
// ③P46/P251/P286 m_type 的 if-else 链改 switch(0,1,7)（ORIG 0x0855c44
//   `movzbl 0x36(%eax),%eax; movzbl %al,%eax; cmp $1; je; cmp $7; je;
//   test; jne` 决策链+叶块序；if 链 4.4 落 `mov %al,-0xd; cmpb` 槽化）；
// ④P190/P31 `int framePad; (void)framePad;` 撑帧 4（ORIG tax@-0x28 对齐）。
// 残余 N：N18a 中转（ORIG `mov 0x2e(%edx),%edx; mov %edx,%ecx` 4.4 折单段）、
// 实参装载序（std::min 两实参 P75）、bool 物化岛与深区槽位（N82/N98）；
// p167 CLEAR 38/38，462 vs 454。
// [2026-09-11 本轮] gap −8 → **−3**（457/454）：拆 `isEquipableItemType()!=1 &&
// m_addInfo2<addInfo` 的 `&&`（ORIG 08555cfe/08555d0a 两条独立守卫同落跳过点）
// 消 5 条短路物化。残余（N 封顶）：①fee 初始化多一枚 -0x10 临时槽往返（2 条，
//   P0 具名局部族；直构 `unsigned fee(...)` 形实测同，不改）；②N18a 两段中转
//   （ORIG 08555b3f `mov 0x2e(%edx),%edx; mov %edx,%ecx`）；③`switch(proto->
//   GetAttachType())` ORIG 出跳表 `mov 0x8ca13bc(,%eax,4)`，4.4.6 恒发决策树
//   （P163/N，case 集稀疏）；④尾部 `return 10` 与 `xor $0x1` 块序（P194/N104）；
//   ⑤帧槽 -0x1c/-0x10 局部集差 1（N82/P112）。
int CMailBoxHelper::ReqDBSendNewMail(CUser* user, MSG_MAILBOX_SEND* msg,
                                     unsigned int nCharacNo, bool bA,
                                     unsigned int nB, int nC)
{
    // ORIG 0x08555b14：发信核心（金币/物品/AVATAR/CREATURE 校验、扣费、DB 请求）。
    // MSG_MAILBOX_SEND 为 opaque（PacketTypes.h m_pad[0x100]），按 ORIG 偏移访问：
    //   +0x11 收件角色名, +0x2e 金币额, +0x36 物品类型选择, +0x37 槽位,
    //   +0x39 item id, +0x3d add_info/目标角色, +0x41 标题长度, +0x45 标题。
    // 返回码：0=成功 1=物品不存在 0x10=金币不足 0x11=校验失败 0x17=类型/绑定不符。
    // [b28d-solo-CMailBoxHelper] P0/P183：ORIG 无 `m` 具名局部（每处直读形参
    // 槽 `mov 0xc(%ebp),%eax`，0x08555b28/0x08555b43/…）⇒ 删除局部改宏直读。
    // [b28d-solo-CMailBoxHelper] P190/P31 探针：ORIG 槽深 4（tax -0x28 vs
    // -0x24）；double framePad 实测撑帧 +8 过头，改 int dead 局部。
    int framePad;
    (void)framePad;
#define MSGV (reinterpret_cast<MailMsgSendV*>(msg))
    if (MSGV->m_addInfo < 0)
        return 0x11;

    int tax = 1000;
    if (MSGV->m_itemIdx == 0)
        tax = 100;
    if (MSGV->m_money != 0)
    {
        // [b28d-solo-CMailBoxHelper] P0：ORIG 0x08555b6b `mov %eax,-0xfc(%ebp)`
        // 具名局部（m_money*5，unsigned ⇒ mul/shr 无符号除法）。
        unsigned int fee = MSGV->m_money * 5;
        tax = fee / 100 + tax;
        if (MSGV->m_itemIdx == 0)
            tax = std::min(0x2774, tax);
        else
            tax = std::min(0x2af8, tax);
    }
    if (user->getCurCharacMoney() < MSGV->m_money + tax)
        return 10;

    CInventory* inven = user->getCurCharacInvenW();
    int invenType = 6;
    Inven_Item found;      // 槽内物品（GetInvenSlot 返回）
    Inven_Item sendCopy;   // 待发副本

    if (MSGV->m_itemIdx != 0)
    {
        // [b28d-solo-CMailBoxHelper] P46/P251/P286：ORIG 0x0855c44 段为
        // `movzbl 0x36(%eax),%eax; movzbl %al,%eax; cmp $1; je; cmp $7; je;
        // test; jne` 决策链 + 叶块 0/1/7 序 ⇒ switch（无具名 mType 局部槽；
        // if-else 链 4.4 落 `mov %al,-0xd; cmpb` 槽化形）。
        switch (MSGV->m_type)
        {
        case 0:
            invenType = 1;
            break;
        case 1:
            invenType = 2;
            break;
        case 7:
            invenType = 3;
            break;
        default:
            return 0x17;
        }

        found = inven->GetInvenSlot((INVEN_TYPE)invenType, MSGV->m_slot);

        if (found.m_addInfo != MSGV->m_itemIdx)
            return 0x11;
        if (found.m_itemType == 4)
            return 0x17;
        // [2026-09-11] P222/P133：ORIG 08555cfe `xor $0x1; test %al; je <跳过>`
        // + 08555d0a `cmp; jge <跳过>` 为两条独立守卫（同落跳过点）⇒ 拆 `&&`
        // （4.4 短路物化多 `mov $1/jmp/mov $0/test/je` 5 条）。
        if (found.isEquipableItemType() != 1)
        {
            if (found.m_addInfo2 < MSGV->m_addInfo)
                return 0x11;
        }

        CItem* proto = G_CDataManager()->find_item(found.m_addInfo);
        if (proto == 0)
            return 1;
        if (proto->isPackagable() != 1)
            return 0x17;

        // [2026-09-10 D06 · 轴 B 反向试验] ORIG 0x08555de4 `cmp $0x7,%eax; **ja**`
        // （无符号跳表边界，default = `mov $0x17,%eax`）+ `mov <table>(,%eax,4); jmp *%eax`
        // 跳表；实测 `switch ((unsigned int)…)` 形使 hunks 40→19 但 **chg 110→110 未改善**
        // （4.4 对该稀疏 case 集恒发决策树 `cmp $0x3/$0x1`，不发跳表）⇒ 按协议回滚，
        // 归 N（switch lowering 差异；只有 case 连续时才可能出跳表）。
        switch (proto->GetAttachType())
        {
        case 1:
        case 2:
            return 0x17;
        case 3:
            if (found.m_seal != 1)   // 需数量/标志为 1（ORIG 逐字节校验）
                return 0x17;
            break;
        case 7:
            if (!DecreaseTradeLimitCount(found))
                return 0x17;
            user->m_historyLog.ChangeItemLimitCount(invenType, MSGV->m_slot,
                                                    found.m_addInfo, -1, 3);
        }

        if (found.m_upgradeSep.IsTradeRestriction())
            return 0x17;

        found.setCopy(sendCopy);
        if (sendCopy.isEquipableItemType() != 1)
            sendCopy.set_add_info(MSGV->m_addInfo);

        if (proto->IsAvatarItem())
        {
            // ORIG 8555ef2/8555f19：IsTempKey/GetExpireDate 各点重取 GetAvatarItemMgrR()（P48 去缓存）
            if (inven->GetAvatarItemMgrR()->IsTempKey(found.m_addInfo2))
                return 0x11;
            if (inven->GetAvatarItemMgrR()->GetExpireDate(found.m_addInfo2) != 0)
                return 0x11;
        }
    }

    // [W13G 2026-09-11] ORIG 0x08555f5f 实证：本处为**负条件 + 早期返回**形 ——
    //   `call use_money; xor $0x1,%eax; test %al,%al; je 0x08555f70（继续）;
    //    mov $0xa,%eax; jmp 0x085561a6（函数尾统一返回）`。
    //   旧正条件形 `== 1` 使 4.4 直跳 then 块（成功 fall-through）⇒ 本处缺 1 条
    //   `xor $0x1`（bitscan 报 ORIG-only `xor $0x1(1)`）且基本块序与 ORIG 相反。
    //   同一函数的第二处 use_money（ORIG 0x08555fbc = `!= 1` 形）已产 xor ⇒ 同形改写。
    //   语义等价：`if (x==1) {A} else return 10` ≡ `if (x!=1) return 10; {A}`。
    if (inven->use_money(tax, (eMoneySubReason)0x10, 1) != 1)
        return 10;
    {
        if (MSGV->m_money != 0)
        {
            user->GetGoldControl()->SetMailCharName(MSGV->m_receiverName);
            if (inven->use_money(MSGV->m_money, (eMoneySubReason)0x11, 1) != 1)
                return 10;
        }

        if (MSGV->m_itemIdx != 0)
        {
            if (found.m_itemType == 8)   // AVATAR
            {
                if (inven->delete_item((INVEN_TYPE)2, MSGV->m_slot,
                                       1, (eItemDelReason)2, 1) != 1)
                    return 0x11;
                inven->SendAvatarEvent(0x36, found.m_addInfo2, 2,
                                       MSGV->m_slot + 10, 0);
                DB_AvatarChangeOwner::makeRequest(found.m_addInfo2,
                                                  user->getCurCharacNo(),
                                                  nCharacNo, '\0');
                inven->GetAvatarItemMgrW()->UnRegistItem(found.m_addInfo2);
            }
            else
            {
                if (inven->delete_item((INVEN_TYPE)invenType, MSGV->m_slot,
                                       MSGV->m_addInfo,
                                       (eItemDelReason)0xf, 1) != 1)
                    return 0x11;
            }
            if (found.IsCreatureItemType())
            {
                inven->GetCreatureMgrW()->SendMail(&found, MSGV->m_slot, 0xf);
            }
        }

        user->m_historyLog.SendMail((::MSG_MAILBOX_SEND*)msg, nB);

        return _DoSendNewMail(user, sendCopy, MSGV->m_money, nCharacNo,
                              bA, MSGV->m_title, MSGV->m_titleLen, nC);
    }
    return 10;
#undef MSGV
}