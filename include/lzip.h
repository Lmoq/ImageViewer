#ifndef _LIBZIP_
#define _LIBZIP_

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <zip.h>
#include <rapidxml.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace fm
{
    inline static std::unordered_set<std::string> image_extensions = {
        ".jpg",
        ".png",
        ".jpeg",
        ".webp"
    };

    typedef enum {
        ARCHIVED,
        DIRECTORY
    } enum_series_type;

    typedef struct {
        int pageIndex;
        int maxIndex;

        zip *archive = NULL;
        std::vector<const char *> fileNames;
        
        float number;
        std::string title;
        std::string path;
        std::string translator;
    } chapter_t;

    class Series
    {
        public:
        // Series
        static std::string path;
        static enum_series_type series_type;
        static std::vector<chapter_t> chapter_list;

        static enum_series_type check_series_type();

        static bool open_directory( const char *path );
        static bool sort_chapters();

        // Chapter
        static chapter_t *opened_chapter;

        static bool open_archive( int index );
        static bool get_index_buffer( int index, std::vector<char> &out_buffer );

        // Clean up
        static void close();
    };
    HRESULT open_folder_dialog( std::string &result );
    bool get_chapter_xml_doc( const char *archive_path, rapidxml::xml_document<> &xml );
};

#endif