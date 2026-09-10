bool script_tag::isCloseString(std::string& str)
{
    // [b28c-lane17] ORIG 0x08a0c8b2：`movb $0x0,-0x9` 预清 + 条件置 1 +
    // 尾 `movzbl -0x9` ⇒ bool 汇聚局部形；嵌套 if 才产 ORIG 的
    // `seta;test;je` / `sete;test;je` 双物化（&& 被 4.4 折成 jbe/jne）。
    bool result = false;
    if (str.size() > 2)
    {
        if (str.at(1) == '/')
            result = true;
    }
    return result;
