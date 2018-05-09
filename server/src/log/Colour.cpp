#include "Colour.h"

#if !defined(LOG_PLATFORM_WINDOWS) && (defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER))
#define LOG_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <sstream>

#ifndef LOG_CONFIG_TERMINAL_COLORS
#define LOG_CONFIG_TERMINAL_COLORS true
#endif

namespace log {
namespace {

    struct IColourImpl {
        virtual ~IColourImpl() = default;

        virtual void use(Colour::Code _colourCode) = 0;
    };

    struct NoColourImpl : IColourImpl {
        void use(Colour::Code) override {}

        static IColourImpl *instance() {
            static NoColourImpl s_instance;
            return &s_instance;
        }
    };

} // end anon namespace
} // end log namespace

#if !defined( LOG_CONFIG_COLOUR_NONE ) && !defined( LOG_CONFIG_COLOUR_WINDOWS ) && !defined( LOG_CONFIG_COLOUR_ANSI )
#   ifdef LOG_PLATFORM_WINDOWS
#       define LOG_CONFIG_COLOUR_WINDOWS
#   else
#       define LOG_CONFIG_COLOUR_ANSI
#   endif
#endif


#if defined ( LOG_CONFIG_COLOUR_WINDOWS ) /////////////////////////////////////////

namespace log {
namespace {

    class Win32ColourImpl : public IColourImpl {
    public:
        Win32ColourImpl() : stdoutHandle( GetStdHandle(STD_OUTPUT_HANDLE) )
        {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo( stdoutHandle, &csbiInfo );
            originalForegroundAttributes = csbiInfo.wAttributes & ~( BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY );
            originalBackgroundAttributes = csbiInfo.wAttributes & ~( FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY );
        }

        virtual void use( Colour::Code _colourCode ) override {
            switch( _colourCode ) {
                case Colour::None:      return setTextAttribute( originalForegroundAttributes );
                case Colour::White:     return setTextAttribute( FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );
                case Colour::Red:       return setTextAttribute( FOREGROUND_RED );
                case Colour::Green:     return setTextAttribute( FOREGROUND_GREEN );
                case Colour::Blue:      return setTextAttribute( FOREGROUND_BLUE );
                case Colour::Cyan:      return setTextAttribute( FOREGROUND_BLUE | FOREGROUND_GREEN );
                case Colour::Yellow:    return setTextAttribute( FOREGROUND_RED | FOREGROUND_GREEN );
                case Colour::Grey:      return setTextAttribute( 0 );

                case Colour::LightGrey:     return setTextAttribute( FOREGROUND_INTENSITY );
                case Colour::BrightRed:     return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_RED );
                case Colour::BrightGreen:   return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_GREEN );
                case Colour::BrightWhite:   return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );
                case Colour::BrightYellow:  return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN );

//                case Colour::Bright: CATCH_INTERNAL_ERROR( "not a colour" );
//                default: CATCH_ERROR( "Unknown colour requested" );
            }
        }

    private:
        void setTextAttribute( WORD _textAttribute ) {
            SetConsoleTextAttribute( stdoutHandle, _textAttribute | originalBackgroundAttributes );
        }
        HANDLE stdoutHandle;
        WORD originalForegroundAttributes;
        WORD originalBackgroundAttributes;
    };

    IColourImpl* platformColourInstance() {
        static Win32ColourImpl s_instance;
        return LOG_CONFIG_TERMINAL_COLORS
            ? &s_instance
            : NoColourImpl::instance();
    }

} // end anon namespace
} // end log namespace

#elif defined( LOG_CONFIG_COLOUR_ANSI ) //////////////////////////////////////

namespace log {
namespace {

    // use POSIX/ ANSI console terminal codes
    class PosixColourImpl : public IColourImpl {
    public:
        void use(Colour::Code _colourCode) override {
            switch (_colourCode) {
                case Colour::None:
                case Colour::White:
                    return setColour("[0m");
                case Colour::Red:
                    return setColour("[0;31m");
                case Colour::Green:
                    return setColour("[0;32m");
                case Colour::Blue:
                    return setColour("[0;34m");
                case Colour::Cyan:
                    return setColour("[0;36m");
                case Colour::Yellow:
                    return setColour("[0;33m");
                case Colour::Grey:
                    return setColour("[1;30m");

                case Colour::LightGrey:
                    return setColour("[0;37m");
                case Colour::BrightRed:
                    return setColour("[1;31m");
                case Colour::BrightGreen:
                    return setColour("[1;32m");
                case Colour::BrightWhite:
                    return setColour("[1;37m");
                case Colour::BrightYellow:
                    return setColour("[1;33m");

//                case Colour::Bright: CATCH_INTERNAL_ERROR( "not a colour" );
//                default: CATCH_INTERNAL_ERROR( "Unknown colour requested" );
            }
        }

        static IColourImpl *instance() {
            static PosixColourImpl s_instance;
            return &s_instance;
        }

    private:
        void setColour(const char *_escapeCode) {
            std::cout << '\033' << _escapeCode;
        }
    };

    IColourImpl *platformColourInstance() {
        return LOG_CONFIG_TERMINAL_COLORS
               ? PosixColourImpl::instance()
               : NoColourImpl::instance();
    }

} // end anon namespace
} // end log namespace

#else  // not Windows or ANSI ///////////////////////////////////////////////

namespace log {
    static IColourImpl* platformColourInstance() { return NoColourImpl::instance(); }
} // end log namespace

#endif // Windows/ ANSI/ None

namespace log {

    Colour::Colour(Code _colourCode) { use(_colourCode); }

    Colour::Colour(Colour &&rhs) noexcept {
        m_moved = rhs.m_moved;
        rhs.m_moved = true;
    }

    Colour &Colour::operator=(Colour &&rhs) noexcept {
        m_moved = rhs.m_moved;
        rhs.m_moved = true;
        return *this;
    }

    Colour::~Colour() { if (!m_moved) use(None); }

    void Colour::use(Code _colourCode) {
        static IColourImpl *impl = platformColourInstance();
        impl->use(_colourCode);
    }

    std::ostream &operator<<(std::ostream &os, Colour const &) {
        return os;
    }

} // end log namespace