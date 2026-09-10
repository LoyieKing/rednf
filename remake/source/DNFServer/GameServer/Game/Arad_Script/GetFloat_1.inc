float Arad_Script::GetFloat(char* row, char* col)
{
    return GetFloat(GetIdx4Row(row), GetIdx4Col(col));
