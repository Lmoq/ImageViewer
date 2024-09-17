#include <iostream>
#include <image.h>
#include <lzip.h>

// Window
bool ImageViewer::windowDisplayed = FALSE;
UINT8 R = 0, G = 0, B = 0, A = 255;
sf::Color ImageViewer::ClearColor = sf::Color( R, G, B, A );

sf::RenderWindow ImageViewer::window;
sf::WindowHandle ImageViewer::windowHandle;

sf::Image ImageViewer::image;
sf::Texture ImageViewer::texture;
sf::Sprite ImageViewer::sprite;

float ImageViewer::window_width = 0;
float ImageViewer::window_height = 0;

float ImageViewer::sprite_width = 0;
float ImageViewer::sprite_height = 0;


bool ImageViewer::open( const char *path )
{
    // Populate image path list
    if ( !fm::Chapter::open_archive( path ) ) {
        std::cout << "Arhived failed to open\n";
        return false;
    }
    // Finds a vertical image from archive to setup window shape
    setupWindowShape();

    // Create Window
    window.create( sf::VideoMode( window_width, window_height ), "ImageViewer", sf::Style::None );
    windowHandle = window.getSystemHandle();
    
    std::cout << "Window Width[ " << window_width << " ]\n";

    defaultFont.loadFromFile( "C:/Windows/Fonts/Arial.ttf" );
    previewText.setFont( defaultFont );
    
    view = window.getDefaultView();

    // Set window state
    anchorWindow( 1 );
    SetWindowPos( windowHandle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
    hideWindow();

    return true;
}

/// @param pos Anchor window to [0] left, [1] center, [2] right
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

void ImageViewer::setupWindowShape()
{
    int n = 0;
    do { // Loads image until it finds a vertical image to set window shape
        ImageViewer::loadImageFromIndex( n );
        n++;
    } while ( texture.getSize().x > texture.getSize().y );
    
    // Define window size that streches or shrinks vertical image to match image height to screen height
    window_height = sf::VideoMode::getDesktopMode().height;
    window_width = texture.getSize().x / getImageFitScale( texture );
}