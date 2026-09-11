bool CDungeon::isTowerOfDespairDungeon() const
{
    // ORIG 0x0822b4fa：return GetType_DungeonInHeritance() == 1。
    // [b28c-21] ORIG `sub $0x18` 保守帧；4.4 对「TU 内已见定义的 callee」精确
    // 继承 $0x4（P210 三态②）⇒ 把 GetType_DungeonInHeritance 定义移到本函数
    // 之后（本 TU 仅此处调用它），4.4 只见声明 ⇒ 恢复 $0x18（P210 三态③）。
    return GetType_DungeonInHeritance() == 1;
}