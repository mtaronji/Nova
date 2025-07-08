#pragma once

#include <coroutine>
#include <optional>
#include <exception>

template<typename T>
struct Generator {
    struct promise_type {
        std::optional<T> current_value;

        Generator get_return_object() {
            return Generator{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Generator(std::coroutine_handle<promise_type> h) : handle(h) {}

    ~Generator() {
        if (handle) handle.destroy();
    }

    bool resume() {
        if (!handle || handle.done()) return false;
        handle.resume();
        return !handle.done();
    }

    T current_value() const {
        return handle.promise().current_value.value();
    }
};

template<typename T>
struct Task {
    struct promise_type {
        std::optional<T> result;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(T value) { result = std::move(value); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task() { if (handle) handle.destroy(); }

    bool resume() {
        if (!handle.done()) handle.resume();
        return !handle.done();
    }

    T get() {
        if (handle.promise().exception) std::rethrow_exception(handle.promise().exception);
        return *handle.promise().result;
    }
};

struct Action {
    struct promise_type {
        std::exception_ptr exception;

        Action get_return_object() {
            return Action{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Action(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Action() { if (handle) handle.destroy(); }

    bool resume() {
        if (!handle.done()) handle.resume();
        return !handle.done();
    }

    void get() {
        if (handle.promise().exception) std::rethrow_exception(handle.promise().exception);
    }
};