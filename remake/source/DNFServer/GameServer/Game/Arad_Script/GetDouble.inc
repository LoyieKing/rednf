double Arad_Script::GetDouble(int row, int col)
{
    char* local_c;
    local_c = cells_ + (cols_ * row) * 0x80 + col * 0x80;
    return std::atof(local_c);
