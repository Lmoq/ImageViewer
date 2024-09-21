#include <iostream>
#include <ShObjIdl.h>
#include <lzip.h>

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

// Get archive xml data
bool fm::get_chapter_xml_doc( const char *path_, rapidxml::xml_document<> &xml )
{
    std::vector<char> buffer;
    int error = 0;

    // Open archive
    zip *arch = zip_open( path_, ZIP_RDONLY, &error );
    if ( !arch )
    {
        zip_error_t zip_error;
        zip_error_init_with_code( &zip_error, error );

        std::cout << "Archive failed to read : " << zip_error_strerror( &zip_error ) << '\n';
        zip_error_fini( &zip_error );
        return false;
    }

    // Get xml file stat to set buffer size
    zip_stat_t stat = {};
    if ( zip_stat( arch, "ComicInfo.xml", ZIP_RDONLY, &stat ) != 0 )
    {
        std::cout << "Stat failed\n";
        zip_close( arch );
        return false;
    }

    // Open xml file
    zip_file *f = zip_fopen( arch, "ComicInfo.xml", ZIP_RDONLY );
    if ( !f ) 
    {
        std::cout << "Fopen failed\n";
        zip_close( arch );
        return false;
    }

    // Extract xml data to buffer
    // + 1 to include 0 terminator
    buffer.resize( stat.size + 1 ); 
    if ( zip_fread( f, buffer.data(), stat.size ) < 0 )
    {
        std::cout << "Fread failed\n";
        zip_fclose( f );
        zip_close( arch );
        return false;
    }
    zip_fclose( f );
    zip_close( arch );

    // Data should be 0 terminated to be parsed in rapidxml
    buffer[ buffer.size() - 1 ] = 0;
    if ( buffer.size() == 0 || buffer[buffer.size() - 1] != 0 ) 
    {
        std::cout << "Failed to load xml data [ " << path_ << " ]\n" ;
        return false;
    }
    
    // Parse file contents to xml document
    xml.parse<0>( buffer.data() );

    return true;
}


// Clean Up
void Series::close()
{
    if ( Series::opened_chapter->archive != NULL ) {
        zip_close( Series::opened_chapter->archive );
    }
}




