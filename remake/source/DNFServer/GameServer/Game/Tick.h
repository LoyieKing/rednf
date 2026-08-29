#ifndef DNF_GAME_TICK_H_
#define DNF_GAME_TICK_H_

// ============================================================================
// Tick —— 毫秒时钟值包装（uint32 内部值）。
// ORIG 权威证据（df_game_r）：
//   C1Ev        T @0x08ade828  value_=0
//   C1Ej        T @0x08ade836  value_=v
//   setNow      T @0x08ade8b6  *this = now()
//   now()       T @0x08ade844  返回 Tick(nowAsInternalValue())（sret）
//   nowAsInternalValue T @0x08ade86a
//     clock_gettime(CLOCK_MONOTONIC=1,&ts); return ts.tv_sec*1000 + ts.tv_nsec/1000000;
//   getInternalValue const T @0x08ade8c8 → value_
//   setInternalValue(v)    T @0x08ade8d2 → value_=v
//   operator= (aSES_)      T @0x08ade8e0 → value_=rhs.value_，返回 *this
//   operator+=/-=(int)     T @0x08ade8f0/@0x08ade908 → value_ ± span，返回 this
//   operator+(int)         T @0x08ade920 sret  Tick(value_+span)
//   operator-(int)         T @0x08ade94c sret  Tick(value_-span)
//   operator-(Tick) 值参   T @0x08ade97a 返回 int32 差值 this->value_-rhs.value_
//   比较运算符均按值传参（eq/ne/lt/le/gt/ge ES_ @0x8ade98e..0x8ade9f8）
// 布局：sizeof(Tick)=4，唯一成员 unsigned int value_（DWARF: value_）。
// ============================================================================

class Tick
{
public:
    Tick();                                     // _ZN4TickC1Ev
    explicit Tick(unsigned int v);

    void setNow();                              // _ZN4Tick6setNowEv
    static Tick now();                          // _ZN4Tick3nowEv
    static unsigned int nowAsInternalValue();   // _ZN4Tick18nowAsInternalValueEv

    void setInternalValue(unsigned int v);      // _ZN4Tick16setInternalValueEj
    unsigned int getInternalValue() const;      // _ZNK4Tick16getInternalValueEv

    Tick& operator=(Tick rhs);                  // _ZN4TickaSES_（ORIG 按值参）
    Tick& operator+=(int span);                 // _ZN4TickpLEi
    Tick& operator-=(int span);                 // _ZN4TickmIEi
    Tick operator+(int span) const;             // _ZNK4TickplEi（sret）
    Tick operator-(int span) const;             // _ZNK4TickmiEi（sret）
    int  operator-(const Tick rhs) const;       // _ZNK4TickmiES_（ORIG 返回差值 int32）

    bool operator==(const Tick rhs) const;      // _ZNK4TickeqES_
    bool operator!=(const Tick rhs) const;      // _ZNK4TickneES_
    // 比较（ORIG = 有符号差值比较：lt=(a-b)>>31, ge=~(a-b)>>31,
    // le/gt=test+(setle/setg)）
    bool operator< (const Tick rhs) const;      // _ZNK4TickltES_
    bool operator<=(const Tick rhs) const;      // _ZNK4TickleES_
    bool operator> (const Tick rhs) const;      // _ZNK4TickgtES_
    bool operator>=(const Tick rhs) const;      // _ZNK4TickgeES_

private:
    unsigned int m_value;
};

#endif  // DNF_GAME_TICK_H_
