// ============================================================================
// importPvpMissionParameterScript（ORIG
// _Z31importPvpMissionParameterScriptR22MissionParameterScript，0x8a5b7ec）。
// 反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionParameterScript.md。
// ============================================================================
bool importPvpMissionParameterScript(MissionParameterScript& param)
{
    param.clear();
    if (!loadRDARScriptFile("Etc/", "MissionSystem.etc"))
        return false;

    std::string token;
    bool ok = false;
    // [R26D] 头置条件形（P283/P164 反向）：ORIG 0x8a5b847 入口 `jmp +0x4a` 后
    // 循环头即 ScanType（`xor $1;test;jne +0x266` 失败跳尾部出口），
    // `while(ScanType(...))` 会被 4.4 旋转成尾置条件（我方旧形循环头在 +0x239）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if ("[frequent daily mission]" == token)
        {
            int count = ScanInt(&ok);
            if (!ok)
                return false;
            for (int i = 0; i < count; ++i)
            {
                unsigned int idx = (unsigned int)ScanInt(&ok);
                if (!ok)
                    return false;
                // [P2S1PvPQuest 2026-09-11 P167] ORIG 0x8a5b8ba-0x8a5b8cd：
                //   `mov -0x38(%ebp),%eax; mov %eax,-0x28(%ebp); lea -0x28,%edx;
                //    ...; call push_backEOj`（**右值版**）⇒ 实参是 prvalue（先构造
                //   临时 -0x28 再传址）；写 `push_back(idx)` 是左值 ⇒ 选
                //   `push_backERKj`（EXTRA）+ 少 2 条指令 + 浅端槽整体错位 4B。
                //   cast 到同型作 prvalue（值不变，零语义差）。
                param.m_dailyIndices.push_back((unsigned int)idx);   // +0x00
            }
        }
        else if ("[casual daily mission]" == token)
        {
            param.m_field0c = ScanInt(&ok);               // +0x0c
            if (!ok)
                return false;
            for (;;)
        {
                int weight = ScanInt(&ok);
                if (!ok)
                break;                                 // ORIG 0x8a5b944 nop / 945 jmp 836
                                                       //（4.4 goto 形会复制一份条件头，实测 +6，故留 break）
                int missionIdx = ScanInt(&ok);
                if (!ok)
                    return false;
                param.m_dailyList.push_back(
                std::make_pair(weight, missionIdx));   // +0x10
                param.m_field18 += weight;                      // +0x18
        }
    }
        else if ("[mission reward exp table]" == token)
            {
            param.m_baseExp.resize(0x1e);                 // +0x1c
            for (int i = 0; i < 0x1e; ++i)
        {
            // ORIG 0x8a5ba1d..0x8a5ba35：operator[] 取址 → ScanInt → 直 store → ok 检查
            //（无 v 具名槽，P0）
            param.m_baseExp[i] = (unsigned int)ScanInt(&ok);
                if (!ok)
                    return false;
        }
    }
}
    return true;
}