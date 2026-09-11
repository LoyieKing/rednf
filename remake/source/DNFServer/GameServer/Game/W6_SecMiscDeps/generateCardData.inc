char* WongWork::CSecurityCardCenter::generateCardData(char* out) const
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P30 add-vs-lea）；42→7]
    // ORIG 0x085fe1ba：帧 0x40。
    // 源码形态按 ORIG 反汇编还原：①`movl $0x26,-0x18` 死局部（写后从未读，
    // P171/P232 照抄）；②`for (pos=0; pos<=0x25; ++pos)`（尾测 setbe 物化 +
    // jmp 到条件块；P310 翻案后 int 局部 + 显式 unsigned 比较已逐条复现）；
    // ③begin/end 各求值一次后缓存（P13，`it != end` 用缓存槽
    // -0x28/-0x2c）；④seen 声明先于 pick（槽 -0x10/-0xc）。
    // ORIG T 0x085fe1ba (size 0x128)：拷贝 m_characterSet 至临时 list；循环
    // （pos<=0x25 继续）：以 list.size() 为上界随机取槽，顺序迭代到第 pick 个
    // 元素写出并 erase（每字符只使用一次）。
    std::list<unsigned char> pool(m_characterSet);
    int unusedLimit = 0x26;   // ORIG -0x18：写后从未读（死局部，照抄）
    (void)unusedLimit;
    // [P310 翻案] ORIG 0x085fe29c 尾测两段寄存器形（`mov -0x14(%ebp),%eax;
    // cmp $0x25,%eax; setbe`）⇒ int 局部 + 显式 unsigned 比较复现
    //（同 init/checkPhoneNumber 三件同形）；残余仅 0x085fe242 `add 0xc(%ebp),%ebx`
    // vs 4.4 lea（P30/N4 封顶）。
    for (int pos = 0; (unsigned int)pos <= 0x25; ++pos)
    {
        int seen = 0;
        const int pick = get_rand_int((int)pool.size());
        std::list<unsigned char>::iterator it = pool.begin();
        std::list<unsigned char>::iterator end = pool.end();
        while (it != end)
        {
            if (seen == pick)
            {
                out[pos] = static_cast<char>(*it);
                pool.erase(it);
                break;
            }
            ++seen;
            ++it;
        }
    }
    return out;
}