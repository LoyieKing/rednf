bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel)
{
    // ORIG 0x08a390e7 段：入口（byLevel spill 之后）即 `call _ZNSsC1Ev` 构造 tok，
    // 紧随 `movb $0x0,-0x21` 置 isType —— 两个局部**声明在函数头**（P0：槽图基线；
    // isType@-0x21 / tok@-0x44 为其后全部局部的分配基准）。放分支内会整体浅移。
    std::string tok;
    bool isType = false;
    // ORIG 0x08a391xx 入口 `movb $0x0,-0x21(%ebp)`（P198① 预清必写）⇒ 函数头另有
    // 一个 bool；[targeting bonus] 两处 `mov %al,-0x21` 即写它（复用同一变量）。
    bool ok = false;
    if (*tag == "[think term]")
    {
        info->m_thinkTerm = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[destination change term]")
    {
        info->m_destinationChangeTerm = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[ai pattern]")
    {
        int bucket = 0;
        while (ScanTypeOrStr(isType, tok, true))
        {
            if (isType)
            {
                if (tok == "[/ai pattern]")
                    break;
                if (tok == "[easy]")         bucket = 0;
                else if (tok == "[medium]")  bucket = 1;
                else if (tok == "[hard]")    bucket = 2;
                else if (tok == "[ultimate]") bucket = 3;
                else if (tok == "[hero]")    bucket = 4;
            }
            else
            {
                // ORIG 0x08a39280：直落块是 !byLevel（makeAIFullPath），byLevel
                // 块在跳转目标处（`movzbl;xor $1;test;je` 布局）⇒ 块序交换。
                if (!byLevel)
                    makeAIFullPath(info->m_dir, tok);
                else
                    tok = info->m_dir + tok;   // ORIG: operator+ 后赋回
                std::string* name = new std::string(tok);   // ORIG operator new(4) + string copy
                info->m_patterns[bucket].push_back(name);
            }
        }
        return true;
    }
    if (*tag == "[no change target]")
    {
        info->m_noChangeTarget = (ScanInt((bool*)0) != 0);
        return true;
    }
    if (*tag == "[targeting bonus]")
    {
        for (int i = 0; i <= 21; ++i)
        {
            info->m_targetingBonus[i] = ScanInt((bool*)0);
            if (i == 20)
            {
                // [laneC] ORIG 单站点循环（0x08a393f8..0x08a39468）：
                //   0x328 `lea -0x50; call string::C1Ev`（每轮重新构造 s）
                //   0x32e `call ScanStr`（**唯一**站点）→ 0x333 `mov %al,-0x21(%ebp)`（写
                //   函数级 ok 槽）→ 0x336 `movzbl -0x21; xor $1; test %al,%al; jne <退出>`
                //   → body（0x341..0x388）→ 0x38d `jmp 0x328` 回跳。
                // 原 `ok=ScanStr(&s); while(ok){…; ok=ScanStr(&s);}` 每段 2 个 ScanStr
                // 站点 ⇒ p167 `EXTRA +2 _Z7ScanStrPSs`（§6.2 判据②真缺口方向）；
                // for(;;)+头 ScanStr+`if(!ok)break` 复现单站点与 ORIG 回跳边界。
                for (;;)
                {
                    std::string s;
                    ok = ScanStr(&s);
                    if (!ok)
                        break;
                    // ORIG 0x08a39463：实参右值 → push_back(int&&)。
                    info->m_targetingBonusStr[i].push_back(
                        stringToInt(s.c_str(), ACTIVEOBJECT_STATE_STRING, -1));
                }
            }
            else if (i == 21)
            {
                // [laneC] 同上（ORIG 0x08a39493 `lea -0x54; call C1Ev` → 0x3c9
                // `call ScanStr` 唯一站点 → 0x3ce `mov %al,-0x21` → 0x3da jne 退出）。
                for (;;)
                {
                    std::string s;
                    ok = ScanStr(&s);
                    if (!ok)
                        break;
                    info->m_targetingBonusStr[i].push_back(
                        // [R19scanFGV G 轴 ④] ORIG 0x08a394bd..0x08a394d5 直证：
                        // movl $0x13,0x8(%esp); movl $0x93f54e0,0x4(%esp) → 表 = ACTIVESTATUS_ATTACK_TAG
                        // （0x93f54e0，19 项；ACTIVEOBJECT_STATE_STRING 仅 16 项 ⇒ 查错表 + 越界 3 项）。
                        stringToInt(s.c_str(), ACTIVESTATUS_ATTACK_TAG, 0x13));
                }
            }
        }
        return true;
    }
    if (*tag == "[keep distance with target]")
    {
        info->m_keepDistance = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[keep range distance with target]" ||
        *tag == "[keep range distance with target 1]" ||
        *tag == "[keep range distance with target 2]" ||
        *tag == "[keep range distance with target 3]" ||
        *tag == "[keep range distance with target 4]" ||
        *tag == "[keep range distance with target 5]")
    {
        // ORIG 0x08a3961c..0x08a396b3：4 条逐句写 m_keepRange0..3（非循环）。
        info->m_keepRange0.push_back(ScanInt((bool*)0));
        info->m_keepRange1.push_back(ScanInt((bool*)0));
        info->m_keepRange2.push_back(ScanInt((bool*)0));
        info->m_keepRange3.push_back(ScanInt((bool*)0));
        return true;
    }
    if (*tag == "[warlike]")
    {
        info->m_intelligence = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[intelligence]")
    {
        info->m_intelligence = ScanInt((bool*)0);
        info->m_field1f0 = ScanInt((bool*)0);
        info->m_field1f4 = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[vision]")
    {
        info->m_vision = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[back vision]")
    {
        info->m_backVision = ScanInt((bool*)0);
        return true;
    }
    return false;
}