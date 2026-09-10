bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
                         int type, bool flag)
{
    type = 3;
    bool subMatched = false;
    moduleId = -1;
    subId = -1;
    int i = 0;
    for (; i <= 0x10; ++i)
    {
        if (*tag == g_moduleNameTag[i][0] ||
            *tag == g_moduleNameTag[i][1])
        {
            moduleId = i;
            subMatched = (*tag == g_moduleNameTag[i][1]);
            if (i != type && type <= 0x10)
            {
                // 操作数序 [i] 左 [type] 右：4.4 二元比较 RHS 先求值 ⇒
                // type 槽先装载，与 ORIG 6f5-6fa（mov KE;shl3;lea/mov SE;shl3）
                // 行序一致。
                if (g_moduleNameTag[i][0] == g_moduleNameTag[type][0])
                    moduleId = type;
            }
            break;
        }
    }

    if (moduleId != -1)
    {
        if (moduleId == type)
        {
            if (!subMatched)
                subId = type;
        }
        else if (g_moduleNameTag[moduleId][0] == "[dungeon]")
        {
            if (!subMatched)
            {
                subId = 3;
                LogManager::logFormat(0, "../RDARScriptSkill.cpp",
                                      __FUNCTION__, 0xc0,
                                      "skillscript separating : \xbc\xba\xb0\xf8 : \xb1\xe2\xba\xbb\xb0\xaa \xb8\xf0\xb5\xe2\xc5\xc2\xb1\xd7\xc0\xcc\xb9\xc7\xb7\xce \xc1\xf8\xc7\xe0 tag %s",
                                      tag->c_str());
            }
        }
        else if (g_moduleNameTag[moduleId][0] == "[pvp]")
        {
            // ORIG 8a97810..8a9782a：`cmpl $9;je;cmpl $5;jne;mov $1,%eax;jmp;
            // mov $0,%eax;test %al,%al;je` —— 4.1 -O0 对 `||` 条件先**物化 bool 到
            // %al 再测试**（无栈槽，P0 ⇒ 原码无具名局部）。4.4 两形实测：
            // ①具名 bool（本形）恒落槽 `mov %al,-0xf;cmpb $0,-0xf`（214 条，gap +1）；
            // ②P226 语句表达式 `({type==9||type==5;})` 被前端整折为
            //   `cmpl $9;je;cmpl $5;jne`（208 条，gap −5，无物化段）。
            // 无第三形可复现 4.1 的「物化但零槽」（P0：非 call 结果的纯比较在 4.4
            // -O0 恒折直跳）⇒ N（跨版本 if 条件 bool 物化；与 setcc-fold 同族）。
            bool hit = (type == 9 || type == 5);
            if (hit)
                subId = 4;
        }
        else
        {
            bool foundSub = false;
            bool matched;
            for (;;)
            {
                ScanType(*tag, false);
                matched = false;
                for (int j = 0; j <= 0x10; ++j)
                {
                    if (*tag == g_moduleNameTag[moduleId][1])
                    {
                        foundSub = true;
                        matched = true;
                        break;
                    }
                }
                if (matched)
                    break;
            }
            if (!foundSub)
            {
                LogManager::logFormat(0, "../RDARScriptSkill.cpp",
                                      __FUNCTION__, 0xf3,
                                      "ERROR : skillscript separating : \xbf\xa1\xb7\xaf : \xc0\xdf\xb8\xf8\xb5\xc8 \xb8\xf0\xb5\xe2\xb1\xb8\xba\xd0\xc5\xc2\xb1\xd7 \xbb\xe7\xbf\xeb %s",
                                      tag->c_str());
            }
            return 0;
        }
    }

    if (subMatched)
        return 0;
    if (!flag && moduleId != -1)
        ScanType(*tag, false);
    return 1;
