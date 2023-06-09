#include "Logger.h"
#include <vector>

struct Listener {
    Print *print;
    std::unordered_map<std::string, Logger::Verbosity> extra;
    Logger::Verbosity max_level;
    bool detached;
    bool managed;
};

static std::vector<Listener> listeners;

int Logger::attach_listener_internal(Print *print, Verbosity max_level, std::unordered_map<std::string, Verbosity> &&extra, bool managed) {
    int len = listeners.size();
    for (int i = 0; i < len; ++i) {
        if (listeners[i].detached) {
            listeners[i] = { print, std::move(extra), max_level, false, managed };
            return i;
        }
    }
    listeners.push_back({ print, std::move(extra), max_level, false, managed });
    return len;
}

void Logger::detach_listener(int id) {
    listeners[id].detached = true;
    while (!listeners.empty() && listeners.back().detached) {
        listeners.pop_back();
    }
}

void Logger::log(Verbosity message_verbosity, const std::string &message) const {
    if (message_verbosity <= NONE) {
        return;
    }

    unsigned long ms = millis();

    for (Listener &listener : listeners) {
        if (listener.detached) {
            continue;
        }
        if (message_verbosity > listener.max_level) {
            auto it = listener.extra.find(module_name);
            if (it == listener.extra.end() || message_verbosity > it->second) {
                continue;
            }
        }

        size_t written = 0;

        // [  74.3345] [error] [GNSS] Initialisation failed.
        // [ 142.7421] [ info] [WebServer] Listening on port 80.
        
        written += listener.print->printf("[%4lu.%04lu]", ms / 1000, ms % 1000);

        switch (message_verbosity) {
            case ERROR:
                written += listener.print->print(" [error] ");
                break;
            case WARN:
                written += listener.print->print(" [ warn] ");
                break;
            case INFO:
                written += listener.print->print(" [ info] ");
                break;
            case DEBUG:
                written += listener.print->print(" [debug] ");
                break;
        }

        written += listener.print->print('[');
        written += listener.print->write(module_name.data(), module_name.size());
        written += listener.print->print("] ");

        written += listener.print->write(message.data(), message.size());
        written += listener.print->println();

        if (listener.managed && (written == 0 || listener.print->getWriteError())) {
            delete listener.print;
            listener.detached = true;
        }
    }

    while (!listeners.empty() && listeners.back().detached) {
        listeners.pop_back();
    }
}
