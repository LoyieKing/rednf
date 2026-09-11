// ---------------------------------------------------------------------------
// convertFromUtf8 —— ORIG T 0x08adf893。两遍式 UTF-8→wchar_t：pass1 统计宽
// 【验收 batch-11】orig=161 / our=162（gap −1）：修正旧实现多出的 4 字节分支
// （ORIG >0xef 一律 return false，@0x51-0x71 无第 4 档，此前语义偏差已消除）；
// ASCII 首块 js 直跳形、合成段 add 非 or、(unsigned short)(b<<12) 两段形均已
// 对齐。残 1 行 = ws 析构块 EH 聚合布局（4.1 共享 dtor tail vs 4.4 散布
// return 岛，跨版本负例族），封顶勿再追。
// 字符数（ASCII(js 直跳) / c<=0xdf&&i+1<len:2 / c<=0xef&&i+2<len:3 / 其余
// ——含 4 字节序列与截断——一律 return false，ORIG 无 4 字节分支 @0x51-0x71）；
// count==0 → *out=L""（operator=(const wchar_t*)）返 1；否则 createTerminated
// + getBuffer 得 wchar_t* 游标（*dst++，ORIG addl $4,-0x1c 形态）逐字合成，
// 具名 WideString tmp(data) + op= + 具析构后返 1。合成式与 ORIG 逐指令：
// 2 字节 (c&0x1f)<<6 | (signed)src[i+1]&0x3f；3 字节
// (unsigned short)(c<<12)（shl $0xc; movzwl %ax 两段形）| (b1&0x3f)<<6 | (b2&0x3f)。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N(N82/N93): 槽位分配差（wideCount ORIG -0x24 vs 4.4 -0x1c、
// c ORIG -0x11 vs 4.4 -0xa；帧同 0x30，i 同 -0x18）+ 返回物化 %eax/%ebx；
// 161/162 gap −1，p167 CLEAR 8=8，语义完成 | ORIG 0x08adf893]
// [M2S5StringRaw 2026-09-11 复核] gap −1 的两条真因已定位并修复（见函数体内两处
// [M2S5StringRaw] 注释）：① 第二循环复用 i 令 4.4 压缩槽基；② tmp 未用显式块
// 令 4.4 把返回 1 提前物化到 %ebx。探针 /tmp/probeC2（4.4.7 默认路由）实测
// 161=ORIG 且 /tmp/seqcmp.py 归一化序列逐条全同（旧形 162）。
bool convertFromUtf8(const char* src, int srcLength, WideString* out)
{
    int wideCount = 0;                        // -0x24
    int i = 0;                                // -0x18
    while (i < srcLength)
    {
        const unsigned char b = (unsigned char)src[i];
        // ORIG 块排布：ASCII 为 fall-through 首块（test;js→多字节区 @0xf8）。
        if ((char)b >= 0)
        {
            wideCount += 1;
            i += 1;
        }
        else
        {
            if (b <= 0xdf && i + 1 < srcLength)
    {
            wideCount += 1;
            i += 2;
        }
            else if (b <= 0xef && i + 2 < srcLength)
        {
            wideCount += 1;
            i += 3;
        }
        else
                return false;
        }
    }
    if (wideCount == 0)
    {
        *out = L"";                           // ORIG operator=(wchar_t const*)
        return true;
    }

    WideStringData* data = WideStringData::createTerminated(wideCount);
    wchar_t* dst = data->getBuffer();
    // [M2S5StringRaw 2026-09-11] ORIG -0x10：第二循环是**独立具名局部**（i 槽
    // -0x18 不再被重置复用）。旧形 `i = 0` 令 4.4 复用 -0x18 槽并把整段槽基
    // 压缩 4（wideCount -0x1c vs ORIG -0x24…）。探针 /tmp/probeC1 亦证。
    int j = 0;
    while (j < srcLength)
    {
        const unsigned char b = (unsigned char)src[j];
        // 同上：ASCII 首块 + js 直跳多字节区；合成段间用加法（ORIG add
        // %eax,%edx，4.1 对无重叠位 or 发射 add；4.4 的 | 直出 or）。
        if ((char)b >= 0)
        {
            *dst++ = (wchar_t)b;
            j += 1;
        }
        else
        {
            if (b <= 0xdf && j + 1 < srcLength)
    {
                *dst++ = (wchar_t)(((b & 0x1f) << 6) +
                                   ((int)src[j + 1] & 0x3f));
            j += 2;
        }
            else if (b <= 0xef && j + 2 < srcLength)
        {
                *dst++ = (wchar_t)((unsigned short)(b << 12) +
                                   (((int)src[j + 1] & 0x3f) << 6) +
                                   ((int)src[j + 2] & 0x3f));
            j += 3;
        }
        else
                return false;
        }
    }
    // [M2S5StringRaw 2026-09-11] ORIG 0x08adfa5c `jmp 0x8adfa79`（tmp 析构块）
    // ⇒ tmp 的作用域先于 return 结束（显式块）：4.4 因此不再把返回 1 提前
    // 物化到 callee-saved %ebx（旧形多 `mov $1,%ebx` + 尾 `mov %ebx,%eax`）。
    // 探针 /tmp/probeC2/Game/StringResource.cpp 实测 161=ORIG 且归一化序列
    // 逐条全同（/tmp/seqcmp.py）。
    {
        WideString tmp(data);
        *out = tmp;
    }
    return true;
}