unsigned int DisPatcher_MoveItem::check_error(CUser* user, MSG_BASE& msg)

{
    const MSG_MOVE_ITEMSPACE& req =
        *reinterpret_cast<const MSG_MOVE_ITEMSPACE*>(&msg);
    if ((int)req.m_field14 < 0)
    {
        return -1;
    }
    char needLock = 0;
    // [b28e-DispCore] P339 头部声明试验（movable 提前到函数头、赋值留块内）实测
    // **槽位不变**（仍 -0x11）⇒ 属 4.1 类对象(sret)与标量的分区差，判 N99/N82 族。
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
            // [K6-DpcSigSmallCheckErr·负结论] ORIG 0x081c587d-0x081c589e 为 **4.1
            // 分支式 bool 物化岛**（`test %al,%al; je 0x081c5899`（左假）→
            // `mov $0x1,%eax; jmp 0x081c589e` / 0x081c5899 `mov $0x0,%eax` /
            // `test %al,%al; je 0x081c58a9`，值在 %eax 无栈槽）⇒ ORIG 少 5 条对应我方
            // 直跳折叠。三形实测（本轮全部复编验证）：①`== true` ②裸 `({ … })` 语句
            // 表达式（P226 条件向）③`( … ) ? true : false` —— ours_insns **恒 103**
            // （ORIG 108）⇒ 4.4 对无副作用 `&&` 恒直跳、且即便要求取值也只发 setcc 而
            // 非 4.1 的分支式 0/1 物化 ⇒ 归跨版本 codegen（4.1 分支物化 vs 4.4 setcc）
            // 天花板，保持原形。语义等价。
            // [DPC-SigSmall 2026-09-11 二轮·终] ORIG 0x081c587d-0x081c58a6 的 0/1 岛
            // （`je L0` 左假 → `mov $1; jmp` / L0 `mov $0` / `test %al; je`）**不可造**：
            // probe9（-m32 -O0 c6446r）证 4.4.6 在 if 上下文对 `A && B`、
            // `(A && B) == true`、嵌套 if **全部折直跳**（t1/t2/t4 均无 `mov $1,%eax`），
            // 唯一产岛的是含**函数调用**的侧（本件另一处 `({ needLock && CheckItemLock(); })`
            // 与 probe4 w7 同因：调用结果需 bool 规范化）；本轮再试语句表达式包住
            // 纯字段比较形，实测 ours 仍 102 / ndiff 仍 15（逐字节不变）⇒ 维持 N 封顶。
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
