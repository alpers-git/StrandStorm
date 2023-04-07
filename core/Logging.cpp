#include <csignal>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/formatter.h>
#include <spdlog/fmt/fmt.h>
#include <Logging.hpp>

namespace spdlog::sinks {
    template<typename Mutex>
    class error_proxy_sink : public base_sink<Mutex> {
    /* Thanks to @tt4g for this class (https://github.com/gabime/spdlog/issues/1363#issuecomment-567068416) */
    private:
        using BaseSink = base_sink<Mutex>;

        std::shared_ptr<sink> sink_;

    public:
        explicit error_proxy_sink(std::shared_ptr<sink> sink) : sink_(sink){}

        error_proxy_sink(const error_proxy_sink&) = delete;
        error_proxy_sink& operator=(const error_proxy_sink&) = delete;
    protected:
        void sink_it_(const spdlog::details::log_msg &msg) override {
            if (sink_->should_log(msg.level)) {
                sink_->log(msg);
            }
            if (spdlog::level::err == msg.level) {
                std::raise(SIGINT);
            }
        }

        void flush_() override {
            sink_->flush();
        }

        void set_pattern_(const std::string &pattern) override {
            set_formatter_(spdlog::details::make_unique<spdlog::pattern_formatter>(pattern));
        }

        void set_formatter_(std::unique_ptr<spdlog::formatter> sink_formatter) override {
            BaseSink::formatter_ = std::move(sink_formatter);

            sink_->set_formatter(BaseSink::formatter_->clone());
        }
    };

    using error_proxy_sink_mt = error_proxy_sink<std::mutex>;
    using error_proxy_sink_st = error_proxy_sink<spdlog::details::null_mutex>;
}

void setupLogging()
{
    auto stderrSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto errSink = std::make_shared<spdlog::sinks::error_proxy_sink_mt>(stderrSink);
    auto formatter = std::make_unique<spdlog::pattern_formatter>("%^%L%$> %v");
    auto logger = std::make_shared<spdlog::logger>("logger", errSink);
    logger->set_formatter(std::move(formatter));
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
}

void _checkGLError(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        spdlog::error("GL_{} - {}:{}", error, fs::path(file).filename().string(), line);
    }
}

void APIENTRY GLDebugMessageCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar *msg, const void *data)
{
    std::string _source;
    switch (source) {
        case GL_DEBUG_SOURCE_API:               _source = "api"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     _source = "window"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   _source = "shader"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       _source = "3rd party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:       _source = "app"; break;
        case GL_DEBUG_SOURCE_OTHER:             _source = "UNKNOWN"; break;
        default: _source = "UNKNOWN"; break;
    }

    std::string _type;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               _type = "error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: _type = "deprecated"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  _type = "undefined"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         _type = "portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         _type = "performance"; break;
        case GL_DEBUG_TYPE_OTHER:               _type = "other"; break;
        case GL_DEBUG_TYPE_MARKER:              _type = "marker"; break;
        default: _type = "UNKNOWN"; break;
    }

    std::string _severity;
    spdlog::level::level_enum lvl;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            _severity = "high";
            lvl = spdlog::level::warn;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            _severity = "med";
            lvl = spdlog::level::warn;
            break;
        case GL_DEBUG_SEVERITY_LOW:
            _severity = "low";
            lvl = spdlog::level::debug;
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            _severity = "notif";
            lvl = spdlog::level::trace;
            break;
        default:
            _severity = "UNKNOWN";
            lvl = spdlog::level::debug;
            break;
    }

    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        spdlog::log(lvl, "GL[{}|{}|{}] {}", _type, _severity, _source, msg);
    }
}