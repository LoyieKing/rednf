int TowerOfDespairMgr::LoadScript(const char* dir, const char* path)
{
    // [b28d-solo-CtorProducers_1 判词·P164+P171] ORIG 0x08643e7e：旧「N82 内联块
    // 位置封顶」判词作废。本轮修：①外层 `while (ScanType(token,true))` → 4.4 产
    // 无 xor 的 `test; je`；改 `for(;;){ if(!ScanType(...)) break; … }` 复现 ORIG
    // `call; xor $0x1,%eax; test %al,%al; jne <退出块>`（P164 配套形）；②删除两处
    // `ok = false;`（ORIG 仅在函数头预清一次，P171 照抄不写）。
    // 残余：`_Set_LayerByDungeonIndex(layer, dungeonIdx)` 两实参装载序（ORIG 先
    // `mov -0x10,%edx` 再 `mov -0x14,%eax`，4.4 反）——N82/P75 封顶。
    // ORIG 0x08643e7e：失败返回 0x65；段式扫描后成功返回 0。
    if (!loadRDARScriptFile(dir, path))
        return 0x65;
    std::string token;
    bool ok = false;
    // P164 配套形：外层 `for(;;){ if(!ScanType(token,true)) break; … }` 复现
    // ORIG 的 `call ScanType; xor $0x1,%eax; test %al,%al; jne <退出块>` 头部
    // 条件块 + 取反物化（`while (ScanType(...))` 在 4.4 恒产无 xor 的 `test; je`）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[enterable count]")
            m_enterableCount = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[min Level]")
            m_minLevel = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[top layer]")
            m_topLayer = static_cast<unsigned short>(ScanInt((bool*)0));
        else if (token == "[each layer matching dungeon]")
        {
            for (;;)
            {
                // [PoolB b28f · NEAR→AE 翻正] P248 档③ 异型化：局部 int（callee
                // 形参 unsigned int）复现 ORIG 两实参「先 `mov -0x10,%edx` 再
                // `mov -0x14,%eax`」分离物化 + `mov %edx,0x8(%esp)`/
                // `mov %eax,0x4(%esp)` 压栈序；同型（unsigned）与语句表达式/
                // 纯 register 三形均劣（旧判词 N82/P75 作废）。
                // 读数：verdict AE（clean 150/150、blocks=0）；TU ae 762→763、
                // near 6→5、diff 4 不变（零劣化）。
                int layer = ScanInt(&ok);
                if (!ok) break;
                int dungeonIdx = ScanInt((bool*)0);
                _Set_LayerByDungeonIndex((unsigned int)layer,
                                         (unsigned int)dungeonIdx);
            }
        }
        else if (token == "[npc layer]")
        {
            for (;;)
            {
                unsigned int layer = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) break;
                _Set_APCIndexByLayer(layer);
            }
            std::sort(m_npcLayers.begin(), m_npcLayers.end());
        }
    }
    return 0;
}