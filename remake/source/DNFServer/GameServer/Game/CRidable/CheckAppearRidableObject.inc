// ORIG 0x82fe9c8：遍历 m_objects；坐标 (m_x,m_y)==(a,b) 且 state==4 或
// (state==3 && m_flag18) 的对象置 state=1、计数并发送生成包；循环后
// p->put_byte(idx, 计数) 收尾。party 形参 ORIG 未使用（保留签名）。
// 成员偏移按 CRidable.h：pair.second 起 +4 → m_x@+4/m_y@+8/m_field0c@+0xc/
// m_field10@+0x10/m_field14@+0x14/m_field18@+0x18/m_state@+0x1c。
// [A28 round5 定稿] 本轮修：①`begin()` 提到 counter 之前（ORIG 0x082fe9d1..de
//   先 begin → counter(-0x1c) → idx(-0x28) → put_byte）；②`p->put_byte(counter)`
//   （ORIG `mov -0x1c(%ebp),%edx` 读槽变量，非立即数 0）。
//   残余 1 条：ORIG `mov 0x18(%eax),%eax; mov %eax,%edi` vs 4.4 折单段
//   `mov 0x18(%eax),%edi`（%edi 直落）⇒ N18a（4.4 单段直落不可造；P254 enum 化
//   需改 CDungeon.h 的 RidableObject（多 TU 共享）且无 ORIG enum 证据，不改）。
//   [A28 round5 · C07 命中形实测 2026-09-10 19:48] 按 Main 转发的「字段读升格
//   callee-saved ⇒ 写具名 int 局部」试一次旋钮：**反劣**（帧 0x3c→0x40、丢
//   `push %edi`、迭代器槽 -0x24→-0x18 整体平移）⇒ 已回滚，N18a 维持（一次旋钮
//   纪律：无改善即回滚封顶）。
//   与 A14 `CPrivateStore::BuyItem` 反例同构：本件的字段读消费者是**被调方的按值
//   实参**（SendSpawnRidableObject 第 5 参），非「call 后仍被本函数复用」⇒ 具名
//   局部必然落槽 +2，符合 C07 命中形的否定边界。
void CRidable::CheckAppearRidableObject(CParty*, int a, int b, PacketGuard* p)
{
    // ORIG 0x082fe9d1 段序（P28/P50「先声明后赋值」）：begin() 最先
    //   （0x082fe9d1..de，it 落 -0x24）→ counter(-0x1c)=0 → idx(-0x28)=get_index()
    //   → `put_byte(counter)`（ORIG `mov -0x1c(%ebp),%edx` 读槽变量，非立即数 0）
    //   → jmp 底部条件。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.begin();
    int counter = 0;
    int idx = p->get_index();
    p->put_byte(counter);
    for (; it != m_objects.end(); ++it)
    {
        if (it->second.m_x == a && it->second.m_y == b)
        {
            if (it->second.m_state == 4 ||
                (it->second.m_state == 3 && m_flag18 != 0))
            {
                it->second.m_state = 1;
                ++counter;
                // ORIG 0x082feaae..0x082feadf 实参装载序：field18 `mov
                // 0x18(%eax),%eax; mov %eax,%edi`（%eax 中转落 %edi）→ field0c
                // `mov 0xc(%eax),%esi` → field14 `mov 0x14(%eax),%ebx` → field10
                // `mov 0x10(%eax),%eax`。4.4 直读恒单段 `mov 0x18(%eax),%edi`
                // ⇒ 令前三者走 register 局部（callee-saved 家寄存器由 4.4
                // 按 %edi/%esi/%ebx 分配，探针 e2 逐条同 ORIG 形）。
                // [落地] 122/122 AE（原 121/122 DIFF），本 TU diff 1→0。
                register unsigned int field18 = (unsigned int)it->second.m_field18;
                register int field0c = it->second.m_field0c;
                register int field14 = it->second.m_field14;
                SendSpawnRidableObject(p, it->second.m_field10,
                                       field14,
                                       field0c,
                                       (int)field18);
            }
        }
    }
    p->put_byte(idx, counter);
}