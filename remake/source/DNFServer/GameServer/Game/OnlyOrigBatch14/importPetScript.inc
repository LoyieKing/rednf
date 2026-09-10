bool importPetScript(PetScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_petScriptBaseDirectory, path))
        return false;
    script->clear();
    getDirectoryPath(script->m_directoryPath, "%s%s",
                     g_petScriptBaseDirectory, path);
    std::string token;
    std::string tmp;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x08a359 段在 token/tmp 构造后立即
    // `movl $0,-0x14(v); movb $0,-0x15(ok)`（P212 全头部声明）⇒ v/ok 提到函数头。
    // [b28d-solo-OnlyOrigBatch14 判词·AE 归零（300=300）] 本波两修：
    // ①P223/P59：ScanType 失败块改 `goto pet_script_done` 尾标签（ORIG
    // 0x08a359ab `xor $1; test; jne <函数尾共享块>`；4.4 对 if-body 恒内联
    // 为 je 直落）；②P167：[int data] 循环改单 call 点
    // `for(;;){ v=ScanInt(&ok); if(!ok) break; … }`（ORIG 仅 1 条 ScanInt
    // call 指令，我方 for-init+increment 有 2 条）。v/ok 提升 + 声明序按
    // ORIG 0x08a35 段对齐。
    int v = 0;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            // [b28d-solo-OnlyOrigBatch14] P223/P59：ORIG 0x08a359ab 段
            // `xor $1; test; jne <函数尾共享块>` ⇒ 失败块在尾部，用 goto
            // 尾标签复现（4.4 对 if-body 恒内联为 je 直落）。
            goto pet_script_done;
        if (token == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (token == "[width]")
        {
            script->m_width1 = ScanInt((bool*)0);
            script->m_width2 = ScanInt((bool*)0);
        }
        else if (token == "[floating height]")
        {
            script->m_floatingHeight = ScanInt((bool*)0);
        }
        else if (token == "[layer]")
        {
            ScanStr(&tmp);
            script->m_layer = getDrawLayer(tmp.c_str());
        }
        else if (token == "[recovery time]")
        {
            script->m_recoveryTime = ScanInt((bool*)0);
        }
        else if (token == "[direction correct]")
        {
            script->m_directionCorrect = ScanInt((bool*)0) != 0;
        }
        else if (token == "[move speed]")
        {
            script->m_moveSpeed[0] = ScanInt((bool*)0);
            script->m_moveSpeed[1] = ScanInt((bool*)0);
            script->m_moveSpeed[2] = ScanInt((bool*)0);
            script->m_moveSpeed[3] = ScanInt((bool*)0);
            script->m_moveSpeed[4] = ScanInt((bool*)0);
        }
        else if (token == "[gravity]")
        {
            script->m_gravity = ScanInt((bool*)0);
        }
        else if (token == "[basic motion]")
        {
            ScanStr(&script->m_basicMotion);
        }
        else if (token == "[etc motion]")
        {
            script->m_etcMotion.clear();
            while (ScanStr(&tmp))
            {
                script->m_etcMotion.push_back(tmp);
            }
        }
        else if (token == "[attack info]")
        {
            script->m_attackInfo.clear();
            while (ScanStr(&tmp))
            {
                script->m_attackInfo.push_back(tmp);
            }
        }
        else if (token == "[int data]")
        {
            script->m_intData.clear();
            // [b28d-solo-OnlyOrigBatch14] P167：ORIG 该段仅 **1 条** ScanInt
            // call 指令（`call; mov %eax,-0x14; movzbl -0x15; xor $1; test;
            // je <exit>`），我方 for 形有 2 条（init+increment）⇒ 改
            // 单点 `for(;;){ v=ScanInt(&ok); if(!ok) break; … }`。
            for (;;)
            {
                v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_intData.push_back(v);
            }
        }
        else if (token == "[string data]")
        {
            script->m_stringData.clear();
            while (ScanStr(&tmp))
            {
                script->m_stringData.push_back(tmp);
            }
        }
    }
pet_script_done:
    script->m_idx0 = g_petScriptFileList.FindIndexByFullScan(path);
    return true;
}
