// ORIG T 0x08ac2de4（isWideStrInUnicodeRange.md 全文）：
//   IsNoneUnicodeRange 构造（call 0x08ac3240）→ wcslen（call 0x0807e130）结果落
//   局部槽 -0x10（P0：具名 size_t 局部）→ any_of(str, str+len, pred)（call
//   0x08ac3253）→ 取反返回。any_of 实例 ORIG W _ZSt6any_ofIPKw
//   N23restrict_inputting_name18IsNoneUnicodeRangeEEbT_S4_T0_ @0x08ac3253
//   （c6root libstdc++ 4.4.7 bits/stl_algo.h:778 有该模板，可实例化）。
//
// [b28d-solo-RestrictNameDeps] verdict=AE（compare_tu clean 27/27 逐条相等；
// 旧 [b22判词·N-EMPTYCLS-SRA]「结构不可复现」判词作废）。
//   根因=4.4 对**空类**对象做标量替换：pred 虽在 -0x11 构造，但传参用寄存器
//   `mov %bl,0x8(%esp)`（prologue `push %ebx` + `sub $0x24`），ORIG 4.1 从栈槽读
//   `movzbl -0x11(%ebp),%edx; mov %dl,0x8(%esp)`（`sub $0x28` / `leave`）。
//   修法=给 IsNoneUnicodeRange 加 1 字节成员（RestrictNameDeps.h，已 R2 广播）：
//   sizeof 仍 1（空类 sizeof==1）、基类布局/全部偏移/mangled/符号集不变、C1 体不变；
//   探针 /tmp/probe_sra2.cpp 实证空类→%bl、带 1 字节成员→movzbl -N(%ebp)。
//   加成员后 4.4 逐条复现 ORIG（sub $0x28 / 无 push ebx / movzbl -0x11(%ebp),%edx
//   / leave），同 TU any_of/find_if/none_of/__find_if 四实例一并 DIFF→AE。
bool isWideStrInUnicodeRange(const wchar_t* str)
{
    IsNoneUnicodeRange pred;
    size_t len = wcslen(str);
    // P0：ORIG 把 any_of 结果物化到 bool 槽 -0x9（mov %al,-0x9(%ebp)），
    // 再以 cmpb $0,-0x9 / je 两支各 mov $0/$1 返回 → 源码为 if 展开形态
    // （`return !found;` 会被 4.4.7 编成 movzbl+xor，与 ORIG 分支形态不符）。
    bool found = std::any_of(str, str + len, pred);
    if (found)
        return false;
    return true;
}