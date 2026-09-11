// ---------------------------------------------------------------------------
// 20) CUser::AddTicket(int ticketIdx, bool notify)
//     报告：docs/class_func_reports/CUser/AddTicket.md。ORIG 0x08678282。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08678282：p167=M0/E5:_ZNSt20_List_const_iteratorIiEC1ERKSt14_List_iteratorIiE;_ZNKSt20_List_const_iteratorIiEdeEv;_ZNSt20_List_const_iteratorIiEppEv，gap=-1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08678282：N82/P28 封顶——list 迭代器族槽序
// （p167 M0/E5 迭代器转换构造，gap -1）。
void CUser::AddTicket(int ticketIdx, bool notify)
{
    // [b28d-solo-CUserOnlyOrig FL5 2026-09-10] ORIG 0x08678290 入口形
    //   `movzbl -0x2c(%ebp),%eax; xor $0x1,%eax; test %al,%al; je <主体>`
    //   = 4.4.6 对**裸 `if (!bool形参)`** 的唯一产物（探针 /tmp/fl5/p1.cpp f4 vs
    //   f1 `if (b)`→cmpb / f2 具名 bool→cmpb / f5 uchar^=1→xorb+cmpb）；
    //   故原码是「否定早判 + push_back 落 fall-through、主体在 je 目标」⇒ 反转两块。
    if (!notify)
    {
        m_list8cf80.push_back(ticketIdx);
    }
    else
    {
        // [b28d-solo-CUserOnlyOrig P167/P96/P28] ORIG 命中非 const
        // `_List_iterator<int>::operator*() const / operator++(int) / operator!=(const&) const`；
        // 旧 const_iterator + 前置 ++ 产 `_List_const_iterator` 转换构造差集。
        for (std::list<int>::iterator it = m_list8cf80.begin();
             it != m_list8cf80.end(); it++)
        {
            if (*it == ticketIdx)
                return;
        }
        m_list8cf80.push_back(ticketIdx);

        // [FL5 2026-09-10 P259] ORIG 0x08678358..0x08678387 直证：Acquire 结果
        // `mov %eax,0x4(%esp)` 直入 CStreamGuard 构造实参，**无** store/load 对
        // ⇒ 删具名 Stream* 局部，表达式内联（同时释放 -0xc 槽给迭代器临时）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x703d), true);
        // [b28d-solo-CUserOnlyOrig P167/P132] ORIG 0x08678xxx 命中
        // `CStreamGuard::operator*()` ×2 + `operator->()` ×1 ⇒ 原码是
        // `(*guard) << x` / `guard->put_binary(...)` 形（直接 guard<< 省略 de() 调用）。
        guard.operator*()->operator<<(0x28);
        guard.operator*()->operator<<((int)GetUID());
        // [CoBatchD 2026-09-11 负结论·已回滚] payload 声明前移（复现 ORIG 深→浅序
        // payload(-0x24)<guard(-0x1c)<it(-0x14)）实测 **135→135 中性**（4.4 未按声明序
        // 翻转槽位）⇒ 回滚；本件 gap -1 属 N82 槽分配族。
        // [M8COA 2026-09-11 P339 二试·已回滚] 把 payload 声明前移到 else 块首实测
        // **仍 NEAR(134/135)、槽错配 4 处不变**，且映射恶化：payload 由 (-0x20,-0x1c)
        // 退到 (-0x18,-0x14)（更浅）、guard -0x18→-0x20、it 仍 -0x24；
        // ORIG 深→浅序 payload(-0x24,8B) > guard(-0x1c) > it(-0x14)（0x086783cb
        // `lea -0x24(%ebp),%eax` 为 payload memset 目标、0x0867828a `movzbl -0x2c`）
        // 恰为**展开序的反序** ⇒ 4.4 按「首次展开先占深槽」而 4.1 按「后展开占深槽」
        // （跨版本栈分配方向差），声明位置/顺序不可控 ⇒ N82 确认封顶。
        // [M8COA 2026-09-11 P339 三试·已回滚] 再试声明置于**函数级 block**（gcc 栈槽
        // 按 BLOCK 前序分配的假设）实测 **NEAR(134/135)、槽映射与二试逐值相同**
        // （payload -0x18/-0x14、guard -0x20、it -0x24）⇒ 4.4 恒按**首次使用序**分配、
        // 与声明所在 block 无关；ORIG payload(-0x24)/guard(-0x1c)/it(-0x14) 为 4.1
        // 「后展开占深槽」方向 ⇒ N82 封顶确认（三次试形：原位/else 块首/函数级）。
        struct { unsigned int characNo; int ticket; } payload;
        memset(&payload, 0, 8);
        payload.characNo = getCurCharacNo();
        payload.ticket = ticketIdx;
        guard->put_binary(&payload, 8);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}