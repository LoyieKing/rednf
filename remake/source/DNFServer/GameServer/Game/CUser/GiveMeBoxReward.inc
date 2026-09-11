// ORIG _ZN5CUser15GiveMeBoxRewardEv T 0x08693758：单行转发 DB 请求。
void CUser::GiveMeBoxReward()
{
    DBInsertGiveBoxEventReward::makeRequest(this);
}