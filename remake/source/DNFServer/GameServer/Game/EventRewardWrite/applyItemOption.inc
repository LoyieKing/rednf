// ============================================================================
// eventReward::applyItemOption —— ORIG T 0x080f0d6e
// ============================================================================
void eventReward::applyItemOption(Inven_Item& inven, const CItem* item,
                                  const itemData& data)
{
    if (item->is_stackable())
    {
        inven.m_addInfo2 = data.m_count;            // item+0x07 = data+0x08
        return;
    }
    reinterpret_cast<unsigned char&>(inven.m_seal) = data.m_seal;
    register unsigned int attachType = item->GetAttachType();  // ORIG 单次调用（寄存器化）
    // 负结论（4.4 位折叠族，N 类；本波五变体实测，75 为封顶最优）：
    //  ORIG 0x080f0da7–0x080f0dc3 = `call; cmp $5,%eax; ja` + `mov $1,%edx; mov %edx,%ebx;
    //  mov %eax,%ecx; shl %cl,%ebx; mov %ebx,%eax; and $0x27,%eax; test %eax,%eax`
    //  （attachType 直用 %eax，无具名寄存器副本）。
    //  4.4 恒折 `(1u<<x)&0x27` → `(0x27>>x)&1`（shr $cl + and $1）+ 多发 `mov %eax,%ebx`；
    //  实测：操作数序交换 75、`(int)`cast 75、有符号 `1<<x`（sar）75、嵌套 if 75、
    //  P226 语句表达式 80（语句内变量落 -0xc 槽 + setne 岛）⇒ 均不可消，74/75 封顶。
    // [P8S5 2026-09-11 负结论] 掩码 cast 破位折叠**无效**：`(unsigned long)1 << attachType`
    // & 0x27 实测与 `1u` 逐条同形（blocks 8、our 75 不变 ⇒ 4.4 仍折 `shr %cl; and $1`）
    // ⇒ 已回滚。封顶维持 74/75（ORIG 0x080f0da7 `mov $1,%edx; shl %cl,%ebx` 形不可造）。
    if (attachType < 6 && ((1u << attachType) & 0x27))
    {
        inven.m_seal = 0;
    }
    if (data.m_upgrade > 1)                         // +0x00 字节
    {
        inven.SetUpgrade(data.m_upgrade);
    }
    // ORIG 0x80f0d6e：虚调用 vptr+0x4c = vtable+0x54 = vt[21] IsRandomOption
    // （vptr 指向 vtable+8；勿与 vt[19] IsPossibleKiriProtection 混淆）
    // ORIG +0x91..+0xaf：短路物化岛（mov $1/jmp/mov $0 汇聚 + test）⇒ P226 语句表达式形。
    // ORIG +0x98 `movzbl 0x2(%eax),%eax; xor $0x1,%eax; test %al,%al; je`（取反物化
    //   ⇒ P357 字段轴/P158：+0x02 字段真型 bool）。
    // [R19ty T-2a 2026-09-13] 字段已由 `unsigned char` 改 `bool`（itemData.h）；本 `== 0`
    //   写法在 bool 字段下由 4.4 直接产出 ORIG 形（fresh .o 实证：`movzbl 0x2(%eax),%eax;
    //   xor $0x1,%eax; test %al,%al; je` = ORIG 0x080f0e06/0a/0d/0f 全等），写法无需改动。
    //   （旧注释「uchar 下 `== 0` 与 `!m_roi` 均产 test;jne 无 xor」已随类型轴更正。）
    if (({ item->IsRandomOption() && data.m_roi == 0; }))  // +0x02 字节
    {
        this->unsealRandomOption(item, inven);
    }
}