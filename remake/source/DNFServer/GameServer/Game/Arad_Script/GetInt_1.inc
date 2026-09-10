int Arad_Script::GetInt(char* row, char* col)
{
    return GetInt(GetIdx4Row(row), GetIdx4Col(col));
