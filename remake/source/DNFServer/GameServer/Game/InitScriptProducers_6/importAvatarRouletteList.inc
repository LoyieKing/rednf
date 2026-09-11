// ============================================================================
// importAvatarRouletteList —— ORIG T 0x089116eb
//   bool (char* path, AvatarRoulette&)
// [avatar coin info]/[rare clone avatar rate]/[rare avatar rate]/
// [upper avatar rate]/[normal avatar rate] 五个标量 + 十个 job 节
// ([swordman]..[DemonicSwordman])，每节 do{Detail}while(token==[/节名])。
// EOF（ScanTypeOrInt 非型且返 0）或 load 失败之外的出口恒为 true。
// ============================================================================
// b28d-21 复验 — ORIG 0x089116eb。真语义 bug 已修：job 节 do-while 继续条件原写
// `token == "[/节名]"`（方向反，会多吃一个节头），按 ORIG 0x089118a5 `je body`
// 实证改为 `!(token == "[/节名]")`；十个 job 节为**独立 if 序列**（非 else-if）。
// 本轮：十节 do-while 改 `for(;;){Detail; if(token=="[/节名]")break;}` 复现 4.1
// 形 `jmp body; nop; body…; test %al,%al; je body`（do-while 在 4.4 恒产
// `xor $1; test; jne body`）⇒ gap 21→2（raw 328 vs 326，norm 307 vs 305）。
// 残差 2 条：① m_avatarCoinInfo 首节 store ORIG `mov %eax,%edx; mov this,%eax;
// mov %edx,(%eax)` RHS-first+中转 vs 我方 LHS-first（T1–T4 无字段类型直证，
// P321 T5 禁以装载序改类型）⇒ N76。
// [SweepInitProd9 本轮] ② 已翻正：旧判「空 else/continue 两形不闭合」被 P223
// **尾空标签**形推翻——五个标量节尾各 `goto after_sections;`（ORIG 0891179f/
// 089117cd/089117fb/08911829/08911857 的 `jmp 08911b5c`），循环体尾置
// `after_sections:;`（=ORIG 08911b5c 共享中继岛 `jmp 主循环头`）。实测：
// jcc 48=48、目标块签名差异段 6→**0**（逐位全等）；ourN 305→306、ORIG 307
// ⇒ gap 2→1。剩余唯一条 = N76 store 序（`mov %eax,%edx` 中转）。
// p167 CLEAR（47=47）。
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
}