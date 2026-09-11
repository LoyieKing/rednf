// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d1ebe orig/ours=54/45 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    0x4d44(%eax),%edx | lea    -0x10(%ebp),%eax | lea    -0x10(%ebp),%eax
bool Dispatcher_ChangeEmotion::isVaildEmotionValue(short value)
{
    // [FL1 2026-09-10] ORIG 0x081d1ebe 直证（报告全文已读）：
    //   ① two 独立 `call _Z14G_CDataManagerv`（+0x0d / +0x3c），无 manager 缓存；
    //   ② begin()/end() 各一次（非 const 版 _ZNSt6vectorIiSaIiEE5beginEv/3endEv）；
    //   ③ 两具名 const_iterator **声明即初始化**（-0x14 / -0x18），各含一次
    //      `C1IPiE` 转换构造（0x080f99ec ×2），无默认构造、无 operator=；
    //   ④ 循环条件在**底部**（序言末 `jmp +0x99` 直落条件块，P283 底条件布局），
    //      比较走 out-of-line `_ZNK9__gnu_cxxne...`，自增 `ppEv`。
    std::vector<int>::const_iterator it =
        G_CDataManager()->m_emotionTags.m_tags.begin();
    std::vector<int>::const_iterator itEnd =
        G_CDataManager()->m_emotionTags.m_tags.end();
    while (it != itEnd)
    {
        if (*it == (int)value)
            return 1;
        ++it;
    }
    return 0;
}