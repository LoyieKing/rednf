// ============================================================================
// importDeathTowerScript —— ORIG T 0x089089f3
//   int (const char*, stDeathTower_t&)，0=成功，否则 init.md Fail(%d) 行号码。
// 八个节：单值节带错误码；四个 200 表节从下标 1 开始连续 ScanFloat/ScanInt
// 直至失败（ORIG 无边界检查，忠实保留）；未知节名 → 0x18cd 错误。
// 所有 Scan 均为「先写入字段、后校验 ok」（fstp/mov 先于 ok 判定；
// [reward item prob] 三列前缀和亦同），与 ORIG 指令顺序一致。
// ============================================================================
// b28d-21 复验 — ORIG 0x089089f3。已修：ScanType 失败走 break + 循环外 return 0
// （ORIG `jne <返回0岛>` 0x08908d0f）；表节循环 `field[i]=Scan; ++i; if(!ok)break`
// （ORIG `fstps; addl; xor; test; je body`）；[reward item prob] 三列前缀和无具名
// 中转局部。p167 CLEAR（24=24）。残差 gap 1（raw 243 vs 242，norm 234 vs 233）：
// ① 3 处 store ORIG 为 RHS-first+`mov %eax,%edx` 中转（[apc exp weight] 首节
// 08908a73 段 + [reward card num] 08908b06 段 + [illusion reward card num]
// 08908ba8 段）vs 我方 LHS-first——m_rewardItemProb/m_rewardCardNum 的 int/unsigned
// 无 T1–T4 直证，且改型需动 CDataManager.h + 4 个消费 TU（P321 T5 禁）⇒ N76；
// ② [reward item prob] 两个 += 段我方 `lea (%edx,%eax,1); mov %eax,%edx` vs ORIG
// `add %eax,%edx`（N4/P30）⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 234 / ourN 233（gap +1）；jcc 40=40 且
// **目标块签名差异段 = 0**（逐位交叉验证全等，含全部条件方向/落点）——控制流
// 与 ORIG 完全同构，残差纯为上述 N76 store 序 + N4/P30 与 1 条 nop。
int importDeathTowerScript(const char* path, stDeathTower_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x187e;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[apc exp weight]")
        {
            out.m_apcExpWeight = ScanFloat(&ok);    // fstps (+0x000)，float 位模式
            if (!ok) { return 0x1887; }
        }
        else if (token == "[reward exp weight]")
        {
            int i = 1;                              // ORIG 下标从 1 开始
            for (;;)
            {
                out.m_rewardExpWeight[i] = ScanFloat(&ok);   // fstps 0x4(%eax,%ebx,4)
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[reward card num]")
        {
            int i = 1;
            for (;;)
            {
                // [L16 本轮] P211 档③ 试形：RHS 显式 unsigned 与 int 字段异型
                // ⇒ 复现 ORIG RHS-first + `mov %eax,%edx` 中转（08908b06 段）。
                out.m_rewardCardNumU[i] = ScanInt(&ok);  // (0xc8+i)*4+4 → +0x324+i*4
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward exp weight]")
        {
            int i = 1;
            for (;;)
            {
                out.m_illusionRewardExpWeight[i] = ScanFloat(&ok);  // (0x190+i)*4+4 → +0x644+i*4
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward card num]")
        {
            int i = 1;
            for (;;)
            {
                out.m_illusionRewardCardNumU[i] = ScanInt(&ok);  // P211 档③（08908ba8 段）
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[monster exp weight]")
        {
            out.m_monsterExpWeight = ScanFloat(&ok);  // fstps +0xc84
            if (!ok) { return 0x18b8; }
        }
        else if (token == "[reward item prob]")
        {
            // P211：字段 int 与 RHS unsigned 异型 ⇒ RHS-first + `mov %eax,%edx` 中转。
            out.m_rewardItemProbU[0] = ScanInt(&ok);   // ORIG 先存储后校验（08908a73 段）
            if (!ok) { return 0x18be; }
            // [L16 本轮] 书写序两形实测：`mem + call()` 反劣（ourN 236、ndiff 26）；
            // `call() + mem` 最优（ourN 232、ndiff 6，残 lea-vs-add 2 处 = N4/P205）。
            out.m_rewardItemProbU[1] = (unsigned int)ScanInt(&ok) + out.m_rewardItemProbU[0];
            if (!ok) { return 0x18c1; }
            out.m_rewardItemProbU[2] = (unsigned int)ScanInt(&ok) + out.m_rewardItemProbU[1];
            if (!ok) { return 0x18c4; }
        }
        else if (token == "[reward gold weight]")
        {
            out.m_rewardGoldWeight = ScanFloat(&ok);  // fstps +0xc94
            if (!ok) { return 0x18ca; }
        }
        else
        {
            return 0x18cd;                          // ORIG 未知节名即失败
        }
    }
    return 0;
}