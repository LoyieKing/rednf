// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::GetCandidateMapIndexList（原 TU 第 2822 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-solo-PvP_Room] AE 归零（76=76，本轮）：三处新形组合 ——
// ①P248 档②：PVPMAP_ONLY_* 数组元素改 bool（ORIG 对 `!arr[i]` 发
//   `movzbl; xor $1; test; je/jne`；char 元素时 4.4 恒丢 xor）；
// ②循环改 for 形（增量入第三式、CAND_MAP_NEXT 空标签）⇒ 4.4 循环头/尾直落
//   call，消 while 形的两处中继 jmp；
// ③`if ((mode!=1)&&(mode!=4)) { } else if (!local_d) goto NEXT;` 的
//   fall-through 空 then 取代 `goto CAND_MAP_PUSH` ⇒ 消向前 goto 的 `jmp <nop+1>`。
// 【语义核对】local_d==0 才跳过（等价原短路项 `(mode==1||mode==4) && local_d` 为假）。
int PvP_Room::GetCandidateMapIndexList(std::vector<int>& out)
{
    bool local_d;
    local_d = false;
    // [b28d-solo-PvP_Room] 试 for 形：ORIG 0x085d87xx 循环头/尾各直落 call
    // （4.4 while 形多中继 jmp）；增量入 for 第三式，CAND_MAP_NEXT 为空标签。
    int local_14;
    for (local_14 = 1;
         (local_14 < G_CDataManager()->m_mapList->GetPVPMapCount() + 1) && (local_14 < 0x14);
         local_14 = local_14 + 1)
    {
        // [b28d-solo-PvP_Room] P222/P223：ORIG 0x085d87e6 `call; test %al; je PUSH`
        // （bool callee 正极性直判）+ mode==3 / local_d 两独立岛（0xa7/0xaa）；
        // 短路 `==0 ||` 形在 4.4 反产 xor 物化 + 汇合岛。
        if (IsDeathMatchMap(local_14, local_d))
        {
            if (m_pvpBattleMode == 3)
                goto CAND_MAP_NEXT;
            if ((m_pvpBattleMode != 1) && (m_pvpBattleMode != 4))
            {
                // fall through 到 PUSH（删 `goto CAND_MAP_PUSH`：4.4 为向前 goto
                // 紧邻标签恒发 `jmp <nop+1>` 中继，ORIG 无）。
            }
            else if (!local_d)
            {
                // [b28d-solo-PvP_Room] 语义核对：ORIG `movzbl local_d; xor $1; test; jne NEXT`
                // ⇒ local_d==0 才跳过（等价原短路项 `(mode==1||mode==4) && local_d` 为假）。
                goto CAND_MAP_NEXT;
            }
        }
    CAND_MAP_PUSH:
        // [b28d-7] P167 EXTRA 修正：ORIG 0x085d885e 只有**一个**共享
        // push_back 调用点（两分支 goto 汇入），独立两语句多 1 个调用点。
        // [b28d-solo-PvP_Room] ORIG 0x085d885x：channel==7 分支尾直落 push_back
        // （无 `jmp PUSH_BACK`）；改 if/else 两臂各 `goto NEXT` 后直落，消多余 jmp。
        if (G_GameWorld()->GetChannelType() == 7)
        {
            if (!PVPMAP_ONLY_TOURNAMENT_RANDOM[local_14])
                goto CAND_MAP_NEXT;
        }
        else
        {
            if (!PVPMAP_ONLY_NORMAL_EXPOSURE[local_14])
                goto CAND_MAP_NEXT;
        }
    CAND_MAP_PUSH_BACK:
        out.push_back(local_14);
    CAND_MAP_NEXT:
        ;
    }
}
