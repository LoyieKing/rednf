bool Arad_Dispatcher_Manager::open()
{
    // ORIG 0x0818fe38：operator new(8) → BuyItemCheck_Impl ctor → 注册 → return 0。
    Arad_Dispatcher_BuyItemCheck_Impl* impl =
        new Arad_Dispatcher_BuyItemCheck_Impl();
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->register_dispatcher(impl);
    return false;
