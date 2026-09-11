float Arad_Script::GetFloat(int row, int col)
{
    char* local_c;
    local_c = cells_ + (cols_ * row) * 0x80 + col * 0x80;
    return (float)std::atof(local_c);
}