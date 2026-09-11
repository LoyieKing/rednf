// ----------------------------------------------------------------------------
// 公告收发
// ----------------------------------------------------------------------------
void CSharedServerMessageManager::SendServerMessageInfo(CUser* pUser,
                                                       SIG_EVENT_SERVER_MESSAGE* pMsg,
                                                       char bFlag)
{
    // [b28d-solo-CSharedServerMessageManager 判词 · AE] ORIG 0x086008d8（fresh 196=196，
    // TU near=0 diff=0，p167 25/25 CLEAR）。原 N93 判词（迭代器槽 -0x2c/-0x28 方向）作废：
    // 根因是 ①P50 复活（iterator 先声明后赋值复现 C1Ev + begin() sret 临时 -0x50/-0x40）；
    // ②P198 死局部（ev ctor 前 `movl $0x0,-0x18(%ebp)`）；③P231 纯 register `nSent` 三件套
    // （mov $0/$1,%ebx + 尾 test，无槽）；④ev 内层作用域使 `~ev()` 落在 test 之前、
    // `~PacketGuard()` 之后（ORIG 0x08600b43/b48/b50 顺序）。
    PacketGuard pg;
    pg.put_header(0, 0xc0);
    pg.put_byte((int)bFlag);

    // [b28d-S112] ORIG 0x0860091f `movl $0x0,-0x18(%ebp)` 为 ev ctor 前的死局部初值
    // （P198 孪生：4.4 对未使用标量局部仍发清零 store）。
    int nUnused = 0;
    (void)nUnused;
    // [b28d-S112] P231（纯 register 存储类提示，无 asm 绑定，P0 合规）：ORIG 0x086009a9
    // `mov $0x0,%ebx` / 0x08600b21 `mov $0x1,%ebx` / 0x08600b48 `test %ebx,%ebx` 三件套
    // 无栈槽 ⇒ 原码是 `register int nSent` 标志 + 末尾 `if (nSent) return;`（探针
    // /tmp/gs_probe6.cpp f8 逐条同形；普通 bool/int 局部 4.4 恒槽化且空 if 被 DCE）。
    // ORIG 布局为 `jbe <body>`（≤0x32 跳远处 body）+ 直落 `mov $0x0,%ebx; jmp tail`
    // ⇒ 源形须写成 `if (count > 0x32) { nSent = 0; } else { body; nSent = 1; }`
    // （4.4 对 then 短/else 长恒 then 直落、else 远置，与 f8 探针镜像）。
    register int nSent;
    // [b28d-S112] ORIG 0x08600b43 `~ev()` 在 0x08600b48 `test %ebx,%ebx` **之前**、
    // `~PacketGuard()` 之后 ⇒ ev 位于内层作用域（先于 test 析构），pg 在函数级作用域。
    {
        EVENT_SERVER_MESSAGE ev;
        ev.m_count = 0;
        for (int i = 0; i < pMsg->m_count; ++i)
        {
            if (m_adapter.IsOnIndex(pMsg->m_msg[i].m_index))
            {
                ++ev.m_count;
                ev.m_vecMsg.push_back(pMsg->m_msg[i]);
            }
        }

        if (ev.m_count > 0x32)
        {
            nSent = 0;
        }
        else
        {
            pg.put_byte((int)ev.m_count);

            // ORIG 循环条件为 i<count || it!=end（两者同步推进，等价于 count 次）
            // [b28d-S112] P50 复活：ORIG 0x086009c9 先 `iterator::C1Ev` 占 -0x40，再以
            // begin() 的 sret 临时 -0x50 拷入 ⇒ 必须先声明后赋值（初始化式形 4.4 直构无临时槽）。
            std::vector<stSERVER_MESSAGE>::iterator it;
            it = ev.m_vecMsg.begin();
            for (int i = 0; i < (int)ev.m_count || it != ev.m_vecMsg.end(); ++i)
            {
                stSERVER_MESSAGE* p = &(*it);
                pg.put_byte((int)p->m_index);
                pg.put_int(p->m_lenMsg);
                pg.put_str(p->m_msg, p->m_lenMsg);
                pg.put_int(p->m_lenMsg2);
                pg.put_str(p->m_msg2, p->m_lenMsg2);
                it++;
            }

            pg.finalize(true);
            pUser->Send(pg);
            nSent = 1;
        }
    }
    if (nSent)
        return;
}