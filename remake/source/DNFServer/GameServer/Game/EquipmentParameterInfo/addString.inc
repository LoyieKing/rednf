int EquipmentParameterInfo::addString(std::string str)
{
    // [b28c-6] N（跨版本 bool 物化）：内层 while(b) 条件 ORIG 4.1 直发
    // `cmpb $0,-0xd; jne`，4.4 恒插 `setne %al; test %al,%al`（char/bool/
    // uchar/显式比较四形均实测不可去，P168a③b 族）。ndiff 2 封顶。
    int key = -1;
    // [b28d-solo-EquipmentParameterInfo] ORIG 两处 `cmpb $0,-0xd; jne` 均无 setne：
    // 4.4 只对 **bool** 局部在 while 条件直判（char/uchar/int 恒插 `setne;test`，
    // 探针 /tmp/b28d_epi/p6.cpp w1..w6），而 bool 的 `!b` 又恒产
    // `movzbl;xor $1;test;je`（p7.cpp w9/w11/w12）⇒ 外层改用 `if (b) {} else {…}`
    // （p8.cpp w19：`cmpb $0; jne` 直判 + body 直落，与 ORIG 外层同形）。
    bool b = false;
    if (b)
    {
    }
    else
    {
        std::map<int, std::string>::iterator it = m_descMap1c4.begin();
        std::map<int, std::string>::iterator e = m_descMap1c4.end();
        while (it != e && (b ^= 1))
        {
            // ORIG 0x89c2eeb-0x89c2f22：p=*it 在内层循环外取一次（89c2eeb），
            // 随后 `jmp` 进入 while(b) 条件块（89c2f22 在 body 之后 = 回边形）；
            // 命中 break 至 it++，未命中 b=0 退出。
            std::pair<const int, std::string>& p = *it;
            while (b)
            {
                if (p.second == str)
                {
                    key = p.first;
                    break;
                }
                else
                {
                    b = false;
                }
            }
            it++;
        }
    }
    if (key == -1)
    {
        key = (int)m_descMap1c4.size() + 1;
        m_descMap1c4[key] = str;
    }
    return key;
