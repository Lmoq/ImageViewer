#include <image.h>
#include <iostream>

UINT screen_width = sf::VideoMode::getDesktopMode().width - 1;
UINT screen_height = sf::VideoMode::getDesktopMode().height;

UINT initial_width = 0;
UINT initial_height = 0;

void ImageViewer::open( const char *folder )
{
    image_folder = folder;
    for ( auto &p : fs::directory_iterator( folder ) )
    {
        if ( valid_extensions.count( p.path().extension().string() ) ) {
            filepaths.push_back( p.path().string() );
        }
    }
    setImage( filepaths[ pageIndex ] );

    initial_width = sprite.getLocalBounds().width * default_scale;
    initial_height = sprite.getLocalBounds().height * default_scale;

    // screen_width = initial_width;
    // screen_height = initial_height;

    window.create( sf::VideoMode( initial_width , screen_height ), "ImageViewer", sf::Style::None );
    windowHandle = window.getSystemHandle();


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

    UINT sprite_width = sprite.getLocalBounds().width * resize_scale;
    // sprite.setTextureRect( sf::IntRect( 0, 0, sprite_width, screen_height ) );
    sprite.setPosition( (screen_width - ( sprite_width )) / 2, 0 );
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
    if ( (pageIndex - 1) < -1 ) {
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
            UINT sprite_width = sprite.getLocalBounds().width * resize_scale;

            // sprite.setTextureRect( sf::IntRect( 0, 0, sprite_width, screen_height ) );
            std::cout << "sprite x: " << ((screen_width - ( sprite_width )) / 2) << '\n';
            sprite.setPosition( (screen_width - ( sprite_width )) / 2, 0 );
        }
    }
    else if ( delta < 0 )
    {
        resize_scale -= interval;
        if ( resize_scale < default_scale ) 
        {
            resize_scale = default_scale;
        }
        if ( resize_scale != sprite.getScale().x )
        {
            sprite.setScale( resize_scale, resize_scale );
            UINT sprite_width = sprite.getLocalBounds().width * resize_scale;

            // sprite.setTextureRect( sf::IntRect( 0, 0, sprite_width, screen_height ) );
            std::cout << "sprite x: " << ((screen_width - ( sprite_width )) / 2) << '\n';
            sprite.setPosition( (screen_width - ( sprite_width )) / 2, 0 );
        }
    }
}