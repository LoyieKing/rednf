// ============================================================================
// cUserHistoryLog::SkillSave @ 0x08685dce
//   SkillSave(_Mastered_skill const*, unsigned int, int, eSkillSaveReason)
//   收集非空技能（跳过 0x8a..0x95 保留区），按每 15 个一批写入
//   "SaveLethe,%d, %d/%d/%d<,或/>%d/%d…" 的 Lethe 保存日志。
// ============================================================================
// [b28d-solo-cUserHistoryLog] DIFF 257/260：修真 bug——buf 清零 0x100→整块 0x400 字节
// （ORIG rep stos ecx=0x100 dwords，P116/P253）、chunk 0x80→0x200；make_pair 去 (int) cast
// 以匹配 ORIG 实例 make_pair<uchar const&,char const&>（0x0869c165）；lineCount/end 预清（P198①）。
// 残差 N78（tmp memset 恒 call）+ N107（&tmp[0x10] lea+add）+ 字符串地址 AE 伪差。
void cUserHistoryLog::SkillSave(const _Mastered_skill* skills, unsigned int a, int b,
                                eSkillSaveReason reason)
{
    // ORIG 0x08685e02 段首 `movl $0xf,-0x44(%ebp)`（具名 15 局部，P0 槽往返）。
    int perLine = 15;
    char header[16];
    sprintf(header, ",%d, ", (int)reason);

    // ORIG 0x08685dfb 段：lineCount(-0x40) 预清先于 vec 构造（P198① 预清两句式）。
    int lineCount = 0;
    std::vector<std::pair<int, int> > vec;
    int count = 0;
    for (int i = 0; i < 0xcc; i++)
    {
        if (skills[i].m_index != 0 && (i < 0x8a || 0x95 < i))
        {
            // ORIG 0x0869c165 实例 = make_pair<unsigned char const&, char const&>
            // （C++0x 完美转发重载，实参为无 cast 的左值）；(int) cast 会选错实例。
            vec.push_back(std::make_pair(skills[i].m_index, skills[i].m_level));
            count++;
        }
    }

    lineCount = (int)((float)count / 15.0f) + 1;
    for (int line = 0; line < lineCount; line++)
    {
        // ORIG 0x08685ef2 段：rep stos ecx=0x100 dwords = 0x400 字节（整块 buf 清零，P116/P253）。
        // 旧 memset(buf,0,0x100) 只清 0x100 字节（少清 0x300）——真语义缺口。
        char buf[1024] = {0};
        memcpy(buf, "SaveLethe", 10);
        strcat(buf, header);

        int start = line * 0xf;
        // ORIG 0x08685f48 段：`movl $0x0,-0x2c`（end 预清 0，P198① 两句式）。
        int end = 0;
        if (line < lineCount - 1)
        {
            end = start + 0xe;
        }
        else
        {
            end = (count % 0xf) - 1 + start;
        }
        int n = end - start + 1;
        if (n == 0)
            break;

        // [b28d-solo-cUserHistoryLog 2026-09-09 翻案] 原 `char tmp[40]` 两处残差：
        // ①`&tmp[0x10]` 4.4 恒发 lea -0x94 + add $0x10（ORIG 单条 lea -0x84）；
        // ②循环内 memset 恒发 call（ORIG 内联 4 条 movl disp）。
        // 拆成「先声明 char num[24]（浅槽 -0x84，ORIG tmp+0x10 区）+ 后声明 16 字节
        // 结构体 pre（深槽 -0x94，4.4 后声明占深槽且与 num 紧邻——探针
        // /tmp/hlprobe/y8.cpp y8 逐条同形）」：num 数组名退化 = 单条 lea -0x84；
        // pre.m_zero[i]=0 = 4 条 movl disp；sprintf((char*)&pre,…) = 单条 lea -0x94。
        // union 双视角：m_chars 供 sprintf/strcat 数组名退化（单条 lea，探针 y10），
        // m_zero 供 4 条 disp movl 清零（探针 y8）。
        union SkillSavePre { char m_chars[16]; unsigned int m_zero[4]; };
        char num[24];          // ORIG tmp+0x10（-0x84）区
        SkillSavePre pre;      // ORIG tmp+0x00（-0x94）区
        // ORIG 循环外无 tmp 清零（sprintf 自写终止符）；清零只在 k 循环内（4.1 内联 4 条 movl）。
        sprintf(num, "%d/%d/%d", a, n, b);
        strcat(buf, num);

        // ORIG 0x08685ff3 段：rep stos ecx=0x80 dwords = 0x200 字节（整块 chunk 清零）。
        char chunk[512] = {0};
        for (int k = 0; k < n; k++)
        {
            int idx = vec[start + k].first;
            int lvl = vec[start + k].second;
            // ORIG 0x08686009 段：4 条 `movl $0,-0x94/-0x90/-0x8c/-0x88(%ebp)`（disp 形）——
            // 结构体成员赋值复现（探针 y8；memset 恒 call，指针 cast 写恒 lea+add）。
            pre.m_zero[0] = 0;
            pre.m_zero[1] = 0;
            pre.m_zero[2] = 0;
            pre.m_zero[3] = 0;
            if (k % 3 == 0)
                sprintf(pre.m_chars, ",%d/%d", idx, lvl);
            else
                sprintf(pre.m_chars, "/%d/%d", idx, lvl);
            strcat(chunk, pre.m_chars);
        }
        strcat(buf, chunk);

        m_user->LogHistory(buf);
    }
}