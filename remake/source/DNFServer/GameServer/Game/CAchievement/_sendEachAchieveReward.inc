void CAchievement::_sendEachAchieveReward(const Quest* quest)
{
    // [b28d-solo-CAchievement AE] 修法：early-return 改正向包裹（P257）⇒ 消 4.4
    // EH 尾块多出的 jmp+nop（早返形尾部恒 +jmp+nop）。ORIG 0x0828c086 单 disp
    // `mov 0xa4(%eax),%eax` 直访 Quest::m_postalReward 具名成员。
    if (quest->m_postalReward.m_itemIdx != 0)
    {
        _sendPostal(quest->m_postalReward.m_itemIdx,
                    quest->m_postalReward.m_addInfo,
                    quest->m_postalReward.m_title,
                    quest->m_postalReward.m_contents);
    }
