// ============================================================================
// Arad_Dispatcher_Manager::open —— ORIG 0x0818fe38。
// [b28 BM11] P210 定义前置翻案（推翻 b23「landing-pad 抑制不可复现」判词，与
// NexonCash C1 同法）：open 定义前置于 BuyItemCheck_Impl ctor 定义之前 ⇒ Impl
// ctor 本 TU 仅原型可见 ⇒ 4.4 对 new-expression 保守生成 EH cleanup
// （delete+Unwind_Resume 岛 + push %edi/%esi + 帧 0x2c/槽 -0x1c，ORIG 全形）。
// ============================================================================
bool Arad_Dispatcher_Manager::open()
{
    // ORIG 0x0818fe38：operator new(8) → BuyItemCheck_Impl ctor → 注册 → return 0。
    Arad_Dispatcher_BuyItemCheck_Impl* impl =
        new Arad_Dispatcher_BuyItemCheck_Impl();
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->register_dispatcher(impl);
    return false;
}