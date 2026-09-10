bool CTradeSpace::_IsTradable(const Inven_Item& item)   // [b28d-Main·P292] int→bool
{
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    if (ci == 0)
        return false;
    if (item.m_upgradeSep.IsTradeRestriction())
        return false;
    // [b28f-L27 判据①/P17] ORIG 0x08529de2..0x08529e5d 是 `1<<t` 位掩码派发
    //   （cmp $5/ja 范围检查 → `&0x36` → `&0x8` → `&0x1` 三组测试，
    //   末组 `and $1;test;jmp tail` 为 case0/default 同体残留）
    //   ⇒ 原码是 switch（if 链恒被折成内存直比链，永不产 1<<t 位掩码）。
    //   位掩码组：{1,2,4,5}⇒return false（0x36）、{3}⇒m_field0 检查（0x8）、
    //   {0}⇒与 default 同体（0x1）。m_field0 为 bool ⇒ `!item.m_field0`
    //   发 ORIG `movzbl;xor $1;test;je tail`（P292）。
    //   ORIG `mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax; cmp $0x5,%eax` 有槽往返
    //   ⇒ 原码是具名 t（P0）。
    //   [b28f-CTS 复核] 三处旋钮：
    //   ①局部改 int + `switch ((unsigned int)nType)` ⇒ 4.4 复现 ORIG
    //     `mov -0xc(%ebp),%eax; cmp $0x5,%eax; ja`（P310：unsigned 局部恒折内存直比，
    //     显式 unsigned cast 触发电寄存器装载）；
    //   ②case 0 内嵌**空 switch** `switch (nType & 0x1) { }`（配方㉓）⇒ 复现 ORIG
    //     第三组 bit test `and $0x1,%eax; test %eax,%eax; jmp tail`（4.4 对
    //     `case 0: break;` 会把该测试整段合并掉，少 3 条）；
    //   ③②带来的 eax 存活需求同时翻正第二组测试为 ORIG 的 `mov %eax,%edx; and $0x8,%edx`。
    int nType = ci->GetAttachType();
    switch ((unsigned int)nType)
    {
    case 1:
    case 2:
    case 4:
    case 5:
        return false;
    case 3:
        if (!item.m_field0)
            return false;
        break;
    case 0:
        switch (nType & 0x1) { }
        break;
    }
    return true;
