#include <llri/llri.hpp>
#include <iostream>

int main()
{
    using namespace legion::graphics;

    //Describe our instance prior to creating it
    //the simplest case requires no extensions and a simple application name (which may also just be empty)
    const llri::instance_desc instanceDesc = {
        0, /* 0 extensions */
        nullptr /* no extension array */,
        "hello_llri" /* potentially useful for debugging */
    };

    //Create our instance and check if the operation was successful
    llri::Instance* instance = nullptr;
    const llri::result createResult = llri::createInstance(instanceDesc, &instance);
    if (createResult != llri::result::Success) //LLRI works with result codes that you can read to learn more about why an operation might've failed
    {
        std::cout << "Failed to create LLRI Instance because " << llri::to_string(createResult) << "\n";
        return -1;
    }
    std::cout << "LLRI Instance creation was a success!\n";

    //Before your application exits, make sure to clean up your allocated resources
    llri::destroyInstance(instance);
    return 0;
}
