StringResource::~StringResource() {}

// ---------------------------------------------------------------------------
// parseStringLine —— ORIG t _ZL15parseStringLinePcPiPS_ @0x08ad490f。
// 行格式 "<key数字> > <value>"；'/' 开头("//")为注释；'\r'/'\n' 开头空行；
// 返回：0=成功（*key=atoi(key 区)、*value=value 起）、-1=key 含非数字、
// -2=key 位数>11。value 内 "\n" 转义为真实换行并左移，
// 其余 '\' 删除前缀字符（与 ORIG 循环形态一致），行结束('\r'/'\n')截断。
// 错误码经调用方 "Parse error : line:%d reason:%d" 记入 error。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource AE: 155=155 逐条全等（P264 归一后） | ORIG 0x08ad490f]
static int parseStringLine(char* line, int* key, char** value)
{
    char* p = line;
    // 前导空白（' '/'\t'）跳过；空行/换行起始 → 失败（ORIG 返回非解析态）
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
            return 0;                   // ORIG @0x8ad4931 共享 return-0 岛
        if (*p == ' ' || *p == '\t')
            ++p;                        // 免 continue：p++ 作循环体尾 fall 回条件
        else
            break;
    }
    // ORIG @0x8ad496d：*p==0（全空白行）与 "//" 注释共享同一 return-0 岛
    //（`test; je island` + `//` 双 cmp 短落到同一 `mov $0` 块），故合并短路或。
    if (*p == '\0' || (p[0] == '/' && p[1] == '/'))
        return 0;

    char* keyStart = p;
    char* valueStart = 0;
    while (*p)
    {
        if (*p == '>')
        {
            *p = '\0';
            ++p;
            valueStart = p;
            break;
        }
        if (*p <= '/' || *p > '9')
            return -1;                  // key 非 [0-9] 字符（ORIG 共享 -1 岛）
        ++p;
    }
    // ORIG @0x8ad49ec：valueStart==0（未找到 '>'）与空 key（len==1）共享
    // 同一 return -1 岛（`cmpl $0; je` 短落到 -1 块），故用 || 合并。
    if (valueStart == 0 || (int)(valueStart - keyStart) == 1)
        return -1;

    if ((int)(valueStart - keyStart) - 1 > 10)
        return -2;                      // ORIG cmp $0xa 上限

    // value 内扫描：'\r'/'\n' 终结该行；"\n" 转义真实换行并左移补位。
    // ORIG @0x8ad4a4f 对 '\' 后非 'n' 一律不处理（两 jne 直落 i++，无删
    // 反斜杠逻辑）——此前 EXTRA 的删除循环系误读，已删。
    // 不写 `p = valueStart;`：key 扫描 break 时 p 恰停在 valueStart（ORIG
    // @0x8ad49b3 `mov %eax,-0x10` 后即复用 p），显式重载会多 3 条
    //（mov %eax; mov %eax; jmp），现复用循环尾 p 值（见我们汇编 value 段入口）。
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        // ORIG @0x8ad4a5e-0x163：命中 "\n" 转换+左移后与 miss 路径共享同一条
        // p++（@0x163），不 continue 跳过步进。
        if (*p == '\\')
        {
            if (p[1] == 'n')
            {
                *p = '\n';
                // 左移覆盖被吃掉的第二个字符（ORIG copy 循环）。
                for (char* q = p + 1; *q; ++q)
                    *q = *(q + 1);
            }
        }
        ++p;
    }

    *key = atoi(keyStart);
    *value = valueStart;
    return 0;
}