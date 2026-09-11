// ============================================================================
// importStageTimeLineParameter @ 0x8899f26：
//   reset() → loadRDARScriptFile("", path) 失败返 0；
//   循环 ScanType(token,true)：
//     - 失败（EOF）→ 返 0；
//     - "[summon object]" → importSummonObject()，非 0 即作为错误码返回；
//     - "[action define]" → importActionDefine()，非 0 即作为错误码返回；
//     - 其它 token 跳过。
// 返回 0 = 成功（init.md 口径非零=Fail），非 0 = 子段错误码。
// ============================================================================
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N78/N82）→ 试=两 token 分支倒置（E2 实测：83/82 gap=+1 与四数 71/115/0/3/42 逐项不变 ⇒ 回滚）；前任显式 `continue;` 补 ORIG 第 3 回边块无效（82→82，4.4 合并 fall-through）⇒ 维持。
int StageTimeLineParameter::importStageTimeLineParameter(const char* path)
{
    // b28d-35: DIFF→N 封顶（N82 帧/槽 + N78 内联清零族）。ORIG @0x08899f26。
    std::string token;
    int unused18 = 0;                   // ORIG -0x18 死存储（保留以对齐栈帧）
    bool unused11 = false;              // ORIG -0x11 死存储（同上）

    reset();
    if (!loadRDARScriptFile("", path))
        return 0;                       // ORIG 08899f69: ebx=0

    for (;;)
    {
        if (!ScanType(token, true))     // ORIG 08899f88
            return 0;                   // 08899ff4..ffa: ebx=0

        if (token == "[summon object]")
        {
            int r = importSummonObject();
            if (r != 0)
                return r;
            continue;                   // ORIG 08899f73 → 循环头
        }
        if (token == "[action define]")
        {
            int r = importActionDefine();
            if (r != 0)
                return r;
            continue;                   // ORIG 08899f79 → 循环头
        }
        // 未知 token：直接回循环头（ORIG 无额外分支）。
        // [S3Stage 2026-09-11 实测] 显式 `continue;` 试补 ORIG 的第 3 个回边块无效
        //   （82→82，块被 GCC 合并到 fall-through）⇒ 该 1 条差属 4.1 vs 4.4.6 布局，
        //   判 N 封顶（本件维持 83/82）。
    }
}