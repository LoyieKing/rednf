// ============================================================================
// DB_DeathTowerLoadTopRanker::dispatch —— ORIG 0x0842a484
// >>reload(byte) → GetDBHandle(DB_HANDLE_2,0) → 回包流 0xe4/-1 +
// SIG_DEATH_TOWER_LOAD_TOP_RANKER{+0 count=0, +4 reload}。双层循环
// partType 0..4 × rank 1..4：reload==0 时先 count(*) 得 total（>5 时
// interval=max(total/0x5f,1)）再查 rank>5 且 rank%interval==0 的行（limit 95）；
// reload!=0 时查 top5 表（limit 5）。每段 m_count += get_n_rows()，逐行
// fetch 后写 rank(+0x06+i*2)、tower_index(+0xfa0+..+0xc)、member_info
// (+0xfa0+..+0x10,0x17)、stage/play_time(+0x1000+..+0xc/+0x10)、
// type=rank(+0xfa0+..+0xe)。任一步失败返 0（不发包）；成功 put(DISPATCH_Q)。
// ============================================================================
// [SweepW8LT b28d] DIFF：gap 11→7（our 301 / ORIG 308）。**真 bug 修复**：ORIG
// 842a702 先存旧 m_count（-0x14）再 `m_count += get_n_rows()`，k 从旧值起（只遍历
// 本次新增行）；原码 k=0 起会重复覆盖历史行。另 interval 声明序前移（ORIG
// `movl $0x1,-0x10` 在 total=0 之前）。p167 29/29 [0缺0多]（C1/C2 别名）。余 -7
// 归 N82 帧（0x80 vs 0x70）+ 槽号差（interval -0x40 vs -0x10）。ORIG 0842a484
bool DB_DeathTowerLoadTopRanker::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned char reload = 1;
    *stream >> reload;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x6c21), true);
    guard.operator*()->operator<<(0xe4);
    guard.operator*()->operator<<(-1);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast）。
    SIG_DEATH_TOWER_LOAD_TOP_RANKER* out =
        guard->GetInBuffer<SIG_DEATH_TOWER_LOAD_TOP_RANKER>();
    out->m_count = 0;
    out->m_flag = reload;
    // ORIG 0x0842a8e6 `cmp $0x4,%eax; setbe`（无符号上界，P2/P310）
    // [F2e-Singles2 轴B 2026-09-10 实测回滚] 试 `int partType` 复现 ORIG 0842a89a
    // `cmpl $0x4,-0x18(%ebp); setle`：setle 确已翻正（我方 -0x18 槽），但 our
    // 309→**310**（+1 条）、diff 451→452 行 ⇒ 净劣，按判据回滚。值域 0..4 无
    // 符号分叉（纯 codegen 形态差）⇒ 维持 N96/N18a 封顶（内层 rank 两段寄存器形同）。
    // （值域 0..4 恒非负 ⇒ 非 P196 分叉，属 codegen 轴，守闸门；内外两循环已穷举。）
    for (unsigned int partType = 0; partType <= 4; ++partType)
    {
        // [A04-r5j · 轴 B **已试无效**] ORIG 0x0842a89a 为 `cmpl $0x4,-0x18(%ebp); setle;
        // test; jne`（有符号）而我方 setbe；按轴 B 把 rank 改 int 后 **ndiff 117→117、
        // our 309→309（零改善、零劣化）** ⇒ 按协议**回滚**（该点 setcc 差非此局部所致，
        // 归 N82 帧-槽族残差），保持 unsigned。
        for (unsigned int rank = 1; rank <= 4; ++rank)
        {
            if (reload == 0)
            {
                // ORIG 842a702 先存旧 m_count（-0x14），再 `m_count += get_n_rows()`，
                // k 从**旧值**起（只遍历本次新增行）；原码 k=0 起会重复覆盖历史行。
                unsigned int interval = 1;
                unsigned int total = 0;
                db->set_query(
                    "seLect count(*) from charac_tower_rank where tower_index=%d and part_type=%d",
                    partType, rank);
                if (!db->exec(true))
                    return 0;
                if (!db->fetch())
                    return 0;
                if (!db->get_uint(0, total))
                    return 0;
                if (total > 5)
                {
                    // 真 bug 修正：ORIG 0x0842a6a2 段魔数 0x58ed2309+shr$6
                    // = total / 0x5f（Ghidra C `local_40 / 0x5f`）；旧码写 /100
                    // 使采样间隔差 5%，rank%interval 命中行完全不同。
                    interval = total / 0x5f;
                    interval = std::max(interval, (unsigned int)1);
                }
                db->set_query(
                    "seLect b.rank,a.tower_index, a.member_info_%d,a.stage_%d,a.play_time_%d from charac_tower_record a,charac_tower_rank b where b.tower_index=%d and b.tower_index=a.tower_index and (b.rank>5 and (b.rank%%%d)=0) and a.charac_no=b.charac_no and b.part_type=%d order by b.rank asc limit %d",
                    rank, rank, rank, partType, interval, partType, 0x5f);
            }
            else
            {
                db->set_query(
                    "seLect b.rank,a.tower_index, a.member_info_%d,a.stage_%d,a.play_time_%d from charac_tower_record a,charac_tower_rank_top5 b where b.tower_index=%d and b.tower_index=a.tower_index and b.rank<=5 and a.charac_no=b.charac_no and b.part_type=%d order by b.rank asc limit %d",
                    rank, rank, rank, partType, partType, 5);
            }
            if (!db->exec(true))
                return 0;
            unsigned int oldCount = out->m_count;
            out->m_count += db->get_n_rows();
            for (unsigned int k = oldCount; k < out->m_count; ++k)
            {
                if (!db->fetch())
                    return 0;
                // P183：ORIG 每次字段访问重发完整 `lea+add` 地址链（无具名指针
                // 缓存；缓存 3 个指针多占 0x10 帧并省物化次数）
                if (!db->get_ushort(
                        0, out->m_ranks[k]))
                    return 0;
                if (!db->get_ushort(
                        1, out->m_rows[k].m_towerIndex))
                    return 0;
                // [bughunt17] ORIG 0x0842a7b0..0x0842a7de: len = rank(-0x18) * 0x17
                //   (`imul $0x17`), i.e. member_info_N holds N party members x 0x17.
                //   The old literal 0x17 only copied the first member's info.
                if (!db->get_binary(
                        2, out->m_rows[k].m_memberInfo,
                        (int)(rank * 0x17)))
                    return 0;
                if (!db->get_uint(
                        3, out->m_rows[k].m_stage))
                    return 0;
                if (!db->get_uint(
                        4, out->m_rows[k].m_playTime))
                    return 0;
                out->m_rows[k].m_type =
                    (unsigned short)rank;
            }
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}