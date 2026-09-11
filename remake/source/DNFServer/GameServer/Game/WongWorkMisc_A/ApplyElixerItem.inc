// ORIG T 0x0831e85c _ZN8WongWork15CCalcCharacStat15ApplyElixerItemEP15_Additioal_infoii
// 报告：ApplyElixerItem.md。灵药道具按 statIdx 选择属性槽做加法（u16 回绕）；
// statIdx>9 静默忽略；case9 对 m_field10[0..3] 各加 value*10。
// ORIG 汇编不物化 (short)value：逐 case 以 32 位 value 乘 50 后 movw 截断
// （mod 2^16 与 s16 乘加等价）；case 物理布局序 = 4,6,5,7,2,3,8,9
// （ORIG 块序 +0x1f/+0x4c/+0x79/+0xa6/+0xd3/+0xea/+0x103/+0x120）。
// [SweepWongWorkEvents P327] 本函数是 ORIG 该 TU 内地址最小的强符号（0x0831e85c <
//   InitializeStat 0x0831e9ca），ORIG 的 GI 键名即它；定义置于 TU 最前使 4.4 发射的
//   _GLOBAL__I__ZN8WongWork15CCalcCharacStat15ApplyElixerItemEP15_Additioal_infoii 键名一致。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：case0..8 与 ORIG 段级逐条全等
//   （仅 jmp/jump-table 重定位伪差）；唯一残差 = case9 循环体寄存器角色互换
//   （ORIG: i→%ecx、元素值→%ebx、`lea (%ebx,%eax,1)`；4.4: i→%ebx、值→%ecx、
//   `add %eax,%ecx`+`lea 0x8(%ebx),%edx`）= N7+N18a。已试 7 形（`=`/`+=`/交换加数/
//   语句表达式/register 索引/register 元素/long 索引）+ 4.4.6 后端复测，同形 ⇒ 封顶。
//   p167: 0 missing / 0 extra；语义与 Ghidra C 逐 case 一致（mod 2^16 等价）。
// [P3Wave4 试形 8·P339 全头部声明·中性已回滚] 上述 7 形均未动 **i 的声明位置**；
//   本波补试「`int i;` 上移函数头 + `for (i = 0; …)`」⇒ 复编实测 **119/121 不变**
//   （寄存器角色残差原样：i→%ebx、值→%ecx、`add %eax,%ecx`+`lea 0x8(%ebx),%edx`），
//   即声明位置在本件不改变 4.4 伪寄存器分配 ⇒ 无收益，按最小改动原则回滚为块内
//   声明（本形 8 已试毕，勿重复）。ORIG 侧 0x0831e985 `mov -0x8(%ebp),%ecx` 与
//   0x0831e99b `mov %eax,%ebx` 证明 ORIG 为 i→%ecx / 值→%ebx（N7+N18a 角色互换）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N7/N78）→ 试=指针衰减形 `*(out->m_field10+i)`（E1 实测更劣：写回退化 `mov %ax,(%ecx)`，已回滚）+ 前人 8 形（含 P339 声明上移、4.4.6 复测同形）⇒ 维持（case9 角色互换：ORIG 0x0831e985 `mov -0x8(%ebp),%ecx` / 0x0831e99b `mov %eax,%ebx`）。
void CCalcCharacStat::ApplyElixerItem(_Additioal_info* out, int statIdx, int value)
{
    // [b27 封顶] 残差仅 case9 循环体寄存器角色（ORIG arr值→ebx/i基址重读→ecx，
    // 4.4 值→ecx/i→ebx+lea 直算——N7 族角色互换）+ jump table 基址 reloc。
    // switch/各 case/store 序全等。
    switch (statIdx)
    {
    case 4:
        out->m_field08 = static_cast<unsigned short>(
            out->m_field08 + value * 0x32);
        break;
    case 6:
        out->m_field0c = static_cast<unsigned short>(
            out->m_field0c + value * 0x32);
        break;
    case 5:
        out->m_field0a = static_cast<unsigned short>(
            out->m_field0a + value * 0x32);
        break;
    case 7:
        out->m_field0e = static_cast<unsigned short>(
            out->m_field0e + value * 0x32);
        break;
    case 2:
        out->m_field00 += value * 250;                       // imul $0xfa
        break;
    case 3:
        out->m_field04 += value * 250;
        break;
    case 8:
        out->m_field42 += value * 10;
        break;
    case 9:
        // [2026-09-10 D06 · Main 新轴（const/符号性）实测不适用] ORIG 0x0831e995 段
        // `movzwl …,%eax; mov %eax,%ebx`（元素值升格 callee-saved %ebx、i 留 %ecx +
        // `add $0x8,%ecx`）；去 cast 形与复合赋值形实测与 cast 形**同值**（119 vs 121，
        // 差异恒为寄存器角色互换）⇒ 循环变量符号性（两侧同为有符号 setle）与 cv 限定
        // 符均非本件变量，维持 N7/N78 封顶。
        // [P3Wave4 试形 8·P339] 声明位置上移已试并复编为中性（119/121 不变），已回滚。
        for (int i = 0; i < 4; ++i)                          // 循环上界 cmp $0x3,setle
            out->m_field10[i] = static_cast<unsigned short>(
                out->m_field10[i] + value * 10);
        break;
    default:
        break;
    }
}