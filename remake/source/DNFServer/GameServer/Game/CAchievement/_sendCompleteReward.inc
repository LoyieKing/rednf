void CAchievement::_sendCompleteReward(const Quest* quest,
                                       ENUM_TITLE_BOOK_CATEGORY category,
                                       int slot)
{
    // b28d-solo P218：ORIG 0x0828bf3x 先 `mov -0x10(%ebp),%ebx`（索引先求值）
    // 再 call G_CDataManager ⇒ 下标局部 int 化触发 size_t 转换中转（RHS 值先）。
    int sectionIdx = (int)_getSectionIdx(category, slot);
    if (sectionIdx == -1)
    {
        return;
    }
    // ORIG 0x828bf54：stTitleSection 复制构造到局部（非引用）
    stTitleSection section =
        GetTitleSections(G_CDataManager())[sectionIdx];
    stTitleRewardBasis* reward = _getPostalReward(&section);
    if (reward == 0)
    {
        return;
    }
    // 邮件字段位于 stRewardPostal（+0x0c..+0x1c）；基类仅承载 type 判定。
    // b28d-solo P0：ORIG 0x0828c0xx 无 postal 具名槽（每次 `mov -0xc(%ebp),%eax`
    // 重读 reward 后单 disp 取字段），删缓存局部以消多余 store 与 4B 帧。
    if (_getTabComplete(&section, category))
    {
        _sendPostal(static_cast<stRewardPostal*>(reward)->m_itemIdx,
                    static_cast<stRewardPostal*>(reward)->m_addInfo,
                    static_cast<stRewardPostal*>(reward)->m_sender,
                    static_cast<stRewardPostal*>(reward)->m_subject);
    }
