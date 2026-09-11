// [b28d-solo-Arad_Script] importScript verdict=N 封顶（fresh .o 90/90 gap=0,
// p167 CLEAR 12/12, ndiff 4）。修复：①删 `_AradJumpingRewardItem` 用户空 dtor
// （P317：ORIG 局部 item 无 D1 call，D1/D2 仍由 allocator::destroy 使用点发射 W）；
// ②外层 while 改 goto 顶测消 loop rotation（P283 反向，ndiff 39→21）；③内层
// `while(a&&b&&c)` 物化岛改 for+`if(!ok)break;`（ndiff 21→4）。残余 4 条 =
// ORIG `mov items,%eax; lea &item,%edx`（this 先求值保活）vs 4.4 实参先装载
// （N7/P75；`(*items).`/语句表达式/static_cast receiver 三形实测均 4 条不变）。
bool AradJumping_Script::importScript(char* path, AradJumping_Script& target)
{
    // 报告 ARAD__SCRIPT__AradJumping_Script/importScript.md 全文转写（T 0x088b4128）。
    // ORIG 返回约定：loadRDARScriptFile 失败 → 1；"[Jumping Gift]" 三元组
    // 解析完毕 → 0。条目循环内每个 ScanInt 失败直接回到 ScanType 主循环。
    std::string token;
    bool ok = false;
    if (!loadRDARScriptFile("", path))
        return true;

    // getRDARScriptData(target) 在解析循环前取一次（ORIG +0x41 存 -0xc(%ebp)）。
    std::vector<_AradJumpingRewardItem>* items = target.getRDARScriptData();

    // [b28d-solo-importScript] ORIG 外层循环为「条件置顶 + 体尾回跳」布局
    // （0x088b4174 条件块 → 0x088b418c `jne 0x088b4213` 退出 → 体 →
    // 0x088b420e `jmp 0x088b4174`），4.4 对 while 恒做 loop rotation
    // （条件置底 + 入口 jmp）⇒ goto 顶测形复现（P283 反向用法）。
    // 内层：ORIG 每条件 `movzbl ok; xor $1; test; je 继续; jmp 出口`（4.1 短路
    // 平铺）；4.4 对 `while(a&&b&&c)` 恒物化 0/1（+6 条）⇒ 改 for(;;)+`if(!ok)break;`
    // 消物化（ndiff 39→4）。残余 4 条 = ORIG `mov items,%eax; lea &item,%edx`（this
    // 先求值保活）vs 4.4 `lea &item,%eax; mov items`（实参先）——this-first 装载序
    // 不可翻（N7/P75 跨版本实参装载时刻；`(*items).`/语句表达式/static_cast receiver
    // 三形实测均 4 条不变）。gap=0、调用点集全等。
    // [L3Free25to36.L3AUserHistoryAradScript 2026-09-11 · 再补一形] receiver 改下标形
    // `items[0].push_back(item)` 复编后 ndiff 仍 =4、两侧助记符差均空 ⇒ 4.4 求值序
    // 不变（实参仍先于 receiver 装载）⇒ 回滚。
outer_test:
    if (!ScanType(token, true))
        goto outer_done;
    if (token == "[Jumping Gift]")
    {
        _AradJumpingRewardItem item;
        for (;;)
        {
            item.m_job = ScanInt(&ok);
            if (!ok)
                break;
            item.m_growValue = ScanInt(&ok);
            if (!ok)
                break;
            item.m_money = (unsigned long)(unsigned int)ScanInt(&ok);
            if (!ok)
                break;
            items->push_back(item);
        }
    }
    goto outer_test;
outer_done:
    return false;
}