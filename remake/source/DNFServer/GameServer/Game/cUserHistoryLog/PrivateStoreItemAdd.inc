// ORIG T 0x868433e（报告全文）："Item+,..."
// GUID 串：ORIG CItemGloballyUniqueIdentifierPrint 对 item+0x15 的渲染
// （W 0x84ebe58/0x84ebe7c，本文件真实建模）。
// b21 NEAR：语义与 ORIG 全等。修：vararg 原缺失 item+0x00 字节（IVar1=*param_3），
//   加在 b32 与 GetReSealCount 之间使 %02X×14+%d+%d 与 26 实参对齐（原 guid 入%02X 移位已消除）。
// b28c-L2 fresh 复验 ndiff 13（orig 165/our 166），残差全部为两族结构性封顶：
//   [N80] ORIG 0x0868434x 在 `call CItemGloballyUniqueIdentifierPrintC1Ev` **之前**
//     就把 op() 实参 `&item+0x15` 物化到 callee-saved %ebx 并跨 ctor 保活
//     （`mov 0x10(%ebp),%eax; lea 0x15(%eax),%ebx; lea -0x2d(%ebp),%eax; mov %eax,(%esp);
//      call C1; mov %ebx,0x4(%esp)`）；4.4 恒 ctor 后求值。本件 print 为**具名对象**
//     （ORIG D1 在 LogHistory 之后、leave 之前的正常路径 0x08684xxx，非语句末临时），
//     故 P227 全表达式临时链翻案不可用（改临时会把 D1 提前、破坏 ORIG 尾段与
//     landing pad 0x0868452x 的 `lea -0x2d; call D1; _Unwind_Resume` 形）⇒ 维持 N80。
//   [N82附] 帧内槽向：ORIG print 聚合对象 @-0x2d、guid 标量 @-0x80（插在 vararg
//     值槽 -0x7c..-0x30 之下），4.4 聚合优先占低槽 + 标量分区 ⇒ print @-0x31、
//     guid @-0x1c（4 处 lea + 2 处 mov 共 6 行）。声明序/pad/临时链均不可翻（N82附）。
//   其余 2 行 = `mov 0x2(%eax),%ebx` 单段直落 vs 4.4 两段中转（N18a）。
//   帧尺寸 sub $0xec、26 个 vararg 装载序与全部 call 目标/宽度与 ORIG 逐条一致。
// b28d-65 复核：去 (int)m_addInfo 冗余 cast 消 N18a 两段中转 ⇒ 165=165；
//   残余仅 N80（ctor 前实参保活）+ N82附（print@-0x2d/guid@-0x80 槽向）+ N107
//   （取址 add-vs-lea）三族，P226 语句表达式实测零收益 ⇒ 维持封顶。
// [b28d-solo-cUserHistoryLog] DIFF 166/165：P227 全表达式临时链（Print()(arg) 作末实参）
// 翻案 N80+N82附——4.4 复现 ORIG 的 ctor 前实参保活与 print/guid 槽向；残差仅 N107：
// 4.4 `add $0x15,%eax; mov %eax,%ebx` vs ORIG `lea 0x15(%eax),%ebx`（数组名退化/
// 语句表达式/reinterpret_cast 三形实测同形；N107 六形探针封顶）。
// [2026-09-09 复核] 上面 N80/N82附/N18a 历史族已全部消解，当前仅剩 N107 ×1；
// 补测 union 数组视角/引用实参/`(char*)&`/`static_cast<const void*>` 等共 16 形
// （/tmp/hlprobe/tb4/tb5/tb6）全部产 add+mov ⇒ 成员取址 N107 封顶。
// [SweepHistoryLog2 2026-09-09 复核] 再试 12 形（/tmp/print_probe{,2,3,4}.cpp 双后端）：
// 引用形参恒 add、唯指针形参才 lea（ORIG operator() 为引用，nm 0x084ebe82 实证）⇒ 维持封顶。
void cUserHistoryLog::PrivateStoreItemAdd(INVEN_TYPE invenType, Inven_Item const& item,
                                          int addNew, int addOld, eItemAddReason reason,
                                          char const* name1, char const* name2)
{
    m_user->LogHistory(
        "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
        (int)invenType, item.m_addInfo, addNew, addOld, (int)reason,
        name1, name2,
        item.GetUpgrade(),
        (unsigned)item.m_fieldb,
        item.m_random.m_random.m_fields[0].m_field0, item.m_random.m_random.m_fields[0].m_field1,
        item.m_random.m_random.m_fields[0].m_field2, item.m_random.m_random.m_fields[1].m_field0,
        item.m_random.m_random.m_fields[1].m_field1, item.m_random.m_random.m_fields[1].m_field2,
        item.m_random.m_random.m_fields[2].m_field0, item.m_random.m_random.m_fields[2].m_field1,
        item.m_random.m_random.m_fields[2].m_field2, item.m_random.m_random.m_seed9.m_field,
        item.m_random.m_random.m_fielda.m_field0, item.m_random.m_random.m_fielda.m_field1,
        item.m_random.m_random.m_fielda.m_field2, item.m_random.m_random.m_seedd.m_field,
        item.m_field0,            // ORIG IVar1 = *param_3（item+0x00 字节）
        item.GetReSealCount(),
        CItemGloballyUniqueIdentifierPrint()(
            *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
}