int CTournamentDungeonReward::GetItemFromCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
                                              int cardNo, Inven_Item* items,
                                              unsigned int maxItems)
{
    // b21 verdict=DIFF（N 残余）：168/170。调用点全等（reset×/set_add_info/create_item/
    //   G_CDataManager）。两处大修：1) G_CDataManager()->getItemList() O0 禁内联产 EXTRA
    //   call → m_itemList 直读(+0xc)；2) `TournamentRewardType& slot` 引用局部只算一次地址
    //   （ORIG 每访问重推导 `this+type*0x18+i*0xc`）→ 删引用改 m_rewards[type][i].member
    //   直写（-64 insn）。残余仅索引计算寄存器/leas 调度(N82)与共享退出块布局(N-JMP,+2)。
    //   语义完成，byte 层 N 封顶。
    // [b28d-N82] fresh 复验 168/170：同上（N82 帧/槽 + N18a 索引寄存器）。
    // [b28d-solo-CTournamentDungeonReward] 试形记录：早返拆分（maxItems>2 / type 越界
    // 各自 return 0）实测 168/174（原形 168/170）⇒ 4.4 仍尾部合并三处失败块，
    // 多 4 条，回退为合并守卫形。
    // [b28d-solo-CTournamentDungeonReward] DIFF 168/170：残 = N18a×5 + 三失败块尾部
    // 合并（N-JMP，ORIG 就地 `mov $0,%eax; jmp` ×3）；p167 9=9 CLEAR ⇒ N 封顶。
    // [SweepTournament 2026-09-09] 真语义 bug 修复：原 `(char)m_flag08 == (char)cardNo`
    // 被编译成字节比较（movzbl+mov+cmp %al,%dl）；ORIG 0x8285092 为 movzbl+movsbl+cmp
    // mem（32 位有符号，char 提升 int）⇒ cardNo ∉ [-128,127] 时结果相反。改
    // `m_flag08 == cardNo` 后我方比较段逐条复现 ORIG；168/170 与 p167 9=9 CLEAR 不变。
// [SweepCTournament 2026-09-09] 复核 168/170：残 = N18a 索引中转×5 + 三失败块
// 尾部合并（N-JMP：ORIG 就地 `mov $0,%eax; jmp`×3）；p167 9=9 CLEAR ⇒ N 封顶。
// [round1-L19 2026-09-09] 复修（判据① jcc 落点 + 判据⑨ 旧判复核）：
// 守卫重构 = ① `maxItems > 2` 独立早返（复现 ORIG 0x0828503a `cmpl $0x2,0x18(%ebp);
// jbe <body>` + fall-through fail 块）；② `(int)type < 0 || (int)type > 1` **合并
// 短路**（复现 ORIG 0x08285049 `js <共享 fail>` + `cmp $1; jle <cont>` 两跳共享
// 0x08285055 fail 块）。旧形 `maxItems<3 && type>=0 && type<2` 生 `ja` 异极性链。
// 读数：168/170 gap=-2 → **168/174 gap=-6**、ndiff 118→110、**槽集合差 ∅（全等）**。
// p167 9=9 CLEAR。残余 = 6 条 `mov %eax,%edx` 二维索引中转（ORIG `lea 0x0(,%eax,4),
// %ecx` 直落 vs 4.4 累加器恒 eax ⇒ 多 1 条/访问，N7/N18a 后端族；12 形探针
// /tmp/idx_probe{,2}.cpp + fB..fI 已验证不可择）+ 1 处 jne/je。
    if (maxItems > 2)
    {
        return 0;
    }
    if ((int)type < 0 || (int)type > 1)
    {
        return 0;
    }
    {
        for (int i = 0; i < 2; ++i)
        {
            // ORIG 0x8285092：movzbl 读 + movsbl 符号扩展后 `cmp 0x10(%ebp),%eax`
            // —— 32 位有符号比较（char 提升 int）。旧写法 `(char)cardNo` 强制字节比较，
            // cardNo ∉ [-128,127] 时行为与 ORIG 相反（真语义 bug）。
            if (m_rewards[(int)type][i].m_flag08 == cardNo)
            {
                if (m_rewards[(int)type][i].m_id == 0)
                {
                    items[0].reset();
                    items[0].set_add_info(m_rewards[(int)type][i].m_field04);
                    items[1].reset();
                    return 1;
                }
                if (m_rewards[(int)type][i].m_id != -1)
                {
                    items[0].reset();
                    items[1].reset();
                    items[1].m_addInfo = m_rewards[(int)type][i].m_id;
                    G_CDataManager()->m_itemList->create_item(  // ORIG mov 0xc(%eax) 直读
                        ENUM_ITEM_CREATE_TYPE_1, items[1],
                        m_rewards[(int)type][i].m_field04);
                    return 1;
                }
                items[0].reset();
                items[1].reset();
                return 0;
            }
        }
    }
    return 0;
}