CDungeon_TowerOfDespair::CDungeon_TowerOfDespair()
{
    // ORIG C1/C2 0x08377606：先构造基类 CDungeon，再覆写 vtable 为 0x8c349f8
    //（唯一虚槽 _do_after_dungeon_start 指向派生实现）；成员构造由编译器生成。
}