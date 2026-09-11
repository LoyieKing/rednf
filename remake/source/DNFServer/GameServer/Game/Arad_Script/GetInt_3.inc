int Arad_Script::GetInt(int row, char* col)
{
    return GetInt(row, GetIdx4Col(col));
}