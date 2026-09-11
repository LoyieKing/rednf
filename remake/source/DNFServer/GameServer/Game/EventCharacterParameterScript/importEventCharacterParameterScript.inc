// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0895b006 orig/ours=423/421 ndiff=376。
// 残差为巨件帧/槽号平移（局部表/缓冲）与 std::map/string 内联差（N89）；
// P167 调用点已核，语义按报告逐条对齐。
// [N3-T3 终判 · 试后回滚] ORIG 0x895b172 / 0x895b1b3 / 0x895b23f 三处 = `xor $0x1,%eax;
// test %al,%al; je 0x895b119/11c/11f`，即循环头前 0x895b117-0x895b122 的 **4 个 3 字节
// continue 岛**（`jmp rel8 0x895b123` + nop；入口 = [job const]/[grow type]/[awakening type]
// 三个 `if (!ok)` + 内层 if 链自然回落，四点共用）。试写 `if (!ok) { …; return false; }
// continue;`（语义等价，且真补出 ORIG 的 `xor $1` 前缀）后：本仓 4.4 产出
// `xor; test; je/jne; jmp` 共 4 条，并把 4 个 continue 全部 jump-thread 到尾部 0x801、
// 自然回落块落尾 ⇒ orig/our 423/424、diff_lines **170→204 劣化 34 行**（|gap| 2→1 改善但
// 主指标劣化）⇒ **回滚**，保留 `if (ok) continue;` 原形（421 / 170）。
// 第二处 ScanType 的 fail 块：ORIG 在尾部 0x895b6ba（`nop; mov $0,%ebx; jmp 0x895b6da`）由
// `jne 0x895b6ba` 进入，我方为 fallthrough+`je` inline —— 同属 gcc 块放置/线程化决策
// （非源语句差），本仓 4.4 无源码级旋钮。
// 语义审计（本件）：p167 calls 53/53、`.rodata` 串多重集无实义差异、[item] reason 边界
// `test %eax,%eax; js` + `cmp $0x1f,%eax; jle` 与源码 `reason < 0 || reason > 31` 一致、
// 各 logFormat 行号/串按报告对齐 ⇒ **无语义缺口**。
bool EventCharacterParameterScript::importEventCharacterParameterScript(const char* file_name)
{
    if (!loadRDARScriptFile(g_eventCharacterScriptBaseDirectory, file_name))
        return false;

    std::string token;
    int job = 0;
    int level = 0;
    STEventCharacterInfo_level level_info;
    bool ok;

    if (!ScanType(token, true)) return false;
    if (token == "[level]") {
        level = ScanInt(&ok);
        if (!ok) {
            LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                __FUNCTION__, 0x5f, "file name(%s) job name [%d]", file_name, job);
            return false;
        }
    }
    for (;;) {
        STEventCharacterInfo current;
        for (;;) {
            if (!ScanType(token, true)) break;
            if (token == "[job const]") {
                job = ScanInt(&ok);
                // [N3-T3 · P223 试后回滚] ORIG 0x895b172-0x895b180 = `xor $1; test %al,%al;
                // je 0x895b119`（循环头前 4 个 3 字节 continue 岛之一）+ `jmp 0x895b684`。
                // 试写 `if (!ok) break; continue;`：本仓 4.4 产出 `xor $1; test; jne 809;
                // jmp 801`（4 条），且 4 个 continue 全被 jump-thread 到尾部 0x801、
                // 自然回落块落尾 —— 件级 diff 170→204 行劣化 ⇒ 回滚原形（3 条，含 jmp）。
                if (ok) continue;
                break;
            }
            if (token == "[grow type]") {
                current.m_growType = (unsigned char)ScanInt(&ok);
                // [N3-T3 · P223 试后回滚] ORIG 0x895b1b3-0x895b1c5 = `xor $1; test; je 0x895b11c`
                // + 落体 logFormat/`mov $0,%ebx; mov $0,%esi; jmp 0x895b6a3`。试写
                // `if (!ok) {...return false;} continue;` 虽补出 `xor $1`（前 3 条与 ORIG 逐字
                // 相同），但多第 4 条 `jmp 801`（continue 被 thread 到尾部）⇒ 见上回滚理由。
                if (ok) continue;
                LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                    __FUNCTION__, 0x79, "file name(%s) job name [%d]", file_name, job);
                return false;
            }
            if (token == "[awakening type]") {
                current.m_secondGrowType = (unsigned char)ScanInt(&ok);
                // [N3-T3 · P223 试后回滚] 同 [grow type]：ORIG 0x895b23f-0x895b24b `xor $1; je 0x895b11f`。
                if (ok) continue;
                LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                    __FUNCTION__, 0x82, "file name(%s) job name [%d]", file_name, job);
                return false;
            }
            if (token == "[skill]") {
                while (true) {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok) break;
                    p.second = ScanInt(&ok);
                    if (!ok) break;
                    current.m_skillList.push_back(p);
                }
                continue;
            }
            if (token == "[item]") {
                while (true) {
                    STEventCharacterItem item;
                    item.m_itemIdx = ScanInt(&ok);
                    if (!ok) break;
                    item.m_count = ScanInt(&ok);
                    if (!ok) break;
                    item.m_reason = ScanInt(&ok);
                    if (item.m_reason < 0 || item.m_reason > 31) {
                        LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                            __FUNCTION__, 0xa4,
                            "file name(%s) upgrade script error [%d] upgrade < 0 || upgrade > 31",
                            file_name, item.m_reason);
                        return false;
                    }
                    if (!ok) break;
                    current.m_itemList.push_back(item);
                }
                continue;
            }
            if (token == "[/job const]") {
                if (level_info.character_info.insert(std::make_pair(job, current)).second == 0) {
                    LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                        __FUNCTION__, 0xb3, "file name(%s) job name [%d]", file_name, job);
                    return false;
                }
                break;
            }
            if (token == "[/level]") {
                if (m_levels.insert(std::make_pair(level, level_info)).second == 0) {
                    LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                        __FUNCTION__, 0xbd, "file name(%s) same level exist[%d]", file_name, level);
                    return false;
                }
                return true;
            }
        }
    }
    return false;
}