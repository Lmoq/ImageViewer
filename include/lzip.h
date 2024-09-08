#ifndef _LIBZIP_
#define _LIBZIP_

#include <vector>
#include <string>
#include <zip.h>


class Libzip
{
    public:

    static char archive_path[ 256 ];
    static zip *archive;
    static int max_index;

    static std::vector<std::string> filenames;

    static bool open_archive( const char *path );
    static bool get_item_buffer( int index, std::vector<char> &out_buffer );
    static void close();
};

#endif