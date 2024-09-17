#include <iostream>
#include <filesystem>
#include <lzip.h>

// Series Classs
namespace fs = std::filesystem;
using namespace fm;

std::string Series::path;
enum_series_type Series::chapter_type;
std::unordered_map<float, std::string> Series::chapter_map;


bool Series::open_series( const char *path_ )
{
    fs::path s_path( path_ );
    if ( !fs::exists( s_path ) ) {
        std::cout << "Path error : " << path_ << '\n';
        return false;
    }

    Series::path = path_;
    chapter_type = check_series_type();

    // Populate series chapter file paths
    Series::sort_chapters();

    // switch ( chapter_type )
    // {
    //     case ARCHIVED:
    //         Chapter::open_archive( chapter_map[ 0 ].c_str() );
    //         break;
    // }
    return true;
}

// Iterate through series path files
// Determine file types if archived or structured chapter folders
enum_series_type Series::check_series_type()
{
    int file_count = 0; 
    int dir_count = 0;
    for ( auto &p : fs::directory_iterator( fs::path ( path ) ) )
    {
        if ( p.path().has_extension() )
        {
            file_count ++;
        }
        else
        {
            if ( fs::is_directory( p.path() ) )
            {
                dir_count ++;
            }
        }
    }
    return ( file_count > dir_count ) ? ARCHIVED : DIRECTORY;
}

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

void Series::sort_chapters()
{
    if ( chapter_type == ARCHIVED )
    {
        // Sort chapters through xml files
        for ( auto &p : fs::directory_iterator( fs::path( path ) ) )
        {
            if ( p.path().extension() == ".cbz" ) 
            {
                zip *arch = zip_open( p.path().string().c_str(), ZIP_RDONLY, NULL );
                if ( !arch ) {
                    std::cout << "Failed to open arch\n";
                    continue;
                }

                std::vector<char> buffer;
                buffer.clear();

                zip_stat_t stat = {};
                if ( zip_stat( arch, "ComicInfo.xml", ZIP_RDONLY, &stat ) != 0 )
                {
                    std::cout << "Stat failed\n";
                    zip_close( arch );
                    continue;
                }

                zip_file *f = zip_fopen( arch, "ComicInfo.xml", ZIP_RDONLY );
                if ( !f ) {
                    std::cout << "Fopen failed\n";
                    zip_close( arch );
                    continue;
                }

                buffer.resize( stat.size + 1 );
                if ( zip_fread( f, buffer.data(), stat.size ) < 0 )
                {
                    std::cout << "Fread failed\n";
                    zip_fclose( f );
                    zip_close( arch );
                }
                // Data should be 0 terminated
                buffer[ buffer.size() - 1 ] = 0;

                rapidxml::xml_document<> xml;
                xml.parse<0>( buffer.data() );

                rapidxml::xml_node<> *ComicInfo = xml.first_node( "ComicInfo" ); 

                char *title = ComicInfo->first_node( "Title" )->value();
                char *chapter_number = ComicInfo->first_node( "Number" )->value();
                char *translator = ComicInfo->first_node( "Translator" )->value();

                std::cout << "Title : " << title << '\n';
                std::cout << "Chapter Number : " << chapter_number << '\n';
                std::cout << "Translator : " << translator << "\n\n";

                zip_fclose( f );
                zip_close( arch );
            }
        }
    }
}

