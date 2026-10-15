// [SweepCParty] 真 bug：tryInsertItemIntoInventory 的 receiver 由 W 改 R（ORIG 0x085b42xx
// `call getCurCharacInvenR`；W 为历史误用）。p167 CLEAR（16=16）。残余 = N63（4.4 对
// if-return 物化 mov $0/jmp）+ N82 帧。
    // [G7-sub 2026-09-10 复核·写域边界] 本 .inc 仅含函数前段（slot 取得 + 4 条守卫 +
    // m_field204 写入；函数余部 0x.cpp:1403..1468）。逐点核对 ORIG 0x085b415a：
    //   ① `GetMemberSlotNo` 落槽 -0x18 ✓；② `slot==-1 || 3<a` 两条守卫跳共享 false 尾块
    //      （085b4180 `je 44cb` / 085b418a `jg 44cb`）✓ goto fail 形已对齐；
    //   ③ HaveAlreadySelectedCard/_checkGoldCardSelectable 的就地 `mov $0;jmp 44d0` ✓。
    // 余差 5 条（我方 250 vs 245）**全部落在 .cpp 段**：三处 bool callee（GetRepeatEvent /
    // IsReturnUserParty / IsEventCharacParty）我方写 `!callee()` 产 `xor $1; je`，
    // ORIG 为无 xor 的 `test; jne`（P168a②/P292 极性）⇒ 需 .cpp 属主改条件极性。
    // 本 .inc 无可改项；p167 CALLSITE CLEAR。
bool CParty::SetCardNumber(CUser* user, char a, eClearRewardCardType_t type)
{
    // b28d-49 判词：N 封顶。P167 已 CLEAR（b28d-solo-CParty2：check_empty_count 处
    // 由 R 版改回 W 版，ORIG 0x085b43aa 实证）；余差为条件物化/装载序 N82 族。@0x85b415a
    int slot = GetMemberSlotNo(user);                   // -0x18
    // ORIG 085b4180 `je 44cb` + 085b418a `jg 44cb`：前 3 条守卫跳共享 false 尾块（44cb）；
    // HaveAlreadySelectedCard/_checkGoldCardSelectable 用就地 `mov $0;jmp 44d0`。
    if (slot == -1 || 3 < (int)a)
        goto fail;
    if (HaveAlreadySelectedCard((char)slot, type))
        return false;
    if ((unsigned char)m_field204[(int)type][(int)a] != 0xff)
        goto fail;
    if (!_checkGoldCardSelectable(user, type, slot))
        return false;

    m_field204[(int)type][(int)a] = (char)slot;
    if ((int)type != 1)
        goto ret_true;                                  // ORIG 085b4230 `jne 44c4`

// §P331 第二形（实测 227→140）：ORIG 每次 reward 字段访问都重物化 `imul 3;shl 3`
// (=slot*0x7a) + `add $0x79d`（imul$0x7a 7 处 / add$0x79d 7 处，本方原 1/0 —— 被 bd +
// reward 两个引用吸收）。slot 此后不再改变 ⇒ 逐次寻址同物，无可观察行为变化。
#define reward (m_battleData.m_itemGroups2[slot][1])   // +0x7ad + slot*0x7a
    // ORIG 085b425f `call *%edx; xor $1,%eax; test; je 440e`：原码为 `if (!IsEventing(0))`
    // 正极性包裹 reward 段；tryInsertItemIntoInventory 为 const ⇒ R 版，返回值直比（无槽）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x41)->IsEventing(0))
    {
    if (reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
            ->tryInsertItemIntoInventory(reward) == -1)
    {
        reward.m_addInfo = 0;                           // +0x7af / +0x7b4 清零
        reward.m_addInfo2 = 0;
    }
    else
    {
        // [G07 2026-09-11 · P354 部分改善] ORIG 0x085b42xx 三处 bool callee 为
        //   `call; test %al,%al; jne/je`（**无 xor**，正极性短路链）；我方旧形
        //   `(!A && !B && !C) || D` 令 4.4 对每个 `!callee()` 物化 `xor $0x1`（共 3 条）。
        //   按 De Morgan 改写为正形，并**保持求值序与副作用完全一致**：
        //   A/B/C 短路 → m_field1 → getCurCharacInvenW()/check_empty_count 仅在前两项
        //   成立时求值（与原 `X 为假 ⇔ (A||B||C) && m_field1==1` 逐点同真），故
        //   enableSaveInven 副作用的触发条件与次数不变。语义等价，仅消 xor。
        if ((GlobalData::s_event_manager->GetRepeatEvent(0x40)->IsEventing(0) ||
             IsReturnUserParty() || IsEventCharacParty()) &&
            reward.m_itemType == 1 &&
            reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                ->check_empty_count(Inven_Item::ITEM_TYPE_EQUIP, 2) != 1)
        {
            // b28d-solo-CParty2（p167）：ORIG 0x85b43aa `call getCurCharacInvenW`（**非 const** 版，
            // 体内先 `call enableSaveInven` 再返回 inven 指针）⇒ 本处必须用 W 版（R 版无
            // enableSaveInven 副作用 = 真语义缺口）。上方 tryInsertItemIntoInventory 处
            // ORIG 0x85b4273 确为 R 版，保持不动。
            reward.m_addInfo = 0;
            reward.m_addInfo2 = 0;
        }
    }
    }

    // C05③ 单尾 fail 块（Main 判据②多层形）：ORIG 5 条失败 jcc
    // （0x2b8/0x2fc/0x319/0x345，另 0x0d6）= 全部汇到同一 0x44c4（mov $1,%eax）
    // ⇒ 原码为嵌套 if + 单一公共尾 return，而非 5 个独立 `goto ret_true`
    // （后者令 4.4 每个 site 各复制一份尾块）。
    if (user != 0)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)8, 0, (ENUM_PACKETCLASS)0);
        int itemId = reward.m_addInfo;                  // -0x14
        if (itemId != 0)
        {
            CItem* item = G_CDataManager()->find_item(itemId);   // -0x10
            if (item != 0)
            {
                int rarity = item->get_rarity();        // -0xc
                if (rarity == 0 || rarity == 1 || rarity == 2 ||
                    rarity == 3 || rarity == 4)
                {
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                        user, (APSystem::_ActionGroupIndex)0x15, 0,
                        (ENUM_PACKETCLASS)0);
                }
            }
        }
    }
ret_true:
    return true;
fail:
    return false;
}