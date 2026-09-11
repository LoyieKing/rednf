// ============================================================================
// importConditionEventChkDungeonScript —— ORIG T 0x08913891
//   bool (const char*, ConditionLevelChkDungeon*)
// [apply level] 写 +0x0c；[unable dungeon] 清空后 do-while push_back；
// 无 unloadRDARScriptFile（ORIG 如此）；out 为空指针时返回 false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: ORIG 0x08913891 — load 检查在最前，之后按
// token(-0x10)/unused(-0x14，仅 ctor/dtor 零引用，P171)/ok(-0x15) 声明；
// 循环出口用 break（ORIG `jne exit`）而非 return true。
bool importConditionEventChkDungeonScript(const char* path,
                                          ConditionLevelChkDungeon* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x10
    std::string unused;                 // ORIG -0x14（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x15

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[apply level]")
        {
            int v = ScanInt(&ok);
            out->m_applyLevel = v;                  // +0x0c
        }
        else if (token == "[unable dungeon]")
        {
            out->m_unableDungeons.clear();
            for (;;)
            {
                int dungeonIdx = ScanInt(&ok);
                if (!ok) { break; }
                out->m_unableDungeons.push_back(dungeonIdx);
            }
        }
    }
    (void)unused;
    return true;
}