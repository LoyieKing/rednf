bool _checkTimeoutItem(const Inven_Item& item);
// NumberToString（ORIG 0x810904b T，定义于 LinkFinalDeps.cpp）——失败日志用。
char* NumberToString(unsigned int value, int radix);
// GetIntegratedPvPItemAttr（ORIG 0x84fc5ff T）定义于 CDataManagerScripts.cpp。
unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
// DeleteInvalidItemScript::IsInvalidItem2Delete（ORIG 0x8ac2096）定义于
// CDataManagerScripts.cpp（类权威声明见 CDataManager.h，此处本地最小视图）。
