RentalMachinePcRoom::RentalMachinePcRoom() : m_impl(new RentalMachinePcRoomImpl)
{
    // ORIG 0x82703c0：IRentalMachine 基类 + vptr；new Impl(0x11c) 存 auto_ptr@+4；
    // initLoggingEnvironment()。
    initLoggingEnvironment();
