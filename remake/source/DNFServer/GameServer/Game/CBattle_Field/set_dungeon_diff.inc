// [b28d-solo-CBattle_Field] N97/P230 封顶（12/14）：4.4 char 形参恒 spill（`mov
// %al,-0x4`，ORIG 4.1 直读 `movzbl 0xc(%ebp),%edx`）+ if-conversion `cmova`
// （-march=i486 才消，本波不改路由表）+ N7 store 寄存器对调（ORIG 0x0830ed1e）。
void CBattle_Field::set_dungeon_diff(char diff)
{
    // [b28d-D43] ORIG 0x0830ed21：`movzbl 0xc(%ebp),%edx; cmp $0x4,%edx; jbe`
    // + clamp 臂 `push $0x4; pop %edx`（值恒驻寄存器、无形参槽写回）⇒ 原码是
    // 三目 clamp 表达式（P35/P77：ORIG 无 spill 槽即表达式右值直用）；改写
    // `if(..) diff=4;` 会引入 movb $4,-0x4 写回槽（12→15）。
    // [b28d-solo-CBattle_Field] 实测三形（当前 14 条为最短）：
    // ①外层 cast 三目=14 条（cmova 形）；②无外层 cast=16 条（双 movzbl+ja 分支，
    // 探针 /tmp/s18/p2.cpp a/d 与 TU 实测一致）；③if 写回形=15 条。
    // 残余不可消：4.4 对 char 形参恒 spill（`mov %al,-0x4`，ORIG 4.1 直读
    // `movzbl 0xc(%ebp),%edx`）+ cmov（P230：需 -march=i486，本波不改路由表）
    // + N7 store 寄存器对调 ⇒ N97/P230 封顶。
    // [M3FreeNext15B.M3S6AucAvBattle 2026-09-11] 本波再证（ORIG 0x0830ed1e，
    // 兜底反汇编 12 条：movzbl 0xc(%ebp),%edx / cmp $4,%edx / jbe / push $4;pop %edx /
    // nop / mov %edx,0x1a0(%eax)）：探针 /tmp/p_setdd/{probe,probe2}.cpp 共 26 形
    // （含新形 ②`(diff & 0xff)` 32 位比较域=14 条、`*(unsigned char*)&diff`=18 条、
    // int 域比较 16 条、<= 反向 16 条）实测最短仍 14 条 ⇒ 14 为 4.4 形下限，
    // 封顶维持。
    m_dungeonDiff =
        (unsigned char)((unsigned char)diff > 4 ? 4 : (unsigned char)diff);
}