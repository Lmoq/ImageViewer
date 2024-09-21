#include <iostream>
#include <filesystem>
#include <lzip.h>

// Series Classs
namespace fs = std::filesystem;
using namespace fm;

// Series Directory
std::string Series::path;
enum_series_type Series::series_type;

// List containing sets of chapter number paired with chapter path
std::vector<chapter_t> Series::chapter_list;

bool Series::open_directory( const char *path_ )
{
    fs::path s_path( path_ );
    if ( !fs::exists( s_path ) ) {
        std::cout << "Path error : " << path_ << '\n';
        return false;
    }

    Series::path = path_;
    series_type = check_series_type();

    // Populate series chapter file paths
    if ( !Series::sort_chapters() ) {
        std::cout << "Failed to sort chapters\n";
        return false;
    }

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

#include <codecvt>
#include <locale>

bool compareChpNum( chapter_t &chp1, chapter_t &chp2 )
{
    return chp1.number < chp2.number;
}

bool Series::sort_chapters()
{
    if ( series_type == ARCHIVED )
    {
        // Sort chapters through xml files
        for ( auto &p : fs::directory_iterator( fs::path( path ) ) )
        {
            if ( p.path().extension() == ".cbz" )
            {
                // Retrieve xml data
                std::wstring chpwPath = p.path().wstring();

                std::wstring_convert< std::codecvt_utf8<wchar_t>> conv;
                std::string chpPath = conv.to_bytes( chpwPath );

                // WideCharToMultiByte( CP_ACP, 0, xmlfile.data(), -1, multiByte.data(), xmlfile.size(), NULL, NULL );
                // Contruct xml document from xml file contents
                rapidxml::xml_document<> xml;
                if ( !fm::get_chapter_xml_doc( chpPath.c_str(), xml ) ) {
                    std::cout << "Failed to retrive xml data : " << chpPath << '\n'; 
                    return false;
                }
                rapidxml::xml_node<> *ComicInfo = xml.first_node( "ComicInfo" );

                // Store chapter title, number, translator to list
                chapter_t ch;
                
                ch.path = chpPath;
                ch.number = atof( ComicInfo->first_node( "Number" )->value() );
                // ch.title = ComicInfo->first_node( "Title" )->value();
                ch.translator = ComicInfo->first_node( "Translator" )->value();
        
                chapter_list.push_back( ch );
            }
        }
        std::sort( chapter_list.begin(), chapter_list.end(), compareChpNum );
        for ( auto &chapter : chapter_list )
        {
            std::cout << "number : " << chapter.number 
                      << " path : " << chapter.path
                      << " translator : " << chapter.translator <<  '\n';
        }
    }
    return true;
}
