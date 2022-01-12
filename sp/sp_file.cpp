#include "sp_file.h"
#include "sp_log.h"

#include <fstream>
#include <sstream>

std::string sp_read_file(const char* path)
{
    std::ifstream t(path);
    if (!t.is_open())
        sp_log_crit("Failed to open file with path %s", path);
    std::stringstream ss;
    ss << t.rdbuf();
    t.close();
    return ss.str();
}

void sp_write_file(const char* path, void* data, i64 size)
{
    std::ofstream t(path);
    t.write((char*)data, std::streamsize(size));
    t.close();
}