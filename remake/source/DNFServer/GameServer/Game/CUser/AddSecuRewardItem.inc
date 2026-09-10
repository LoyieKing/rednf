void CUser::AddSecuRewardItem(unsigned char rewardType)
{
    // ORIG 0x868832b sub $0xa4 + 0x8688334 mov %al,-0xa4(%ebp)（4.1 把 arg→local 复制折叠为单条）。
    // 4.4 双槽（probe17）；big[0x9c] 垫 0xa4 帧。local 初始化多余——删，仅留 arg  spill。
    char big[0x9c]; (void)big;
    (void)rewardType;
