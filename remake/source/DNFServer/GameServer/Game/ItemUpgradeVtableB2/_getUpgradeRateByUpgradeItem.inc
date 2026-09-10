unsigned char CItemUpgrade::_getUpgradeRateByUpgradeItem(
    const Inven_Item& target, const Inven_Item& material, ENUM_ERROR& err)
{
    char rate = 0;
    // ORIG 0x08549018：hasAbility 每用点重取（P48，结果 %al 直测无槽）。
    // ORIG 0x0854901b 为正向 `test %al,%al; jne`（P59）：then 为空 else 才是
    // switch，负向 ! 形会多出 xor $0x1 且 je 反向。
    if (target.m_amp.hasAbility())
    {
    }
    else
    {
        // ORIG 0x0854901d..0x08549059 是 switch 二分决策树（首比较 0x1d29、
        // `sub $0x1c69; cmp $0x2; ja` 小区间、0x1e3f/0x1e40 jb/jbe、0x1e86），
        // 命中共享叶 `movb $0x64,-0x19; movzbl; jmp 返回`；default 叶
        // `movb $0,-0x19` 后**继续**执行（不返回）⇒ 必须写 switch + case 升序
        // （P46/P163），布尔区间链会退化成 cmp 序列且丢 18 insn。
        switch (material.m_addInfo)
        {
            case 0x1c69:
            case 0x1c6a:
            case 0x1c6b:
            case 0x1d29:
            case 0x1e3f:
            case 0x1e40:
            case 0x1e86:
                rate = 100;
                return (unsigned char)rate;
            default:
                rate = 0;
                break;
        }
    }

    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script != 0 && script->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)script;
        if (stack->IsReinforceTicket())
        {
            // ORIG +0x0cb 用 `cmpb` 字节直接判定（char 槽 -0xd 存 rate），非 int。
            char value = (char)stack->GetReinforceRate();
            if (value < 0 || 100 < value)
            {
                rate = 0;
            }
            else
            {
                rate = value;
            }
        }
        else if (stack->IsAmplifyReinforceTicket())
        {
            if (!target.m_amp.hasAbility())
            {
                err = (ENUM_ERROR)0x13;
                return 0;
            }
            const int value = (char)stack->GetAmplifyReinforceRate();
            if (value < 0 || 100 < value)
            {
                rate = 0;
            }
            else
            {
                rate = (char)value;
            }
        }
    }
    return (unsigned char)rate;
