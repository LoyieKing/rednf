// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
#ifndef DNF_GAME_WRITELOGSWITCH_H_
#define DNF_GAME_WRITELOGSWITCH_H_

// ORIG W 0x0854ff7a _ZN14WriteLogSwitchC1Ev（ctor 唯一导出符号）
// 布局无证据（nm-only），sizeof 未证实不加断言。
class WriteLogSwitch
{
public:
    // b28d-D42 verdict=DIFF [N81] ORIG 0x0854ff7a orig/ours=15/16 ndiff=11。
    // ORIG 对 reinterpret_cast<char*>(this)[i]=0 保留 scaled 直存
    // `movb $0x0,(%edx,%eax,1)`；4.4 恒折 `lea (%edx,%eax,1),%eax; movb $0,(%eax)`
    // （N81：register 钉基址/索引、ulong 往返、视图形均无效）⇒ 语义等价封顶。
    // ORIG W 0x0854ff7a（0x2a）：for(i=0;i<=0;++i) ((char*)this)[i]=0（单字节）。
    WriteLogSwitch()
    {
        for (int i = 0; i <= 0; ++i)
            reinterpret_cast<char*>(this)[i] = 0;
    }
    char m_flag;
};

#endif  // DNF_GAME_WRITELOGSWITCH_H_
