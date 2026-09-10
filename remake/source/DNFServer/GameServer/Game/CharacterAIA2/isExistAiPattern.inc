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
