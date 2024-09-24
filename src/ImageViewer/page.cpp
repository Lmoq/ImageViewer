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
    if ( windowDisplayed )
    {
        ShowWindow( windowHandle, SW_HIDE );
        windowDisplayed = FALSE;
    }
}

void ImageViewer::showWindow()
{
    if ( !windowDisplayed )
    {
        ShowWindow( windowHandle, SW_SHOW );
        windowDisplayed = TRUE;
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

        // Reset view
        view.setSize( window.getDefaultView().getSize() );
        view.setCenter( window_width / 2.0, window_height / 2.0 );
        window.setView( view );

        if ( setChapterPreview( Series::chapterIndex + 1, "No Next Chapter" ) ) {
            Series::chapterIndex ++;
        }

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

        // Reset View
        view.setSize( window.getDefaultView().getSize() );
        view.setCenter( window_width / 2.0, window_height / 2.0 );
        window.setView( view );

        if ( setChapterPreview( Series::chapterIndex - 1, "No Previous Chapter" ) ) {
            Series::chapterIndex --;
        }
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

/// <param name="index">- index of the chapter from chapter_list to load preview from</param>
bool ImageViewer::setChapterPreview( int index, const char *failText )
{
    /// Sync resource access to opengl context
    mutex.lock();
    
    bool succeed = true;
    chapter_t *ch = NULL;

    try {
        ch = &Series::chapter_list->at( index );
    }
    catch ( std::out_of_range ) 
    {
        std::cout << "Index out of bound\n";
        succeed = false;
    }

    char buffer[ MAX_PATH ];
    if ( succeed ) {
        sprintf( buffer, 
                 "Title : %s\nChapter : %.1f\nTranslator : %s", 
                 ch->title.c_str(), ch->number, ch->translator.c_str() );
        std::cout << "Chapter details : \n" << buffer << '\n';
    }
    else {
        sprintf( buffer, "%s", failText );
    }
    previewText.setString( buffer );
    previewText.setCharacterSize( 30 );
    
    previewText.setPosition( (window_width - previewText.getLocalBounds().width) / 2, 10 );
    mutex.unlock();

    return succeed;
}