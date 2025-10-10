#pragma once
#include <spdlog/sinks/sink.h>
#include "spdlog/pattern_formatter.h"

namespace LowEngine {
    class LogMemoryBufferSink : public spdlog::sinks::sink {
    public:
        LogMemoryBufferSink(fmt::memory_buffer& buffer) : buffer_(buffer) {}

        void log(const spdlog::details::log_msg& msg) override {
            spdlog::memory_buf_t formatted;
            formatter_->format(msg, formatted);
            buffer_.append(formatted.data(), formatted.data() + formatted.size());
        }

        void flush() override {}

        void set_pattern(const std::string& pattern) override {
            formatter_ = std::unique_ptr<spdlog::formatter>(new spdlog::pattern_formatter(pattern));
        }

        void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {
            formatter_ = std::move(sink_formatter);
        }

    private:
        fmt::memory_buffer& buffer_;
        std::unique_ptr<spdlog::formatter> formatter_;
    };
}
