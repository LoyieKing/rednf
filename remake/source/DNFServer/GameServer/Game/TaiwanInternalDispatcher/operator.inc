// [b28c-31] N7 复核：虚调用 callee ORIG %edx vs 4.4 %eax（18=18，仅寄存器名差）。
void TaiwanInternalDispatcher::FnDelete::operator()(
    std::pair<const TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*>& item)
{
    /* N7 封顶（b26 NL26）：同 TaiwanDBDispatcher::FnDelete 判词。 */
    delete item.second;   // ORIG @0x08161e9a：test+je + vtable D0 调用
}