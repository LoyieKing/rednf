// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// nm 仅 C1 与 clear；无 dtor 符号（trivial 析构），sizeof 未证实不加断言。
#ifndef DNF_GAME_STBUFF_H_
#define DNF_GAME_STBUFF_H_

class STBuff
{
public:
    STBuff();      // ORIG W 0x089c3194
    void clear();  // ORIG W 0x089c31a8
};

#endif  // DNF_GAME_STBUFF_H_
