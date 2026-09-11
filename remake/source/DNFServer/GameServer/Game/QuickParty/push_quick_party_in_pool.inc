// push_quick_party_in_pool —— ORIG 0x0826afb4。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/push_quick_party_in_pool.md
// 正常路径：写 qp 的 dungeon/pool 字段并挂入对应档位 list、按需插等待 timer、
// 广播 0x180 匹配更新包；异常路径（qp 空/档位越界）：逐 party 清数据并发 0x17f
// 解散包后 destroy，记日志返回 false。
// [b23判词·N-XVAL] P167 调用点差集 CLEAR（25/25）：ORIG `list<int>::push_back(int&&)`
// （EOi）——源码以 static_cast<int&&>(qpIndex) 生成 xvalue 精确命中该重载（原 const& 形选错）。
// 残余 gap+2 = gcc 4.4 对 xvalue 实参多一次 `mov %eax,-N(%ebp); mov -N(%ebp),%eax` 存回读
// 往返 + 帧槽 -0x2c→-0x30 位移（N82），跨版本求值/帧结构不可源码复现，语义等价。
// [b28d-L1] N82/N-XVAL 封顶：ORIG 0x0826afb4；P167 CLEAR（25/25）。残余 = xvalue 实参存回读往返 + 帧槽 -0x2c→-0x30 位移（b23 判词复核仍成立）。
// [b28d-solo-QuickParty] verdict=AE（171=171）：修法=内联 xvalue 实参
// `push_back(static_cast<int&&>(qp->get_quick_party_index()))`，消 4.4 的
// `mov %eax,-0x24; mov -0x24,%eax; mov %eax,-0x20` 存回读往返与 PacketGuard
// -0x2c→-0x30 位移（旧 N82/N-XVAL 判词作废）。P167 CLEAR（25/25）。
bool CQuickPartySystemManager::push_quick_party_in_pool(
    const STQuickPartyPoolMap_Key& key, STQuickPartyPoolMap_Data& data,
    CParty* party, int count, CQuickParty* qp, bool flag)
{
    if (!(qp != 0 && count < 4 && 0 < count))
    {
        if (qp != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                CParty* member = qp->get_party_object(i);
                if (member == 0)
                    continue;
                member->init_quick_party_data();
                PacketGuard packet;
                packet.clear();
                packet.put_header(0, 0x17f);
                packet.finalize(true);
                member->send_to_party(packet);
            }
            qp->destroy();
        }
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            899, "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY!! REGIST POOL INDEX(%d)",
            count);
        return false;
    }

    qp->set_dungeon_index(key.m_dungeonIndex);
    qp->set_dungeon_diff(static_cast<int>(static_cast<char>(key.m_dungeonDiff)));
    qp->set_pool_index(count);

    if (party != 0)
        party->set_quick_party_index(qp->get_quick_party_index());

    // [b28d-solo-QuickParty] 修法：ORIG 0x0826b155 只有单槽 `call get_quick_party_index;
    // mov %eax,-0x20(%ebp); …; lea -0x20(%ebp),%edx; call push_back(int&&)`；具名
    // qpIndex 使 4.4 多发 `mov %eax,-0x24; mov -0x24,%eax; mov %eax,-0x20`（+2 条 +
    // 多占 -0x24 槽把 PacketGuard 挤到 -0x30）。改内联 xvalue 实参后单槽直落。
    data.m_lists[count].push_back(
        static_cast<int&&>(qp->get_quick_party_index()));

    if (flag)
        insert_timer_waiting_party_matching(qp);

    unsigned short masterUid =
        qp->get_matching_quick_party_master_unique_id();
    bool changed = false;
    int totalCount = qp->get_quick_party_count();
    int firstCnt = static_cast<int>(qp->get_first_matching_cnt());
    if (firstCnt < totalCount ||
        static_cast<char>(qp->is_change_quick_party_count()) != '\0')
    {
        changed = true;
    }
    qp->send_matching_update_quick_party(static_cast<char>(count), masterUid,
                                         changed);
    return true;
}