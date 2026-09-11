// ============================================================================
// importQuestParameterScript —— ORIG 0x08a6b660
// 报告 _lt_global_gt_/importQuestParameterScript.md；调用点 init.md +0x207a：
//   importQuestParameterScript(this+0x8cc8, "n_Quest/questParameter.etc")。
// [difficulty] 用 ScanTypeOrStr：类型令牌取 sub[0] 为键、ScanInt 为值插入
// m_map0；字符串令牌仅识别 [/difficulty] 终止符。[piece reward] 中首个
// ScanInt 失败即整体返回 false（asm +0x445）。
// ============================================================================
// [b28d-solo3-2 收口] 447/446、P167 CLEAR(66/66)、gap 3→1；两处形态修正
// （合并短路 break / while(appendage!=-1){…break;} 零槽 setne）见文件头判词表。
bool importQuestParameterScript(QuestParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x08a6b660：`mov 0x8(%ebp),%eax; mov %eax,-0x20(%ebp)` —— 形参缓存
    // 进具名局部 p（P0：该槽后续 12 处重读），全部访问走 p->。
    QuestParameterScript* p = script;
    p->clear();

    std::string token;
    int dead1c = 0;                // ORIG -0x1c `movl $0`（dead 局部，P31/P232）
    bool done = false;

    while (true)
    {
        if (!ScanType(token, true))
            goto scan_end;

        if (token == "[difficulty]")
        {
            std::string sub;
            while (true)
            {
                bool isStr;                // ORIG 无预清（ScanTypeOrStr 首写）
                if (!ScanTypeOrStr(isStr, sub, true))
                    break;
                if (isStr)
                {
                    if (sub == "[/difficulty]")
                        break;
                    continue;
                }
                int value = ScanInt((bool*)0);
                // ORIG 0x08a6b7xx：`call c_str` 后 `movl $1,0x8(%esp)` +
                // `call memcpy@plt`（1 字节拷贝到 key 槽）⇒ 原码是显式
                // `memcpy(&key, sub.c_str(), 1)`，不是 `sub.c_str()[0]`
                //（P167 差集：ORIG-only memcpy ×1）。
                char key;
                memcpy(&key, sub.c_str(), 1);
                p->m_map0.insert(std::make_pair(key, value));
            }
            continue;
        }
        if (token == "[green level penalty]")   { p->m_field18 = ScanInt((bool*)0); continue; }
        if (token == "[grey level penalty]")    { p->m_field1c = ScanInt((bool*)0); continue; }
        if (token == "[bal kun penalty]")       { p->m_field20 = ScanInt((bool*)0); continue; }
        if (token == "[exp reward table]")
        {
            p->m_vec24.reserve(200);
            bool ok;                   // ORIG 块级 ok 槽 -0x87（各段独立，不复用）
            int value;
            for (int i = 0; i < 200; ++i)
            {
                if (!(value = ScanInt(&ok), ok))
                    break;
                p->m_vec24.push_back(value);
            }
            continue;
        }
        if (token == "[gold reward table]")
        {
            p->m_vec30.reserve(200);
            bool ok;                   // ORIG 块级 ok 槽 -0x8d
            int value;
            for (int i = 0; i < 200; ++i)
            {
                if (!(value = ScanInt(&ok), ok))
                    break;
                p->m_vec30.push_back(value);
            }
            continue;
        }
        if (token == "[piece reward table]")
        {
            bool ok;                   // ORIG 块级 ok 槽 -0x95
            while (true)
            {
                int key = ScanInt(&ok);
                if (!ok)
                    break;
                int value = ScanInt(&ok);
                if (!ok)
                    break;
                p->m_map3c.insert(std::make_pair(key, value));
            }
            continue;
        }
        if (token == "[piece reward]")
        {
            bool ok;                   // ORIG 块级 ok 槽 -0xa1
            int questIdx = ScanInt(&ok);
            if (!ok)
                return false;

            std::vector<pieceQuestRewardStatus> statuses;
            std::string sub;
            while (true)
            {
                // ORIG 0x08a6bace..0x08a6bb13：合并短路 `||` + bool 物化
                // （jne→mov $1 / je→mov $0 / test; jne break）。探针实测
                // `if (!ScanType(...) || sub == "[/piece reward]") break;`
                // 逐条复现；独立 break 恒产两个直跳出口，少 5 条。
                if (!ScanType(sub, true) || sub == "[/piece reward]")
                    break;

                int appendage = loadAppendageType(sub);
                // ORIG 0x08a6bb2a：`cmpl $0xffffffff,-0xc(%ebp); setne %al;
                // test %al,%al; je <循环头>` —— P225 实证 `while(cond){body;break;}`
                // 精确复现该 setne/test/je 且**零槽**（具名 bool 恒落槽 -0x9，把
                // appendage 由 -0xc 推到 -0x10、整帧 +0x10）。
                while (appendage != -1)
                {
                    pieceQuestRewardStatus status;
                    status.m_appendageType = appendage;
                    std::string typeText;
                    ScanStr(&typeText);
                    status.m_valueType = loadValueType(typeText);
                    status.m_value = ScanFloat((bool*)0);
                    statuses.push_back(status);
                    break;
                }
            }
            p->m_map54.insert(std::make_pair(questIdx, statuses));
            continue;
        }
        if (token == "[quest parameter end]")
        {
            done = true;
            break;
        }
    }

scan_end:
    return done;
}