// Ctor（ORIG W 0x081519ec）：置 vptr（编译器自动），构造 4 个 stRewardItem_t 块
// （编译器在进入函数体前自动调用 stRewardItem_t ctor），再调 reset()。
CBossPlay::CBossPlay()
{
    reset();
}
