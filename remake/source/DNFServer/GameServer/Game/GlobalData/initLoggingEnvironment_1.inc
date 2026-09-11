// [b28d-18 IDENTICAL_AE] 21=21 逐条同形 | ORIG 0x0826efdc
void RentalMachineDimension::initLoggingEnvironment()
{
    // ORIG 0x826efdc：同 PcRoom 日志路径（.rodata 0x8bf90c2 同串）。
    OS_API::snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
                     G_CEnvironment()->get_file_name());
}