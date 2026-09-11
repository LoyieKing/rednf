// ---- LoadPurcahseBonusScript —— ORIG 0x08179eae ----
// 委托 m_bonus.LoadScript()；ORIG 形态为 call 后 xor $1、test/je 双常返回
// （if (!LoadScript()) return false; return true;）。
bool ItemVendingMachine::LoadPurcahseBonusScript()
