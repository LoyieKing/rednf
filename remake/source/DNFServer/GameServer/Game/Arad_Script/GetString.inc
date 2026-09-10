char* Arad_Script::GetString(int row, int col)
{
    return cells_ + (cols_ * row) * 0x80 + col * 0x80;
