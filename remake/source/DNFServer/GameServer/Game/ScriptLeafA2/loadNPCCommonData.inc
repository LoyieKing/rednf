// ============================================================================
// loadNPCCommonData —— ORIG T 0x089fafc5
// 报告 _lt_global_gt_/loadNPCCommonData.md 全文转写。
// 返回：0x3c=脚本打开失败；其余路径一律 0（解析失败与干净 EOF 同值，ORIG 如此）。
// ============================================================================
int loadNPCCommonData(stNPCCommonData_t& out, const char* path)
{
    // [b28d-solo-ScriptLeafA2] 本件修复：
    //   ① CDataManager.h stOneBuff_t::m_isNotPlus int→bool（P248②/P32）复现
    //      ORIG 0x089fb390/0x089fb396 的 `movb $0/$1,-0x50(%ebp)`；
    //   ② 同结构补 +0x09..+0x0b 显式 pad，使 new_allocator::construct 对 +0x08
    //      恢复 ORIG 0x8a01d24 的 dword 块拷贝（bool 单独拷贝退化为 movzbl+movb）；
    //   ③ make_pair 实例改右值版（P61/P81 + P234 static_cast<T&&>）复现 ORIG
    //      0x089fb40d 的 make_pair<int,vector>(int&&,vector&&)。
    //   p167：MISSING 0（CLEAR）。残余 EXTRA = stOneBuff_t::~stOneBuff_t() ×2
    //   （4.4 对含非平凡析构的局部对象恒布 EH cleanup，ORIG 4.1 消除空体析构调用，
    //   N91 族跨版本 EH 块；头内空体实验无效已回退）⇒ N 封顶，语义等价。
    if (!loadRDARScriptFile(g_npcScriptBaseDirectory, path))
    {
        return 0x3c;
    }

    std::string token;
    int idx = 0;
    bool ok = false;

    out.clear();

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[npc favor exp]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorExp[idx] = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
                out.m_maxFavorExp =
                    std::max(out.m_maxFavorExp, out.m_favorExp[idx]);
            }
        }
        else if (token == "[favorable npc list]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorableNpcList.push_back(idx);
            }
        }
        else if (token == "[illustration variable npc list]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_illustrationVariableNpcList.push_back(idx);
            }
        }
        else if (token == "[max favor level npc]")
        {
            out.m_maxFavorLevelNpc = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor system begin quest]")
        {
            out.m_favorSystemBeginQuest = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor system begin level]")
        {
            out.m_favorSystemBeginLevel = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor level reward npc]")
        {
            for (;;)
            {
                int npcId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorLevelRewardNpcList.push_back(npcId);
            }
        }
        else if (token == "[favor duration buff list]")
        {
            for (;;)
            {
                std::vector<stNPCCommonData_t::stOneBuff_t> buffs;
                // [本波实测] 槽序反转律在本块**无效**：把 `int first;` 提前到
                // buffs 之前重编后，first 仍落 -0x1c（ORIG -0x48），帧/计数零变化
                // ⇒ 块内标量+类对象混合区归 N98（4.4 按首次使用序）域，声明序不可翻。
                int first = ScanInt(&ok);
                if (!ok)
                {
                    break;                          // 回主循环（buffs 析构）
                }
                int count = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                for (int i = 0; i < count; ++i)
                {
                    std::string s;
                    if (!ScanStr(&s))
                    {
                        return 0;
                    }
                    int discardedType = getChangeStatusType(s);   // ORIG 存局部后弃用
                    int value = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    stNPCCommonData_t::stOneBuff_t buff;
                    buff.m_changeStatusType = getChangeStatusType(s);   // 第二次入元素
                    buff.m_value = value;
                    if (!ScanStr(&s))
                    {
                        return 0;
                    }
                    if (s == "+")
                    {
                        buff.m_isNotPlus = 0;
                    }
                    else
                    {
                        buff.m_isNotPlus = 1;
                    }
                    buffs.push_back(buff);
                }
                // [b28d-solo-ScriptLeafA2] ORIG 0x089fb40d 调
                // make_pair<int, vector<stOneBuff_t> >(int&&, vector&&)（p167 MISSING
                // 左值引用版 `<int&,vector&>`）；按 P61/P81 实参左值性决定实例名，
                // 用 P234 的 static_cast<T&&>（纯语言构造、零 call）传右值即复现。
                out.m_favorDurationBuffList.push_back(
                    std::make_pair(
                        static_cast<int>(first),
                        static_cast<
                            std::vector<stNPCCommonData_t::stOneBuff_t>&&>(buffs)));
            }
        }
    }
    return 0;
}