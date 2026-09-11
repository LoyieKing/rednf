// ---------------------------------------------------------------------------
// loadFromString —— ORIG T 0x08ad4ac2。
// clear 表后逐行解析；成功且 key!=0 → add(key, CharString(value))；
// key==0 的条目跳过；重复 key 与解析错误写入 errOut（format 串逐字一致：
//   "Parse error : line:%d reason:%d"
//   "Duplicated key : line:%d key:%d" )。
// 行终结：'\n' 直接进下一行；'\r\n' 连续则一并消费（ORIG i++ 补偿）。
// i==len 时对最后一行就地 NUL 终结后解析。遍历边界 i<=len（setle 条件）。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N(N93/N98/N18b/N104): 块内局部槽方向相反（ORIG 声明序深→浅
// val-0x34/key-0x30/msg-0x2c/v-0x28/dupMsg-0x24；4.4 首次使用序 -0x24/-0x28/-0x2c/-0x30/-0x34，
// P212 全头部声明实测劣化 165→205）+ r 槽比较 4.4 恒 mem-cmpl（N18b，unsigned 化无效）+
// EH 少 1×_Unwind_Resume（N104 块合并）；163/165，p167 MISSING 1×_Unwind_Resume，语义完成
// | ORIG 0x08ad4ac2]
// [M2S5StringRaw 2026-09-11 复核] 3 处返回物化 `mov $0/$1,%eax`(ORIG) vs
// `mov $0/$1,%ebx` + 尾 `mov %ebx,%eax`(4.4)：探针 /tmp/probeD|E（auto_ptr 活跃
// 作用域内 return，带/不带显式块）实测 4.4 恒提前物化到 callee-saved %ebx ⇒
// N93 后端族（非局部作用域问题，源形不可翻）；P216/P339 不适用。
// 残余另有 N18b（r 槽 mem-cmpl vs ORIG 寄存器比较）与 N104（少 1 _Unwind_Resume）。
// 163/165 维持，p167 MISSING 1×_Unwind_Resume。
bool StringResource::loadFromString(char* text, int length, CharString* error)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18b）→ 试=④：r 槽零比较 4.4 恒
    //   `cmpl $0x0,-0x10(%ebp)`（ORIG `mov -0x10(%ebp),%eax; test %eax,%eax`；int/unsigned
    //   两态逐字节同，旧实测已回退）+ 尾部返回物化 `mov $0/%1,%ebx` + 尾 `mov %ebx,%eax`
    //   （ORIG 直 `mov $0/%1,%eax`，探针 auto_ptr 作用域双形同折）——P212 全头部声明实测
    //   165→205 反劣。②prett ③strall CLEAR。⇒ 维持 N18b/N93/N104。
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
                    // [N1F 2026-09-11] msg 作用域收口：ORIG 0x08ad4ba2-0x08ad4bad =
                    //   dtor(-0x2c) 先、`mov $0x0,%eax` 后（返回物化在清理之后）⇒
                    //   `return false;` 在 msg 作用域**之外**，4.4 才不发 %ebx 暂存。
                    {
                        CharString msg = CharString::format(
                            "Parse error : line:%d reason:%d", lineNo, r);
                        *error = msg;
                    }
                    return false;
                }
            }
            if (val != 0)               // ORIG @0xf5 判 val（非 key）
            {
                // [N1F 2026-09-11] v 作用域收口（P0/EH 轴）：ORIG 0x08ad4bf8-0x08ad4bfe 在
                //   `call add`（0x08ad4bf0）后**立即**内联销毁 -0x28（v），随后才算
                //   0x08ad4c03 `movzbl -0x9(%ebp),%eax`（!ok）⇒ ORIG 中 v 的 dtor 不在
                //   「返回/续循环」共享清理路径上，且返回物化是 `mov $0,%eax`（0x08ad4bad/0x08ad4c92）
                //   而非 4.4 的 %ebx 暂存 + %esi 选择子。源形 = v 收进内层块、ok 提到块外。
                bool ok;                           // -0x9 byte 槽（ORIG 0x08ad4bf5 mov %al,-0x9(%ebp)）
                {
                    CharString v(toTString(val));  // -0x28（toTString 恒等）
                    ok = add(key, v);
                }
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
}