// ORIG _ZN13user_creature9CCreature9DismantleEP5CUser T 0x08337382：
// SetState(2) 后恒返回 1。
bool CCreature::Dismantle(CUser* user)
{
    SetState(user, CREATURE_STATE_2);
    return true;
}
