// ---- Dimension::GetRewardItemGiveList —— T 0x0826f814（同名报告：透传）-----
int RentalMachineDimension::GetRewardItemGiveList(int job, int growType)
{
    // ORIG 0x0826f814：基址表达式 add 后落 -0xc 槽（指针局部），非直接 lea 传参。
    DimensionActivationRewardData* data =
        reinterpret_cast<DimensionActivationRewardData*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x4b34);
    return (int)data->getRewardGiveItemList(job, growType);
}