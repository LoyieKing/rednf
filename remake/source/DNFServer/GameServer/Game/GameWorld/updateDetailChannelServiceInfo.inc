// [b28d-solo-GameWorld] _ZN9GameWorld30updateDetailChannelServiceInfoEv verdict=NEAR 309/309
// （b28d-solo 本轮；助记符序列全等，仅槽号差 8）。
// 修法：①P163 补未列 case 0 撑宽 range（0x086cb1e8 `cmp $0xa;ja` 直接索引）；
// ②P38/P198① v 提到 switch 外预清复用；③P13 —— ORIG 0x086cb471 begin 后立即
// 0x086cb486 调 end（-0x80，循环前缓存）⇒ 具名 `last` 缓存 + goto 尾条件；
// ④P259 —— `Stream* stream=Acquire(...)` 具名局部删，内联进 CStreamGuard 实参
// （ORIG 0x086cb4d1 %eax 直传，无 stream 槽）；⑤0x086cb5e2 `jne <body>; jmp <exit>`
// 极性：反转条件 + 显式 `return` 出口（4.4 对 `if(cit!=last) goto LOOP3;` 恒产
// `je exit; jmp body`）。余 N82/N115 槽组平移 8（it -0x78 vs -0x80）封顶；p167 CLEAR 49=49。
// [A11-round5 复核] P326 适用性判定**不成立**：P326 前提为「ORIG 比较调点是非 const
// `_Rb_tree_iterator::operator==` 且存在 const 转换临时三级槽」；本件 ORIG 调点
// 0x086cb5e2 段为 `_ZNKSt17_Rb_tree_iteratorISt4pairIKiiEEneERKS3_`（const 成员、两级
// 槽 it/cit=-0x78 + last=-0x80），无转换临时可落；p167 的 MISSING `ne` / EXTRA `eq`
// 即源码比较方向差，改 `!=` 已实测回退（见下 COND3 判词）⇒ 维持 N82/N115 封顶。
void GameWorld::updateDetailChannelServiceInfo()
{
    std::map<int, int> countMap;
    // [b28d-solo-GameWorld] P167：ORIG 用 for-init 直构 + 前缀 ppEv
    // （MISSING ppEv / EXTRA C1Ev+ppEi 实证）⇒ 改 for-init 直构 + `++it`。
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            // ORIG 0x086cb1cd `movl $0x0,-0x14`（在 get_state() 调用之前）⇒ 原码在
            // switch 外声明并预清 v，各 case 复用同一槽（P198① 预清两句式）。
            int v = 0;
            switch (user->get_state())
            {
            // b28d-solo P163：ORIG 0x086cb1d0 段 `cmp $0xa,%eax; ja; mov
            // &jump_table(,%eax,4)` 直接以 get_state() 索引 0..0xa（无 sub）⇒
            // 原码含未列 case 0（与 break 共享）撑宽 range。
            case 0:
                break;
            // ORIG 0x086cb1df 跳转表实证：case 值为 3/5/6&7/8/10（cmp $0xa;ja），非 0/1/2/6/8
            case 3:                    // +0x1f1 → countMap[0]
                // ORIG 0x086cb1f1 段：`call op[]; mov (%eax),%eax; mov %eax,-0x14`
                // 后重算 key 再 `call op[]` ⇒ 具名局部 + 两条语句（P38），
                // 单语句 `m[k]=m[k]+1` 被 4.4 SAVE_EXPR 合并成单次 op[]+%ebx 缓存。
                v = countMap[0];
                countMap[0] = v + 1;
                break;
            case 5:                    // +0x235 → countMap[1]
                v = countMap[1];
                countMap[1] = v + 1;
                break;
            case 6:                    // +0x279：channel 检查 → +0x2ad countMap[9]
            case 7:                    // +0x2f1 → countMap[2]
                if (G_GameWorld()->GetChannelType() == 0xf ||
                    G_GameWorld()->GetChannelType() == 0x10)
                {
                    v = countMap[9];
                    countMap[9] = v + 1;
                }
                else
                {
                    v = countMap[2];
                    countMap[2] = v + 1;
                }
                break;
            case 8:                    // +0x335 → countMap[6]
                v = countMap[6];
                countMap[6] = v + 1;
                break;
            case 10:                   // +0x376 → countMap[8]
                v = countMap[8];
                countMap[8] = v + 1;
                break;
            }
        }
    }

    int privateStoreSize =
        (int)private_store::GetInstancePrivateStoreMgr()->GetPrivateStoreSize();
    if (0 < privateStoreSize)
    {
        countMap[0] = countMap[0] - privateStoreSize;
        countMap[7] = privateStoreSize;
    }

    // [b28d-solo-GameWorld] P13+P167+P283：ORIG 0x086cb471 begin(-0x84) 之后立即
    // 0x086cb486 `end`（-0x80，循环前缓存；条件块只用缓存值）→ `jmp 条件块`（P283
    // 底部条件布局）；增量 ppEv（前缀）⇒ `++cit`；`(*cit)` 选 deEv（P167）。
    std::map<int, int>::iterator cit = countMap.begin();
    std::map<int, int>::iterator last = countMap.end();
    goto COND3;
