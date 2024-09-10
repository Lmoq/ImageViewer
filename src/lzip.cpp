#include <lzip.h>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

zip *Libzip::archive = NULL;
std::vector<std::string> Libzip::filenames;

char Libzip::archive_path[ 256 ] = "";
int Libzip::max_index = 0;

// Set class member archive, max_index and populate filenames list
bool Libzip::open_archive( const char *path )
{
    archive = zip_open( path, ZIP_RDONLY, NULL );
    if ( !archive ) {
        return false;
    }

    int num_files = zip_get_num_entries( archive, 0 );
    if ( num_files <= 0 ) {
        return false;
    }

    zip_stat_t stat = {};
    for ( int i = 0; i < num_files; i ++ )
    {
        if ( zip_stat_index( archive, i, 0, &stat ) == 0 &&
             std::string( ".jpg .png .jpeg " ).find( fs::path( stat.name ).extension().string() ) != std::string::npos ) 
        {
            filenames.push_back( stat.name );
            max_index ++;
        }
    }
    std::sort( filenames.begin(), filenames.end() );
    
    return true;
}

bool Libzip::get_item_buffer( int index, std::vector<char> &out_buffer )
{
    zip_file *file = zip_fopen( archive, filenames[ index ].c_str(), ZIP_RDONLY );
    if ( !file ) 
    {
        zip_fclose( file );
        std::cout << "Failed to open file : " << filenames[ index ] << '\n';

        return false;
    }

    zip_stat_t stat = {};
    if ( zip_stat( archive, filenames[ index ].c_str(), ZIP_RDONLY, &stat ) != 0 ) 
    {
        zip_fclose( file );
        std::cout << "Failed to read stats : " << filenames[ index ] << '\n';

        return false;
    }

    out_buffer.resize( stat.size );
    if ( zip_fread( file, out_buffer.data(), stat.size ) <= 0 ) 
    {
        zip_fclose( file );
        std::cout << "Failed to read file : " << filenames[ index ] << '\n';

        return false;
    }

    zip_fclose( file );
    return true;
}

void Libzip::close()
{
    if ( archive != NULL ) {
        zip_close( archive );
    }
}




