void TowerOfDespairMgr::_Set_LayerByDungeonIndex(unsigned int layer, const CDungeon* dungeon)
{
    // ORIG 0x08643c6c：if (dungeon!=0) m_pLayerDungeon[layer]=dungeon。
    // P223：ORIG 尾空块 nop 岛（je <nop>; body; jmp <pop ebp>; nop）复现形 =
    // goto 尾标签（4.4 对 if/else 恒 jump-thread，不产该岛）。
    if (dungeon == 0)
        goto L_done;
    m_layerDungeons[layer] = dungeon;
L_done:;
}