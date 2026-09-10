bool StringResource::loadFromString(char* text, int length, CharString* error)
{
    m_strings.clear();

    // toTString(const char*)：ORIG _Z9toTStringPKc @0x08adeb75（恒等转换，
    // 定义 SyncScriptDeps.cpp）；本 TU 属字符串底座，按 InitScriptProducers_5
    // 先例局部声明避免拖入 SyncScriptDeps.h 依赖。
    const char* toTString(const char* pszSrc);

    char* val;                      // ORIG -0x34（P212 全头部声明，先声明占深槽）
    int key;                        // ORIG -0x30
    int lineNo = 0;                 // -0x20
    char* lineStart = text;         // -0x1c
    int i = 0;                      // -0x18
    while (i <= length)
    {
        // ORIG @0x2c-0x4a：c 为 byte 槽（-0x11，mov %al）；三条件
        // cmpb je/cmpb je/cmp jne 全部直跳，共享行末 i++（@0x1fd）——
        // 非行内联 \0/补偿块。
        const char c = text[i];
        if (c == '\n' || c == '\r' || i == length)
        {
            ++lineNo;                   // 行号在边界块首自增（@0x50）
            text[i] = '\0';
            val = 0;                    // ORIG @0x5d 唯一 0 初始化（块内）
            // [D01 · C05② 细化负例 2026-09-10] `test %eax,%eax` 零比较子型的
            // 类型旋钮实测无效：本点 ORIG 为 `mov -0x10(%ebp),%eax; test %eax,%eax`
            // （载寄存器两段），而 4.4 对具名局部恒折 `cmpl $0x0,-0x10(%ebp)`，
            // **int 与 unsigned 两态同形**（改 unsigned 后 fresh .o 逐字节不变、
            // 四数 43/63/1/3 不变）⇒ 已回退 int，本点归 N18b 常量比较子型。
            int r = parseStringLine(lineStart, &key, &val);   // -0x10
            if (r != 0)
            {
                // ORIG：仅 error!=0 时报错并 return false；error==0 时
                // 忽略解析失败继续（@0x87 je →val 检查）。
                if (error != 0)
                {
                    // ORIG 0x08ad4b5c：format 为静态 sret，直写 -0x2c 局部槽，
                    // 无 C1Ev/无 op=（P215 展开形）。
                    CharString msg = CharString::format(
                        "Parse error : line:%d reason:%d", lineNo, r);
                    *error = msg;
                    return false;
                }
            }
            if (val != 0)               // ORIG @0xf5 判 val（非 key）
            {
                CharString v(toTString(val));      // -0x28（toTString 恒等）
                bool ok = add(key, v);             // -0x9 byte 槽
                if (!ok)
                {
                    // 重复 key：写 err 后**整表加载失败返回**（@0x16c-0x1d5）。
                    if (error != 0)
                    {
                        CharString dupMsg = CharString::format(
                            "Duplicated key : line:%d key:%d", lineNo, key);
                        *error = dupMsg;
                    }
                    return false;
                }
            }
            // \r\n 连续补偿在 val 处理之后（@0x1d7），随后 lineStart 前移。
            if (c == '\r' && text[i + 1] == '\n')
                ++i;
            lineStart = text + i + 1;
        }
        ++i;
    }
    return true;
