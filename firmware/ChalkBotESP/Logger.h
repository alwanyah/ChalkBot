#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <string>
#include <unordered_map>
#include <utility>

class Logger {
    std::string module_name;

public:
    enum Verbosity {
        NONE,
        ERROR,
        WARN,
        INFO,
        DEBUG,
    };

    class Writer : public Print {
        const Logger &logger;
        Verbosity level;
        std::string message;

    public:
        Writer(const Logger &logger, Verbosity level) : logger(logger), level(level) {}
        
        void finish() {
            logger.log(level, message);
            message.clear();
        }

        virtual size_t write(uint8_t value) override {
            return write(&value, 1);
        }

        virtual size_t write(const uint8_t *buffer, size_t size) override {
            message.append((const char *)buffer, size);
            return size;
        }

        virtual int availableForWrite() override {
            size_t available = message.capacity() - message.size();
            return available < INT_MAX ? available : INT_MAX;
        }
    };

private:
    static int attach_listener_internal(Print *print, std::unordered_map<std::string, Verbosity> &&extra, bool managed);

public:
    Logger(std::string module_name) : module_name(std::move(module_name)) {}

    static int attach_listener(Print &print, std::unordered_map<std::string, Verbosity> verbosity) {
        return attach_listener_internal(&print, std::move(verbosity), false);
    }

    static void attach_listener_managed(Print *print, std::unordered_map<std::string, Verbosity> verbosity) {
        attach_listener_internal(print, std::move(verbosity), true);
    }

    static void detach_listener(int id);

    Writer writer(Verbosity level) const {
        return Writer(*this, level);
    }

    Writer writer_debug() const {
        return writer(DEBUG);
    }

    Writer writer_info() const {
        return writer(INFO);
    }

    Writer writer_warn() const {
        return writer(WARN);
    }

    Writer writer_error() const {
        return writer(ERROR);
    }

    void log(Verbosity level, const std::string &message) const;

    void log_debug(const std::string &message) const {
        log(DEBUG, message);
    }

    void log_info(const std::string &message) const {
        log(INFO, message);
    }

    void log_warn(const std::string &message) const {
        log(WARN, message);
    }

    void log_error(const std::string &message) const {
        log(WARN, message);
    }
};

#endif
