void Arad_Script::SetInt(int row, char* col, int value)
{
    SetInt(row, GetIdx4Col(col), value);
}