#ifndef _HOTKEY_
#define _HOTKEY_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <cmath>
#include <iostream>

static inline HHOOK hook = NULL;
static inline KBDLLHOOKSTRUCT *kbd = NULL;

static inline LRESULT CALLBACK KeyProc( int nCode, WPARAM wParam, LPARAM lParam );


/// Class that install hook and listen for hotkeys\n
/// \note - Remember to explicitly add a terminate hotkey with a callback
/// \note   that handles other thread termination
/// \note - Usage :
/// \note   - Populate hotkeymap with addhotkey()
/// \note   - Run listener with Hotkey::run() and call Hotkey::wait() at the
/// \note     end of main thread if there are no other 
/// \note     thread running besides Hotkey to avoid exceptions
/// \note   - Terminate callback shoud contain :
/// \note       - PostQuitMessage(0) : Terminate Hotkey Message loop
/// \note       - OtherFlags = FALSE : Toggle terminate flag for other threaded loops
class Hotkey
{
    public:

    struct hotkey
    {
        std::unordered_set<DWORD> keyCodes;
        BOOL active = FALSE;

        BOOL ignoreKeypress = FALSE;
        void ( *callback ) ();
    };
    inline static std::thread T;
    inline static BOOL ignoreKeypress = FALSE;

    inline static hotkey *current_active = NULL;
    inline static DWORD prime = 199;

    inline static std::unordered_set<DWORD> keyPressed;
    inline static std::unordered_map<DWORD, hotkey> map;

    inline static void keyhook()
    {
        hook = SetWindowsHookEx( WH_KEYBOARD_LL, &KeyProc, 0, 0 );
        if ( !hook )
        {
            std::cout << "Hook failed\n";
            return;
        }
        MSG msg;
        while ( GetMessage( &msg, NULL, 0,0 ) > 0 )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        UnhookWindowsHookEx( hook );
    }

    /// @brief Populate hotkey map. Remember to add terminate hotey
    /// @param keyCodes Sets of keyCodes or single keyCode.
    /// @param callback Callback function to call when keyCodes are pressed, return type should be void.
    /// @param ignoreKeypress If true, hotkey or single key presses will not be sent to foreground window.
    inline static void add_hotkey( std::unordered_set<DWORD> keyCodes, void ( *callback )(), BOOL ignoreKeypress )
    {
        hotkey hkey;

        hkey.keyCodes = keyCodes;
        hkey.callback = callback;
        hkey.ignoreKeypress = ignoreKeypress;

        DWORD hash = hash_keycodes( keyCodes );
        if ( hotkey_exist( hash ) )
        {
            return;
        }
        map[ hash ] = hkey;
    }

    inline static DWORD hash_keycodes( std::unordered_set<DWORD> keyCodes )
    {
        DWORD hash = 0;
        for ( auto &key : keyCodes ) {
            hash += ( static_cast<int>(std::pow( key, 4 )) ^ prime ) / key;
        }
        return hash % 60000;
    }

    inline static BOOL hotkey_exist( DWORD hash )
    {
        if ( map.find( hash ) != map.end() ) {
            return TRUE;
        }
        return FALSE;
    }

    inline static void keydown( DWORD keyCode )
    {
        keyPressed.insert( keyCode );
        DWORD hash = hash_keycodes( keyPressed );

        if ( !hotkey_exist( hash ) ) {
            return;
        }

        hotkey &hkey = Hotkey::map[ hash ];
        if ( hkey.active == FALSE )
        {
            current_active = &hkey;
            hkey.active = TRUE;
            hkey.callback();

            ignoreKeypress = hkey.ignoreKeypress;
        }
    }

    inline static void keyup( DWORD keyCode )
    {
        if ( keyPressed.count( keyCode ) ) {
            keyPressed.erase( keyCode );
        }

        if ( current_active != NULL ) 
        {
            current_active->active = FALSE;
            current_active = NULL;

            ignoreKeypress = FALSE;
        }
    }

    inline static void run() {
        T = std::thread( keyhook );
    }

    inline static void wait() {
        T.join();
    }
};

LRESULT CALLBACK KeyProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    if ( nCode == HC_ACTION ) {
        kbd = reinterpret_cast<KBDLLHOOKSTRUCT *>( lParam );

        switch ( wParam )
        {
            case WM_KEYDOWN:
                Hotkey::keydown( kbd->vkCode );
                break;
            
            case WM_KEYUP:
                Hotkey::keyup( kbd->vkCode );
                break;

            default:
                break;
        }
    }
    return ( Hotkey::ignoreKeypress ) ? -1 : CallNextHookEx( NULL, nCode, wParam, lParam );
}

#endif