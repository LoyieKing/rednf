void Arad_Dispatcher::unregister_dispatcher(int code)
{
    // ORIG 0x0818e87c：GlobalInstance<Manager>::inst()->unregister_dispatcher(code)
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->unregister_dispatcher(code);
