int importDeadTowerDialogScript(const char* path, STDeadTowerFullDialog& out)
{
    // ORIG 0x8908d4e：bool ok 为函数级单槽（-0x9），三组 dialog 共用（P320）。
    bool ok = false;
    // ORIG 0x8908d52：token 在 loadRDARScriptFile 之前构造（-0x10）。
    std::string token;
    // ORIG 0x8908d64：loadRDARScriptFile("", path) —— dir 空串，path 即文件路径。
    if (!loadRDARScriptFile("", path))
        return 0x18d9;
    for (;;)
    {
        if (!ScanType(token, true))
            break;   // ORIG 0x8908d9a jne 尾块（+0x299）：失败共享尾部 return 0（P164）。
        if (token == "[dialog on map start]")
        {
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x18e6;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x18ea;
            // ORIG 0x8908e13：movb $0x1,-0x14 = dlg1(+0x8) ⇒ m_flag1。
            dlg.m_flag1 = 1;
            out.m_mapStartDialog.push_back(dlg);
        }
        else if (token == "[dialog on map complete]")
        {
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x18f6;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x18fa;
            // ORIG 0x8908ed3：movb $0x1,-0x1f = dlg2(+0x9) ⇒ m_flag2。
            dlg.m_flag2 = 1;
            out.m_mapCompleteDialog.push_back(dlg);
        }
        else if (token == "[dialog on die]")
        {
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x1906;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x190a;
            // ORIG 0x8908f96：movb $0x1,-0x2a = dlg3(+0xa) ⇒ m_flag3。
            dlg.m_flag3 = 1;
            out.m_dieDialog.push_back(dlg);
        }
    }
    return 0;
