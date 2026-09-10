void CStringMaker::operator()(std::vector<float>& values)  // 0x08627146
{
    if (values.empty())
    {
        return;
    }
    if (!m_str.empty())
    {
        m_str += ";";
    }
    // b28d-35：ORIG 0x08627146 的 num 缓冲在循环**外**（内联清零位于 m_str
    //   检查之后、begin() 之前，循环体内仅 sprintf 直接引用 -0x113 槽）⇒
    //   声明移出循环体（`={0}` 复现 4.1 内联三段清零，P214③/P253）。
    char num[0xff] = {0};
    for (std::vector<float>::iterator it = values.begin(); it != values.end();
         ++it)
    {
        // b28d-35：ORIG 循环体语句序 = sprintf → 逗号判断 → += num（0x86271fe
        //   先 deEv+sprintf，0x8627225 才调 begin 比较）⇒ 保持该序。
        sprintf(num, "%f", *it);
        if (it != values.begin())
        {
            m_str += ",";   // 元素间 ORIG 逗号 @0x8ce2dec
        }
        m_str += num;
    }
