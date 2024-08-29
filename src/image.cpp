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

float ImageViewer::screen_width = 0;
float ImageViewer::screen_height = 0;

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

    screen_height = sf::VideoMode::getDesktopMode().height;
    initial_scale_out = static_cast<float>(texture.getSize().y) / screen_height;
    scale_out = initial_scale_out;

    sprite_height = texture.getSize().y / scale_out;
    sprite_width = texture.getSize().x / scale_out;

    screen_width = sprite_width;
    screen_height = sprite_height;

    printf("scrw(%f) : srch(%f)\n", screen_width, screen_height);

    window.create( sf::VideoMode( screen_width, screen_height ), "ImageViewer", sf::Style::None );
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
        scale_out = std::max( 1.3f, scale_out - .08f );

        float xdiff = static_cast<float>( event.mouseWheelScroll.x ) - ( view.getCenter().x / scale_out );
        float ydiff = static_cast<float>( event.mouseWheelScroll.y )- ( view.getCenter().y / scale_out );

        view.move( xdiff, ydiff );
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
    float half_camera_width_pixels = ( screen_width / 2 ) * scale_out;
    float half_camera_height_pixels = ( screen_height / 2 ) * scale_out;

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
