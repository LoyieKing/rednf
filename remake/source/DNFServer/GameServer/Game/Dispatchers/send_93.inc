// ==== Dispatcher_ObjectBringUp::send (ORIG 0x081e1704) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e1704 orig/ours=154/163 ndiff-blocks=34
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0xc(%ebp) | lea    -0x1c(%ebp),%eax
// [R9form4 2026-09-12 形态对齐] 三处源形（ORIG 0x081e1704 逐条实证）：
//   ① ORIG 无 vector 引用槽：begin/end 调用点各自 `mov -0xc(%ebp),%eax; lea 0xc(%eax),%edx`
//      （我方旧形 `add $0xc,%eax; mov %eax,-0x1c(%ebp)` 多槽 + 帧 0x40 vs 0x30）⇒ 改宏内联展开；
//   ② ORIG 无 v0..v3 具名标量槽（我方旧形 `mov %ax,-0x16; movswl -0x16,%eax` 往返 ×4）
//      ⇒ put 调用与 ++it 交错、实参取 `de` 结果直传（P0/P183）；
//   ③ ORIG put_byte(out.m_4) 直传 int（`mov 0x4(%eax),%eax; mov %eax,0x4(%esp)`），
//      无 `and $0xff` ⇒ 删掩码（P162）。
#define R9F4_OBU_REWARDS(p) (const_cast<stObjectBringUp&>(p).m_items)
void Dispatcher_ObjectBringUp::send(CUser* user, ParamBase& param)
{
    // [G1-Dispatchers 负结论] 试删具名 `out` 引用改内联 cast 表达式：our 164→162（gap −11→−9）
    // 但 ndiff 69→113（劣化 44）⇒ 已回滚，保留具名引用形（ORIG 该引用槽位置与 P0 一致）。
    const stObjectBringUp& out = static_cast<const stObjectBringUp&>(param);

    PacketGuard pg;                                         // ORIG 无 clear（同汇编）
    pg.put_header(1, 0x1d5);

    if (out.m_4 == 0)
    {
        pg.put_byte(1);
        pg.put_short(static_cast<unsigned short>(out.m_8));
        pg.put_byte(static_cast<unsigned int>(out.m_a));
        pg.put_byte(static_cast<unsigned int>(out.m_b));

        if (out.m_b != 0)
        {
            // +0x0c 处 vector<int>：每轮消费 4 个 int（short,int,int,short）
            // P96：ORIG 侧为非 const `vector<int>::begin/end` + `__normal_iterator<int*>`，
            // 且 begin/end 调用点各自经宏重取 vector 地址（无引用槽）。
            std::vector<int>::iterator it = R9F4_OBU_REWARDS(out).begin();
            while (it != R9F4_OBU_REWARDS(out).end())
            {
                pg.put_short(*it); ++it;
                pg.put_int(*it); ++it;
                pg.put_int(*it); ++it;
                pg.put_short(*it); ++it;
            }
        }
    }
    else
    {
        pg.put_byte(0);
        pg.put_byte(out.m_4);
    }

    pg.finalize(true);
    user->Send(pg);
}
#undef R9F4_OBU_REWARDS