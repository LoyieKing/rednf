bool advancealtar::RewardItemType::getRewardItemType(const std::string& name,
                                                     T& out)
{
    if (name == "normal")
    {
        out = REWARD_NORMAL;
        return true;
    }
    else if (name == "event")
    {
        out = REWARD_EVENT;
        return true;
    }
    return false;
