#ifndef _IMAGE_
#define _IMAGE_

#define MIHON "F:/Backup/.Oppo/Download/.mpak/Mihon/downloads/Comick (EN)/chapter.cbz"
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
    inline static std::unordered_set<std::string> valid_extensions = {
        ".jpg",
        ".png",
        ".jpeg"
    };

    // Media info
    static int pageIndex;
 
    // Window
    static bool windowDisplayed;
    static sf::Color ClearColor;

    static sf::RenderWindow window;
    static sf::WindowHandle windowHandle;

    static sf::View view;

    static sf::Image image;
    static sf::Texture texture;
    static sf::Sprite sprite;

    static float window_width;
    static float window_height;

    static float sprite_width;
    static float sprite_height;

    // Can be retrived using screen_height / texture_height
    static float resize_scale;
    static float initial_scale;

    // Mouse drag
    static bool draggableImage;
    static sf::Vector2f mousePos;

    static bool open( const char *path );

    static bool setImagefromPath( const std::string &filename );
    static bool setImagefromBuffer( std::vector<char> &buffer );

    // Set texture to selected image index
    static bool loadImageFromIndex( int index );

    static void hideWindow();
    static void showWindow();

    static void nextPage();
    static void prevPage();

    static void zoomImage( sf::Event &event );
    static void dragImage();

    static void keepImage();
};

#endif