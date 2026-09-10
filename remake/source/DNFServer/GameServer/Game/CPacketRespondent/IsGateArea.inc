bool CPacketRespondent::IsGateArea(CUser* user, int area)
{
    // ORIG 0x834025c：两指针槽 -0x10/-0xc 先置 0（探针实证：声明+后续独立赋值形才生成
    // 两条 init，`T* p=f()` 合并形缺 init）。条件的 sete/test/je 物化形系 4.1.2 O0
    // 对槽比零的产物（探针实证 4.4-O0 槽比恒出 cmp/jne 直跳，无法源形复现；
    // 不可改 call 内联比较——ORIG cmp 读槽、每 call 仅一次，内联=双调用真 bug）。
    GameWorld* world = 0;
    Village* village = 0;
    world = G_GameWorld();
    while (world == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x281, 5)("G_GameWorld call failed");
        return false;
    }
    village = world->getUserVillage(user);
    while (village == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x287, 5)("getUserVillage call failed");
        return false;
    }
    return village->get_gate_area() == area;
