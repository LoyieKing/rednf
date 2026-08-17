#ifndef DNF_GAME_ACTIVESTATICINFO_H_
#define DNF_GAME_ACTIVESTATICINFO_H_

// G2 占位（TODO：真实实现属怪物系统批次）。
// 尺寸 0x2f0：map_monster 内嵌成员 @ +0x38..+0x328（ctor 0x8150ec6 / dtor 0x8150ef6）。

class ActiveStaticInfo
{
public:
    ActiveStaticInfo();   // ORIG 0x8150ec6
    ~ActiveStaticInfo();  // ORIG 0x8150ef6

private:
    char m_pad[0x2f0];
};

#endif  // DNF_GAME_ACTIVESTATICINFO_H_
