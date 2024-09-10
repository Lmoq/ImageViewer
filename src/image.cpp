#include <iostream>
#include <cmath>
#include <algorithm>
#include <image.h>
#include <lzip.h>

// Media
int ImageViewer::pageIndex = 37;


// Window
bool ImageViewer::windowDisplayed = FALSE;
UINT8 R = 50, G = 50, B = 50, A = 255;
sf::Color ImageViewer::ClearColor = sf::Color( R, G, B, A );

sf::RenderWindow ImageViewer::window;
sf::WindowHandle ImageViewer::windowHandle;

float ImageViewer::window_width = 0;
float ImageViewer::window_height = 0;

sf::Image ImageViewer::image;
sf::Texture ImageViewer::texture;
sf::Sprite ImageViewer::sprite;

float ImageViewer::sprite_width = 0;
float ImageViewer::sprite_height = 0;

// Mouse
sf::View ImageViewer::view;
sf::Vector2f ImageViewer::mousePos;

float zoomed_out_scale;
float current_view_scale;

// Mouse
bool ImageViewer::draggableImage = FALSE;

bool ImageViewer::open( const char *path )
{
    // Populate image path list
    if ( !Libzip::open_archive( path ) ) 
    {
        std::cout << "Arhived failed to open\n";
        return false;
    }

    int n = pageIndex;
    do { // Loads image until it finds a horizontal image to set window shape
        ImageViewer::loadImageFromIndex( n );
        n++;
    } while ( texture.getSize().x > texture.getSize().y );
    
    // Create a window that streches or fits vertical image height to screen height
    window_height = sf::VideoMode::getDesktopMode().height;
    window_width = texture.getSize().x / ( static_cast<float>( texture.getSize().y ) / window_height );

    // Load image to display
    if ( !ImageViewer::loadImageFromIndex( pageIndex ) ) {
        return false;
    }
    // Calculate zoom out scale for the image to fit into window
    zoomed_out_scale = getImageFitScale( texture );
    current_view_scale = zoomed_out_scale;

    // Create Window
    window.create( sf::VideoMode( window_width, window_height ), "ImageViewer", sf::Style::None );
    windowHandle = window.getSystemHandle();

    // Zoom view to fit image to window
    view = window.getDefaultView();
    std::cout << "wd: " << window_width << '\n';

    ImageViewer::setViewZoom( current_view_scale, true );
    ImageViewer::anchorWindow( 1 );

    SetWindowPos( windowHandle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
    hideWindow();

    return true;
}

void ImageViewer::anchorWindow( int pos )
{
    sf::Vector2i window_pos;
    window_pos.x = 0;
    window_pos.y = static_cast<int>( (sf::VideoMode::getDesktopMode().height - window_height ) / 2 );
    switch ( pos ) 
    {
        case 0:
            window_pos.x = 0;
            break;
        case 1:
            window_pos.x = static_cast<int>(
                ( sf::VideoMode::getDesktopMode().width - window_width ) / 2
            );
            break;
        case 2:
            window_pos.x = static_cast<int>( 
                sf::VideoMode::getDesktopMode().width ) - static_cast<int>( window_width 
            );
            break;
    }
    window.setPosition( window_pos );
}

bool ImageViewer::setImagefromPath( const std::string &filepath )
{
    if ( !texture.loadFromFile( filepath ) ) {
        return false;
    }
    texture.setSmooth( true );
    sprite.setTexture( texture );

    return true;
}

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
    if ( !Libzip::get_item_buffer( index, img_buffer ) )
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
}

void ImageViewer::nextPage()
{
    int new_index = min( static_cast<int>( Libzip::max_index - 1 ), pageIndex + 1 );
    if ( pageIndex != new_index )
    {
        pageIndex = new_index;
        loadImageFromIndex( pageIndex );

        zoomed_out_scale = getImageFitScale( texture );
        current_view_scale = zoomed_out_scale;

        setViewZoom( zoomed_out_scale, true );
        keepImage();
    }
}

void ImageViewer::prevPage()
{
    int new_index = max( 0, pageIndex - 1 );
    if ( pageIndex != new_index )
    {
        pageIndex = new_index;
        loadImageFromIndex( pageIndex );

        zoomed_out_scale = getImageFitScale( texture );
        current_view_scale = zoomed_out_scale;

        setViewZoom( zoomed_out_scale, true );
        keepImage();
    }
}

