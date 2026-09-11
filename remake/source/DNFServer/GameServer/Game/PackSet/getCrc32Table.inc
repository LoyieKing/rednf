unsigned int* Crc32::getCrc32Table()
{
    if (crc_table[255] == 0)
    {
        make_crc_table();
    }
    return crc_table;
}