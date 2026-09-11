// [b28d-solo-GameWorld] _ZNK9GameWorld21get_area_center_pointEiiRiS0_ verdict=AE 80=80
// 修法：P169 外层正向 if 包内层 goto PASS（两失败路径共享后置 RET0 岛，见函数内注释）
// ⇒ 4.4 复现 ORIG `js RET0; mov MAX,%eax; cmp; jl PASS; RET0: mov $0; jmp end`。
bool GameWorld::get_area_center_point(int vill, int area, int& x, int& y) const
{
    // ORIG 0x86c4724-0x86c4732：`js RET0; mov MAX,%eax; cmp %eax,0xc; jl PASS`
    // 第二条件 taken=continue（技巧23 双 goto 钉块序）。
    // 关键：两条失败路径必须**共享同一个 RET0 岛**且该岛位于第二条件之后
    // （ORIG 0x86c4724 `js RET0` → 0x86c4729 `mov MAX,%eax` / 0x86c472e
    // `cmp %eax,0xc` / 0x86c4730 `jl PASS` → RET0: `mov $0x0,%eax; jmp end`）。
    // 写成 `if (vill < 0) { return false; }` 会让 4.4 在原地物化 `mov $0;jmp`
    // （多 2 条）；必须用 goto 把首条失败出口也指向后置于第二条件的 RET0 岛。
    // 变体：外层正向 if（假路径 fall-through 到尾 return），内层唯一 goto PASS
    // ⇒ 期望 ORIG 0x86c4724 `js 岛` + 0x86c4730 `jl PASS` + 岛 `mov $0;jmp end`
    //   且两 return 合并为同一物化块（上一版 `goto RET0` 会多留一条 jmp）。
    if (vill >= 0)
    {
        if (vill < MAX_VILLAGE_NUM)
            goto PASS;
    }
    return false;
PASS:
    // ORIG 0x86c4756 #1：getArea 结果不判空直 `add $0x7c` 调 empty()（每次内联重取，无具名 pArea）
    if (m_pVill[vill].getArea(area)->m_mapAreas.empty())
    {
        return false;
    }
    // b28c-lane3：历史 register bool asm("al") 绑定按 P0/N96 清除（自然形）。
    // ORIG 0x86c4791-0x86c4798 #2：`test %eax,%eax; sete %al; test %al,%al; je`。
    if (m_pVill[vill].getArea(area) == 0)
    {
        return false;
    }
    // ORIG 0x86c47d1 #3：at(0) 结果落具名引用槽 -0xc（引用必物化）
    MapArea& mapArea = m_pVill[vill].getArea(area)->m_mapAreas.at(0);
    x = mapArea.m_x + mapArea.m_w / 2;
    y = mapArea.m_y + mapArea.m_h / 2;
    return true;
}