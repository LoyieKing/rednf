bool importAvatarRouletteList(char* path, AvatarRoulette& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x14
    bool ok = false;                    // ORIG -0xd（死局部，ORIG 仅写一次）
    bool isType = false;                // ORIG -0x15
    bool isInt = false;                 // ORIG -0x16
    (void)ok;

    out.clear();

    for (;;)
    {
        int ret = ScanTypeOrInt(isType, token, isInt, true);   // ORIG -0xc
        if (!isInt && ret == 0)
        {
            break;
        }
        if (token == "[avatar coin info]")
        {
            out.m_avatarCoinInfoU = ScanInt((bool*)0);   // [L16] P211 档③（ORIG 0891179f 段 RHS-first）
            goto after_sections;        // ORIG 0891179f jmp 08911b5c（共享岛）
        }
        else if (token == "[rare clone avatar rate]")
        {
            out.m_rareCloneAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else if (token == "[rare avatar rate]")
        {
            out.m_rareAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else if (token == "[upper avatar rate]")
        {
            out.m_upperAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else if (token == "[normal avatar rate]")
        {
            out.m_normalAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else
        {
            if (token == "[swordman]")
            {
                // ORIG 08911876 形：`body; call operator==; test %al,%al; je body`
                // （4.1 do-while 条件在尾、无 xor）⇒ 用 for(;;)+`if(cond) break;`
                // 复现直分支 je（do-while 在 4.4 恒产 `xor $1; test; jne body`）。
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)0);
                    if (token == "[/swordman]") { break; }
                }
            }
            if (token == "[fighter]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)1);
                    if (token == "[/fighter]") { break; }
                }
            }
            if (token == "[gunner]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)2);
                    if (token == "[/gunner]") { break; }
                }
            }
            if (token == "[mage]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)3);
                    if (token == "[/mage]") { break; }
                }
            }
            if (token == "[priest]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)4);
                    if (token == "[/priest]") { break; }
                }
            }
            if (token == "[at_gunner]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)5);
                    if (token == "[/at_gunner]") { break; }
                }
            }
            if (token == "[thief]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)6);
                    if (token == "[/thief]") { break; }
                }
            }
            if (token == "[at_fighter]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)7);
                    if (token == "[/at_fighter]") { break; }
                }
            }
            if (token == "[at_mage]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)8);
                    if (token == "[/at_mage]") { break; }
                }
            }
            if (token == "[DemonicSwordman]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)9);
                    if (token == "[/DemonicSwordman]") { break; }
                }
            }
            // 注：ORIG 08911b57 节尾另有 1 条显式 `jmp 主循环头`；4.4 对链尾裸
            // if 恒 jump-thread（空 else / 显式 continue 两形实测均不闭合）⇒ N。
        }
        // b28d-solo-InitScriptProducers_6: P223 尾空标签 — ORIG 08911b5c 是
        // 五个标量节共享的 `jmp 主循环头` 中继岛；标量节尾显式 goto 至此，
        // 4.4 若线程化则本形不成立（判词记实测读数）。
    after_sections:;
    }
    return true;
