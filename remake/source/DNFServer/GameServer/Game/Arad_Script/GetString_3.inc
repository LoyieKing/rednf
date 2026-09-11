char* Arad_Script::GetString(int row, char* col)
{
    return GetString(row, GetIdx4Col(col));
}