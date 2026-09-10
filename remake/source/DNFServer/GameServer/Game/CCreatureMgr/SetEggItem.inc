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
    default:
        break;
    }
    return 1;
