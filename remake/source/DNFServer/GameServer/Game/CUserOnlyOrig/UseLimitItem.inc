// ---------------------------------------------------------------------------
// 40. CUser::UseLimitItem @0x08688ae6
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08688ae6：p167=M0/E4:_ZNK9__gnu_cxx17__normal_iteratorIP14LimitItemUsageSt6vectorIS1_SaIS1_EEEdeEv;_ZN9__gnu_cxx17__normal_iteratorIP14LimitItemUsageSt6vectorIS1_SaIS1_EEEppEv，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08688ae6：N82 封顶——帧/槽差（p167 CLEAR）。
unsigned char CUser::UseLimitItem(int itemIdx, bool sendNoti)
{
    unsigned char ok = 1;
    std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin();
    std::vector<LimitItemUsage>::iterator end = m_limitItemUsage.end();
    for (; it != end; it++)   // ORIG 命中 operator++(int)（P28）
    {
        // [L6 判据①/P222 极性重核] ORIG 0x08688ae6 段 `mov 0x15(%eax),%eax;
        // cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je` ⇒ 原码是**正极性**守卫
        // `if (field == itemIdx) { 体 }`（体在 je 之后直落）；旧 `if (field != itemIdx)
        // continue;` 使 4.4 反产 `setne %al; test; je`。体各路径均 return，改写等价。
        if (it->m_field15 == itemIdx)
        {
        if (it->isUsable() != 1)
            ok = 0;
        if (sendNoti)
            SendLimitItemUsageNotiInfo(itemIdx,
                                       // [b28d-solo-CUserOnlyOrig P167] ORIG 0x08688bxx
                                       // 命中 `__normal_iterator<LimitItemUsage*>::operator->()`
                                       // ×3、无 operator* ⇒ 字段一律经 `it->` 具名成员
                                       // （旧 `&*it + 0x11` 生 operator* 差集）。
                                       // [FL5 2026-09-10 P162/类型轴] ORIG 0x08688b8d
                                       // `mov 0x11(%eax),%eax; mov %eax,0x8(%esp)` = **4 字节
                                       // 整字直传**（int 观），旧 `(signed char)m_field11b[0]`
                                       // 产 movzbl+movsbl（1 字节读）⇒ 改 union 的 int 观
                                       // `m_field11`（sizeof/偏移/符号集不变）。
                                       it->m_field11,
                                       it->m_field19);
        if (it->useIt())
        {
            // [G6Sub1 P0/P162 单旋钮] ORIG 0x08688bcc..0x08688c02 为
            // `call ptEv; mov 0xc(%eax),%edi`（单条 4B 装载，无 `add $0xc`、
            // 无栈槽往返）⇒ 原码把 int 字段直接写在实参位，既非 8 字节视图形也
            // 无具名 long long 局部。旧形 `long long t = *reinterpret_cast<long
            // long*>(&it->m_fieldc);` 多出 add+双载+双存+回读共 5 条、帧 +8，
            // 恰为本件 gap（120−115=5）的全部来源。makeRequest 第 5 形参为
            // i386 `long`（4B），读 4B 与截断后语义等价。
            DB_UpdateLimitItemUsage::makeRequest(get_charac_no(-1),
                (signed char)it->m_field10,
                itemIdx, it->m_field19, it->m_fieldc);
            // ORIG 0x08688c1d/0x08688c23：出口不写 %eax 而写 `movb $0x1/$0x0,-0x19(%ebp)`
            // 后 `jmp 尾部`（尾部 `movzbl -0x19,%eax`）⇒ 原码是**赋值 ok + break 出循环**，
            // 非 `return 1/0`（后者产 `mov $1,%eax` 直接返回，跳过 ok 槽）。
            ok = 1;
        }
        else
        {
            ok = 0;
        }
        break;
        }
    }
    return ok;
}