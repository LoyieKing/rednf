// ---------------------------------------------------------------------------
// loadFromResource —— ORIG T 0x08ad4dfa（size 0x0a）：空函数（push ebp /
// mov $0,%eax / pop ebp / ret）——真实 ORIG 即恒返 false（资源型装载入口
// 在 ORIG 中未启用），转写为空实现。签名 (void*, int, CharString*)。
// ---------------------------------------------------------------------------
bool StringResource::loadFromResource(void*, int, CharString*)
{
    return false;
}