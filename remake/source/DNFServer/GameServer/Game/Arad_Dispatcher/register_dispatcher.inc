void Arad_Dispatcher::register_dispatcher(Arad_Dispatcher* d)
{
    // ORIG 0x0818e860：GlobalInstance<Manager>::inst()->register_dispatcher(d)
    GlobalInstance<Arad_Dispatcher_Manager>::inst()->register_dispatcher(d);
