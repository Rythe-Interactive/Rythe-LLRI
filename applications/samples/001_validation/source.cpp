#ifdef NDEBUG
//Validation can be an incredibly useful tool, but runtime checks don't come without a performance cost.
//To prevent validation from causing overhead on builds, you can #define LLRI_DISABLE_VALIDATION.
//Read the documentation to learn more about its exact behaviour.
//
//In this example we disable validation in release builds, which is something you may or may not want to do depending on your use case.
#define LLRI_DISABLE_VALIDATION
#endif
#include <llri/llri.hpp>
using namespace legion::graphics;

#include <cassert>
#include <iostream>

void callback(const llri::validation_callback_severity& severity, const llri::validation_callback_source& source, const char* message, void* userData)
{
    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

int main()
{
    //This example displays LLRI validation and its message callback.
    //as long as LLRI_DISABLE_VALIDATION isn't defined, LLRI will run validation checks, regardless of if a callback is passed along.
    //Without the validation callback, functions still return result codes which provide basic information about the result of the operation.
    //However, the callback function can provide you with a lot more detailed info about why a function returned a particular result code.

    const llri::instance_desc instanceDesc = { 0, nullptr, "validation",
        llri::validation_callback_desc {
            &callback, /* Pass the callback function*/
            nullptr /* A user pointer can be passed into this (e.g. a pointer to a render class) */
        }
    };

    //We're intentionally misusing the API here to display the validation layer's effects
    //if LLRI_DISABLE_VALIDATION is defined, this usage will likely crash internally
    std::cout << "The next LLRI function call will output a validation error because we passed an incorrect parameter\n";
    const llri::result r = llri::createInstance(instanceDesc, nullptr); //passing nullptr to createInstance()
    std::cout << "Instance create result: " << to_string(r) << "\n";
    return 0;
}
