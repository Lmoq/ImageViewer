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

// Clean Up
void Series::close()
{
    if ( Chapter::archive != NULL ) {
        zip_close( Chapter::archive );
    }
}




