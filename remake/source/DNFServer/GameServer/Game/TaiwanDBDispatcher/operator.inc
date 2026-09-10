void TaiwanDBDispatcher::FnDelete::operator()(
    std::pair<const TaiwanInternalPack::T, TaiwanInterfaceDBDispatcher*>& item)
{
    /* N7 封顶（b26 NL26）：唯一残差为虚 deleting-dtor 调用的 callee 寄存器
     * 互换（ORIG vfn=%edx vs 4.4 恒 %eax，临时/引用/限定名均无效）。 */
    delete item.second;   // ORIG @0x0815dfce：test+je + vtable D0 调用
}