LOOP3:
    {
        int type = (*cit).first;
        int count = (*cit).second;
        // ORIG 0x086cb4d1 段：Acquire 结果经 %eax 直作 CStreamGuard 实参（无具名
        // stream 槽，P259）；具名指针局部多 store/load 对并使 guard 槽浅移。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x110f), true);
        **guard << 0x6a;
        **guard << -1;
        // LOGDB 0x6a 载荷（ORIG 0x086cb4f9..0x086cb54e：4 个连续 int，
        // +0x08 ←env+0x198 `mov 0x198(%eax),%eax`）。
        struct __attribute__((packed)) stChannelServiceInfo
        {
            int m_type;   // +0x00
            int m_zero;   // +0x04
            int m_gcNo;   // +0x08
            int m_count;  // +0x0c
        };
        stChannelServiceInfo buf;
        buf.m_type = type;
        buf.m_zero = 0;
        buf.m_gcNo = G_CEnvironment()->m_serverEnvir.m_gcNo;
        buf.m_count = count;
        guard->put_binary(&buf, 0x10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
    ++cit;
COND3:
    // [b28d-sweep] P167 差集实测：ORIG 调 `_Rb_tree_iterator::ne`（MISSING ne /
    // EXTRA eq）⇒ 源形本应是 `cit != last`；但 `!=`/while 两形会让 4.4 把 ne 的
    // 比较临时槽放 -0x78（ORIG -0x7c）并使助记符序列错位，TU 四数 near 5→4 /
    // diff 14→15 回退 ⇒ 按「四数不回退」保留 `== last`（NEAR 309/309）。
    // [H8B 2026-09-11 复核确认·终判] ORIG 0x086cb5e2 段调 `_ZNKSt17_Rb_tree_iterator
    // ISt4pairIKiiEEneERKS3_`（const ne，两级槽 it/cit=-0x78 + last=-0x80）⇒ 源形
    // 本应 `cit != last`；但实测两形（`!=` / `while(cit!=last)`）4.4 恒把 ne 比较
    // 临时槽放 -0x78（ORIG -0x7c）并错位助记符序，TU 四数 near 5→4 / diff 14→15
    // 回退 ⇒ 维持 `== last` + 显式 return 出口，N82/N115 槽组平移封顶。
    // [H8B 2026-09-11 本轮实测] 308/308（等长）+ p167 calls 49/49：MISSING
    // `_ZNKSt17_Rb_tree_iteratorISt4pairIKiiEEneERKS3_` / EXTRA `eq`（即上方极性差，
    // 唯一非别名差）+ 其余 C1/C2·D1/D2 同址别名 ⇒ 无未解释差。
    // 旧注：ORIG 0x086cb5e2 `jne <body>; jmp <exit>` ⇒ 反转条件 + 显式 return 出口
    // （4.4 对 `if (cit != last) goto LOOP3;` 恒产 `je exit; jmp body`）。
    // [H8B 2026-09-11 第三轮实测 · 父泳道指定形亦回退] 按父泳道「ORIG 用 ne，改 `!=`」
    // 复测 `if (cit != last) { goto LOOP3; } return;`：本件仍 308=308（等长），但
    // TU 四数 near 4→3 / diff 12→13（本件 NEAR→DIFF）⇒ 劣化，已回滚。
    // 根因与历史两轮一致：4.4 把 `const ne(const&)` 的比较临时槽放 -0x78（ORIG 0x086cb5e2
    // 段为 -0x7c，`jne <body>; jmp <exit>`）⇒ 槽组整体平移，非源形可控（N82/N115 封顶）。
    if (cit == last)
    {
        return;
    }
    goto LOOP3;
}