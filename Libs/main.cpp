#include <iostream>
#include <dlfcn.h>
#include <memory>
#include "static_lib/mul.h"
#include "dynamic_lib/sum.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::unique_ptr;

class LibHandler {
    void *lib;

    void throw_error(char * error, string const& description) {
        if (error == nullptr) {
            throw std::runtime_error(description);
        }
        throw std::runtime_error(description + "\nError: " + string(error));
    }

    void throw_error(string const& description) {
        auto error = dlerror();
        throw_error(error, description);
    }

public:
    LibHandler(string const& path, int mode) {
        dlerror(); // clean last error
        lib = dlopen(path.c_str(), mode);
        if (lib == nullptr) {
            throw_error("Can't open library: " + path);
        }
    }

    template <class R>
    R get_function(string const& name) {
        dlerror(); // clean last error
        void* function = dlsym(lib, name.c_str());
        auto error = dlerror(); // check a new error
        if (error != nullptr) {
            throw_error(error, "Can't load function: " + name);
        }
        return reinterpret_cast<R>(function);
    }

    ~LibHandler() {
        if (lib != nullptr) {
            dlerror(); // clean last error
            int code = dlclose(lib);
            if (code != 0) {
                throw_error("Can't close library");
            }
        }
    }
};

int main() {
    cout << "3 * 2 = " << mul(3, 2) << endl;
    cout << "3 + 2 = " << sum(3, 2) << endl;

    try {
        unique_ptr<LibHandler> handler = std::make_unique<LibHandler>("loadable_lib/sub.so", RTLD_LAZY);
        auto sub = handler->get_function < int(*)(int, int) > ("sub");

        cout << "3 - 2 = " << sub(3, 2) << endl;

        handler.reset(nullptr);
    } catch (std::runtime_error const& e) {
        cerr << e.what() << endl;
    }
    return 0;
}