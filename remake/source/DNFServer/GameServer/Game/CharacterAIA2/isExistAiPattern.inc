// ============================================================================
// isExistAiPattern —— ORIG T 0x08a2232f
// 报告 _lt_global_gt_/isExistAiPattern.md 完整转写。
// bucket==-1 遍历 5 个难度桶；否则只查指定桶；任一非空返 1，否则 0。
bool isExistAiPattern(STAIInfoScript* info, int bucket)   // ORIG 调用侧 test %al 直判（P9/P22）
{
    if (info != 0)
    {
        if (bucket != -1)
        {
            if (!info->m_patterns[bucket].empty())
                return 1;
        }
        else
        {
            for (int i = 0; i < 5; ++i)
            {
                if (!info->m_patterns[i].empty())
                    return 1;
            }
        }
    }
    return 0;
}