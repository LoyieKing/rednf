// ============================================================================
// ProcUpgrade(MSG_ENCHANT*) @ 0x0854a1ba（ProcUpgrade_0854a1ba.md）
//   三代强化（ENCHANT 消息）主流程。返回错误码；0 成功。
// [2026-09-10 判词] DIFF（gap 4→0；origN=ourN=437；jcc 逐位一致；imm 多重集一致；
//   p167 CLEAR 41/41）。
//   本轮真 bug 修：①**范围检查方向反**——ORIG 0x0854a39c `jl +0x204`/0x0854a3b0
//   `jg +0x204` 均跳「继续」，fall-through 才是 `mov $0x13`，即正极性
//   `if (m_0d >= start && m_0d <= end) return 0x13`（[3,8]=ITEM_TYPE_0 非装备栏
//   槽 ⇒ 报错）；旧写 `m_0d < start || m_0d > end` 方向全反（判据⑧/jcc 逐位）；
//   ②**GetSellItemPrice 第三实参来源错**——ORIG 0x0854a5f0 读 slotA+0x07
//   (m_addInfo2)，旧写 `(short)msg->m_19`（判据㉕）；③删 ORIG 无有的 `& 0xffff`
//   截断（判据⑮）。
//   本轮形修：P198① 预清三处（cond/penalty/doEnchant 两句式）、P225 while 形 ×2、
//   P0 去 equipType/ret 具名槽、P174 返回类型 int→ENUM_ERROR（__PRETTY_FUNCTION__
//   串 0x53 与 ORIG 全等）、P248① ProcUpgrade_3rdChronicle int→bool、P272 块序。
//   残余 = 帧 0x1ac/0x19c 与槽号平移（N82/N95）+ 位域读 2 处多 `movzbl %al,%eax`
//   （P168a③）⇒ N 封顶。
// ============================================================================
// [P174 串定返回类型] ORIG __PRETTY_FUNCTION__ 串 = "ENUM_ERROR WongWork::
//   CItemUpgrade::ProcUpgrade(CUser*, MSG_ENCHANT*, PacketGuard*)" ⇒ 返回
//   ENUM_ERROR；函数体 return 立即数与 ORIG 一致（枚举底层 int）。
ENUM_ERROR CItemUpgrade::ProcUpgrade(CUser* user, MSG_ENCHANT* msg, PacketGuard* packet)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x0854a1ba 开头**无** user/msg/packet 判空
    // （P171 照抄：加防御反而多 3 组 cmp/je 并顶浅全帧槽）。
    CInventory* invenW = user->getCurCharacInvenW();
    // ORIG 亦无 invenW 判空（mov %eax,-0x30 后直接作 GetInvenSlot 的 receiver）。

    Inven_Item slotA = invenW->GetInvenSlot(1, msg->m_0d);
    if (slotA.isEmpty())
        return (ENUM_ERROR)4;
    if (slotA.m_addInfo != msg->m_0f)
        return (ENUM_ERROR)4;

    // ORIG ProcUpgrade_0854a1ba 554 行：Inven_Item::Inven_Item(&material) 先
    // 默认构造，再 GetInvenSlot 返回临时 + 拷贝赋值（4.1 无 copy-elision）。两段式必写。
    Inven_Item material;
    material = invenW->GetInvenSlot(1, msg->m_13);
    if (material.isEmpty())
        return (ENUM_ERROR)0x16;

    int start = 0, end = 0;
    invenW->getStartEndPos((Inven_Item::ITEM_TYPE)0, start, end);
    // [jcc 逐位核对 2026-09-10] ORIG 0x0854a39c `cmp %eax,%edx; jl +0x204`（m_0d < start
    //   → 继续）、0x0854a3b0 `cmp %eax,%edx; jg +0x204`（m_0d > end → 继续），
    //   fall-through 才是 `mov $0x13`。即原码为**正极性 &&**：目标槽位落在
    //   ITEM_TYPE_0 范围 [3,8]（CSlotBoundChecker::init insert(3,8,ITEM_TYPE_0)）
    //   内 ⇒ 非装备栏槽位 ⇒ 返回 0x13。旧重建写成 `m_0d < start || m_0d > end`
    //   方向全反（只有 [3,8] 内才放行）⇒ 真语义 bug。
    if (msg->m_0d >= start && msg->m_0d <= end)
        return (ENUM_ERROR)0x13;

    CItem* itemScript = G_CDataManager()->find_item(slotA.m_addInfo);
    if (itemScript == 0)
        return (ENUM_ERROR)4;
    CItem* materialScript = G_CDataManager()->find_item(material.m_addInfo);
    if (materialScript == 0)
        return (ENUM_ERROR)4;

    // [2026-09-10 P198① 预清必写] ORIG 0x0854a416 在 call 前有
    //   `movl $0x0,-0x24(%ebp)`（cond 预清）⇒ 源必须两句式；
    //   [P225] ORIG 0x0854a45b `cmpl $0x0,-0x24; setne %al; test %al,%al; je`
    //   四段物化 ⇒ while+break 族形。
    int cond = 0;
    cond = _CheckCondition(user, slotA, itemScript, material, msg->m_19);
    while (cond != 0)
    {
        LogManager::logFormat(1, "ItemUpgrade.cpp",
            __PRETTY_FUNCTION__,
            0x756,
            "3RD_CHRONICLE_LOG : ENCHANT SYSTEM _CheckCondition() ERROR!! CHARAC NO(%u) ERROR(%u)",
            user->getCurCharacNo(), cond);
        return (ENUM_ERROR)cond;
    }

    // [2026-09-10 P198① 预清必写] ORIG 0x0854a4b0 `movb $0x0,-0x1e(%ebp)` 在
    //   isDoEnchant 调用前预清 doEnchant ⇒ 源必须两句式。
    bool doEnchant = false;
    doEnchant = isDoEnchant(slotA, ((CStackableItem*)materialScript)->m_chars30d);

    // ORIG ProcUpgrade_0854a1ba 605/611 行：delete_item 与 material2 的 GetInvenSlot
    // 各自重新调用 getCurCharacInvenW()（不复用函数头的 invenW/local_34）。
    if (!user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, msg->m_13, 1, (eItemDelReason)0xc, true))
        return (ENUM_ERROR)0x11;

    packet->put_byte(1);
    packet->put_short(msg->m_13);
    Inven_Item material2 = user->getCurCharacInvenW()->GetInvenSlot(1, msg->m_13);
    // [2026-09-10 判据⑮ 多余截断] ORIG 0x0854a595..0x0854a5a4：get_add_info 返回值
    //   经 %eax 直压 0x4(%esp) 传 put_short(int)（无 movzwl/and $0xffff）⇒ 原码
    //   无 `& 0xffff` 截断，删之照抄。
    packet->put_short(material2.get_add_info());
    if (doEnchant)
        packet->put_byte(1);   // ORIG 619 行
    else
        packet->put_byte(0);   // ORIG 616 行

    // [2026-09-10 P272 块序] ORIG 0x0854a5d7 `movzbl doEnchant; xor $0x1; test
    //   %al,%al; je 0x0854a6fc`：**!doEnchant 真则直落 _ProcPenalty 块**
    //   （0x0854a5e6），3rdChronicle 块在 0x0854a6fc（then 在后、else 直落）。
    //   正极性 `if (doEnchant) {A} else {B}` 在 4.4 反产「then 直落」⇒ 写
    //   `if (!doEnchant) {B} else {A}` 复现。
    // [P0] ORIG 3rdChronicle 调用点**无 ret 具名槽**（call 结果直接 xor 判），
    //   且 [P248 档①] callee 返回 bool ⇒ 写 `if (!ProcUpgrade_3rdChronicle(...))`。
    if (!doEnchant)
    {
        int sellPrice = 0;
        // [2026-09-10 判据㉕ 实参来源] ORIG 0x0854a5f0 `mov -0xd2(%ebp),%eax;
        //   movswl %ax,%ebx`：-0xd2 = slotA(-0xd9)+0x07 = m_addInfo2（数量域），
        //   **不是** msg->m_19（ORIG 全函数 movzbl 0x19(%eax) 仅 3 次：_CheckCondition/
        //   ProcUpgrade_3rdChronicle/Enchant3rdChronicleItem 各一次）。旧重建传
        //   (short)msg->m_19 ⇒ 售价统计用错数量来源（真 bug）。
        G_Store()->GetSellItemPrice(slotA, itemScript, (short)slotA.m_addInfo2,
                                    false, sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0xf, user, (unsigned int)sellPrice);
        // [2026-09-10 P198①] ORIG 0x0854a655 `movl $0x0,-0x1c(%ebp)` 预清；
        //   [P225] ORIG 0x0854a697 `cmpl $0x0,-0x1c; setne %al; test %al,%al; je`。
        int penalty = 0;
        penalty = _ProcPenalty(user, &slotA, itemScript, msg->m_0d, packet);
        while (penalty != 0)
        {
            // [P227/P328⑩] ORIG 0x0854a75e 先求值 vararg（getCurCharacNo→%ebx）
            //   再 ctor（movl x3 + lea + call C1），最后 `mov %ebx,0xc(%esp)`；
            //   两语句具名形恒 ctor 先 ⇒ 必须写全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x77e, 5)(
                // [P8S4 2026-09-11] §6.5 真错：ORIG 串（0x0854a75e 站点实参）尾部
                // 无 `\n`，原重建多一个换行 ⇒ 运行期日志文本不一致，删除。
                "3RD_CHRONICLE_LOG : ENCHANT SYSTEM _ProcPenalty() ERROR!! ERROR CODE(%d)!! CHARAC NO(%u)",
                penalty, user->getCurCharacNo());
            return (ENUM_ERROR)penalty;
        }
    }
    else
    {
        // ORIG CItem 虚表 +0xc（GetItemType）返回装备类型。
        // [2026-09-10 P0] ORIG 0x0854a706..0x0854a72d：GetItemType() 结果经
        //   %eax 直接压 0x8(%esp)，**无 equipType 具名槽**（无 store/load 往返）
        //   ⇒ 表达式内联进实参。
        if (!ProcUpgrade_3rdChronicle(user,
                                      (ENUM_EQUIPMENTTYPE)itemScript->GetItemType(),
                                      msg->m_19, slotA, materialScript))
        {
            // [P227] 同上：ORIG 0x0854a80f 先 getCurCharacNo→%ebx 跨 ctor 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0x786, 5)(
                // [P8S4 2026-09-11] §6.5 真错：ORIG 串（0x0854a80f 站点实参）尾部无 `\n`。
                "3RD_CHRONICLE_LOG : ENCHANT SYSTEM ProcUpgrade_3rdChronicle() ERROR!! CHARAC NO(%u)",
                user->getCurCharacNo());
            return (ENUM_ERROR)1;
        }
    }

    if (slotA.m_addInfo != 0)
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_0d, slotA);
    // ORIG 0x0854a8a0 `movzwl 0xd(%eax),%eax; movzwl %ax,%eax`（零扩展两段）
    // ⇒ 原码此处按 unsigned short 读 m_0d（PacketTypes.h 声明为 short，冻结头
    // 不改，调用点显式收窄复现）。
    user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                         (unsigned short)msg->m_0d);

    // ORIG 0x0854a8b2 `movb $0x0,-0x1d; addb $0x1,-0x1d` ⇒ count 是 **char**；
    // 且无 f 指针局部（`movzbl -0xba(%ebp),%eax` = slotA+0x1f 直取）。
    // [L6S6 2026-09-11] 残差 2 条 `movzbl %al,%eax`（我方多）：ORIG 0x0854a8bc/
    //   0x0854a8cb 为单条 `movzbl mem,%eax; and $0x1,%eax`。探针 /tmp/L6S6probe/
    //   p2-p4.cpp 实证 4.4.7 对 `unsigned char & 1`（数组/标量/union 视角、!=0/
    //   ==1/无比较、%2 共 11 形）恒产 `movzbl; movzbl %al; and`；唯**1 位位域**
    //   （g9: `m_bits.b0`）产 ORIG 同形。⇒ 需在 Inven_Item.h 的
    //   STEnchantedSkillOption 匿名 union 增 1 位位域视角（sizeof/偏移/mangled
    //   零变化，P308 允许）后本处即可归零；.h 属禁改域 ⇒ 仅报告，待属主落。
    char count = 0;
    // [L6 父泳道 2026-09-11] 直接用已存在的 1 位位域视角 `m_bits.m_flag`（= bit0，
    // 与 `m_enchFields[i] & 1` 同义）⇒ 试复现 ORIG 单条 `movzbl mem,%eax; and $0x1`，
    // 免去新增 .h 视角（S6 请求）。若不成则回滚并保留原 `& 1` 形。
    if (slotA.m_random.m_skills[0].m_bits.m_flag)  ++count;
    if (slotA.m_random.m_skills[1].m_bits.m_flag)  ++count;

    user->m_historyLog.Enchant3rdChronicleItem(doEnchant,
        itemScript->get_index(), material.m_addInfo, msg->m_19, count);
    return ENUM_ERROR_NONE;
}