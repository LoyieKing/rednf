// ORIG 0x08307b2a：相邻格（含斜向禁止）越界检查。
// [b28d-solo-BattleMonsterDepsC2] P0（已修）：ORIG 无 MapInfo* 具名局部 ⇒ 两处调用内联
// （消 -0x1c 槽，帧 0x5c→0x4c 与 ORIG 全等）。
// 残差判 N87/N18a：ORIG 绝对值段=单赋值寄存器链 8 条（`sub %eax,%edx; mov %edx,%eax;
// sar $0x1f,%eax; xor %eax,%edx; mov %edx,-0x20; sub %eax,-0x20`）；4.4 两语句形先存槽后
// 读回且把 `-s` 归约为 `+(s&1)`（14 条/处）、单表达式展开 3 次（115）、裸 register 免槽但
// 使 curX/curY 槽浅 4（99，与 ORIG 有槽往返的 P0 证据冲突）。条件块第 4 跳 4.4 恒反极 je。
// 三档类型旋钮（P248）试尽，封顶。
int CBattle_Field::check_movemap_range(int x, int y)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    // [b28f-L27] ORIG 0x08307b5a..0x08307b83 为 abs() 内建内联展开
    //   （`mov %edx,%eax; sar $31,%eax; xor %eax,%edx; mov %edx,slot;
    //     sub %eax,slot`）⇒ 原码写 abs() 而非手写位运算（手写两句会先落槽
    //     再重读，+7 条/处）。
    int dx = abs(x - curX);
    int dy = abs(y - curY);
    // [b28f-L27] ORIG 块序 = 主流程(logFormat)+`mov $0x15`+`jmp epi` 在前、
    //   共享 `mov $0,%eax` 尾块在最后 ⇒ 源码是**正向包裹**（P222/P257）：
    //   条件取反包住主流程，最后的裸 return 0 自然落尾并被两条 `jne` 共享。
    if (dx > 1 || dy > 1 || (dx == 1 && dy == 1))
    {
        LogManager::logFormat(
            1, "battle_field.cpp", __PRETTY_FUNCTION__,
            0x119f,
            "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
            "[Map Kind : %s]",
            m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
            x, y,
            GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
        return 0x15;
    }
    return 0;
}