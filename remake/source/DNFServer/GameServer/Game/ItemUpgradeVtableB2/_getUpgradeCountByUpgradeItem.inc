// ============================================================================
// _getUpgradeCountByUpgradeItem @ 0x08548e68
//   [b28d-58] AE（P46 叶块排放序 = case 书写序）：原 case 序 1d29/1c6a/1c6b/1c69…
//   使各 je 落点与 ORIG 叶块错位；改 ORIG 排放序 1c69/1c6a/1c6b/1d29/1e3f/1e40/1e86
//   后逐条全等（ORIG 0x08548ecd..0x08548ee5）。
//   默认按材料 idx 映射强化档位（仅非增幅物品）；材料为堆叠物时改读
//   ReinforceTicket/AmplifyReinforceTicket 数值（[0,0x1f] 越界归零）。
//   增幅券在目标无增幅能力时报 ENUM_ERROR 0x13。
// ============================================================================
unsigned short CItemUpgrade::_getUpgradeCountByUpgradeItem(
    const Inven_Item& target, const Inven_Item& material, ENUM_ERROR& err)
{
    short count = 0;
    // ORIG 0x08548e7c：hasAbility 每用点重取（P48，%al 直测）。
    // ORIG 为正向 `test %al,%al; jne`（P59）：then 空 else 才是 switch。
    if (target.m_amp.hasAbility())
    {
    }
    else
    {
        switch (material.m_addInfo)
        {
            // 叶块排放序 = case 书写序（P46）；照 ORIG +0x65..+0x95 排：
            // 1c69/1c6a/1c6b/1d29/1e3f/1e40/1e86（ORIG 0x08548ecd..ee5）。
            case 0x1c69: count = 7;  break;
            case 0x1c6a: count = 10; break;
            case 0x1c6b: count = 12; break;
            case 0x1d29: count = 7;  break;
            case 0x1e3f: count = 3;  break;
            case 0x1e40: count = 5;  break;
            case 0x1e86: count = 13; break;
            default:     count = 0;  break;
        }
    }

    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script != 0 && script->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)script;
        if (stack->IsReinforceTicket())
        {
            // ORIG 0x08548f70 `cwtl` + int 槽 -0x10 + `cmpl` 区间判定，末了
            // `mov %ax,-0x1a` 落 short ⇒ 票面值先入 int 局部（P10/P11）。
            const int value = (short)stack->GetReinforceVaule();
            if (value < 0 || 0x1f < value)
            {
                count = 0;
            }
            else
            {
                count = (short)value;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!target.m_amp.hasAbility())
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            const int value = (short)stack->GetAmplifyReinforceValue();
            if (value < 0 || 0x1f < value)
            {
                count = 0;
            }
            else
            {
                count = (short)value;
            }
        }
    }
    return (unsigned short)count;
}