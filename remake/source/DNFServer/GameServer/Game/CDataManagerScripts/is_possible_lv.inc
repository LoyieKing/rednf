// ORIG W 0x822af22 is_possible_lv：FindOneByLv(0x822aebe 存 lv) 谓词
// （0x822aecc）：min==max → lv==min；否则 min<=lv<max。
bool stSeriaBlessingScript::is_possible_lv(int lv)
{
    // ORIG 0x822af22：pred 入口构造(-0x18)；find_if 内联实参（右到左求值：
    // 0x822af47 end()→临时 -0x14 先、0x822af5c begin()→临时 -0x10 后）；
    // 外层 ne 0x822afa9 的 this=第二 end() 临时(-0xc)、rhs=find_if 结果(-0x1c)
    // ⇒ 源码方向 end() != find_if(...)。
    std::map<int, Value>::iterator found =
        std::find_if(m_map0.begin(), m_map0.end(), FindOneByLv(lv));
    return m_map0.end() != found;
}