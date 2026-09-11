// [b28d-solo-InitScriptProducers_8 N] ORIG 0890b2a3：p167 CLEAR（39/39 调用点全等）；
// clean 320/321（fresh .o 口径，ORIG 327 insns，净 1 条）。已对齐：删多余死 string
// （ORIG 无第二 string ⇒ 帧 0xb0→0xa0、ok 回 -0x35）、ok!=true 的 xor 物化 ×14 与
// ORIG 全等、output/bonus 改 for(;;)+if(!ok)break 形（P165 两份物理独立块）、
// make_pair<unsigned long&,unsigned int&>、push_back(O&&)、P171 越界读照抄。
// 残余为 4.1/4.4 块序（ORIG 两连 jmp 中继 vs 4.4 jump-thread）——N104/N121 封顶。
// [P1T2 复验 2026-09-11] 残余 4 块（clean 320/321，原 327/328）：ORIG 在循环头
// 0x890b330 前有 3 条 `jmp 890b330` 中继（0x890b327 fall-through 入口、
// 0x890b32a/0x890b32d 分别由 0x890b3c1/0x890b42f[je] 进入），4.4 把 continue
// 就地物化（我方 0x2167 等）⇒ 净差 1 条。A/B 三档（-B cc1plus446bin、
// -B …/4.4.4/、-fno-thread-jumps）TU 四数逐项相同、件级 gap 不变 ⇒ 源码不可翻。
int importVendingMachineInfoCore(const char* dir, const char* file,
                                 stVendingMachine_t& vm)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1e0b;

    std::string token;
    bool ok = false;

    for (;;)
    {
    scan_again:
        // ---- 组外：未知标签记日志，直到 [item group] 或 EOF ----
        for (;;)
        {
            if (!ScanType(token, true))
                goto eof_exit;                     // ORIG jne 尾岛（890b714）
            if (token == "[item group]")
                break;
            goto unknown_tag;                      // ORIG 冷块落函数尾
        }

        stVendingMachineInfo_t info;                   // ctor 后自 clear

        for (;;)
        {
            if (!ScanType(token, true))
                break;                                 // EOF：析构后外层再扫失败→0
            if (token == "[/item group]")
            {
                vm.m_infoList.push_back(info);
                info.clear();
                break;
            }
            if (token == "[group num]")
            {
                info.m_groupNum = ScanInt(&ok);
                if (ok != true)
                    return 0x1e27;
                continue;
            }
            if (token == "[material]")
            {
                info.m_payType = ScanInt(&ok);
                if (ok != true)
                    return 0x1e2d;
                info.m_payAmount = ScanInt(&ok);
                if (ok != true)
                    return 0x1e30;
                continue;
            }
            // P165：ORIG 0x890b444/0x890b572 为两份物理独立块（各 4 次
            // ScanInt(bool*)，槽集不同）⇒ 源内展开两份，勿用 bonus 标志参数化。
            if (token == "[output]")
            {
                stVendingMachineOutput_t entry;
                for (;;)
                {
                    entry.m_itemId = ScanInt(&ok);
                    if (!ok)
                        break;
                    const int rate = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e3d;
                    entry.m_count = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e40;
                    if (ScanInt(&ok) == 1)
                        info.m_lottery.push_back(
                            std::make_pair(
                                reinterpret_cast<unsigned long&>(
                                    entry.m_itemId),
                                entry.m_count));
                    if (ok != true)
                        return 0x1e44;
                    info.m_giveCount += rate;
                    entry.m_cumWeight = info.m_giveCount;
                    info.m_vecGive.push_back(entry);
                }
                continue;
            }
            if (token == "[bonus output]")
            {
                stVendingMachineOutput_t entry;
                for (;;)
                {
                    entry.m_itemId = ScanInt(&ok);
                    if (!ok)
                        break;
                    const int rate = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e55;
                    entry.m_count = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e58;
                    if (ScanInt(&ok) == 1)
                        info.m_lottery.push_back(
                            std::make_pair(
                                reinterpret_cast<unsigned long&>(
                                    entry.m_itemId),
                                entry.m_count));
                    if (ok != true)
                        return 0x1e5c;
                    info.m_bonusCount += rate;
                    entry.m_cumWeight = info.m_bonusCount;
                    info.m_vecBonus.push_back(entry);
                }
                continue;
            }
            // 组内其他标签：静默继续（ORIG 无日志路径）
        }
    }
unknown_tag:
    LogManager::logFormat(
        0, "../RDARScriptEtcParameter.cpp",
        __FUNCTION__, 0x1e66,
        "ERROR : Unknown tag error. %s", token.c_str());
    goto scan_again;
eof_exit:
    return 0;
}