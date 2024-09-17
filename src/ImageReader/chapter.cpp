#include <iostream>
#include <filesystem>
#include <lzip.h>

namespace fs = std::filesystem;
using namespace fm;

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
