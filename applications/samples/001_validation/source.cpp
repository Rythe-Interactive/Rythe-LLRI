/**
 * @file source.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#ifdef NDEBUG
// Validation can be an incredibly useful tool, but runtime checks don't come without a performance cost.
// To prevent validation from causing overhead on builds, you can #define LLRI_DISABLE_VALIDATION.
// Read the documentation to learn more about its exact behaviour.
//
// In this example we disable validation in release builds, which is something you may or may not want to do depending on your use case.
#define LLRI_DISABLE_VALIDATION
#endif
#include <llri/llri.hpp>
#include <iostream>

void callback(llri::message_severity severity, llri::message_source source, const char* message, void* userData)
{
    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

int main()
{
    // This example displays LLRI validation and the message callback.
    // as long as LLRI_DISABLE_VALIDATION isn't defined, LLRI will run validation checks, regardless of if a callback is passed along.
    // Without the callback, functions still return result codes which provide basic information about the result of the operation.
    // However, the callback function can provide you with a lot more detailed info about why a function returned a particular result code.

    llri::setMessageCallback(
        &callback, // pass a pointer to our callback function.
        nullptr // optional user pointer.
    );

    const llri::instance_desc instanceDesc = { 0, nullptr, "validation" };

    // We're intentionally misusing the API here to display the validation layer's effects
    // if LLRI_DISABLE_VALIDATION is defined, this usage will likely cause an internal crash.
    std::cout << "The next LLRI function call will output a validation error because we passed an incorrect parameter\n";
    const llri::result r = llri::createInstance(instanceDesc, nullptr); // passing nullptr to createInstance()
    std::cout << "Instance create result: " << to_string(r) << "\n";
    return 0;
}
