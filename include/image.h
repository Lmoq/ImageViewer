#ifndef _IMAGE_
#define _IMAGE_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <filesystem>
#include <unordered_set>
#include <vector>
#include <SFML/Graphics.hpp>

namespace fs = std::filesystem;

class ImageViewer
{
public:
    // Media info
    static int pageIndex;
    static bool previewNextChapter;
 
    // Window
    static bool windowDisplayed;
    static sf::Color ClearColor;

    static sf::RenderWindow window;
    static sf::WindowHandle windowHandle;
    static sf::View view;

    static sf::Image image;
    static sf::Texture texture;
    static sf::Sprite sprite;

    static sf::Text previewText;
    static sf::Font defaultFont;

    static float window_width;
    static float window_height;

    static float sprite_width;
    static float sprite_height;

    static sf::Mutex mutex;

    // View
    static float default_view_scale;
    static float current_view_scale;

    static bool draggableImage;
    static sf::Vector2f mousePos;

    static bool open( const char *path );
    static void anchorWindow( int pos );
    static void setupWindowShape();

    static void hideWindow();
    static void showWindow();
    
    static bool setImagefromPath( const std::string &filename );
    static bool setImagefromBuffer( std::vector<char> &buffer );
    static bool loadImageFromIndex( int index );

    static bool setChapterPreview( int index, const char *failText );
    static void nextPage();
    static void prevPage();

    static float getImageFitScale( sf::Texture &image_texture );
    static void setViewZoom( float zoom_scale, bool center );

    static void zoomImage( sf::Event &event );
    static void dragImage( sf::Event &event );

    static void keepImage();
};

#endif