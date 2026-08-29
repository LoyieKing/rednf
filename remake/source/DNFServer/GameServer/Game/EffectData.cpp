// df_game_r 还原 —— effect_data 独立 TU（batch 迁移自 GameStubs.cpp）。
// 权威符号（ORIG df_game_r）：
//   effect_data::effect_data()            W @0x085bff3e（构造 +0x08 vector<unsigned short>）
//   effect_data::~effect_data()           W @0x085bff54（析构 +0x08 vector<unsigned short>）
//   effect_data::effect_data(effect_data const&)        W @0x085c129c
//   effect_data::operator=(effect_data const&)          W @0x085c1332
#include <vector>

#ifndef DNF_GAME_EFFECT_DATA_H_
#define DNF_GAME_EFFECT_DATA_H_

class effect_data
{
public:
    effect_data();
    effect_data(effect_data const& o);
    ~effect_data();
    effect_data& operator=(effect_data const& o);

private:
    char m_byte;             // +0x00（1 字节；ORIG 拷贝 movzbl/movdl，非数组）
    int  m_value;            // +0x04（char 后自然对齐；ORIG 拷贝 movl）
    std::vector<unsigned short> m_seq;  // +0x08
};

#endif  // DNF_GAME_EFFECT_DATA_H_

effect_data::effect_data()
{
}

effect_data::effect_data(effect_data const& o)
    : m_byte(o.m_byte), m_value(o.m_value), m_seq(o.m_seq)
{
}

effect_data::~effect_data()
{
}

effect_data& effect_data::operator=(effect_data const& o)
{
    m_byte = o.m_byte;
    m_value = o.m_value;
    m_seq = o.m_seq;
    return *this;
}
