// premium_info_set @ 0x0818f1ca（0x2db 指令）。
// entries 事件入账遍历：eventIdx∈[50000..89999] 有效。
//   [70000..89999] 或受限 premium → 压栈到 data+(count+0xa4c)*4+0xc 并 count++（ushort@+0x293a）；
//   [0xa029..0xa02b] → arad_sp_select_member_brodcast(p, entry, serverGroup)；
//   其余 → arad_sp_select_member_premium(p, entry, dstA(空), copy(dstB))。
int Arad_Dispatcher_Login_Impl::premium_info_set(
    SIG_LOGIN_DATA* data,
    std::vector<DB_Login::tagDNFEventEntry>& entries,
    int serverGroup)
{
    // [b28d-solo 判词·AE] ORIG 0x0818f1ca：215=215 指令数全等、mnemonic 全等、
    // p167 CLEAR。修法：entry 改**值拷贝**（ORIG operator* 后逐 dword 搬 -0x50）；
    // 条件一律 `it->m_eventId`（每次重调 operator->）；压栈目标
    // `m_restrictedGoods[m_restrictedGoodsCount++]`（ORIG 读 count 一次入 %ebx 复用 +
    // `lea 0x1(%ebx)` 写回）；pushCount 改 int（ORIG `movl $0,-0xc`）且仅结尾
    // `m_premiumCount = pushCount`（恒 0，P171 照抄原码不更新）；+0x394c 经
    // SigPad394cView 具名视图清零；P13 for 初始化区缓存 end()。**真语义 bug 修正**：
    // 旧重建 else 分支的 `vecB = vecA;` 让 select_member_premium 第 4 实参携带
    // vecA 内容，ORIG 传的是空 vecB 的按值拷贝（同时该语句链多发射 19 个
    // vector<int>::operator= 族弱符号）。
    int pushCount = 0;   // ORIG `movl $0,-0xc`（4B 写）⇒ int 非 unsigned short

    data->m_premiumCount = 0;
    data->m_restrictedGoodsCount = 0;
    ((SigPad394cView*)data)->m_field = 0;

    std::vector<int> vecA;                       // select_member_premium 目标 A
    std::vector<int> vecB;                       // 每次 copy 为临时实参 B

    typedef std::vector<DB_Login::tagDNFEventEntry>::iterator It;
    // P13：ORIG 在循环初始化区求值 end() 一次（两迭代器槽 -0x34/-0x38）
    for (It it = entries.begin(), endIt = entries.end(); it != endIt; ++it)
    {
        DB_Login::tagDNFEventEntry entry = *it;   // ORIG 值拷贝到 -0x50

        if (it->m_eventId < 50000 || it->m_eventId > 89999)
        {
            continue;
        }

        if (it->m_eventId >= 70000 && it->m_eventId <= 89999)
        {
            // ORIG 读 count 一次入 %ebx 复用（索引 + `lea 0x1(%ebx)` 写回）⇒ 后置 ++
            data->m_restrictedGoods[data->m_restrictedGoodsCount++] =
                it->m_eventId;
            continue;
        }

        if (WongWork::CUserPremium::IsRestrictedPremium(it->m_eventId))
        {
            data->m_restrictedGoods[data->m_restrictedGoodsCount++] =
                it->m_eventId;
            continue;
        }

        if (it->m_eventId >= 0xa029 && it->m_eventId <= 0xa02b)
        {
            DATABASE::LOGIN_QUERY::arad_sp_select_member_brodcast(
                data, entry, serverGroup);
        }
        else
        {
            DATABASE::LOGIN_QUERY::arad_sp_select_member_premium(
                data, entry, vecA, vecB);
        }
    }

    data->m_premiumCount = pushCount;
    return 0;
}