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
                std::string s;
                // ORIG 0x08a3940e：`call ScanStr; mov %al,-0xa5(%ebp)`（-0xa5 = 函数级
                // `ok`）+ `movzbl; xor $1; test; jne <exit>` ⇒ 原码把 ScanStr 结果写回
                // **复用**的函数级 ok（P0 槽往返；直写 `while (ScanStr(&s))` 被 4.4
                // 折成裸 `test %al,%al; je`，少 mov+取反）。
                ok = ScanStr(&s);
                // [C05④ 步骤 2 实测·取反形回退] `while (true) { if (!ok) break; … }` 亦试过：
                // our 473→**478**（gap 2→−3，劣 5 条；A17 探针的 `if(!r)` 取反形在此
                // body 下不成立）⇒ 保留 `while (ok)` 具名形（gap=2，本件最优）。
                while (ok)
                {
                    // ORIG 0x08a39463：实参右值 → push_back(int&&)。
                    info->m_targetingBonusStr[i].push_back(
                        stringToInt(s.c_str(), ACTIVEOBJECT_STATE_STRING, -1));
                    ok = ScanStr(&s);
                }
            }
            else if (i == 21)
            {
                std::string s;
                ok = ScanStr(&s);                    // 同上（ORIG 0x08a394a9）
                while (ok)
                {
                    info->m_targetingBonusStr[i].push_back(
                        stringToInt(s.c_str(), ACTIVESTATUS_ATTACK_TAG, 0x13));
                    ok = ScanStr(&s);
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
