unsigned int Dispatcher_PcRoomRentItem::check_error(CUser* user, MSG_BASE& msg)
{
    // 【实测记录 batch-17】改写为 `invalid = (state != 3) || (characR == 0)`
    // 命名 bool + `if (invalid) return 3;` 可让首跳方向与 ORIG 一致（jne），但
    // 4.4 会把 bool 落槽再 `mov; cmpb` 回读（70/71）；保持合取原形为 70/70，
    // 故此处保留 && 形，方向差记 N76/N18a（勿再改写）。
    // [b28d-solo-S206] P224/P45 极性翻正：ORIG 0x081e5762 `cmp $3,%eax; jne <mov$1岛>`
    //   + `call; test; jne <mov$0岛>` + 汇聚 `test %al; je 主体; mov $3` ⇒ 原码是
    //   否定短路 `state!=3 || characR==0`（非 `A&&B{body}` 形，后者产 je 反向首跳）。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return 3;
    }
    {
        // ①ORIG 0x081e577x：msg 引用槽在 state/characR 判定**之后**才物化
        //   （`mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`，技巧13 声明位=落栈时机）；
        //   我方在函数入口物化，槽号亦从 -0xc 漂到 -0x10。
        MSG_RENT_ITEM& req = reinterpret_cast<MSG_RENT_ITEM&>(msg);
        // ②ORIG 对该 byte 字段是**两次独立装载 + 寄存器测**（`movzbl 0xe(%eax),%eax;
        //   testb %al,%al; jle` 与再装载一次 `cmp $2,%al; jle`），我方具名 char 局部
        //   使 4.4 折成 `mov %al,-0x9(%ebp)` + 两次 `cmpb $K,-0x9(%ebp)`（技巧36）。
        //   整数域内 `type <= 0` 与原 `type < 1` 等价，语义不变。
        if (static_cast<signed char>(req.m_0e) <= 0 ||
            static_cast<signed char>(req.m_0e) > 2)
        {
            return 8;
        }
        if (user->GetPCRoomNo() == 0 && static_cast<signed char>(req.m_0e) == 1)
        {
            return 8;
        }
        // type==2：重复租借事件(0x5e)未进行中 → 错误。vptr+0x34 槽 = IsEventing。
        // 【batch-17 DispC8 交接项】必须写**嵌套独立 if**：ORIG 0x081e57f5..0x081e5835
        // 是 `cmp $0x2,%al; jne 尾` + `call IsEventing; xor $0x1,%eax; test%al;
        // je 尾` + `mov $0x8,%eax; jmp end`，即两个条件各自 jcc 直跳、**无 bool
        // 汇聚**；写成 `A && !call()` 会让 4.4 生成 `mov$1/jmp/mov$0` 汇聚岛
        // （实测多 7 insn，70/75 → 70/70 的达成点）。
        if (static_cast<signed char>(req.m_0e) == 2)
        {
            if (!GlobalData::s_event_manager->GetRepeatEvent(0x5e)
                     ->IsEventing((CUser*)0))
            {
                return 8;
            }
        }
        return 0;
    }