float ImageViewer::getImageFitScale( sf::Texture &image_texture )
{
    UINT max_size = max( image_texture.getSize().x, image_texture.getSize().y );

    float fit_zoom_scale = ( max_size == image_texture.getSize().y ) ? 
        static_cast<float>( image_texture.getSize().y ) / window_height : 
        static_cast<float>( image_texture.getSize().x ) / window_width;

    return fit_zoom_scale;
}

void ImageViewer::setViewZoom( float zoom_scale, bool center )
{
    view.setSize( window.getDefaultView().getSize() );
    view.zoom( zoom_scale );

    sprite_width = texture.getSize().x / zoom_scale;
    sprite_height = texture.getSize().y / zoom_scale;

    if ( center ) {
        view.setCenter( 
            static_cast<float>(texture.getSize().x) / 2, 
            static_cast<float>(texture.getSize().y) / 2 
        );
    }
    window.setView( view );
}

void ImageViewer::zoomImage( sf::Event &event )
{
    float delta = event.mouseWheelScroll.delta;
    float offset = 0.10;
    // Zoom in
    if ( delta > 0 )
    {
        float new_scale = max( 1.3f, current_view_scale - offset );
        if ( current_view_scale != new_scale ) {
            view.move(
                ( static_cast<float>( event.mouseWheelScroll.x ) - ( window_width / 2.0 ) ) / 8, 
                ( static_cast<float>( event.mouseWheelScroll.y ) - ( window_height / 2.0 ) ) / 8 
            );
        }
        current_view_scale = new_scale;
    }
    // Zoom out
    else if ( delta < 0 )
    {
        current_view_scale = min( zoomed_out_scale, current_view_scale + offset);
    }
    ImageViewer::setViewZoom( current_view_scale, false );

    ImageViewer::keepImage();
}

void ImageViewer::dragImage()
{
    sf::Vector2f newPos = static_cast<sf::Vector2f>( sf::Mouse::getPosition( window ) );
    sf::Vector2f deltaPos = mousePos - newPos;

    deltaPos.x *= current_view_scale;
    deltaPos.y *= current_view_scale;

    view.move( deltaPos.x, deltaPos.y );
    window.setView( view );

    mousePos = newPos;
    ImageViewer::keepImage();
}

void ImageViewer::keepImage()
{
    // Bounds where view center position must be in order to keep sprite corners on window
    float center_view_limit_left = ( window_width / 2 ) * current_view_scale;
    float center_view_limit_top = ( window_height / 2 ) * current_view_scale;

    float center_view_limit_right = static_cast<float>( texture.getSize().x ) - center_view_limit_left;
    float center_view_limit_bottom = static_cast<float>( texture.getSize().y ) - center_view_limit_top;

    sf::Vector2f center = view.getCenter();
    sf::Vector2i sprPos = window.mapCoordsToPixel( sprite.getPosition() );

    printf( "Spry[%.d] SprBttm[%d] ImageIn[%d]\n", 
            sprPos.y, sprPos.y + static_cast<int>(sprite_height),
            ( sprite_width < sprite_height || (sprite_width > sprite_height && (sprPos.y <= 0 && sprPos.y + sprite_height >= window_height))) );


    // Check if sprite bounds reveals window background
    float left = sprPos.x;
    float top = sprPos.y;

    float right = sprPos.x + sprite_width;
    float bottom = sprPos.y + sprite_height;

    if ( left > 0 ) {
        view.setCenter( center_view_limit_left, center.y );
    }
    else if ( right < window_width ) {
        view.setCenter( center_view_limit_right, center.y );
    }

    if ( sprite_width > sprite_height && !( sprite_height >= window_height ) ) {
        std::cout << "Centered\n";
        view.setCenter( view.getCenter().x, texture.getSize().y / 2 );
    }

    if ( sprite_width < sprite_height ||
       ( sprite_width > sprite_height && ( sprite_height >= window_height ) ) )
    {
        if ( top > 0 ) {
            std::cout << "Top\n";
            view.setCenter( view.getCenter().x, center_view_limit_top );
        }
        else if ( bottom < window_height ) {
            view.setCenter( view.getCenter().x, center_view_limit_bottom );
        }
    }
    // if ( sprite_width > sprite_height )
    window.setView( view );
}
