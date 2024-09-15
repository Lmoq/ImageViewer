#include <iostream>
#include <filesystem>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShObjIdl.h>

#include <lzip.h>

namespace fs = std::filesystem;
using namespace fm;


HRESULT fm::open_folder_dialog( std::string &result )
{
    HRESULT hr;
    IFileDialog *pfd = NULL;

    // Create File Open Dialog object
    if ( FAILED( hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pfd ) ) ) ) 
    {
        std::cout << "CoCreateInstance Failed\n";
        return hr;
    }

    // Retrieve the options
    DWORD dwFlags;
    if ( FAILED( hr = pfd->GetOptions( &dwFlags ) ) ) 
    {
        pfd->Release();
        std::cout << "GetOptions Failed\n";
        return hr;
    }

    // Add option to set dialog for folders
    if ( FAILED( hr = pfd->SetOptions( dwFlags | FOS_PICKFOLDERS ) ) ) 
    {
        pfd->Release();
        std::cout << "SetOptions Failed\n";
        return hr;
    }

    // Show the dialog
    if ( FAILED( hr = pfd->Show( NULL ) ) ) 
    {
        pfd->Release();
        std::cout << "Show Failed\n";
        return hr;
    }

    // Results will be stored in IShellItem object after user clicked open button
    IShellItem *pResult = NULL;
    if ( FAILED( hr = pfd->GetResult( &pResult ) ) )
    {
        pfd->Release();
        return hr;
    } 

    // Retrive the folder path from result
    PWSTR wFolder = NULL;
    if ( FAILED( hr = pResult->GetDisplayName( SIGDN_FILESYSPATH, &wFolder ) ) )
    {
        std::cout << "Failed GetDisplayName\n";
        pResult->Release();
        pfd->Release();
        return hr;
    }

    // Convert wide string to UTF-8
    int bufferSize = WideCharToMultiByte( CP_UTF8, 0, wFolder, -1, NULL, 0, NULL, NULL );
    if( bufferSize <= 0 ) 
    {
        std::cout << "WchartoByte failed\n";
        return hr;
    }

    // Resize result string to contain converted wide string
    result.clear();
    result.resize( bufferSize, 0 );
    WideCharToMultiByte( CP_UTF8, 0, wFolder, -1, result.data(), bufferSize, NULL, NULL );

    // Release Memory
    CoTaskMemFree( wFolder );
    pResult->Release();
    pfd->Release();
    
    return hr;
}

// Series
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

            buffer.resize( stat.size );
            if ( zip_fread( f, buffer.data(), stat.size ) < 0 )
            {
                std::cout << "Fread failed\n";
                zip_fclose( f );
                zip_close( arch );
            }
            rapidxml::xml_document<> xml;
            xml.parse<0>( buffer.data() );

            char *c = xml.first_node( "ComicInfo" )->first_node( "Number" )->value();

            zip_fclose( f );
            zip_close( arch );
            std::cout << "Node : " << c << '\n';
        }
    }
    }
}


// Chapter
zip *Chapter::archive = NULL;
std::string Chapter::path;
std::vector<std::string> Chapter::file_names;

int Chapter::num_index;

// Set class member archive, max_index and populate chapter.file_names list
bool Chapter::open_archive( const char *path_ )
{
    archive = zip_open( path_, ZIP_RDONLY, NULL );
    if ( !archive ) {
        return false;
    }

    // Get archive total files
    int num_files = zip_get_num_entries( archive, 0 );
    if ( num_files <= 0 ) {
        return false;
    }

    // Retrieve file names inside archive
    zip_stat_t stat = {};
    for ( int i = 0; i < num_files; i ++ )
    {
        if ( zip_stat_index( archive, i, 0, &stat ) == -1 )
        {
            zip_close( archive );
            return false;
        }
        // List images only
        if ( image_extensions.count( fs::path( stat.name ).extension().string() ) > 0 )
        {
            file_names.push_back( stat.name );
            num_index ++;
        }
    }
    num_index --;
    std::sort( file_names.begin(), file_names.end() );

    Chapter::path = path_;
    
    return true;
}

// Retrieve image buffer from current opened archive
bool Chapter::get_index_buffer( int index, std::vector<char> &out_buffer )
{
    // Open item from archive through index
    zip_file *file = zip_fopen( archive, file_names[ index ].c_str(), ZIP_RDONLY );
    if ( !file ) 
    {
        zip_fclose( file );
        std::cout << "Failed to open file : " << file_names[ index ] << '\n';

        return false;
    }

    // Get stat to retrieve buffer size
    zip_stat_t stat = {};
    if ( zip_stat( archive, file_names[ index ].c_str(), ZIP_RDONLY, &stat ) != 0 ) 
    {
        zip_fclose( file );
        std::cout << "Failed to read stats : " << file_names[ index ] << '\n';

        return false;
    }

    // Extract to referenced buffer
    out_buffer.resize( stat.size );
    if ( zip_fread( file, out_buffer.data(), stat.size ) <= 0 ) 
    {
        zip_fclose( file );
        std::cout << "Failed to read file : " << file_names[ index ] << '\n';

        return false;
    }

    zip_fclose( file );
    return true;
}



// Clean Up
void Series::close()
{
    if ( Chapter::archive != NULL ) {
        zip_close( Chapter::archive );
    }
}




