#include <iostream>
#include <image.h>
#include <lzip.h>

using namespace fm;

// Media
int ImageViewer::pageIndex = -1;
bool ImageViewer::previewNextChapter = true;

sf::Text ImageViewer::previewText;
sf::Font ImageViewer::defaultFont;

// Load image texture from file path
bool ImageViewer::setImagefromPath( const std::string &filepath )
{
    if ( !texture.loadFromFile( filepath ) ) {
        return false;
    }
    texture.setSmooth( true );
    sprite.setTexture( texture );

    return true;
}

// Load image texture from buffer
bool ImageViewer::setImagefromBuffer( std::vector<char> &buffer )
{
    texture = sf::Texture();
    sprite = sf::Sprite();

    if ( !texture.loadFromMemory( buffer.data(), buffer.size() ) ) {
        return false;
    }
    texture.setSmooth( true );
    sprite.setTexture( texture );

    return true;
}

bool ImageViewer::loadImageFromIndex( int index )
{
    std::vector<char> img_buffer;
    // Retrive buffer of the selected index from the current opened archive 
    if ( !Series::get_index_buffer( index, img_buffer ) )
    {
        std::cout << "Failed to get buffer\n";
        return false;
    }
    if ( !ImageViewer::setImagefromBuffer( img_buffer ) )
    {
        std::cout << "Failed to load texture from buffer\n";
        return false;
    }
    return true;
}

void ImageViewer::hideWindow()
{
    ShowWindow( windowHandle, SW_HIDE );
    windowDisplayed = FALSE;
}

void ImageViewer::showWindow()
{
    ShowWindow( windowHandle, SW_SHOW );
    windowDisplayed = TRUE;

    if ( previewNextChapter ) {
        drawChapterPreview();
    }
}

void ImageViewer::nextPage()
{
    int new_index = pageIndex + 1;
    // Display info for next chapter if page number exceeded chapter pages
    if ( new_index > Series::opened_chapter->maxIndex )
    {
        // Retrive info for next chapter and display preview
        previewNextChapter = true;
        pageIndex = Series::opened_chapter->maxIndex + 1;

        view.setSize( window.getDefaultView().getSize() );
        view.setCenter( window_width / 2.0, window_height / 2.0 );
        window.setView( view );

        return;
    }
    else {
        previewNextChapter = false;
    }
    
    // Display next page
    pageIndex = new_index;
    if ( !loadImageFromIndex( pageIndex ) ) {
        return;
    }
    default_view_scale = getImageFitScale( texture );
    current_view_scale = default_view_scale;

    setViewZoom( default_view_scale, true );
    keepImage();
}

void ImageViewer::prevPage()
{
    int new_index = pageIndex - 1;
    // Display info for previous chapter if page number subceeded minimum page no.
    if ( new_index < 0 ) 
    {
        // Should be last chapter
        previewNextChapter = true;
        pageIndex = -1;

        view.setSize( window.getDefaultView().getSize() );
        view.setCenter( window_width / 2.0, window_height / 2.0 );
        window.setView( view );

        // drawChapterPreview();
        return;
    }
    else {
        previewNextChapter = false;
    }
  
    // Display previous page
    pageIndex = new_index;
    if ( !loadImageFromIndex( pageIndex ) ) {
        return;
    }
    default_view_scale = getImageFitScale( texture );
    current_view_scale = default_view_scale;

    setViewZoom( default_view_scale, true );
    keepImage();
}

void ImageViewer::drawChapterPreview()
{
    // Parse Next Chapter ComicInfo.xml
    previewText.setString( "Title : \nChapter : \nTranslator : " );
    previewText.setCharacterSize( 30 );
    
    previewText.setPosition( (window_width - previewText.getLocalBounds().width) / 2, 10 );
}