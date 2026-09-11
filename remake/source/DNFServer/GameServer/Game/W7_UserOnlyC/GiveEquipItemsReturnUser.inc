// ORIG T 0x08692140（objdump 直转）：装备回馈走 +0xb550 =
// m_returnUserReward.m_levelUp 表。
// b28d-solo-W7_UserOnlyC 判词：本件已收至 120/121 —— 唯一残差为
// `mov -0x30(%ebp),%eax; mov %eax,%ebx`（4.1 冗余 regmove 跨 G_CDataManager call
// 保活）。4.4 恒折单段 `mov -0x30(%ebp),%ebx`，语句表达式 / unsigned cast 两旋钮
// 实测均无效 ⇒ 按 N18a（反向亦不可造）封顶；调用点差集与语义已对齐。
// [M2S1] p167 复核 ORIG 0x08692140：19/19 调用数相同，差集仅
// LevelUpRewardItemTableKey/LevelUpRewardItemData/Inven_Item 的 **C1↔C2**
// （ORIG 4.1 局部对象 ctor 走 base 版 C2、dtor 走 D2；4.4 恒 C1/D1，同一地址别名，
// 4.4.7 对 local 对象无 C2 选择旋钮）⇒ 工具层编译版本差，封顶。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N7，gap=+0/blocks=1）→ 试=①新口径 n1_v 复核
//   （121/121，唯一残差 ORIG 0x086921f0 `mov -0x30(%ebp),%eax; mov %eax,%ebx` vs 我方
//   折单条 `mov -0x30(%ebp),%ebx`）②prett/q3_gap2 缺口 0、strall 串 CLEAR
//   ③同族直证：本 TU `reqSendMailCertify` 的 ORIG 0x0868f??? 段亦有同型
//   `mov 0x8e0f8(%eax),%eax; mov %eax,%esi` ⇒ 属 **4.1 冗余 regmove 惯例**（值先落 %eax
//   再转保活寄存器），4.4 恒直落目标寄存器 ⇒ 源码无对应形 ⇒ 维持 N7 封顶。
void CUser::GiveEquipItemsReturnUser()
{
    // P0/P48：ORIG growType/level/growType2/job 均无槽（%ebx/%eax 直传）⇒ 内联实参；
    // key 有具名槽 -0x24。getCurCharFirstGrowType 每次使用均重调（两遍）。
    const int key = getReturnUserLevelKey(get_charac_level(),
                                          getCurCharFirstGrowType());

    LevelUpRewardItemTableKey tableKey(key, get_charac_job(),
                                       getCurCharFirstGrowType());
    LevelUpRewardItemData data;
    // P168a②：ORIG `call; xor $1; test %al; jne` ⇒ bool callee 的否定形
    // `if (!get_data(...))`（4.4 直产 xor 段）。
    if (!G_CDataManager()->m_returnUserReward.m_levelUp.get_data(tableKey,
                                                                 data))
    {
        return;       // P285：外层空 else 保留 ORIG 的 jmp+nop 岛
    }
    else
    {
        Inven_Item item;
        // [N1B 2026-09-11 负结论] ORIG 0x086921f0 段 `mov -0x30(%ebp),%eax;
        // mov %eax,%ebx; call G_CDataManager; mov %ebx,0x4(%esp)` = 实参先算后
        // 跨 call 保活（4.1 冗余 regmove）；`(*G_CDataManager())` 解引用形已实测
        // **无效**（残差逐字不变，仍折单段 `mov -0x30(%ebp),%ebx`）⇒ N7 封顶。
        CItem* base = G_CDataManager()->find_item(data.m_field0);
        if (base != 0)
        {
            item.m_addInfo = data.m_field0;
            base->make_item(item);
            item.set_add_info(data.m_field4);

            const char* msgBody =
                g_scriptStringManager_.findString(4, "return_user_msg_2", 0);
            // P0/P52：group/msgLen/characNo/sender 无具名槽 ⇒ 内联实参。
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "return_user_msg_title",
                                                  0),
                item, 0u, static_cast<unsigned int>(getCurCharacNo()),
                msgBody, static_cast<int>(strlen(msgBody)), 7u,
                GetServerGroup(), false, false);
        }
        else
        {
            return;   // P285：内层空 else 保留 ORIG 的 nop 占位岛
        }
    }
}