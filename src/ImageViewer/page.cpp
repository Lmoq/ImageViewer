#include <iostream>
#include <image.h>
#include <lzip.h>

// Media
int pageOffset = 1;
int ImageViewer::pageNumber = 0;
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
    if ( !fm::Chapter::get_index_buffer( index, img_buffer ) )
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
    // Display info for next chapter if page number exceeded chapter pages
    if ( pageNumber + 1 > fm::Chapter::num_index - pageOffset ) 
    {
        // Retrive info for next chapter and display preview
        previewNextChapter = true;
        pageNumber = fm::Chapter::num_index;

        view.setSize( window.getDefaultView().getSize() );
        view.setCenter( window_width / 2.0, window_height / 2.0 );
        window.setView( view );
        
        drawChapterPreview();
        return;
    }
    else {
        previewNextChapter = false;
    }

    // Display next page
    int new_index = min( static_cast<int>( fm::Chapter::num_index - pageOffset ), pageNumber + 1 );
    if ( pageNumber != new_index )
    {
        pageNumber = new_index;
        if ( !loadImageFromIndex( pageNumber - pageOffset ) ) {
            return;
        }
        default_view_scale = getImageFitScale( texture );
        current_view_scale = default_view_scale;

        setViewZoom( default_view_scale, true );
        keepImage();
    }
}

void ImageViewer::prevPage()
{
    // Display info for previous chapter if page number subceeded minimum page no.
    if ( pageNumber - 1 <= 0 ) 
    {
        // Should be last chapter
        previewNextChapter = true;
        pageNumber = 0;

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
    int new_index = max( 0 + pageOffset, pageNumber - 1 );
    if ( pageNumber != new_index )
    {
        pageNumber = new_index;
        if ( !loadImageFromIndex( pageNumber - pageOffset ) ) {
            return;
        }
        default_view_scale = getImageFitScale( texture );
        current_view_scale = default_view_scale;

        setViewZoom( default_view_scale, true );
        keepImage();
    }
}

void ImageViewer::drawChapterPreview()
{
    // Parse Next Chapter ComicInfo.xml
    previewText.setString( "Title : \nChapter : \nTranslator : " );
    previewText.setCharacterSize( 30 );
    
    previewText.setPosition( (window_width - previewText.getLocalBounds().width) / 2, 10 );
}