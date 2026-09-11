// ============================================================================
// CCreatureItemConverter::SetEggItem —— ORIG 0x08336eae (T)
// _ZN13user_creature22CCreatureItemConverter10SetEggItemEP10Inven_Item
// 语义（反编译）：槽物 item 的 itemIdx = *(int*)(item+2)；
//   find_item(itemIdx) 为 0 → 返回 0；
//   物品 is_stackable → 返回 0；
//   否则按 CEquipItem::get_sub_type()：
//     ==1 → 仅置 item->m_field0 = 1；
//     ==2 → get_rand_int(m_randomTable.m_field0) 得随机权重，
//           GetRandomItemIndex(table, idx) 得目标 itemIdx，
//           写回 item->m_addInfo 并置 item->m_field0 = 1；
//   均返回 1。
// ============================================================================
// [b28d-23 N·P133] 分支布局/极性（get_sub_type 两跳同落）⇒ N。ORIG 0x08336eae
// [b28d-solo-CCreatureMgr N] N111：ORIG switch 决策链残留死 `test %eax,%eax`（4.4 不产死指令，补 case 0 被折叠）；ORIG 0x8336eae
// [b28d-solo-CCreatureMgr 复测] 58 vs 57（差即该死 test）；case 0 已试折叠 ⇒ 维持 N111 封顶。
// [2026-09-11 收口判词 · N111 死指令] ORIG 0x08336eae（58 条）vs 我方 57 条（gap +1）。
//   ORIG 0x8336f0f `test %eax,%eax` 后紧跟无条件 `jmp 0x8336f55`（flags 无消费者）
//   = 4.1 对「case 0 与 default 同 leaf」保留的合并比较残留；4.4 前端对该死比较恒 DCE。
//   已试形（历史 + 本轮复查）：补 `case 0: break;`（4.4 视为与 default 同体直接折叠）、
//   if-else 等价链（同样不产 test）⇒ 合法 C++ 不可造死指令 ⇒ N111 封顶，勿再耗轮次。
//   p167 调用点差集 **CLEAR**（G_CDataManager/find_item/is_stackable/get_sub_type/
//   get_rand_int/GetRandomItemIndex 计数全等）⇒ 语义无缺口。
bool CCreatureItemConverter::SetEggItem(Inven_Item* item)
{
    // ORIG 0x08336eae：入口无 null 检查（直 `mov 0x2(%eax),%eax`），删臆造判空。
    // ORIG push %ebx：`mov 0x2(%eax),%eax; mov %eax,%ebx` 跨 call 直落寄存器
    // （P0 无槽往返）⇒ P197② 裸语句表达式实参形（CArtifact::EquipArtifact 同法实证）。
    CItem* found = G_CDataManager()->find_item(({ item->m_addInfo; }));
    if (found == 0)
        return 0;
    if (found->is_stackable())
        return 0;

    // ORIG：`mov -0x14,%eax; mov %eax,-0x10(%ebp); mov -0x10(%ebp),%eax; call`
    // equip 具名槽 -0x10；subType 无槽，switch 三级下探 cmp1;je;cmp2;je;test
    // （技巧 34 直写 switch）。
    // [batch-19 N 封顶 ndiff=1] ORIG +0x61 的 `test %eax,%eax` 是**死指令**：
    // 其后紧跟无条件 `jmp +0xa7`（= `mov $1,%eax` 出口），flags 无消费者 ⇒
    // 4.1 为 `case 0:`/`default:` 同体生成的合并比较残留。实测：补
    // `case 0: break;` 后 4.4 直接折叠该比较（仍 64 insn，ndiff 不变）⇒
    // 源码层无法保留死 test，封顶。
    CEquipItem* equip = static_cast<CEquipItem*>(found);
    switch (equip->get_sub_type())
    {
    case 1:
    {
        item->m_field0 = 1;
        break;
    }
    case 2:
    {
        // ORIG 0x8336f1b..0x8336f52：仅 range 具名槽(-0xc)；randIdx/target 均
        // 内联（call 结果 %eax 直落 arg，无槽）。
        int range = equip->m_randomTable.m_field0;
        item->m_addInfo = equip->m_randomTable.GetRandomItemIndex(get_rand_int(range));
        item->m_field0 = 1;
        break;
    }
    // [TowerSkill20 2026-09-11 两形试毕·已回滚] ①去 `default:` 无标签形、②`case 0: default:`
    // 合并形 —— 实测均仍 57 条 / ndiff 1（4.4 前端恒折叠该比较）⇒ ORIG 0x08336f0f
    // `test %eax,%eax`（后随无条件 jmp）确认不可造，维持 N111 封顶。
    default:
        break;
    }
    return 1;
}
