void CMission_combo_clear::_makeTotalMissionInfo(MissionInfo& info,
                                                 const CUser& user) const
{
    combo_script_key key;
    key.m_job = (char)user.get_charac_job();
    key.m_growType = (short)user.getCurCharFirstGrowType();

    std::map<combo_script_key, combo_script_data>::const_iterator it =
        m_comboMap.find(key);
    // [b28c-35] ORIG 0x085e441b 为 `test %al,%al; je <函数尾>`（正向包裹：it!=end
    // 时落 body，假则直跳尾）；早返 `if (it==end) return;` 产反向 `jne` 布局（P59）。
    // [b28c-35] ORIG 0x085e4414 的 operator!= 实参序为 `(&it, &end)`（0x4(%esp)=&it）；
    // 写 `it != end` 时 4.4 先求右操作数 ⇒ `(&end, &it)` 反序。交换操作数即对齐。
    if (m_comboMap.end() != it)
    {
        int total = (int)it->second.m_rewardList.size();
        int i = 0;
        while ((i < 4) && (i < total))
        {
            info.m_count = (short)((int)info.m_count << 4);
            info.m_count = (short)(info.m_count +
                                   (it->second.m_rewardList[i].second & 0xf));
            i++;
        }
        if (total > 4)
        {
            for (i = 4; i < 8; i++)
            {
                info.m_streak = (short)((int)info.m_streak << 4);
                if (i < total)
                {
                    info.m_streak = (short)(
                        info.m_streak +
                        (it->second.m_rewardList[i].second & 0xf));
                }
            }
        }
    }
