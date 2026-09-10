void CUser::GiveMeBoxReward()
{
    DBInsertGiveBoxEventReward::makeRequest(this);
