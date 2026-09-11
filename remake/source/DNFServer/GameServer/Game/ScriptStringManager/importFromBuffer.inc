// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N82**（238 vs 242 条；
// p167: 3 MISSING 右值模板实例 + 3 EXTRA）。已按 Ghidra C 逐句复现：双独立守卫、
// offset 循环尾读 + 哨兵 goto、memcpy 后查越界（P171）、删 len<0 护栏。
// 残差 = 槽整体深 4-8B（ORIG push_back 内联临时槽 -0x2c/-0x28，4.4 无）＋
// %esi/%ebx 角色互换；MISSING 为 C++0x 右值 `_M_insert_aux`（同 insertString）。
bool ScriptStringManager::importFromBuffer(const unsigned char* buffer,
                                           int buffSize)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a/N82）→ 试=④push_back 右值→左值
    //   （Main 线索）：本 TU 路由为 `-O2 -std=gnu++98`（dnf_game_cxx.sh ScriptStringManager
    //   分支），`vector::push_back` 恒走 C++03 `_M_insert_aux(…, RKT)`（nm 实证我方仅
    //   lvalue 版 `…RKi`/`…RKSs`）⇒ ORIG 的 `_M_insert_auxIIiEE…DpOT_`（C++0x 变参版）
    //   属**旗标差**（-std），源码形态不可翻；槽差（count ORIG -0x440 vs 我方 -0x43c）+
    //   %esi↔%ebx 角色归 N82/N18a。②prett ③strall CLEAR。⇒ 维持。
    // ORIG 8acf97f-8acf992：两个**独立**守卫（buffer==NULL 早退 8acf984；
    // count 先读入槽 -0x440 再判 `buffSize > 3` 正向 8acf98c/992）。
    // [L4-r2 判据⑩ 一次性试验已回退] 试把 count 下移到 offsets 之后 ⇒ vector
    // ctor 被提到 NULL 守卫之前（多 3 条 movl $0）且 count 槽仍 -0x43c ⇒ 反劣，
    // 已还原。槽差归 N82/N18a（寄存器角色 %esi↔%ebx 同族）。
    unsigned int count;
    if (buffer == 0
        || (count = *reinterpret_cast<const unsigned int*>(buffer), buffSize < 4))
        return false;

    // 局部偏移向量（ORIG local_38 的 vector<int>，异常路径自动清理）
    std::vector<int> offsets;
    offsets.reserve(count);  // 8acf9bf

    const unsigned char* src = buffer + 4;    // local_440 / local_448
    // [round5 轴B 实测负例（已回滚）] ORIG 8acfa22 `cmp %ebx,0x10(%ebp); jl 8acfc10`
    //   （有符号）vs 我方 `cmp %edx,-0x444(%ebp); ja`（无符号）：按轴B 试 `int pos`
    //   + 去 `(unsigned)` cast ⇒ 实测 ours_insns 与 diff 规模逐值不变
    //   （238 / +132−136，与改前完全相同）⇒ 无收益，按「四数不劣但 ndiff/gap 须严格
    //   改善」回退（轴B 在该点被 N82 槽差掩盖，属清单交叉配对类）。
    unsigned int pos = 8;                     // local_44c：读取游标

    if (count != 0)
    {
        // ORIG 8acf9e0-8acfa60：先取 offsets[0]，循环尾读下一个；读满 count 个
        // 后 `pos += 4` 跳 LAB_08acfa69 补哨兵（Ghidra C 逐句同形）。
        unsigned int off = *reinterpret_cast<const unsigned int*>(buffer + 4);
        if (buffSize > 7)
        {
            unsigned int i = 0;
            do
            {
                src = buffer + pos;               // 8acf9f1：循环头
                offsets.push_back(off);           // 8acfa0b：内联原地/扩容
                if (count <= i + 1)
                {
                    pos += 4;                     // 8acfa60
                    goto after_offsets;
                }
                ++i;                              // 8acfa2b
                off = *reinterpret_cast<const unsigned int*>(buffer + pos);
                pos += 4;
            } while ((int)pos <= buffSize);
        }
        return false;  // 8acfc10：buffSize 不足
    }
after_offsets:
    // LAB_08acfa69：哨兵 offsets[count] = *(uint*)src（src 停在最后一个条目）
    if (buffSize < (int)pos)  // 8acfa77（ORIG 有符号 jl：B 轴对齐）
        return false;
    offsets.push_back(*reinterpret_cast<const unsigned int*>(src));
    strings_.reserve(count);  // 8acfabb

    if (count != 0)
    {
        // 字符串区从 src+4 = buffer+4*count+8 起（export 写入起点一致）。
        // ORIG 8acfb20/8acfbfa：**先 memcpy 后查越界**（P171 照抄原码顺序），
        // 且无 len<0 护栏（ORIG 直接 `if ((int)__n < 0x400)`）。
        unsigned int i = 0;
        src += 4;  // local_448
        do
        {
            ++i;
            const int len = offsets[i] - offsets[i - 1];
            char temp[1024];  // ORIG local_438[1023] + local_39
            if (len < 0x400)
            {
                std::memcpy(temp, src, len);
                if (buffSize < static_cast<int>(pos + len))  // 8acfb20
                    return false;
                temp[len] = '\0';
            }
            else
            {
                // 长字符串：原版仅拷前 1023 字节（栈缓冲截断）
                std::memcpy(temp, src, 1023);
                if (buffSize < static_cast<int>(pos + len))  // 8acfbfa
                    return false;
                temp[1023] = '\0';
            }
            src += len;
            pos += len;
            strings_.push_back(std::string(temp));  // 8acfb4e/8acfb53 构造并入列
        } while (i < count);
        if (buffSize < static_cast<int>(pos))  // 8acfc66：文件总大小边界
            return false;
    }
    return true;  // 8acfc71
}