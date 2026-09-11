// ==== Dispatcher_ObjectBringUp::send (ORIG 0x081e1704) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e1704 orig/ours=154/163 ndiff-blocks=34
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0xc(%ebp) | lea    -0x1c(%ebp),%eax
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
            // P96：ORIG 侧为非 const `vector<int>::begin/end` + `__normal_iterator<int*>`（p167：我方
            // const 版 begin/end/ppEv/de 共 10 项 EXTRA），故此处必须取非 const 引用。
            std::vector<int>& rewards =
                *reinterpret_cast<std::vector<int>*>(
                    const_cast<char*>(reinterpret_cast<const char*>(&out)) + 0xc);
            std::vector<int>::iterator it = rewards.begin();
            while (it != rewards.end())
            {
                const short v0 = static_cast<short>(*it); ++it;
                const int v1 = *it; ++it;
                const int v2 = *it; ++it;
                const short v3 = static_cast<short>(*it); ++it;
                pg.put_short(v0);
                pg.put_int(v1);
                pg.put_int(v2);
                pg.put_short(v3);
            }
        }
    }
    else
    {
        pg.put_byte(0);
        pg.put_byte(out.m_4 & 0xff);
    }

    pg.finalize(true);
    user->Send(pg);
}