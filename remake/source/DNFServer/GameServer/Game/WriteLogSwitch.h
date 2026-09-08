// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
#ifndef DNF_GAME_WRITELOGSWITCH_H_
#define DNF_GAME_WRITELOGSWITCH_H_

// ORIG W 0x0854ff7a _ZN14WriteLogSwitchC1Ev（ctor 唯一导出符号）
// 布局无证据（nm-only），sizeof 未证实不加断言。
class WriteLogSwitch
{
public:
    // b28d-solo-OnlyOrigBatch7_Force：ORIG W 0x0854ff7a（0x2a）为
    // for(i=0;i<=0;++i) 清零首字节。成员写成 char[1] 后 4.4 复现 ORIG 的
    // scaled 直存 `mov -0x4(%ebp),%eax; mov 0x8(%ebp),%edx;
    // movb $0x0,(%edx,%eax,1)`（P288：成员数组/数组引用产 scaled 索引；
    // 指针下标 `((char*)this)[i]` 4.4 恒折 lea 中转）⇒ 逐条同形。
    // sizeof(WriteLogSwitch)=1 / m_flag 偏移 0 / 全部 mangled 不变。
    WriteLogSwitch()
    {
        for (int i = 0; i <= 0; ++i)
            m_flag[i] = 0;
    }
    char m_flag[1];
};

#endif  // DNF_GAME_WRITELOGSWITCH_H_
