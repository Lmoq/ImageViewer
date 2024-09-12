#include <iostream>
#include <filesystem>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShObjIdl.h>

#include <lzip.h>

namespace fs = std::filesystem;

zip *File_M::archive = NULL;
std::vector<std::string> File_M::filenames;

std::string File_M::archive_path = "";
int File_M::max_index = 0;


HRESULT File_M::open_folder_dialog( std::string &result )
{
    HRESULT hr;
    IFileDialog *pfd = NULL;

    // Create File Open Dialog object
    if ( FAILED( CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pfd ) ) ) ) 
    {
        std::cout << "CoCreateInstance Failed\n";
        return hr;
    }

    // Retrieve the options
    DWORD dwFlags;
    if ( FAILED( pfd->GetOptions( &dwFlags ) ) ) 
    {
        pfd->Release();
        std::cout << "GetOptions Failed\n";
        return hr;
    }

    // Add option to set folder dialog
    if ( FAILED( pfd->SetOptions( dwFlags | FOS_PICKFOLDERS ) ) ) 
    {
        pfd->Release();
        std::cout << "SetOptions Failed\n";
        return hr;
    }

    // Show the dialog
    if ( FAILED( pfd->Show( NULL ) ) ) 
    {
        pfd->Release();
        std::cout << "Show Failed\n";
        return hr;
    }

    IShellItem *pResult = NULL;
    if ( FAILED( pfd->GetResult( &pResult ) ) )
    {
        pfd->Release();
        return hr;
    }

    // Retrive the folder path from shell item
    PWSTR wFolder = NULL;
    if ( FAILED( pResult->GetDisplayName( SIGDN_FILESYSPATH, &wFolder ) ) )
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
    result.resize( bufferSize, 0 );
    WideCharToMultiByte( CP_UTF8, 0, wFolder, -1, result.data(), bufferSize, NULL, NULL );

    // Release Memory
    CoTaskMemFree( wFolder );
    pResult->Release();
    pfd->Release();
    
    return hr;
}

// Set class member archive, max_index and populate filenames list
bool File_M::open_archive( const char *path )
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
    max_index --;
    std::sort( filenames.begin(), filenames.end() );
    
    return true;
}

bool File_M::get_item_buffer( int index, std::vector<char> &out_buffer )
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

void File_M::close()
{
    if ( archive != NULL ) {
        zip_close( archive );
    }
}




