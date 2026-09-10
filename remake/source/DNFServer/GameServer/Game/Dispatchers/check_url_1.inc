bool check_url(const char* url)
{
    if (url == 0)
        return false;
    // ORIG 0x08205791：单一指针槽 -0x10（首字符检查处 `mov 0x8,%eax; mov %eax,-0x10`
    // 先物化，循环内 `mov 0x8,%eax; add -0xc,%eax; mov %eax,-0x10` 覆写同槽），
    // 每次字符比较都重新 `mov -0x10,%eax; movzbl (%eax),%eax; cmp $K,%al`
    // —— 不得引入 unsigned char 局部（4.4 会 spill 成 `mov %al,-0x9` + `cmpb mem`）。
    const char* p = url;                                // -0x10
    if ((unsigned char)*p == 0x5f)                      // 首字符 '_' 拒绝
        return false;
    // 循环变量必须本身是 unsigned int：写成 int + `(unsigned int)i` 会让 4.4 在
    // strlen 比较处多一步 `mov -0xc,%edx`（ORIG 是 `cmp -0xc(%ebp),%eax; seta`
    // 直接把 mem 作 src），并使 `url + i` 的装载序变成 index 先载。
    for (unsigned int i = 0; strlen(url) > i; ++i) // i → -0xc
    {
        // 指针加法经 (unsigned long) 往返（技巧26）阻断 4.4 的 lea 折叠，
        // 得到 ORIG 的 2 条 `mov/add` 形（lea (%edx,%eax,1) 三条不可接受）。
        p = (const char*)((unsigned long)url + i);
        // ORIG 0x82057d0 控制流（if-else-if 链，三支均落共享 i++）：
        //   cmp $0x60; jbe 57e4                 ；c<=0x60 走下支
        //   cmp $0x7a; jbe 5802 (i++)           ；a..z 放行
        // 57e4: cmp $0x2f; jbe 57f8             ；c<=0x2f 走下支
        //        cmp $0x57; jbe 5802 (i++)      ；0..W 放行
        // 57f8: cmp $0x5f; jne 5808 (拒绝)      ；'_' fall 进 i++，他字符拒绝
        if ((unsigned char)*p > 0x60 && (unsigned char)*p <= 0x7a) {
            // a..z → continue（fall 至循环尾 = i++）
        } else if ((unsigned char)*p > 0x2f && (unsigned char)*p <= 0x57) {
            // '0'..'W' → continue
        } else if ((unsigned char)*p != 0x5f) {
            // [N·块序] ORIG 0x082057d0 尾支 `cmp $0x5f; jne 拒绝; [fall] addl
            // i++`（4.1 fall-through 放行）；4.4 恒把拒绝块排为 fall-through、
            // 产 `je <i++岛>`——本波补测显式 `continue` 形（`==0x5f` → continue）
            // 仍产同形（ndiff 5 不变）⇒ 块排序不可控，记 N。语义两侧等价。
            return false;
        }
    }
    return true;
