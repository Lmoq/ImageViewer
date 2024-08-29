#include <iostream>
#include <cmath>
#include <image.h>

// Media
int ImageViewer::pageIndex = 0;
fs::path ImageViewer::image_folder;
std::vector<std::string> ImageViewer::filepaths;

// Window
BOOL ImageViewer::displayWindow = FALSE;
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
BOOL ImageViewer::draggableImage = FALSE;

void ImageViewer::open( const char *folder )
{
    image_folder = folder;
    for ( auto &p : fs::directory_iterator( folder ) )
    {
        if ( valid_extensions.count( p.path().extension().string() ) ) {
            filepaths.push_back( p.path().string() );
        }
    }
    // Setup window and sprite shape
    texture.loadFromFile( filepaths[0] );

    window_height = sf::VideoMode::getDesktopMode().height;
    initial_scale_out = static_cast<float>(texture.getSize().y) / window_height;
    scale_out = initial_scale_out;

    sprite_height = texture.getSize().y / scale_out;
    sprite_width = texture.getSize().x / scale_out;

    window_width = sprite_width;
    window_height = sprite_height;

    window.create( sf::VideoMode( window_width, window_height ), "ImageViewer", sf::Style::None );
    window.setPosition( sf::Vector2i(
        static_cast<int>( sf::VideoMode::getDesktopMode().width ) - static_cast<int>( window_width ),
        0 ) 
    );
    windowHandle = window.getSystemHandle();

    view = window.getDefaultView();
    setImage( filepaths[ pageIndex ] );

    view.setCenter( texture.getSize().x / 2, texture.getSize().y / 2 );
    view.zoom( scale_out );
    window.setView( view );

    SetWindowPos( windowHandle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
    hideWindow();
}

void ImageViewer::setImage( const std::string &filepath )
{
    texture.loadFromFile( filepath );
    texture.setSmooth( true );

    sprite.setTexture( texture );
}

void ImageViewer::hideWindow()
{
    ShowWindow( windowHandle, SW_HIDE );
    displayWindow = FALSE;
}

void ImageViewer::showWindow()
{
    ShowWindow( windowHandle, SW_SHOW );
    displayWindow = TRUE;
}

void ImageViewer::nextPage()
{
    pageIndex = std::min( static_cast<int>( filepaths.size() - 1 ), pageIndex + 1 );
    setImage( filepaths[ pageIndex ] );
}

void ImageViewer::prevPage()
{
    pageIndex = std::max( 0, pageIndex - 1 );
    setImage( filepaths[ pageIndex ] );
}

void ImageViewer::zoomImage( sf::Event &event )
{
    float delta = event.mouseWheelScroll.delta;
    if ( delta > 0 )
    {
        float new_scale = std::max( 1.3f, scale_out - .08f );
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
        scale_out = std::min( initial_scale_out, scale_out + .08f );
    }
    view.setSize( window.getDefaultView().getSize() );
    view.zoom( scale_out );
    window.setView( view );

    sf::Vector2i sprPos = window.mapCoordsToPixel( sprite.getPosition() );

    sprite_width = texture.getSize().x / scale_out;
    sprite_height = texture.getSize().y / scale_out;
    keepImage();
}

void ImageViewer::dragImage( sf::Event &event )
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
