// ============================================================================
// importBloodDungeonScript —— ORIG T 0x08909016
//   int (const char*, stBloodDungeon_t&)，0=成功。
// [ultimate reward prob] 以 map find 判重：key 已存在 → 0x196e 错误。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x08909016。已修：
// [ultimate reward prob] 改 `it == end()` 正极性 + `make_pair(key, prob)` 经
// pair<const int,T> converting ctor（P61/P178），p167 由 3 MISSING/2 EXTRA 转
// CLEAR（33=33）；[reward exp weight] 改 `++i` 先于 ok 检查。残差 N112（nop ×）
// + P211 store 序 + 槽（271 vs 264）⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 263 / ourN 265（gap −2），jcc 36=36、
// 目标块差异 7 段（全部为 ORIG nop 中继岛/块首落点差，jcc 方向与数量逐位一致）。
// [C03CryptoInit68 修·2026-09-10] 按 P0/配方⑦ 删 4 处「ORIG 无槽」具名 int 局部：
//  `[exp standard monster cnt]` 与 `[reward item prob]` 三次 ScanInt 的结果在 ORIG
//  均为 `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0xN(%eax)` **直存**
//  （0x089090e4/0x0890912e/0x08909164/0x0890919a），无 v/v1/v2/v3 槽 ⇒ 全部内联。
//  读数：ourN 265→**264**、本次报告 origN 271（新 r_sdc4 口径），jcc 目标偏移与 ORIG
//  逐位对齐（`je +0x45`/`+0x162` 等全等）；帧由 0x70 回到 0x60 邻域。
// 残余（跨版本族）：ORIG 直存的 RHS-first 中转（`mov %eax,%edx`）vs 我方 LHS-first
// 直存（P211/P76：字段与 ScanInt 返回同型 ⇒ 4.4 恒 LHS-first，源侧不可择）；
// 以及 3 处 ORIG `nop; jmp` 中继岛（N112/P223）⇒ 记 N 封顶。
int importBloodDungeonScript(const char* path, stBloodDungeon_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x191c;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 0;
        }

        if (token == "[reward exp weight]")
        {
            int i = 0;                              // 本表下标从 0 开始
            for (;;)
            {
                out.m_rate[i] = ScanFloat(&ok);     // ORIG fstps 成员直写、后查 ok
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[exp standard monster cnt]")
        {
            // ORIG 0890e1/0890e4 `mov 0xc(%ebp),%edx; mov %eax,0x320(%edx)` ⇒ 直存，
            // 无具名 int 局部（P0/配方⑦）。
            out.m_field320 = ScanInt(&ok);
            if (!ok) { return 0x192f; }
        }
        else if (token == "[reward item prob]")
        {
            // ORIG 0890911e..0890919a：三次 ScanInt 结果均 `mov %eax,%edx` 后与成员
            // 相加/直存，无具名 int 局部（P0/配方⑦）；ok 检查在各直存之后。
            out.m_field324[0] = ScanInt(&ok);
            if (!ok) { return 0x1936; }
            out.m_field324[1] = out.m_field324[0] + ScanInt(&ok);
            if (!ok) { return 0x1939; }
            out.m_field324[2] = out.m_field324[1] + ScanInt(&ok);
            if (!ok) { return 0x193c; }
        }
        else if (token == "[reward gold weight]")
        {
            out.m_field330 = ScanFloat(&ok);        // ORIG fstps 0x330(%eax) 直写
            if (!ok) { return 0x1942; }
        }
        else if (token == "[reward exp inout count weight]")
        {
            for (;;)
            {
                float v = ScanFloat(&ok);
                if (!ok) { break; }
                out.m_vec334.push_back(v);
            }
        }
        else if (token == "[ultimate difficulty point]")
        {
            out.m_ultDiffPoint344 = ScanInt((bool*)0);
            out.m_ultPointTotal348 = ScanInt((bool*)0);
        }
        else if (token == "[ultimate reward prob]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);
                if (!ok) { break; }
                STUltimateRewardProb prob;
                prob.m_field0 = ScanInt((bool*)0);
                prob.m_field4 = ScanInt((bool*)0);

                std::map<int, STUltimateRewardProb>::iterator it =
                    out.m_map358.find(key);
                if (it == out.m_map358.end())
                {
                    out.m_map358.insert(std::make_pair(key, prob));
                }
                else
                {
                    return 0x196e;                  // ORIG key 重复即失败
                }
            }
        }
    }
}