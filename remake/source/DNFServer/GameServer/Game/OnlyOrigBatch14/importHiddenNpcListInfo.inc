// [b14 本波·AE 归零] 内层循环局部槽序（旧判词「残余 NEAR=N98 槽序」作废）：
// ORIG s@-0x1c(深)/isInt@-0x16/isType@-0x15/v@-0x14(浅)，4.4「后声明更深」
// ⇒ 声明序改 v/isType/isInt/s（v 声明与赋值分离，P244）后 121=121、ndiff=0。
bool importHiddenNpcListInfo(const char* path, std::vector<int>& out)
{
    // ORIG 0x089178de：`mov 0xc(%ebp),%eax; mov %eax,0x4(%esp);
    // movl $<rodata 串>,(%esp); call loadRDARScriptFile` —— **第一参数为
    // rodata 基串、第二才是 path**（我们原写反，真语义 bug）。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x08917900 条件块 `cmpb $0x0,-0x9(%ebp);
    // jne <exit>`（内存直测、无 movzbl/xor 取反物化）⇒ 原局部为**字节标量**
    // 且条件写正向 `done == 0`（N122：bool 局部取反恒 xor，不可翻）。
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x089178f8 `movb $0x0,-0xa(%ebp)` 为
    // 第二个 dead 字节局部预清（P171 照抄），补以对齐槽集。
    bool unusedFlag = false;
    unsigned char done = 0;
    out.clear();
    for (;;)
    {
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08917900 `cmpb $0x0,-0x9(%ebp);
        // jne <exit>`（字节局部直测、无 sete 物化）⇒ 条件写 `if (done) break`。
        if (done)
            break;
        if (!ScanType(token, true))
            return false;
        // [DIFF→AE] ORIG 089178d8 `je 089179ad`：fail 块（mov $0,%ebx;jmp exit）
        // 物理位于函数尾部（089179ad，紧邻成功返回块 089179b4）⇒ 4.4 对 guard 形
        // `if(!=) return false;` 把 fail 块内联在跳转点之后（前部，实测 DIFF）；
        // 改正向 `if(==){…} else {return false;}` 使 4.4 后置 else 块，复现块序。
        if (token == "[hidden npc]")
        {
        for (;;)
        {
            // ORIG 0x089179ad：isType/isInt 为**循环内每轮**置零
            // （movb $0,-0x15/-0x16，P171 dead-init 照抄）。
            int v;
            bool isType = false;
            bool isInt = false;
            std::string s;
            // 声明序 v/isType/isInt/s：4.4「后声明更深」⇒ s 最深(-0x1c)、
            // v 最浅(-0x14)，与 ORIG 槽图逐槽一致；v 声明与赋值分离(P244)。
            v = ScanTypeOrInt(isType, s, isInt, 1);
            if (!isInt)
                return false;
            if (isType)
            {
                if (s == "[/hidden npc]")
                {
                    // [gap-3 修复] ORIG 0891794b/4f/54：`movb $0x1,-0x9(done);
                    // mov $0x1,%esi; jmp 08917990`（统一 dtor 点后
                    // `cmp $0x1,%esi; je 外层循环头`）⇒ 原码是 **break 内层
                    // 循环**（done=1 后回外层，由外层 done 检查退出），不是
                    // 直接 return true（后者只产 `mov $1,%ebx; mov $0,%esi`，
                    // 缺 cmp/je 两条 + 外层 nop 岛）。
                    done = 1;
                    break;
                }
                return false;
            }
            out.push_back(v);
        }
        }
        else
        {
            return false;
        }
    }
    return true;
}
