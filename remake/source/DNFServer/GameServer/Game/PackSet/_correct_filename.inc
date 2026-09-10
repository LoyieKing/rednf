int _correct_filename(char const* src, char* dst, unsigned int dst_size,
                      char* dir, unsigned int dir_size)
{
    if (src == 0 || dst == 0)
    {
        return 1;
    }

    // b28d-solo-PackSet 判词（N 封顶：跨版本循环体/exit 头块序，ORIG 0x08ac3b9a
    // size 0x247；本轮 ndiff 164→29、gap -10→+1）：
    // ①p167：ORIG 全部 stack 调用为**非 const** `std::stack<char*>`（push 左值、
    //   top 返 char*）⇒ 16=16 CLEAR。
    // ②全槽位对齐：prev_tok -0x18 / w -0x44 / seg -0x14 / r -0x10 / c -0x9 /
    //   deque 临时 -0x40 / stack -0x6c。关键两处：**seg 声明先于 r**（两指针槽
    //   互换归位）；**尾部复用 seg 而非独立 tail 局部**（4.4 不复用两者槽 ⇒
    //   全帧深移 4）。
    // ③`while ((c = *r++) != 0)` 复现底部 cond 块 + `setne %al; addl $1,r;
    //   test %al,%al; jne` 物化（P225/P283；goto 尾标签形无 setne）。
    // ④`*(unsigned short*)seg == 0x2e2e` 复现 ORIG `movzwl; cmp $0x2e2e,%ax`
    //   （`seg[0]=='.'&&seg[1]=='.'` 在 4.4 生两段 movzbl+cmp）。
    // ⑤dir 单出口 `if (dir != 0) {…} return 0;` 复现 `cmpl $0,dir; je <共享
    //   return 块>`（P47）。
    // ⑥恒真守卫 `if (c != 0) { 结算 }` 复现 exit 头 `cmpb $0,c; jne +0x110`
    //   （break 出口与分隔符路径共用该检查）。
    // 残余（语义等价，N 封顶）：4.1 把结算段物理置于 cond/exit 头**之后**
    // （+0x110）；4.4 恒把结算段发射在 cond 之前且守卫生成 `je` 负极性 ⇒ 差
    // 1 条 jne + 1 条 jmp（ndiff 29 全为块序对齐残差，指令集只差这对跳转）。
    int prev_tok = 0;                // -0x18（ORIG dead 局部，Ghidra local_1c 无读）
    (void)prev_tok;
    char* w = dst;                   // -0x44
    char* seg;                       // -0x14
    char const* r = src;             // -0x10
    char c;                          // -0x9
    std::stack<char*> st;            // 分量起点栈（ORIG stack<char*, deque<char*>>）
    st.push(w);                      // 初始哨兵（ORIG push 实参 = w 局部 &-0x44，非形参 dst）

    // b28d-solo-PackSet（P283/P184）：ORIG 循环条件块在**物理底部**（+0xd9 读 c/
    // 物化 setne/`++r`/test/jne 回体），入口 +0x88 无条件 `jmp +0xd9`，且体首
    // `cmpb $0,c; je +0xf3` 为 4.1 保留的死块 ⇒ 用 while(赋值表达式) 形复现
    // （4.4 对 `for(;;)`+break 恒把条件置顶）。
    while ((c = *r++) != 0)
    {
        if (c == 0)
        {
            break;                    // ORIG 死块（4.1 保留）
        }
        if (c == '/' || c == '\\')
        {
            *w++ = '/';
        }
        else
        {
            if ('A' <= c && c <= 'Z')
            {
                *w++ = static_cast<char>(c + 0x20);
            }
            else
            {
                *w++ = c;
            }
            continue;                 // 普通字符不触发分量结算（ORIG jmp 读下个字符）
        }

        // ---- 分量结算（ORIG @08ac3caa）----
        // b28d-solo-PackSet：ORIG exit 头 +0xf4 有 `cmpb $0,c; jne +0x110`
        // （break 出口与分隔符路径共用该检查；c 恒非 0）⇒ 显式写出恒真守卫，
        // 复现该 cmpb/jne 对（4.4 对无守卫形直接内联结算段，少 2 条）。
        if (c != 0)
        {
            seg = st.top();
            // b28d-solo-PackSet（P0/P183）：ORIG 无 diff 栈槽，三处 if 各自重算
            // `w - seg`（`mov w,%eax; mov %eax,%edx; mov seg,%eax; mov %edx,%ecx;
            // sub %eax,%ecx`）；具名 diff 会被 4.4 合并成单次计算 + 槽往返。
            if (static_cast<unsigned long>(w - seg) == 1)
            {
                w = seg;                             // 段为空
            }
            else if (static_cast<unsigned long>(w - seg) == 2 && seg[0] == '.')
            {
                w = seg;                             // 抛弃 "./"
            }
            // b28d-solo-PackSet：ORIG +0x176 `movzwl (%eax),%eax; cmp $0x2e2e,%ax`
            // ⇒ 原码是 16 位双字节比较（`seg[0]=='.'&&seg[1]=='.'` 在 4.4 生两段
            // movzbl+cmp）。
            else if (static_cast<unsigned long>(w - seg) == 3 &&
                     *reinterpret_cast<unsigned short const*>(seg) == 0x2e2eu)
            {
                if (st.empty())
                {
                    return 1;                        // 空栈分支实际不可达，忠实保留
                }
                st.pop();
                w = st.top();
            }
            else
            {
                st.push(w);
            }
        }
    }

    *w = '\0';

    // b28d-solo-PackSet（P0/P47）：ORIG `cmpl $0,dir; je +0x212`（跳到共享
    // return 块）⇒ 单出口 `if (dir != 0) {…} return 0;`；ORIG 尾部复用 seg 槽
    // -0x14（无独立 tail 局部），避免多占 4 字节使全帧深移。
    if (dir != 0)
    {
        seg = st.top();
        ss_strcpy(dir, dir_size, seg);
        if (seg == dst)
        {
            seg[0] = '\0';
        }
        else
        {
            seg[-1] = '\0';                         // 去尾段（ORIG sptr[-1]=0）
        }
    }
    return 0;
