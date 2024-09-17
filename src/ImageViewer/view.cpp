#include <iostream>
#include <image.h>

// Window view
sf::View ImageViewer::view;
sf::Vector2f ImageViewer::mousePos;
bool ImageViewer::draggableImage = FALSE;

float ImageViewer::default_view_scale;
float ImageViewer::current_view_scale;

void ImageViewer::setViewZoom( float zoom_scale, bool center )
{
    view.setSize( window.getDefaultView().getSize() );
    view.zoom( zoom_scale );

    sprite_width = texture.getSize().x / zoom_scale;
    sprite_height = texture.getSize().y / zoom_scale;

    if ( center ) {
        view.setCenter( 
            static_cast<float>( texture.getSize().x ) / 2, 
            static_cast<float>( texture.getSize().y ) / 2 
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
        current_view_scale = min( default_view_scale, current_view_scale + offset);
    }
    ImageViewer::setViewZoom( current_view_scale, false );
    ImageViewer::keepImage();
}

void ImageViewer::dragImage( sf::Event &event )
{
    sf::Vector2f newPos( static_cast<sf::Vector2f>( sf::Mouse::getPosition( window ) ) );
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

    // Check if sprite corners detached from window corners
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

    // Center the view when zooming to a horizontal image and disable y drag
    if ( sprite_width > sprite_height && !( sprite_height >= window_height ) ) {
        view.setCenter( view.getCenter().x, texture.getSize().y / 2 );
    }

    if ( sprite_width < sprite_height ||
        // Enable y drag if horizontal image size is zoomed enough to exceed view size
       ( sprite_width > sprite_height && ( sprite_height >= window_height ) ) )
    {
        if ( top > 0 ) {
            view.setCenter( view.getCenter().x, center_view_limit_top );
        }
        else if ( bottom < window_height ) {
            view.setCenter( view.getCenter().x, center_view_limit_bottom );
        }
    }
    window.setView( view );
}

float ImageViewer::getImageFitScale( sf::Texture &image_texture )
{
    UINT greater_size = max( image_texture.getSize().x, image_texture.getSize().y );

    float fit_zoom_scale = ( greater_size == image_texture.getSize().y ) ? 
        static_cast<float>( image_texture.getSize().y ) / window_height : 
        static_cast<float>( image_texture.getSize().x ) / window_width;

    return fit_zoom_scale;
}