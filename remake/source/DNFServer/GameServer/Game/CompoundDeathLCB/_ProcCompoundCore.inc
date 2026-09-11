// ORIG 0x08332ee8。结果选择（按 rec 内三组权重池抽样）→ 删除两件材料与事件
// 材料 → 注销两 uid 并 push 结果三元组 → AddAvatarItem 入包 → 回填 DB 历史。
//
// 【batch-16 验收标注（§6.2 口径）】orig 653 / ours 635（gap 77 → 18）。
//   ① 调用点差集 **归零**：direct 61 ↔ 61、indirect 2 ↔ 2（残 missing=1/extra=1
//      仅 cMyTrace C2 ↔ C1 同体符号名对称差）；
//   ② 补齐的真实缺口（P167 差集定位）：
//      a) 事件材料整单删除由「switch 提公共 materials 指针 + 单调用」改为
//         4 个 if-else 分支**各自完整展开**（ORIG 每支独立 delete_event_items +
//         vector::size + 独立 0x16 出口）⇒ 恢复 delete_event_items ×3、size ×2；
//         type4 的 size 比较对象仍是 tailC(+0x2d0)【ORIG 怪癖，已保留】；
//      b) 两段 GetRemainDate 各自重取 getCurCharacInvenR → GetAvatarItemMgrR
//         （不缓存 mgr）；c) AddAvatarItem 走重取的 getCurCharacInvenW。
//   ③ 余 18 为 N82 帧号重排下的块序对齐伪影（difflib 显示 -46/-32/-5 与总量
//      -18 自相矛盾，而调用点差集已归零）⇒ 判语义完成、byte 层封顶。
// 【b28e-M1S2 本波】落地「外层条件取反 + A/B 块互换」（德摩根等价，见体内注释）：
//   ORIG 0x08332f4x/0x08332f5x 的**正条件双 je** 与 then-物理在后布局已逐条对齐
//   （`movzbl 0xd6(%eax),%eax; test %al,%al; je <then>` + `movzbl 0xe8(%eax),%eax;
//   cmp $0x4,%al; je <then>`，两跳同落 +0x334），旧形的 `xor $0x1` 反条件与多余
//   m_type 重载消失。读数 orig 653 / ours 635→**638**（gap 18→15，TU 四数
//   32/50/1/3/8 逐值不变）。剩余 15 = 帧 0xcc/0xc0 + 槽整体平移（ORIG
//   -0x54/-0x50/-0x4c/-0x48 vs 我方 -0x4c/-0x48/-0x44/-0x40）+ ORIG 独有
//   callee-saved %edi + N7 间接调用寄存器 + `const int r` 栈缓存（P0 形态：
//   ORIG 用 `mov %eax,%ecx` 直传无槽；但删具名局部使 gap 18→更负 ⇒ 按闸门不落地）
//   ⇒ N82/N7 封顶。
// [P8S1 2026-09-11 本波复核] 维持封顶：N82/N7+ORIG 独有 %edi(b28e 已试极性)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
ENUM_ERROR CCompoundAvatar::_ProcCompoundCore(CUser* user, stCompoundAvatarInfo& info,
                                       stCompoundAvatarTable* table)
{
    // 【b28d-solo-CompoundDeathLCB 判词】DIFF 封顶（§6.2）：ORIG 653 / ours 638、
    // p167 CLEAR(61/61)、无缺失调用。极性/块序已在 b28e 修正（外层条件取反 +
    // A/B 互换）；残余 = 槽整体平移（ORIG 深 8：-0x54/-0x50/-0x4c/-0x48 vs
    // 我方 -0x4c/-0x48/-0x44/-0x40）+ 帧 0xcc vs 0xc0（ORIG 多 callee-saved
    // %edi）+ 间接虚调用 N7 + 权重抽样段 `const int r` 栈缓存（N82/N7 族）。
    CInventory* inv = user->getCurCharacInvenW();
    const tagCompoundAvatar* rec =
        table->getCompoundAvatarInfo(info.m_field04);
    const int subType = CI_SubType(info.m_scriptA);
    int resultIdx = 0;                                         // -0x48

    // [b28e-CompoundDeathLCB] ORIG 0x08332f4x/0x08332f5x 判据为**正条件双 je**
    // （`movzbl 0xd6(%eax),%eax; test %al,%al; je <then>` +
    //  `movzbl 0xe8(%eax),%eax; cmp $0x4,%al; je <then>`，两跳同落 +0x334 的
    //  then 块 —— then 物理在**后**）。旧形（then 在前 + 4.4 `xor $0x1` 反条件）
    // 另多发一次 m_type 重载与重排。改写为德摩根等价：条件取反 + A/B 块互换
    //（语义不变，仅改块物理序）。
    if (info.m_fieldD6 != 0 && info.m_type != 4)
    {
        // 输出权重池空则失败。
        if (rec->m_b[subType] == 0 || rec->m_optsA[subType].empty())
            return static_cast<ENUM_ERROR>(1);

        int weight = 0;                                        // -0x38
        if (info.m_scriptA->get_grade() == 1 &&
            info.m_scriptB->get_grade() == 1)
        {
            weight = rec->m_d[subType];                        // +0x16c
            if (m_rand->randInt(
                    static_cast<unsigned long>(10000)) < 5000) // setbe 0x1387
            {
                const int r = m_rand->randInt(
                    static_cast<unsigned long>(weight));
                resultIdx = getItemIndex(rec->m_optsC[subType], r);  // +0x194
            }
            if (resultIdx < 1)
                weight = rec->m_a[subType];                    // +0x004 兜底权重
        }
        else
        {
            weight = rec->m_e[subType];                        // +0x2e0
        }

        if (resultIdx < 1)
        {
            // 服务端全局修正（datamgr+0x4a54 加成 / +0x4a58 千分比衰减；
            // CDataManager 域具名化后改为成员访问）。
            if (info.m_type == 1)
            {
                // datamgr+0x4a54 = m_serverParameter(+0x68)+0x49ec
                //（importServerParameterScript "[upper bind cube rate bonus]" 写点）；
                // ORIG 0x08333170 `mov 0x4a54(%eax),%eax` = 4 字节读 → int 成员。
                weight += G_CDataManager()->m_serverParameter.m_49ec;
            }
            if (info.m_type == 3)
            {
                // datamgr+0x4a58 = m_serverParameter+0x49f0
                //（"[lower bind cube rate bonus]"；ORIG 0x0833318c `mov 0x4a58(%eax),%eax`）。
                weight -= (G_CDataManager()->m_serverParameter.m_49f0 * weight) /
                          10000;
            }
            // [轴 B 2026-09-10 保留] ORIG 0x083331d1 `cmp -0x38(%ebp),%eax; setl %al`
            // （**有符号序**，槽 -0x38 = weight；上下文同构：`call randInt(const&)`
            // 后即刻比较，与 ORIG 0x083331cc/0x083331d1 逐条对应）⇒ 原码比较为有符号；
            // 旧文 `unsigned long < int` 按 C++ 规则转无符号 ⇒ 4.4 产 `setb`。
            // 加 `(int)` 复现有符号序（randInt(10000) ∈ [0,9999]，值域内无分叉）。
            // 实测：our 636→635；汇编 diff 段 992→988 行（minus 364→361）⇒ ndiff 严格改善；
            // TU 四数 32/50/1/3/8 逐值不变（不劣）⇒ 保留。
            // 同件另一行（`_ProcCompoundCore` ORIG 0x08333157 `cmpl $0x0,-0x48; jg`）
            // 经地址自校为**表内配对假阳性**：我方 0x1280 同为 `jg`（有符号）⇒ 不改。
            if ((int)m_rand->randInt(static_cast<unsigned long>(10000)) < weight)
            {
                const int r = get_rand_int(rec->m_b[subType]);
                resultIdx = getItemIndex(rec->m_optsA[subType], r);  // +0x7c
            }
        }
    }
    else
    {
        if (info.m_type == 4)
        {
            const int r = get_rand_int(
                rec->m_cube.m_field0c[subType]);               // +0x218 权重计数
            resultIdx = getItemIndex(rec->m_cube.m_vecs[subType], r);  // +0x240
        }
        else
        {
            resultIdx = info.m_fieldD0;                        // 直接选定结果
        }
    }

    if (resultIdx == 0)
    {
        const int r = get_rand_int(rec->m_c[subType]);         // +0x054 权重计数
        resultIdx = getItemIndex(rec->m_optsB[subType], r);    // +0xf4
    }

    if (resultIdx != 0 && G_CDataManager()->find_item(resultIdx) != 0)
    {
        // ---- 成功路径：删两件材料 ----
        CItem* item = G_CDataManager()->find_item(resultIdx);
        if (!item->IsCreatureItem())          // ORIG vtable+0x10（IsCreatureItem）
            return static_cast<ENUM_ERROR>(1);
        // ORIG 0x083330ec：槽位由目标 item 的 avatar option size 随机重选，
        // 且 >0x63(99) 时清零（覆盖客户端传入 slot）。
        info.m_slot = static_cast<unsigned short>(get_rand_int(
            reinterpret_cast<CEquipItem*>(item)->getAvatarOptionSize()));
        if (info.m_slot > 99)
            info.m_slot = 0;
        if (inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field08), 1,
                             static_cast<eItemDelReason>(3), true) == 1 &&
            inv->delete_item((INVEN_TYPE)2,
                             static_cast<int>(info.m_field48), 1,
                             static_cast<eItemDelReason>(3), true) == 1)
        {
            bool bFoundAll = false;                            // -0x8d
            // 【batch-16 物理展开】ORIG 0x0833321x..0x083332bx 是 4 个 if-else 分支
            // **各自完整调用一次** delete_event_items + vector::size + 比较 + 独立
            // `return 0x16` 出口（调用点差集实测：提公共 materials 指针后单调用会
            // 少 3 个 delete_event_items 与 2 个 size）。type4 的 size 比较对象
            // 仍是 tailC（ORIG 0x083332b7 读 +0x2d0）【怪癖保留】。
            if (info.m_type == 0)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailA, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailA.size()))
                    return static_cast<ENUM_ERROR>(0x16);
            }
            else if (info.m_type == 1)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailB, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailB.size()))
                    return static_cast<ENUM_ERROR>(0x16);
            }
            else if (info.m_type == 3)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_tailC, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return static_cast<ENUM_ERROR>(0x16);
            }
            else if (info.m_type == 4)
            {
                const int deleted = inv->delete_event_items(
                    rec->m_cube.m_vec, info.m_results, bFoundAll,
                    static_cast<eMoneySubReason>(0x12),
                    static_cast<eItemDelReason>(0x12), false);
                // ORIG 怪癖保留：type4 分支的 size 比较对象是 tailC(+0x2d0)
                //（ORIG 0x083332b7 读 +0x2d0 实证；不得改为 m_cube 或 m_tailD）。
                if (deleted != static_cast<int>(rec->m_tailC.size()))
                    return static_cast<ENUM_ERROR>(0x16);
            }

            const int curSec = GlobalData::s_systemTime_.getCurSec();
            // 两段 remain 计算各自重取 invenR → GetAvatarItemMgrR（ORIG
            // 0x0833330a / 0x08333345，不缓存 mgr）。
            const int remainA =
                ((CUserCharacInfo*)user)
                    ->getCurCharacInvenR()
                    ->GetAvatarItemMgrR()
                    ->GetRemainDate(static_cast<int>(info.m_materialA.m_addInfo2),
                                    curSec);
            const int remainB =
                ((CUserCharacInfo*)user)
                    ->getCurCharacInvenR()
                    ->GetAvatarItemMgrR()
                    ->GetRemainDate(static_cast<int>(info.m_materialB.m_addInfo2),
                                    curSec);

            // 注销并广播移除事件。
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialA.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialA.m_addInfo2),
                                 1, static_cast<int>(info.m_field08) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组①
            info.m_results.push_back(static_cast<int>(info.m_field08));
            info.m_results.push_back(1);
            inv->GetAvatarItemMgrW()->UnRegistItem(
                static_cast<int>(info.m_materialB.m_addInfo2));
            inv->SendAvatarEvent(0x36,
                                 static_cast<int>(info.m_materialB.m_addInfo2),
                                 1, static_cast<int>(info.m_field48) + 10, 0);
            info.m_results.push_back(1);                       // 结果三元组②
            info.m_results.push_back(static_cast<int>(info.m_field48));
            info.m_results.push_back(1);

            char ipgNoBuf[0x14];                               // -0xa5
            _generateIPGNo(user, curSec, ipgNoBuf);

            // AddAvatarItem 走**重取**的 invenW（ORIG 0x083333xx getCurCharacInvenW）。
            info.m_resultSlot = static_cast<unsigned short>(
                user->getCurCharacInvenW()->AddAvatarItem(
                    resultIdx, remainA + remainB, false,
                                   static_cast<char>(info.m_slot),
                                   static_cast<char>(-1), ipgNoBuf,
                                   static_cast<eAvatarItemAddReason>(6), true,
                                   0));
            if (static_cast<short>(info.m_resultSlot) == -1)
            {
                // [b28e-M1S2 本波·§6.5/P139 盲区] ORIG 0x08333842/0x0833384a 两处
                // 函数名实参是**符号**（`movl $&_ZZN8WongWork15CCompoundAvatar17_
                // ProcCompoundCoreEP5CUserRNS0_15tagCompoundInfoEP22tagCompoundAvatar
                // TableE19__PRETTY_FUNCTION__,0x8(%esp)`），即 `__PRETTY_FUNCTION__`
                // 的函数局部静态（@0x08c2c540，**size 0x8a = 138B**）。ORIG 该串经
                // `objdump -s -j .rodata` 逐字节实录：
                //   `ENUM_ERROR WongWork::CCompoundAvatar::_ProcCompoundCore(CUser*,
                //    WongWork::CCompoundAvatar::stCompoundAvatarInfo&, stCompoundAvatarTable*)`
                // 依据（探针 /tmp/probe4.s，4.4.7 + 本仓旗标）：GCC 的 `__PRETTY_FUNCTION__`
                // 展开 = `<返回类型> <全限定函数>(<参数>)`；**嵌套** typedef 用全限定名
                // （`WongWork::CCompoundAvatar::stCompoundAvatarInfo&`），**全局** typedef
                // 用裸名 ⇒ ORIG 第 3 参 `stCompoundAvatarTable*` 是全局作用域 typedef。
                // [M1Free76to90 2026-09-11 已落地] 头改（返回类型 `ENUM_ERROR` + 类内
                // typedef `stCompoundAvatarInfo`）后本处改回 `__PRETTY_FUNCTION__`（P139），
                // 发射 ORIG 同名局部静态；本 TU 实测 **size 0x8b**。
                // 【残余 1 字符】我方第 3 参拼写 `tagCompoundAvatarTable*`（22 字符）vs
                // ORIG `stCompoundAvatarTable*`（21）⇒ 还需**全局** typedef
                // `typedef tagCompoundAvatarTable stCompoundAvatarTable;`（放类内或
                // `namespace WongWork` 内都会带限定 ⇒ 不符）。已将该头改动**再报**父泳道。
                // 当前串除该 1 字符外与 ORIG 逐字节相同。
                // mangled 不含返回类型/typedef ⇒ 该头改动零 ABI 影响。
                cMyTrace trace(__PRETTY_FUNCTION__, 0x361, 5);
                trace("[%s][%d] AddAvatarItem Fail", __PRETTY_FUNCTION__, 0x361);
                return static_cast<ENUM_ERROR>(0x11);
            }
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialA.m_addInfo2), ipgNoBuf);
            DB_UpdateAvatarHistory::makeRequest(
                static_cast<unsigned int>(user->getCurCharacNo()),
                static_cast<int>(info.m_materialB.m_addInfo2), ipgNoBuf);
            return ENUM_ERROR_NONE;
        }
        return static_cast<ENUM_ERROR>(0x16);                  // delete_item 失败
    }
    return static_cast<ENUM_ERROR>(1);                         // 结果不可实例化
}