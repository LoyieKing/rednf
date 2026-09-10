unsigned int DisPatcher_MoveItem::check_error(CUser* user, MSG_BASE& msg)

{
    const MSG_MOVE_ITEMSPACE& req =
        *reinterpret_cast<const MSG_MOVE_ITEMSPACE*>(&msg);
    if ((int)req.m_field14 < 0)
    {
        return -1;
    }
    char needLock = 0;
    // [b28e-L5] P212/P331 实测不可翻（已回退）：ORIG bool 局部占深槽 -0x4e
    // （0x081c57ec `movb $0x1,-0x4e`；0x081c5803 `lea -0x4e,%edx` 传址），而 4.4
    // 无论块内声明（-0x11）或提至函数头（仍 -0x11，且多一条 ORIG 无的
    // `movb $0x0,-0x11` 预清）都不可复现 ⇒ 归 4.1/4.4 块作用域槽分配差（P212 限定
    // 「混合尺寸区声明序不可控」），保持原形。
    // ORIG 0x081c57cf 决策树：cmp $0x5/jg、cmp $0x4/jge、cmp $0x3/je、cmp $0xe/jne
    // ⇒ 原码是 switch(寄存器直比，无槽，P251/P261)，叶块物理序 4/5/0xe 先、3 后。
    switch ((int)(char)req.m_toSpace)
    {
    case 4:
    case 5:
    case 0xe:
        needLock = 1;
        {
            bool movable = true;
            CheckItemTradeLimitCountToMoveByUser(
                *user, GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_fromSpace),
                (int)req.m_fromPos, movable);
            if (!movable)
            {
                return 0x72;
            }
        }
        break;
    case 3:
        if ((unsigned char)req.m_fromSpace == 7)
        {
            Inven_Item item = ((CInventory*)user->getCurCharacInvenR())
                                  ->GetInvenSlot(3, (int)req.m_fromPos);
            // [DPC_CheckErr] P198②/codegen：ORIG 0x081c587d-0x081c58a6 该条件为
            // **bool 寄存器物化岛**（`test %al,%al; je 0x081c5899; <右条件>; mov
            // $0x1,%eax; jmp 0x081c589e; 0x081c5899: mov $0x0,%eax; 0x081c589e:
            // test %al,%al; je 0x081c58a9`，值在 %eax 无栈槽）⇒ 原码的 `&&` 结果
            // 被物化为临时 bool 再作 if 条件。我方原 `({ ... })` 语句表达式令 4.4
            // 直跳折叠（少 5 条）。试 `== true` 形触发 4.4 的 bool 归一物化。
            if ((item.m_field0 != 0 && req.m_fromPos > 0x8b) == true)
            {
                needLock = 1;
            }
        }
        break;
    default:
        break;
    }
    // ORIG 0x081c58a9：&& 物化岛（mov $1/jmp/mov $0/test/je）后再判，非直跳。
    if (({ needLock != 0 && user->CheckItemLock(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_fromSpace),
                (int)req.m_fromPos); }))
    {
        return 0xd5;
    }
    return 0;
}
