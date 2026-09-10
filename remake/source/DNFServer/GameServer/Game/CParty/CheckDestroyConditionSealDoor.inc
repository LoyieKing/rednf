bool CParty::CheckDestroyConditionSealDoor(CDungeon const* dungeon,   // b28d-Main：与 CParty.h:509 同步（P292 bool）
                                          std::vector<int>& list1,
                                          std::vector<int>& list2)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS 非 const normal_iterator 3 项 + EXTRA const 版
    // 4 项（P96）；改 iterator 实测 identical 回归，暂封 N。@0x85a7d86
    // ORIG 0x85a7d86：入口 4×int 初始化 + Inven_Item 局部（-0x6d，非对齐槽——
    // 4.1 结构拷贝按 dword+残基拆分，本工具链按对齐槽整体赋值，拷贝序列差属
    // 跨版本布局伪差）+ result 字节槽 + i。list2 用迭代器双缓存
    // （begin 循环前一次、end 每轮重取、operator!= 调用）；
    // GetInvenSlot(1,slot) 结果先整体赋给 item，再比 item.m_addInfo2。
    // P331：ORIG 0x085a7d90..0x085a7da5 初始化序 = passCount(-0x20)、validCount(-0x1c)、
    // haveCount(-0x18)、slot(-0x14) ⇒ 声明序照抄（同尺寸标量区先声明更深）。
    int passCount = 0;       // local_24
    int validCount = 0;      // local_20
    // b28d-solo-CParty2 试形记录：ORIG 0x85b7fxx `mov -0x18(%ebp),%ebx` 为 callee 前
    // 寄存器装载形；haveCount 改 unsigned + `(unsigned int)list2.size()` 比较实测仍折
    // `cmp -0x18(%ebp),%eax` 内存直比（S1 一次旋钮已用）⇒ 恢复 int 直证形态，判 N18b。
    // [2026-09-10 F2 复查（C05 范式②，已实测并回退）] 改 `unsigned int haveCount`
    // + 无 cast 比较 `haveCount == list2.size()` 后 **仍折** `cmp -0x18(%ebp),%eax`
    // 内存直比（ORIG 0x085a7fd7 为两段 `mov -0x18(%ebp),%ebx; call size;
    // cmp %eax,%ebx; sete`），且总条数 188→187、ops 1→3 / delta 2→4 **劣化**
    // ⇒ F2 在本上下文（比较对象为 `size()` 调用返回值 + 跨越界检查）不成立，
    // 维持 int 直证形态 + 显式 cast 比较，判 N18b。
    int haveCount = 0;       // local_1c
    int slot = 0;            // local_18
    Inven_Item item;         // local_6d
    // ORIG 0x085a802f `movzbl -0xd(%ebp),%eax` 无 setne 归一化 ⇒ bool 直返（P9 勘误/P292）。
    bool result = false;     // local_11（ORIG 0x085a7db7 movb $0,-0xd）
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        ++validCount;
        haveCount = 0;                       // ORIG 0x085a7de8（++validCount 之后立即）
        // P183：ORIG 每处重算 m_member[i].m_pUser（0x085a7def/0x085a7ec3），无 u 缓存。
        // ORIG 0x085a7e1f `je <主体>` ⇒ 正极性 then=++passCount。
        if (CheckHellDungeonFreepassItemHaveAndDel(m_member[i].m_pUser, false))
        {
            ++passCount;
        }
        else
        {
            if (dungeon->get_min_level() < 0x2d)
                return 0;
            int needCount = getHellPartyEntranceItemNeedCount(dungeon->get_min_level());
            // Ghidra C 实证 ORIG 用非 const `__normal_iterator<int*,vector<int>>`
            //（0x085a7e6c begin / 0x085a7fca operator!= 均无转换构造临时）。
            for (std::vector<int>::iterator it = list2.begin();
                 it != list2.end(); ++it)
            {
                slot = m_member[i].m_pUser->getCurCharacInvenR()->check_item_exist(*it);
                // P225 试形记录（已回滚）：`while (slot != -1) { body; break; }` 可补回
                //   ORIG 0x085a7eb4 的 `setne %al; test %al,%al` 两段，但同时多发
                //   `jmp <join>; nop`（+2）且总条数 187→193 劣于 ORIG 191 ⇒ 回滚为 if 形。
                // [2026-09-10 试形·已回滚] P226/P316 语句表达式 `({ slot != -1; })` 实测
                // 仍产 4.4 直跳（无 setne/test）⇒ ORIG 0x085a7eb8 的两段 = 跨版本 bool
                // 物化（P292/N122 族），非源形可控。判 N。
                if (slot != -1)
                {
                    item = m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(1, slot);
                    if (item.m_addInfo2 >= needCount)
                        ++haveCount;
                }
            }
            // P310：ORIG `mov -0x18(%ebp),%ebx; call size; cmp %eax,%ebx; sete`
            //   两段寄存器形；F2 同型无 cast 形实测劣化 ⇒ 维持显式 cast 比较。
            if ((unsigned int)haveCount == (unsigned int)list2.size())
                ++passCount;
            else
                list1.push_back(i);
        }
    }
    if (validCount <= passCount)
        result = 1;
    return result;
