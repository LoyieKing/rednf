// ORIG T 0x08334844：空体 C1（push/mov/pop/ret；C1/C2 同址 T，类无虚函数）。
// 调用点：CGameManager::GetConditionEventManager 的 placement new。
// 置于 TU 首个成员定义位，使 _GLOBAL__I_ 键名按 C2Ev（匹配 ORIG 键名，P108）。
CConditionEventManager::CConditionEventManager()
{
}