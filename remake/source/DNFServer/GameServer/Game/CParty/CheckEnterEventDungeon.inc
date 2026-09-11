// CParty::CheckEnterEventDungeon @ 0x859ee66 —— 事件地下城条件（必备/奖励材料）。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x60/0x70 + 守卫极性；259 vs 276
bool CParty::CheckEnterEventDungeon(CDungeon const* dungeon, unsigned char& flag,
                                    Inven_Item* items, int* param)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x859ee66
    // P133/P225：ORIG 0x859ee94..0x859eedc 为两层「条件失败即 return 1」，
    // 每个失败分支各自 `mov $1,%eax; jmp epilog`，不是 && 合并短路。
    if (!((dungeon->m_entryRequiredItem2.m_itemIdx != -1) ||
          (dungeon->m_entryRequiredItem.m_itemIdx != -1)))
        return 1;
    if (!((dungeon->m_entryRequiredItem2.m_itemIdx == -1) ||
          (dungeon->m_entryRequiredItem2.m_enableFlag == 1) ||
          (dungeon->m_entryRequiredItem.m_itemIdx == -1) ||
          (dungeon->m_entryRequiredItem.m_enableFlag == 1)))
        return 1;
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        // [G07 2026-09-11 · P158 正解] ORIG 0x0859ef5a 区段 `movzbl -0x9(%ebp),%eax;
        // xor $0x1,%eax; test %al,%al; je` = 对 **bool 变量** 求 `!v` 的 XOR 物化形；
        // 声明 char 时 4.4 生 `cmpb $0x0,-0x9(%ebp); jne`（无 XOR）⇒ char→bool。
        bool local_d = true;
        // [G7-sub] ORIG 0x0859ef10..0x0859ef51：`shl $2,%eax; mov %eax,%ebx;
        // add 0x18(%ebp),%ebx` + `mov %eax,(%ebx)` + `mov (%ebx),%eax` —— &param[i]
        // 全程在 %ebx（callee-saved），**无指针槽** ⇒ 原码无具名指针（P183/P0）。
        // 旧码 `int* pi = &param[i]` 落槽 -0x14（+1 store，每次解引用 +1 load，帧 +4）。
        // [G07 2026-09-11 · N 类翻案] 原判「N82 帧/守卫极性」不完整：ORIG 该处
        //   `mov %eax,(%ebx)` + **`mov (%ebx),%eax`**（&param[i] 一次算好留 %ebx）
        //   + `cmp $-1; setne %al; test %al,%al; je` 三段物化 = **赋值内嵌条件**形
        //   `if ((param[i] = call(...)) != -1)` 的 4.4 实测码形（本地 probe
        //   /tmp/g07/probe/cond.cpp f2 逐指令复现：见 `mov (%ebx),%eax` 重载 +
        //   `sete/setne %al; test; je`）。分两句写（赋值后另起 if）恒生「地址重算 +
        //   `cmp; jne`」⇒ 必 DIFF。P15「赋值即条件惯用法」的数组元素变体。
        if ((param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                 ->check_item_exist(dungeon->m_entryRequiredItem2.m_itemIdx)) != -1)
        {
            // ORIG 0x0859ef61..0x0859f011：&items[i] 亦全程在 %ebx
            //（`imul $0x3d; mov %eax,%ebx; add 0x14(%ebp),%ebx`），逐字段拷贝 16 条
            // 直写 `mov %eax,N(%ebx)` ⇒ 无具名指针（P0）。
            items[i] = m_member[i].m_pUser->getCurCharacInvenR()
                       ->GetInvenSlot(1, param[i]);
            if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem2.m_itemCount)
            {
                flag = (unsigned char)i;
                local_d = 0;
            }
        }
        else
        {
            flag = (unsigned char)i;
            local_d = 0;
        }
        if (!local_d)
        {
            local_d = 1;
            if ((param[i] = m_member[i].m_pUser->getCurCharacInvenR()
                     ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx)) != -1)
            {
                items[i] = m_member[i].m_pUser->getCurCharacInvenR()
                           ->GetInvenSlot(1, param[i]);
                if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
                {
                    flag = (unsigned char)i;
                    local_d = 0;
                }
            }
            else
            {
                flag = (unsigned char)i;
                local_d = 0;
            }
        }
        if (!local_d)
            return 0;
    }
    return 1;
}