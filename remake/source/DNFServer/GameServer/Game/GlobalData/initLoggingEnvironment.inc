void RentalMachinePcRoom::initLoggingEnvironment()
{
    // ORIG 0x8270886：snprintf(impl, 0x103, "./log/%s/dw2_RentalItems", 文件名)。
    OS_API::snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
                     G_CEnvironment()->get_file_name());
}