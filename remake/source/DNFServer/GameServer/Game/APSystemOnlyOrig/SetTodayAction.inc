void CActionPointManager::SetTodayAction(int level, unsigned int characNo,
                                         ENUM_EXPERT_JOB_TYPE jobType,
                                         const std::map<unsigned short,
                                                        std::vector<CActionPointEx> >& table)
{
    Reset();
    m_sig.m_characNo = characNo;

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmc;
    localtime_r(&now, &tmc);
    int weekday = tmc.tm_wday;

    std::map<unsigned short, std::vector<CActionPointEx> >::const_iterator it =
        table.begin();
    int row = 0;
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x0812167a..0x08121686：init 后
    // 无条件 `jmp` 到底部条件块（4.1 for 循环旋转布局）⇒ 源码必须写成
    // 标准 for(; cond; it++) 形（body 尾 ++row、cond 失败后 m_flag00=1），
    // 而非 `for(;;){ bool cond; ...; if(!cond) return; }`（4.4 条件置顶）。
    for (; row < 300 && it != table.end(); it++)
    {
        const std::vector<CActionPointEx>* list = &it->second;
        int total = static_cast<int>(list->size());
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x081216a5..0x08121a8f：`je 0x475`
        // 落到 nop 岛、then 块尾 `jmp 0x476` 跳过岛（4.1 空 else 占位）⇒
        // P223 复现形 `if (total==0) goto after_body; { body } after_body:;`
        // （4.4 探针 /tmp/ap_probe.cpp fA 精确产 `je <nop>; body; jmp; nop`）。
        if (total == 0) goto after_body;
        {
            bool useWeekday = false;
            bool useLevelBand = false;
            if ((*list)[0].m_weekday != 7)
            {
                useWeekday = true;
            }
            if ((*list)[0].m_record.m_levelMin != 0)
            {
                useLevelBand = true;
            }
            std::vector<const CActionPointEx*> candidates;
            for (unsigned int pass = 0; static_cast<int>(pass) < total; ++pass)
            {
                candidates.push_back(&(*list)[pass]);
            }
            int remain = 0;
            for (unsigned int i = 0; static_cast<int>(i) < total; ++i)
            {
                // [b28d-solo-APSystemOnlyOrig] ORIG 0x08121772..0x0812198e：
                // 空 then 占位 nop 岛（`jne 0x371`→nop→`jmp 0x375`）⇒ P223
                // fH 形（探针 /tmp/ap_probe2.cpp fH 产 `je L; body; jmp L+1;
                // L: nop`）。
                if (candidates[i] == 0) goto next_candidate;
                {
                    if (useWeekday)
                    {
                        if (_gmWeekday.m_gmOn != 0)
                        {
                            weekday = _gmWeekday.m_weekday;
                        }
                        if (candidates[i]->m_weekday != weekday)
                        {
                            candidates[i] = 0;
                            goto next_candidate;
                        }
                    }
                    if (useLevelBand)
                    {
                        // [b28d-solo-APSystemOnlyOrig] ORIG 0x081217e0..0x0812181a：
                        // 两次独立 `(*list)[i]` 直读（P183 地址重物化），lo/hi
                        // **无栈槽**（P0）⇒ 表达式内联，勿声明具名局部。
                        if (level < static_cast<int>(
                                        (*list)[i].m_record.m_levelMin) ||
                            static_cast<int>(
                                (*list)[i].m_record.m_levelMax) < level)
                        {
                            candidates[i] = 0;
                            goto next_candidate;
                        }
                    }
                    {
                        int group = candidates[i]->m_record.m_groupIndex;    // +0x02
                        if (group == 0x1d || group == 0x1e || group == 0x1f ||
                            group == 0x2d || group == 0x20)
                        {
                            if (group == 0x1d && jobType != ENUM_EXPERT_JOB_TYPE_2)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x1e && jobType != ENUM_EXPERT_JOB_TYPE_4)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x1f && jobType != ENUM_EXPERT_JOB_TYPE_1)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x20 && jobType != ENUM_EXPERT_JOB_TYPE_3)
                            {
                                candidates[i] = 0;
                            }
                            else if (group == 0x2d && jobType != ENUM_EXPERT_JOB_TYPE_3)
                            {
                                candidates[i] = 0;
                            }
                            if (candidates[i] == 0)
                            {
                                goto next_candidate;
                            }
                        }
                        if (group == 0x22 && jobType != ENUM_EXPERT_JOB_TYPE_0)
                        {
                            candidates[i] = 0;
                        }
                        else
                        {
                            ++remain;
                        }
                    }
                }
            next_candidate:;
            }

            if (remain == 0)
            {
                --row;
            }
            else
            {
                int pick = DNFFLib::get_rand_int(remain);
                int seen = 0;
                const CActionPointEx* chosen = 0;
                for (unsigned int k = 0; static_cast<int>(k) < total; ++k)
                {
                    // [b28d-solo-APSystemOnlyOrig] ORIG 0x081219ec..0x08121a1e：
                    // 同 fH 形（`sete; jne <nop 岛>; then; jmp; ++seen`）。
                    if (candidates[k] == 0) goto next_k;
                    {
                        if (seen == pick)
                        {
                            chosen = candidates[k];
                            break;
                        }
                        else
                        {
                            ++seen;
                        }
                    }
                next_k:;
                }
                if (chosen == 0)
                {
                    --row;
                }
                else
                {
                    memcpy(&m_sig.m_actionPoints[row],
                           reinterpret_cast<const void*>(chosen),
                           sizeof(m_sig.m_actionPoints[row]));
                }
            }
        }
    after_body:;
        ++row;
    }
    m_flag00 = 1;
