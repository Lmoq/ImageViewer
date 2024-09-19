#include <iostream>
#include <filesystem>
#include <lzip.h>

namespace fs = std::filesystem;
using namespace fm;

// Chapter
chapter_t *Series::opened_chapter = NULL;


// Series::open_directory() should be called first before calling this function
/// \param index Index from the chapter_list to be initialize
bool Series::open_archive( int index )
{
    chapter_t *chp;
    try 
    {
        chp = &chapter_list.at( index );
    }
    catch ( std::out_of_range &ex ) 
    {
        std::cout << "Exception occured : " << ex.what() << '\n';
        return false;
    }
    // opened_chapter_archive
    chp->archive = zip_open( chp->path.c_str(), ZIP_RDONLY, NULL );
    if ( !chp->archive ) {
        return false;
    }

    // Get opened_chapter_archive total files
    int num_files = zip_get_num_entries( chp->archive, 0 );
    if ( num_files <= 0 ) {
        return false;
    }
    chp->maxIndex = -1;

    // Retrieve file names inside chp->archive
    zip_stat_t stat = {};
    for ( int i = 0; i < num_files; i ++ )
    {
        if ( zip_stat_index( chp->archive, i, 0, &stat ) == -1 )
        {
            zip_close( chp->archive );
            return false;
        }
        // List images only
        if ( image_extensions.count( fs::path( stat.name ).extension().string() ) > 0 )
        {
            chp->fileNames.push_back( stat.name );
            chp->maxIndex ++;
        }
    } 
    std::sort ( 
        chp->fileNames.begin(), 
        chp->fileNames.end(), 
        []( const char *s1, const char *s2 ) { return atoi( s1 ) < atoi(s2); } 
    );
    opened_chapter = chp;

    std::cout << chp->title << " files\n";
    for ( auto &p : chp->fileNames )
    {
        std::cout << p << '\n';
    }

    return true;
}

// Retrieve image buffer from current opened archive
bool Series::get_index_buffer( int index, std::vector<char> &out_buffer )
{
    // Open item from archive through index
    zip_file *file = zip_fopen ( 
        opened_chapter->archive, 
        opened_chapter->fileNames[ index ], ZIP_RDONLY 
    );
    if ( !file ) 
    {
        zip_fclose( file );
        std::cout << "Failed to open file : " << opened_chapter->fileNames[ index ] << '\n';
        return false;
    }

    // Get stat to retrieve buffer size
    zip_stat_t stat = {};
    if ( zip_stat( opened_chapter->archive, opened_chapter->fileNames[ index ], ZIP_RDONLY, &stat ) != 0 ) 
    {
        zip_fclose( file );
        std::cout << "Failed to read stats : " << opened_chapter->fileNames[ index ] << '\n';
        return false;
    }

    // Extract to referenced buffer
    out_buffer.resize( stat.size );
    if ( zip_fread( file, out_buffer.data(), stat.size ) <= 0 ) 
    {
        zip_fclose( file );
        std::cout << "Failed to read file : " << opened_chapter->fileNames[ index ] << '\n';
        return false;
    }

    std::cout << "Retrived buffer for " << opened_chapter->fileNames[ index ] << '\n';

    zip_fclose( file );
    return true;
}
