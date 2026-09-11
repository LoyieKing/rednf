// ORIG _ZN5CUser17AddSecuRewardItemEh T 0x08688328（0x14 字节）：仅参数存
// 未使用局部（ORIG 即空操作）。
void CUser::AddSecuRewardItem(unsigned char rewardType)
{
    // ORIG 0x868832b sub $0xa4 + 0x8688334 mov %al,-0xa4(%ebp)（4.1 把 arg→local 复制折叠为单条）。
    // 4.4 双槽（probe17）；big[0x9c] 垫 0xa4 帧。local 初始化多余——删，仅留 arg  spill。
    char big[0x9c]; (void)big;
    (void)rewardType;
    // P139 prett 缺口（P5 父泳道点名）：ORIG .rodata 有
    //   `_ZZN5CUser17AddSecuRewardItemEhE19__PRETTY_FUNCTION__` @0x8cf2660 size=0x2d
    //   （串 "void CUser::AddSecuRewardItem(unsigned char)"，逐字节同）；
    //   全库 ORIG 反汇编 0 处引用它（DEAD 串），但 4.1 为该函数发射了它 ⇒ 我方
    //   也须发射以对齐符号形态。sizeof 是编译期量，**零指令/零行为影响**（/tmp/P5/p_prett
    //   探针：加了 sizeof 后函数仍是 0x14 字节逐条同，且 emit 该 prett 符号）。
    (void)sizeof(__PRETTY_FUNCTION__);
}