#include <iostream>
#include <cmath>
#include <algorithm>
#include <image.h>
#include <lzip.h>

// Media
int ImageViewer::pageIndex = 0;


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

float initial_scale_out;
float scale_out;

// Mouse
bool ImageViewer::draggableImage = FALSE;

bool ImageViewer::open( const char *path )
{
    // Pupulate path list
    if ( !Libzip::open_archive( path ) ) 
    {
        std::cout << "Arhived failed to open\n";
        return false;
    }

    // Setup window and sprite shape
    if ( !loadImageFromIndex( 0 ) ) {
        return false;
    }
    
    // Calculate zoom out scale for the image to fit into window height
    window_height = sf::VideoMode::getDesktopMode().height;
    initial_scale_out = static_cast<float>( texture.getSize().y ) / window_height;
    scale_out = initial_scale_out;

    sprite_height = texture.getSize().y / scale_out;
    sprite_width = texture.getSize().x / scale_out;

    // Create a window that fits the image to load
    window_width = sprite_width;
    window_height = sprite_height;

    window.create( sf::VideoMode( window_width, window_height ), "ImageViewer", sf::Style::None );
    window.setPosition( sf::Vector2i(
        static_cast<int>( sf::VideoMode::getDesktopMode().width ) - static_cast<int>( window_width ),
        0 ) 
    );
    windowHandle = window.getSystemHandle();

    // Zoom out view to fit image to window
    view = window.getDefaultView();

    view.setCenter( texture.getSize().x / 2, texture.getSize().y / 2 );
    view.zoom( scale_out );
    window.setView( view );

    SetWindowPos( windowHandle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
    hideWindow();

    return true;
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
    if ( !setImagefromBuffer( img_buffer ) )
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
        std::cout << "index : " << pageIndex << '\n';
        loadImageFromIndex( pageIndex );
    }
}

void ImageViewer::prevPage()
{
    int new_index = max( 0, pageIndex - 1 );
    if ( pageIndex != new_index )
    {
        pageIndex = new_index;
        loadImageFromIndex( pageIndex );
    }
}

void ImageViewer::zoomImage( sf::Event &event )
{
    float delta = event.mouseWheelScroll.delta;
    float speed = 0.10;
    if ( delta > 0 )
    {
        float new_scale = max( 1.3f, scale_out - speed);
        if ( scale_out != new_scale ) {
            view.move(
                ( static_cast<float>( event.mouseWheelScroll.x ) - ( window_width / 2.0 ) ) / 8, 
                ( static_cast<float>( event.mouseWheelScroll.y ) - ( window_height / 2.0 ) ) / 8 
            );
        }
        scale_out = new_scale;
    }
    else if ( delta < 0 )
    {
        scale_out = min( initial_scale_out, scale_out + speed);
    }
    view.setSize( window.getDefaultView().getSize() );
    view.zoom( scale_out );
    window.setView( view );

    sf::Vector2i sprPos = window.mapCoordsToPixel( sprite.getPosition() );

    sprite_width = texture.getSize().x / scale_out;
    sprite_height = texture.getSize().y / scale_out;
    keepImage();
}

void ImageViewer::dragImage()
{
    sf::Vector2f newPos = static_cast<sf::Vector2f>( sf::Mouse::getPosition( window ) );
    sf::Vector2f deltaPos = mousePos - newPos;

    deltaPos.x *= scale_out;
    deltaPos.y *= scale_out;

    view.move( deltaPos.x, deltaPos.y );
    window.setView( view );

    mousePos = newPos;
    keepImage();
}

void ImageViewer::keepImage()
{
    float half_camera_width_pixels = ( window_width / 2 ) * scale_out;
    float half_camera_height_pixels = ( window_height / 2 ) * scale_out;

    sf::Vector2f center = view.getCenter();

    float leftGap = half_camera_width_pixels - center.x;
    float topGap = half_camera_height_pixels - center.y;

    float rightGap = center.x - ( static_cast<float>( texture.getSize().x ) - half_camera_width_pixels );
    float bottomGap = center.y - ( static_cast<float>( texture.getSize().y ) - half_camera_height_pixels );

    if ( leftGap > 0 )
        view.move( leftGap, 0 );
    else if ( rightGap > 0 )
        view.move( -rightGap, 0 );

    if ( topGap > 0 )
        view.move( 0, topGap );
    else if ( bottomGap > 0 )
        view.move( 0, -bottomGap );
    
    window.setView( view );
}
