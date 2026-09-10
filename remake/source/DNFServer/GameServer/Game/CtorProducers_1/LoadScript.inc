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
                unsigned int layer = static_cast<unsigned int>(ScanInt(&ok));
                if (!ok) break;
                unsigned int dungeonIdx = static_cast<unsigned int>(ScanInt((bool*)0));
                // [b28d-solo-CtorProducers_1 判词·N/P75] ORIG 0x08643e7e 两实参
                // 先物化 %edx/%eax 再写 esp 槽（4.1 求值-压栈分离）；4.4 逐实参
                // 「载入-压栈」。语句表达式/纯 register 两形实测均劣化
                // （前者零效、后者帧 -8 且槽漂移）⇒ P75 跨版本封顶。
                _Set_LayerByDungeonIndex(layer, dungeonIdx);
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
