bool CQuickPartyRewardManager::init()
{
    if (!importQuickPartyScript("Etc/QuickParty.etc", this))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x92, 0);
        trace("QUICK_PARTY_LOG : QuickParty.etc FILE LOAD FAIL!!");
        return false;
    }

    m_rewards[0][11] = new RewardCardGold(BASIC_REWARD_CARD_GOLD, *this);          // +0x1ec
    m_rewards[0][12] = new HellPartyDifficulty(BASIC_REWARD_HELL_DIFFICULTY, *this);  // +0x1f0
    m_rewards[1][0]  = new KillMonsterExp(RANDOM_BUFF_INCREASE_EXP, *this);        // +0x1f4
    m_rewards[1][3]  = new EquipItemDropRate(RANDOM_BUFF_INCREASE_DROPPROB, *this);  // +0x200
    m_rewards[1][9]  = new CointLimitCount(RANDOM_BUFF_INCREASE_COINLIMIT, *this);   // +0x218
    m_rewards[1][10] = new ReturnAncientDungeonTicket(RANDOM_BUFF_RETURN_TICKET, *this);  // +0x21c

    set_rand_buff_total_prob();
    return true;
