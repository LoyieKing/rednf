// [b28d-20] makeCompoundItem（ORIG 0x832ccc0，249/244，nd 192→130）：本轮修
//   P192/p167 三处 `std::make_pair`→`std::pair<unsigned int,unsigned int>` 直构
//   （ORIG `C1IjjEEOT_OT0_`）；p167 差集 18/18 **CLEAR**。残差 = 随机数取模
//   除法链与 bonus/found 局部槽号（N82/N7 族）⇒ 语义完成、byte 层封顶。
// [b28d-solo-CItemList] DIFF | p167 CLEAR（18/18 calls）。本轮按 ORIG 槽序
//   （0x0832cced baseLevel=-0x28 / 0x0832cd0c inRarity=-0x24 / 0x0832cd0f
//   outputRarity=-0x20 / 0x0832cd42 r0=-0x1c / 0x0832cd71 r1=-0x18 /
//   0x0832cd74 found=-0x14）重排声明：outputRarity 前置、found 提到分支前。
//   残 = 全帧槽基差 4（我方 -0x24 起，ORIG -0x28 起；帧同为 0x94）⇒ N95/N115
//   （4.4 帧顶 8B gap/outgoing 预留，TU 级旗标，禁 pad 强凑）+ gap 5（除法链）。
// [b28d-tu-CItemList] 真语义修正 ×2（range 颠倒）：第 2/3 处
//   `_FindItemAll_BindSphere` 的 pair 实参原写 (baseLevel+5, +1) /
//   (baseLevel+0xa, +6)，ORIG 0x0832cea2-0x0832ceb7 ctor 槽
//   `+1→-0x3c→0x4(%esp)`(first)、`+5→-0x40→0x8(%esp)`(second) 与
//   0x0832cf09-0x0832cf1e `+6→0x4`、`+0xa→0x8` ⇒ 正确形 = (+1,+5) / (+6,+10)
//   （同 _exception_Rare 前轮修正形，本函数漏改）。p167 18/18 CLEAR 复验。
//   残差 = 帧基 4B 漂移（N95 级联）+ 条件短路 bool 临时槽物化岛（ORIG
//   `movl $0x0,-0x10` ×2，4.4 折叠，N7）+ `mov mem,%eax; test` 形 vs 4.4
//   `cmpl $0x0,mem` 折形（N18a）。
//   [§3 取证补记 + 修正版 p167 复判] ExtreamDungeon__CCompound_ExtreamDun_Item/
//   makeCompoundItem.md 汇编段 L165-171（+5→-0x40→0x8、+1→-0x3c→0x4）与
//   L194-197（+0xa→0x8、+6→0x4）逐行支撑本件 pair 修正；18=18，3 对
//   pairIjjEC2↔C1 镜像别名，豁免后零差。
bool CCompound_ExtreamDun_Item::makeCompoundItem(                   // ORIG T 0x832ccc0
    const CEquipItem** refs, char select, Inven_Item& out, eBindSphereType bind)
{
    out.reset();
    // [b28d-tu-CItemList2 2026-09-09] P211/P274 同型化实测反劣（250/250→248，
    //   inRarity 两处 `mov mem,%eax` 被折掉）⇒ 保持 int baseLevel（P321 T5：
    //   类型不用于凑装载序）。
    int baseLevel = _get_base_level(refs);
    // [K9-CItemList 2026-09-11] ORIG 0x0832ce0e/-0x0832ce1b 段（inRarity 窗口
    //   守卫）为 `mov -0x24(%ebp),%eax; test %eax,%eax; jle` + `mov; cmp $0x2;
    //   jg` 两段载入形；4.4 对 **int** 局部恒折 `cmpl $0x0/-0x24(%ebp)` 单段
    //   （P209/N18a，改 `inRarity > 0` 等写法无效）。探针实证（/tmp/probe44，
    //   同 4.4.6 路由）：局部类型为**枚举**时 4.4 恒发 mov+test/mov+cmp 两段
    //   ⇒ 类型改 ENUM_INPUTITEMS_RARITY（该局部在 ORIG 只被当作 rarity 用，
    //   取值域 0..2，转换点显式化，语义等价）。
    ENUM_INPUTITEMS_RARITY inRarity = (ENUM_INPUTITEMS_RARITY)_convertInputItem(
        (ENUM_UNUSABLE_EQUIPMENT_TYPE)refs[0]->GetUsableEquipmentType());

    // [b28d-solo] ORIG 槽序（0x832cced/-0x28、0x832cd0c/-0x24、0x832cd0f/-0x20、
    //   0x832cd42/-0x1c、0x832cd71/-0x18、0x832cd74/-0x14）⇒ outputRarity 与
    //   found 的声明须前置于 r0/r1 之间与分支之前（P28/P309 声明序=槽序）。
    unsigned int outputRarity = 4;
    // 双次 randInt()%1000000 + 1（magic 常量 0x431bde83 实证；ORIG 为无符号除法）
    unsigned int r0 = ((unsigned int)m_rand->randInt() % 1000000) + 1;
    unsigned int r1 = ((unsigned int)m_rand->randInt() % 1000000) + 1;
    // [b28d-tu-CItemList2 2026-09-09] ORIG 0x0832cd74 `movl $0x0,-0x14(%ebp)`：
    //   原码在两次 randInt 之后另有一个置 0 的具名 int 局部（Ghidra local_18，
    //   全函数只写不读），它占据 -0x14 槽并决定后续 found(-0x10) 与 sret(-0x60)
    //   的槽位。按 P0 双槽判据恢复该具名局部（死写照抄 P171）。
    int legacyValue = 0;
    (void)legacyValue;
    if (inRarity == 0)
    {
        outputRarity = (unsigned int)_determine_rarity(r1,
            (ENUM_INPUTITEMS_RARITY)inRarity);
    }
    else
    {
        outputRarity = (unsigned int)_convertInput_To_Output(
            (ENUM_INPUTITEMS_RARITY)inRarity);
    }

    // [b28d-tu-CItemList2 2026-09-09] 真语义修正：ORIG 0x0832cdcc `mov
    //   -0x1c(%ebp),%edx; mov %edx,0xc(%esp)`（Ghidra `local_20` = 第一次
    //   randInt 的 r0）⇒ _determine_level_bonus 第二实参是 **r0**（掷点值），
    //   原重建误传 baseLevel。同时 found 声明后移至本调用之后（ORIG
    //   0x0832cde5 `movl $0x0,-0x10(%ebp)` 在 call 之后，-0x14 才是上句 legacy）。
    std::pair<unsigned int, unsigned int> bonus = _determine_level_bonus(
        (ENUM_INPUTITEMS_RARITY)inRarity, r0,
        (ENUM_OUTPUTITEMS_RARITY)outputRarity, bind);

    int found = 0;
    // [b28d-tu-CItemList2 2026-09-09] 正极性 if/else：ORIG 0x0832cdf2
    //   `jne 0x0832ce24`(else) + 真分支 `movl $0x0,-0x10(%ebp); jmp`
    //   （Ghidra C `if (cond) { local_14 = 0; } else { … }`）⇒ 条件成立时
    //   显式 found = 0，取反形会折成 `ja` 单跳少 2 条。
    if (bonus.second == 0xa && inRarity != 1 &&
        ((unsigned int)baseLevel % 10) > 4)
    {
        found = 0;
    }
    else
    {
        // [b28d-20] P192/p167：ORIG 0x832ccc0 段三处均为 `std::pair<unsigned
        //   int,unsigned int>` 直构（`_ZNSt4pairIjjEC1IjjEEOT_OT0_`），
        //   make_pair 版本落 EXTRA（3 处）。
        found = _FindItemAll_BindSphere(refs, select,
            (ENUM_OUTPUTITEMS_RARITY)outputRarity,
            std::pair<unsigned int, unsigned int>(
                // [K9-CItemList 2026-09-11] ORIG 0x0832ce5e/-0x0832ce79 段为
                //   `mov -0x5c(%ebp),%eax; add -0x28(%ebp),%eax`（内存加，2 条）；
                //   原写 `bonus.first + (unsigned int)baseLevel` 使 4.4 走无符号
                //   lea 形（`mov;mov;lea` 3 条，两处共 +2 ⇒ gap 0 平衡的另一半）。
                //   探针实证（/tmp/probe44，同 4.4.6 路由）：转换包住**有符号**
                //   加法即复现 mov+add-mem 单对。baseLevel/_get_base_level 恒非负
                //   （等级均值），(int) 转换在实值域内语义等价。
                (unsigned int)(baseLevel + (int)bonus.first),
                (unsigned int)(baseLevel + (int)bonus.second)));
    }
    if (found == 0)
    {
        if (inRarity >= 1 && inRarity <= 2)
        {
            found = _FindItemAll_BindSphere(refs, select,
                (ENUM_OUTPUTITEMS_RARITY)outputRarity,
                // [b28d-tu-CItemList] 真语义修正（range 颠倒）：ORIG
                // 0x0832cea2-0x0832ceb7 段 ctor 实参槽 0x4(%esp)=-0x3c
                // （baseLevel+1）、0x8(%esp)=-0x40（baseLevel+5）⇒
                // pair = (first=+1, second=+5)；原写 (+5,+1) 使
                // range.first/second 颠倒（同 _exception_Rare 前轮修正形）。
                std::pair<unsigned int, unsigned int>(
                    (unsigned int)(baseLevel + 1),
                    (unsigned int)(baseLevel + 5)));
            if (found == 0)
            {
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)outputRarity,
                    // [b28d-tu-CItemList] 真语义修正（同上）：ORIG
                    // 0x0832cf09-0x0832cf1e `+0xa → -0x30 → 0x8(%esp)`、
                    // `+6 → -0x2c → 0x4(%esp)` ⇒ pair = (+6, +10)。
                    std::pair<unsigned int, unsigned int>(
                        (unsigned int)(baseLevel + 6),
                        (unsigned int)(baseLevel + 0xa)));
            }
        }
        else if (inRarity == 0)
        {
            found = _exception_Rare(refs, select,
                (ENUM_OUTPUTITEMS_RARITY)outputRarity, (unsigned int)baseLevel);
        }
    }
    if (found == 0)
    {
        return false;
    }
    out.m_addInfo = found;
    // [b28d-tu-CItemList2 2026-09-09] P92/P0：ORIG 0x0832cfc5-0x0832cfd4
    //   `mov 0x14(%ebp),%eax; mov -0x10(%ebp),%edx; mov %edx,0x2(%eax)` 后
    //   **重新** `mov 0x2(%eax),%eax; mov %eax,%ebx; call G_CDataManager` ⇒
    //   实参来源是 out.m_addInfo 字段而非 found 局部。
    CItem* info = G_CDataManager()->find_item(out.m_addInfo);
    if (info == 0)
    {
        return false;
    }
    info->make_item(out);
    return true;
}