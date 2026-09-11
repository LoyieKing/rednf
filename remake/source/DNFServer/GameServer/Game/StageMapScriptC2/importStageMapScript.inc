// ORIG T 0x088a9f28.  This is a direct C++ transcription of the report's
// ScanType/ScanInt state machine.  In particular, [stage] consumes rows of
// four integers until the first failed ScanInt, then appends the current
// district; [slot type] consumes five-integer records and only inserts a new
// key; and [/area] appends the area without appending the pending district.
// [本波收敛] ORIG 0x088a9f28 / 457 insns；本波 475→459（纯指令差 2 条）：
//   ① P160-P162 槽复用：全部 ScanInt 结果写 ORIG 同一槽 -0xc（单具名 int），
//      13 个 v0..v4 独立局部曾使帧深 0x38（帧 0xd0→0xa0 与 ORIG 全等）；
//   ② P164 显式回边：`scan_next:` 标签 + `goto finish`/`goto scan_next`
//      复现 ORIG「入口直落 + 双 jmp/nop 岛」（ORIG +0x50/+0x53）;
//   ③ P222 正向包裹：`if (token == "[area]") {...}` 消掉 4.4 的 `xor $1` 物化；
//   ④ 去 finalizeAfterArea 具名 bool（ORIG 用 ebx 寄存器）⇒ 槽再上移 4；
//   ⑤ StageData 用户空析构删除（P12：ORIG D1/D2 为空体 W，且 importStageMapScript
//      全程不调用它）⇒ [stage] 失败路径的 dtor 调用消失，missing 86→82；
//   ⑥ operator[] 结果用 `register SlotType&` 绑定（非 register 会落槽 +6 条）。
// 残差 2 条（N 封顶，均有 ORIG 指令形证据）：
//   · 0x088aa2e7 段 ORIG `mov SLOT,%edx; mov %edx,(%eax)` vs 4.4 把引用绑进
//     %ebx 并多 1 条 `mov %eax,%ebx`（引用/间接基址寄存器分配差，N7 族）；
//   · 0x088aa46c 岛区 4.4 多 1 条 `jmp`（多目标共享 continue 岛布局，
//     负例「空 else nop;jmp 岛 vs jump-threading」族）。
// [C05 范式复查留痕] 已按 Main 转发的 F1（尾部 fail 块）/F2（int 局部⇒cmp 内存
//   直比）/③（P223 早返中继岛）逐条复查本件残差，**均不适用**：
//   · 本函数无 `cmp` 类残差（元组不同），F2 无对象；
//   · 残差 jmp 经 objdump 目标分析为 **4.4 跳转合并跳板**：6 处 `jmp 962`（其中
//     0x8f3/0x92b 用 rel8、0x61f/0x64c/0x685/0x805 用 rel32）+ 跳板 `962: jmp 5bc`
//     共 7 条 vs ORIG 6 条直跳 `jmp 088a9f7e`；属编译器跳转合并布局（P223 管的是
//     「早返 return 各发 jmp+nop 中继岛」，形不同，已实测非其族）；
//   · 另一条 `mov %eax,%ebx` 为间接基址寄存器分配差（N7/N18a 族）。
//   · D08/C05③ **试前闸门**（Main 定：我方岛数 − ORIG 岛数 ≥ 2 才试合并）：本件
//     岛数 **3 vs 3 相同**（0x088aa46c/46f/472 ↔ 我方 960/968/96b），差 = 0 ⇒
//     按闸门**不试**；实测反例族（A06 `_onChangeGrowType` 合并 `||` 反劣 +4、
//     A28 `generateRandomItem` 改 `goto end` 无效）亦支持判 A28 epilogue-threading。
bool importStageMapScript(stagemap::StageMapScript* out, const char* path)
{
    if (!loadRDARScriptFile(g_stageMapScriptBaseDirectory, path))
        return false;

    out->clear();

    std::string token;
    // [本波·P160-P162 槽复用] ORIG 全部 ScanInt 结果写同一槽 -0xc
    // （0x088aa019/0x088aa086/0x088aa10b/0x088aa1ca… 全为 `mov %eax,-0xc(%ebp)`）
    // ⇒ 源码只有一个被复用的具名 int；写成 v0..v4 独立局部会让帧深 +0x38。
    int scanValue = 0;
    bool ok = false;

    for (;;)  // outer token loop
    {
    scan_next:                     // [P164] 显式回边落点：入口直落条件头
        if (!ScanType(token, true))
            goto finish;

        if (token == "[name]")
        {
            (void)ScanStr(&out->m_name);
            goto scan_next;
        }

        if (token == "[map image]")
        {
            (void)ScanStr(&out->m_mapImage);
            out->m_field8 = ScanInt((bool*)0);
            goto scan_next;
        }

        if (token == "[dungeon]")
        {
            scanValue = ScanInt(&ok);
            if (!ok)
                goto finish;  // ORIG finalizes directly at +0x5da
            out->m_dungeonIdx = scanValue;
            goto scan_next;
        }

        // [P222] 正向包裹：ORIG 0x088aa051 为 `test %al,%al; je <loop_head>`
        // （无 xor 物化）；写 `if (!(...)) goto` 会让 4.4 先 xor $1 再分支。
        if (token == "[area]")
        {
        // The first [area] integer is special: a failed scan selects the
        // ORIG fatal/finalize path (ebx=0), rather than merely ending this
        // area.  The lexical scope still destroys AreaData as ORIG does.
            std::string areaToken;
            stagemap::AreaData area;
            area.clear();

            scanValue = ScanInt(&ok);
            if (!ok)
            {
                // [P164] ORIG 0x088aa099 `mov $0x0,%ebx` → +0x4b4 共享析构块
                // → 收尾 return true（不设具名标志：ORIG 用 ebx 寄存器，
                // 4.4 的具名 bool 会多占一个栈槽并整体加深帧 0x4）。
                goto finish;
            }
            else
            {
                area.m_field0 = scanValue;
                stagemap::DistrictData district;
                district.clear();

                for (;;)  // token loop within [area]
                {
                    if (!ScanType(areaToken, true))
                        break;  // ORIG cleanup, then resume outer loop

                    if (areaToken == "[district]")
                    {
                        district.clear();
                        scanValue = ScanInt(&ok);
                        if (!ok)
                            break;  // ORIG +0x547 cleanup path
                        district.m_field0 = scanValue;
                        continue;
                    }

                    if (areaToken == "[slot image]")
                    {
                        (void)ScanStr(&area.m_slotImage);
                        continue;
                    }

                    if (areaToken == "[append image]")
                    {
                        (void)ScanStr(&district.m_appendImage);
                        district.m_field14 = ScanInt((bool*)0);
                        continue;
                    }

                    if (areaToken == "[slot type]")
                    {
                        for (;;)  // five-int slot records
                        {
                            stagemap::SlotType slot;
                            slot.clear();

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field0 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field4 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field8 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_fieldC = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field10 = scanValue;

                            // ORIG performs find first and only fills a new
                            // key; an existing slot type is left untouched.
                            if (area.m_slots.find(slot.m_field0) ==
                                area.m_slots.end())
                            {
                                register stagemap::SlotType& dst =
                                    area.m_slots[slot.m_field0];
                                dst.m_field0 = slot.m_field0;
                                dst.m_field4 = slot.m_field4;
                                dst.m_field8 = slot.m_field8;
                                dst.m_fieldC = slot.m_fieldC;
                                dst.m_field10 = slot.m_field10;
                            }
                        }
                        continue;
                    }

                    if (areaToken == "[stage]")
                    {
                        for (;;)  // rows of four ints
                        {
                            stagemap::StageData stage;
                            stage.clear();

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;   // P276 共享调用点（ORIG 单处）
                            stage.m_field0 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_field4 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_field8 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_fieldC = scanValue;
                            district.m_vec4.push_back(stage);
                        }
                    stage_rows_done:
                        // [本波·P276] p167 EXTRA push_back<DistrictData>×3 ⇒
                        // ORIG 四条失败分支 **goto 汇入单一 push_back**
                        // （原写四份独立调用点）。
                        area.m_vec20.push_back(district);
                        continue;
                    }

                    if (areaToken == "[survival]")
                    {
                        scanValue = ScanInt(&ok);
                        if (!ok)
                            break;  // ORIG +0x54a cleanup path
                        area.m_field2c = scanValue;
                        continue;
                    }

                    if (areaToken == "[/area]")
                    {
                        out->m_areaVec.push_back(area);
                        break;  // pending district is intentionally not added
                    }

                    // Unknown area tags are ignored just as the ORIG chain of
                    // string comparisons does.
                }
            }
        }
        goto scan_next;
    }

finish:
    out->m_fileIndex = g_stageMapScriptFileList.GetIndex(path);
    return true;
}