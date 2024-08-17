#include <image.h>
#include <iostream>

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

int ImageViewer::screen_width = 0;
int ImageViewer::screen_height = 0;

sf::Image ImageViewer::image;
sf::Texture ImageViewer::texture;
sf::Sprite ImageViewer::sprite;

int ImageViewer::sprite_width = 0;
int ImageViewer::sprite_height = 0;

float ImageViewer::initial_scale;
float ImageViewer::resize_scale = initial_scale;

// Mouse
BOOL ImageViewer::draggableImage = FALSE;
POINT ImageViewer::mouse_image_distance;

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
    initial_scale = static_cast<float>(screen_height) / static_cast<float>(texture.getSize().y);
    resize_scale = initial_scale;

    sprite_width = texture.getSize().x * resize_scale;
    sprite_height = texture.getSize().y * resize_scale;

    screen_width = sprite_width;
    screen_height = sprite_height;

    window.create( sf::VideoMode( screen_width, screen_height ), "ImageViewer", sf::Style::None );
    windowHandle = window.getSystemHandle();

    setImage( filepaths[ pageIndex ] );

    // std::cout << "screen_width  : " << screen_width << '\n'
    //           << "screen_x      : " << window.getPosition().x << '\n'
    //           << "sprite width  : " << sprite_width << "\n\n"
    //           << "scr_w - spr_w : " << (screen_width - sprite_width) << '\n'
    //           << "scr_w-spr_w/2 : " << ((screen_width - ( sprite_width )) / 2) << '\n'
    //           << "quo - scr_x   : " << (((screen_width - ( sprite_width )) / 2) - window.getPosition().x) << '\n'
    //           << "sprite x      : " << sprite.getLocalBounds().left << '\n';

    // SetWindowLong( windowHandle, GWL_EXSTYLE, GetWindowLong( windowHandle, GWL_EXSTYLE ) | WS_EX_LAYERED );
    // SetLayeredWindowAttributes( windowHandle, RGB( R,G,B ), A, LWA_COLORKEY | LWA_ALPHA );

    SetWindowPos( windowHandle, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
    hideWindow();
}

void ImageViewer::setImage( const std::string &filepath )
{
    texture.loadFromFile( filepath );
    texture.setSmooth( true );

    sprite.setTexture( texture );
    sprite.setScale( resize_scale, resize_scale );

    sprite_width = sprite.getLocalBounds().width * resize_scale;
    sprite.setPosition( ((screen_width - ( sprite_width )) / 2), 0 );
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
    if ( (pageIndex + 1) < filepaths.size() ) {
        pageIndex += 1;
        setImage( filepaths[ pageIndex] );
    }
}

void ImageViewer::prevPage()
{
    if ( (pageIndex - 1) > -1 ) {
        pageIndex -= 1;
        setImage( filepaths[ pageIndex ] );
    }
}

void ImageViewer::zoomImage( sf::Event &event )
{
    float delta = event.mouseWheelScroll.delta;
    float interval = 0.04;

    if ( delta > 0 )
    {
        resize_scale += interval;
        if ( resize_scale > 0.80 )
        {
            resize_scale = 0.80;
        }
        if ( resize_scale != sprite.getScale().y )
        {
            sprite.setScale( resize_scale, resize_scale );
            sprite_width = sprite.getLocalBounds().width * resize_scale;

            sprite.setPosition( ((screen_width - ( sprite_width )) / 2), 0 );
        }
    }
    else if ( delta < 0 )
    {
        resize_scale -= interval;
        if ( resize_scale < initial_scale )
        {
            resize_scale = initial_scale;
        }
        if ( resize_scale != sprite.getScale().x )
        {
            sprite.setScale( resize_scale, resize_scale );
            sprite_width = sprite.getLocalBounds().width * resize_scale;

            sprite.setPosition( ((screen_width - ( sprite_width )) / 2), 0 );
        }
    }
}

void ImageViewer::dragImage( sf::Event &event )
{
    float xdiff = event.mouseMove.x - mouse_image_distance.x;
    float ydiff = event.mouseMove.y - mouse_image_distance.y;

    auto rect = sprite.getGlobalBounds();

    sprite.setPosition( 
        xdiff,
        ydiff
    );

    if ( xdiff > 0 ) {
        xdiff = 0;
        sprite.setPosition( 0, ydiff );
    }

    else if ( xdiff + rect.width < static_cast<float>(screen_width) ) {
        xdiff = static_cast<float>( screen_width ) - rect.width;
        sprite.setPosition( xdiff, ydiff );
    }

    if ( ydiff > 0 ) {
        sprite.setPosition( xdiff, 0 );
    }

    else if ( ydiff + rect.height < static_cast<float>( screen_height ) ) {
        sprite.setPosition( xdiff, static_cast<float>( screen_height ) - rect.height );
    }

}