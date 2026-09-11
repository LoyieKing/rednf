// 对背包/仓库/账号货物全部在库且 flag==1 的道具槽打上 0x20 交易限制标志，
// 最后按当前角色号发起 DB_TestTradeRestrict 请求。
// 槽位记录步长 0x3d：+0x01 flag 字节、+0x02 itemIndex dword、+0x33 限制字节。
// [b28d-19 / b28c-R26 复验] verdict=DIFF(routed) [N82/P223 边界] ORIG 0x084b2640
//   orig/ours=214/215（gap −1）。p167 CLEAR（15=15）。残差三处（均 4.1 独有形）：
//   ① 循环 3 ORIG 0x084b27a9 `test %eax,%eax; sete %al; test %al,%al; jne <skip>`
//      （"==0" 的 4.1 bool 物化）vs 我方 `je <skip>` 直跳（P225 只给出 "!=0" 的
//      setne 形；负向 sete 形探针不可复现）。
//   ② 循环 4 ORIG 每处 if-join 出 `jmp <tail>; nop` 双岛（P223 正例），
//      4.4 对 `if(a&&b)` 恒 jump-thread 消岛。
//   ③ 循环 5 ORIG 判空 `cmpl $0,-0x24(%ebp); je` 位于条件块；我方把
//      `i<0x38 && p!=0` 物化为 `jg` + `mov $1/$0` 岛（P223 岛数恒=条件数或 0）。
// [round5-GmCmds_G 复核] orig/ours=214/214（gap 0）。旧判词③被**推翻**：
//   ④ 循环 5 ORIG 0x084b2862 `movl $0x0,-0xc; jmp 084b28af`（i 先初始化、跳至
//      前测）+ 0x084b286b 循环体**首句** `cmpl $0x0,-0x24(%ebp); je <循环后>`
//      ⇒ 原码是 `for(i=0;i<=0x37;++i){ if(!accSlot) break; ... }`（guard 在体内
//      首句），非 `while(i<0x38 && p)`。改 for+break 后 `jg`+`mov $1/$0` 岛消失，
//      215→214 条。⇒ 该残差属**源形错**，不是 P223 编译器差。
//   残余 = ① 循环 1/2/3/4 的 4.1 负向 bool 物化（ORIG `test; sete/setne %al;
//      test %al,%al; jne` 4 条 vs 我方 `je/jne` 1 条，ORIG 反而多 3 条/处）；
//      ② 我方 `|=` 恒双地址计算（4.4 -O0 固有，探针 /tmp/probe_or.cpp test2/test3
//      实证：复合赋值 lvalue 求值两次，4.1 单次）——两类方向相反互相抵消；
//      ③ N98 槽序（我方 i 槽 -0x1c vs ORIG -0x20 一族）。p167 CLEAR(15=15)。
void CTestTradeRestrict::execute()
{
    CUser* user = GetUser();
    if (user == 0)
        return;

    // ---- 循环 1：m_pEquipSlot 数组（0x39 槽）----
    for (int i = 0; i < 0x39; ++i)
    {
        // ORIG 每用点 fresh 取 getter/基址（P48），右值表达式寻址（P0 无具名）
        if (user->getCurCharacInvenR()->m_pEquipSlot[i].m_addInfo != 0 &&
            user->getCurCharacInvenR()->m_pEquipSlot[i].m_field1 == 1)
            user->getCurCharacInvenW()->m_pEquipSlot[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 2：inventory 对象内嵌数组（0x1a 槽，m_invenItem）----
    for (int i = 0; i < 0x1a; ++i)
    {
        if (user->getCurCharacInvenR()->m_invenItem[i].m_addInfo != 0 &&
            user->getCurCharacInvenR()->m_invenItem[i].m_field1 == 1)
            user->getCurCharacInvenW()->m_invenItem[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 3：m_pAvatarSlot 数组（0x69 槽，仅判 itemIndex）----
    for (int i = 0; i < 0x69; ++i)
    {
        if (user->getCurCharacInvenR()->m_pAvatarSlot[i].m_addInfo != 0)
            user->getCurCharacInvenW()->m_pAvatarSlot[i].m_upgradeSep.m_field |= 0x20;
    }

    // ---- 循环 4：角色仓库 CCargo（0x98 槽）----
    Inven_Item* cargoStart =
        const_cast<Inven_Item*>(user->getCurCharacCargoW()->get_cargo_start());
    for (int i = 0; i < 0x98; ++i)
    {
        if (cargoStart[i].m_addInfo != 0 &&
            cargoStart[i].m_field1 == 1)            // ITEM_TYPE_EQUIP
        {
            cargoStart[i].m_upgradeSep.m_field |= 0x20;  // 交易限制位
        }
    }

    // ---- 账号货物（GetSlotRef(0)，0x38 槽；引用为空则跳过）----
    Inven_Item* accSlot = user->GetAccountCargo()->GetSlotRef(0);
    // ORIG 0x084b2862 `movl $0x0,-0xc; jmp 084b28af`（i 先初始化、跳至前测）+
    // 0x084b286b 循环体首句 `cmpl $0x0,-0x24(%ebp); je <循环后>` ⇒ 原码形为
    // `for (i=0; i<=0x37; ++i) { if (accSlot == 0) break; ... }`（guard 在体内
    // 首句），而非 `while (i<0x38 && accSlot != 0)`（后者 4.4 把 && 物化成
    // jg + mov $1/$0 岛）。
    for (int i = 0; i < 0x38; ++i)
    {
        if (accSlot == 0)
            break;
        if (accSlot[i].m_addInfo != 0 &&
            accSlot[i].m_field1 == 1)               // ITEM_TYPE_EQUIP
        {
            accSlot[i].m_upgradeSep.m_field |= 0x20;  // 交易限制位
        }
    }

    DB_TestTradeRestrict::makeRequest((unsigned int)user->getCurCharacNo());
}