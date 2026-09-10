int CDataManager::getClearTimeBonusPoint(int avgTime, int clearTime)
{
    // ORIG 0x08363ac8：result(-0x10)/diff(-0xc) 双双入口零初始化；早退
    // `return result`（0x08363ae8 mov -0x10 而非立即 0）；cur(-0x20)/
    // prev(-0x28) 两个带 ctor 的结构本地（0x08363af0/af6）；vector 走非
    // const begin/end/==/operator->/postfix ++（无 const 实例⇒const_cast
    // 内联形，0x08363b06 二段装载 N18a）；命中 `cur.m_a > diff` 时
    // it==begin() → result=0 走 -0x10 物化（0x08363b9f..0xa6）。
    int result = 0;
    int diff = 0;
    if (avgTime == 0 || clearTime == 0)
        return result;

    typedef std::vector<stBonusPointValues> Vec;
    // [b28d-solo-CDataManager] ORIG 0x08363af0：先构造 cur(-0x20) 再 prev(-0x28)
    // ⇒ 声明序 cur → prev；diff 赋值在两次 ctor 之后（ORIG 0x08363b06）。
    stBonusPointValues cur;
    stBonusPointValues prev;
    diff = avgTime - clearTime;
    // 指针形 const_cast：4.4 对引用 cast 结果会物化临时槽（把 cur 起整体
    // 平移 4B），指针形直传 this 与 ORIG `lea 0x5120(%eax)` 一致。
    // end 缓存到 e（ORIG 0x08363b30 循环外一次 end → -0x30，循环条件只 ne(it,e)；
    // 旧形每次迭代重调 end() 使 it/end 槽位与 ORIG 相反，P13）。
    for (Vec::iterator it =
             const_cast<Vec*>(&m_rankSystem.m_dungeonRank.m_vec18)->begin(),
         e = const_cast<Vec*>(&m_rankSystem.m_dungeonRank.m_vec18)->end();
         it != e;
         it++)
    {
        cur = *it;
        if (cur.m_a > diff)
        {
            if (it ==
                const_cast<Vec*>(&m_rankSystem.m_dungeonRank.m_vec18)->begin())
            {
                result = 0;
                return result;
            }
            result = prev.m_b;
            return result;
        }
        prev = cur;
    }
    result = prev.m_b;
    return result;
