void TowerOfDespairMgr::_Set_LayerByDungeonIndex(unsigned int layer, unsigned int dungeonIdx)
{
    // ORIG 0x08643c86：写 uint 表；set.insert(idx)；find_dungeon 命中转调重载。
    m_dungeonIndexOfLayer[layer] = dungeonIdx;
    m_layerSet.insert(static_cast<int>(dungeonIdx));
    // 技巧 37 免槽直用：ORIG 0x08643c?? 把 find_dungeon 的 %eax 直接写进
    // 下一 call 的 0x4(%esp)，**没有** `mov %eax,-slot; mov -slot,%eax` 往返
    // （我们具名 dungeon 局部造出该往返，多 2 条）。
    _Set_LayerByDungeonIndex(layer,
                             G_CDataManager()->find_dungeon(static_cast<int>(dungeonIdx)));
