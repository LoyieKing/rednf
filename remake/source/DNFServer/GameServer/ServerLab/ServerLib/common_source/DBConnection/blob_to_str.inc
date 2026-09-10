char* DBConnection::blob_to_str(int idx, void* blob, int size)
{
    if (((idx < 0) || (9 < idx)) || (blob == (void*)0) || (0x5fff < size))
    {
        G_TraceLog()->errorLog("set SESSION wait_timeout Error");
    }
    temp_buf[idx][0] = '\0';
    if (0 < size)
    {
        char* p = temp_buf[idx];
        p = p + (int)mysql_real_escape_string(h_db, p, (const char*)blob, size);
        *p = '\0';
        p = p + 1;
    }
    return temp_buf[idx];
