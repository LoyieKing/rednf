// b21 NEAR：语义与 ORIG 全等（两真修已落，CItemList 编译绿）。残余仅 N-86b：
//   冗余 setne/test、虚拟调用寄存器选择（call* %eax vs call* %edx）、shl 寄存器分配、
//   IsTradeRestriction 后冗余 setne（结果本就是 0/1，行为全同）+ 全量 AE/分支偏移。
// [b28c-lane17] fresh 复验：258=258 助记符全等 ⇒ NEAR（残差同上 N86b/N7 族），
// 调用点差集无缺项，判「语义完成、byte 层 N 封顶」。
// [b28d-20] fresh 复验：4.4.6 路由下 256=256 **IDENTICAL**（归一化零差异；
//   旧 N7 `call *%eax/%edx` 差在 446h46 后端下消失，P230 分诊法命中）。
//   TSV 基线 NEAR/1 系 4.4.7 口径，已由本轮 routed 复验推翻。
// [b28d-solo-CItemList] 复验 = IDENTICAL/AE（compare_tu 口径 0 差异行）。
int CCompound_ExtreamDun_Item::checkCompoundable(                   // ORIG T 0x832d7d6
    Inven_Item* items, const CEquipItem** refs, char kind) const
{
    int inputType[2];
    for (int i = 0; i < 2; ++i)
    {
        if (items[i].isEmpty() || refs[i] == 0)
        {
            return 0x31;
        }
        inputType[i] = _convertInputItem(
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)refs[i]->GetUsableEquipmentType());
        // vtable +0x4c 槽（P5 直调）
        if (refs[i]->IsRandomOption())
        {
            return 7;
        }
        if (inputType[i] == 3)
        {
            return 0x17;
        }
        if (inputType[i] == 2 && refs[i]->getItemGroupName() > 0x19 &&
            refs[i]->getItemGroupName() <= 0x32)
        {
            return 0x17;
        }
        if (!items[i].m_amp.isIdentified())
        {
            return 0x13;
        }
        if ((unsigned char)items[i].m_seal != 1)
        {
            return 0x12;
        }
        // ORIG +0x115/+0x123：两次独立 getItemGroupName 调用（P48 无 CSE）。
        // ORIG +0x173 `jle`（有符号 ≤0 落 bVar1=true 岛）/ +0x18b `jbe`（无符号
        // ≤0x3a 落 bVar1=false 岛）双岛形 ⇒ 原码是短路 `||`：`(signed)a <= 0 ||
        // (unsigned)b > 0x3a`（旧 `&&` 写法把区间判反 = 真语义 bug）。
        if (refs[i]->getItemGroupName() <= 0 ||
            (unsigned int)refs[i]->getItemGroupName() > 0x3a)
        {
            return 0x11;
        }
        if (refs[i]->GetAttachType() == 1)
        {
            return 1;
        }
        // vtable +0x48 槽（P5 直调）
        if (refs[i]->PeriodAfterUnsealing() != 0)
        {
            return 1;
        }
        if (items[i].m_upgradeSep.IsTradeRestriction())
        {
            return 1;
        }
    }
    if (inputType[0] != inputType[1])
    {
        return 0x16;
    }
    if (kind == 0)
    {
        if (inputType[0] != 0)
        {
            return 0x11;
        }
    }
    else if (kind == 1 || kind == 3)
    {
        if (inputType[0] != 1 && inputType[0] != 2)
        {
            return 0x11;
        }
    }
    else
    {
        return 0x11;
    }
    if (kind == 3)
    {
        if (refs[0]->getUsableLevel() > 0x3c || refs[1]->getUsableLevel() > 0x3c)
        {
            return 0x11;
        }
    }
    else
    {
        // ORIG else 块返回 0x11 条件 = BOTH > 0x45（asm `cmp; jle 定 false` 两级
        // 与 Ghidra `!(L0<0x46) && !(L1<0x46)` 一致）；kind==3 块才是 OR
        // （`jg` 短路 true / `!(L0<0x3d)||!(L1<0x3d)`）。原 `||` 语义差异，改 &&。
        if (refs[0]->getUsableLevel() > 0x45 && refs[1]->getUsableLevel() > 0x45)
        {
            return 0x11;
        }
    }
    return 0;
}