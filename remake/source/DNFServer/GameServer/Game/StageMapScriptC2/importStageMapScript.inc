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
