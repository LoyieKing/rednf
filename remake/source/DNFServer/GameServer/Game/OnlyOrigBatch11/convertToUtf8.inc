// ORIG T 0x08adf6e7：宽字符数组按 UTF-8 编码宽度累计（<=0x7f → 1 字节、
// <=0x7ff → 2 字节、其余 3 字节）；总长 0 → out="" 返 1；否则
// CharStringData::createTerminated + 逐字符编码写入 out。
// [batch-19 S→A] 原 S 档 missing-calls（C1EP14CharStringData/D1/aSERKS_）为
// 形错：ORIG 主路径构造**临时 CharString tmp(data)** 再 `*out = tmp`（拷贝
// 赋值 + 临时析构，含 EH 保护区），非 attachData 直挂；且两循环编码游标用
// `char* buf++` 指针自增、ch 为 signed int（`cmpl $0x7f; jg`）、首循环逐臂
// `++i`（while 形）、createTerminated 已含终止符无需 buf[pos]='\0'。订正后
// ndiff 103→11（残差 = 尾部 tmp 析构公共块布局 + 返回值寄存器调度）。
// [b28d-solo-OnlyOrigBatch11 收口] 旧 D66 的「P78 N 封顶」判词作废：把尾部
// `CharString tmp(data); *out = tmp; return true;` 的 tmp 收进**显式块**
// `{ CharString tmp(data); *out = tmp; } return true;` 后，`mov $1,%eax` 落到
// D1 之后（ORIG +0x1a0 形），4.4 不再把返回值提前物化到 %ebx ⇒ fndiff clean
// 135=135 **AE-EQUAL**（`movl $"",0x4(%esp)` 的 0x24b addend 为 P264 未链接伪差）。
bool convertToUtf8(const wchar_t* src, int srcLength, CharString* out)
{
    int need = 0;
    for (int i = 0; i < srcLength; )
    {
        const int ch = static_cast<int>(src[i]);
        if (ch <= 0x7f)
        {
            need += 1;
            ++i;
        }
        else if (ch <= 0x7ff)
        {
            need += 2;
            ++i;
        }
        else
        {
            need += 3;
            ++i;
        }
    }
    if (need == 0)
    {
        *out = "";
        return true;
    }
    CharStringData* data = CharStringData::createTerminated(need);
    char* buf = data->getBuffer();
    for (int i = 0; i < srcLength; ++i)
    {
        const int ch = static_cast<int>(src[i]);   // ORIG signed（主循环 `cmpl $0x7f; jg`）
        if (ch <= 0x7f)
        {
            *buf++ = static_cast<char>(ch);
        }
        else if (ch <= 0x7ff)
        {
            *buf++ = static_cast<char>(0xc0 | (ch >> 6));
            *buf++ = static_cast<char>(0x80 | (ch & 0x3f));
        }
        else
        {
            *buf++ = static_cast<char>(0xe0 | (ch >> 12));
            *buf++ = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
            *buf++ = static_cast<char>(0x80 | (ch & 0x3f));
        }
    }
    // ORIG 0x8adf75a..0x8adf861：构造临时 CharString tmp(data)（lea -0x28）→
    // *out = tmp（aSERKS_）→ 析构 tmp（D1，含 EH 保护区）；非 attachData
    // 直挂（那样缺 C1EP14CharStringData / aSERKS_ / D1 三个调用）。
    // [b28d-solo-OnlyOrigBatch11] tmp 用**显式块**限定生存期（块尾析构），使
    // `return true` 的 `mov $1,%eax` 落在 D1 之后（ORIG +0x1a0 形），避免 4.4
    // 把返回值提前物化到 %ebx 跨 D1 call 保活（P78）。
    {
        CharString tmp(data);
        *out = tmp;
    }
    return true;
}