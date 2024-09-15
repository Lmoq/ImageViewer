#ifndef _LIBZIP_
#define _LIBZIP_

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <zip.h>

namespace fm
{
    inline static std::unordered_set<std::string> image_extensions = {
        ".jpg",
        ".png",
        ".jpeg"
    };

    typedef enum {
        ARCHIVED,
        DIRECTORY
    } enum_series_type;

    HRESULT open_folder_dialog( std::string &result );

    class Chapter
    {
        public:

        static zip *archive;
        static std::string path;

        static std::vector<std::string> file_names;
        static int num_index;

        static bool open_archive( const char *path );
        static bool get_index_buffer( int index, std::vector<char> &out_buffer );
    };

    class Series
    {
        public:
        
        static std::string path;
        static enum_series_type chapter_type;

        static std::unordered_map<float, std::string> chapter_map;

        static enum_series_type check_series_type();
        static bool open_series( const char *path );
        static void sort_chapters();

        static void close();
    };
};

#endif