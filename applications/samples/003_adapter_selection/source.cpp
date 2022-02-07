/**
 * @file source.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <iostream>

void callback(llri::message_severity severity, llri::message_source source, const char* message, [[maybe_unused]] void* userData)
{
    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

llri::Adapter* selectAdapter(llri::Instance* instance);

int main()
{
    llri::setMessageCallback(&callback);

    llri::result r;
    const llri::instance_desc instanceDesc = { 0, nullptr, "adapter_selection" };

    llri::Instance* instance;
    r = llri::createInstance(instanceDesc, &instance);
    if (r != llri::result::Success)
        return -1;

    // This sample displays adapter enumeration and shows an approach you might take to selecting adapters.
    // There are many ways of doing adapter selection, you may for example eliminate GPUs based on required features or extensions,
    // and you might rate GPUs higher based on e.g. their VRAM, or other optional supported features.
    // 
    // You can also select multiple adapters for a multi-gpu setup.
    // 
    // this function selects adapters by giving them a score based on their featureset:
    llri::Adapter* selectedAdapter = selectAdapter(instance);
    if (selectedAdapter)
    {
        llri::adapter_info info = selectedAdapter->queryInfo();
        std::cout << "Selected adapter: " << info.adapterName << "\n";
    }

    // The adapter may be used now to e.g. create an llri::Device
    // this sample however, ends here

    llri::destroyInstance(instance);
    return 0;
}

llri::Adapter* selectAdapter(llri::Instance* instance)
{
    // The first step to adapter selection is to call Instance::enumerateAdapters() which hands you a list of non-owning Adapter pointers
    std::vector<llri::Adapter*> adapters;
    llri::result r = instance->enumerateAdapters(&adapters);
    if (r != llri::result::Success)
        return nullptr;

    // Then these adapters may be iterated over and their properties may be read to try and find the best fit
    std::unordered_map<int, llri::Adapter*> sortedAdapters;
    for (auto* adapter : adapters)
    {
        // An adapter's information (product, vendor, name, type, etc) can be queried through queryInfo()
        llri::adapter_info info = adapter->queryInfo();

        // An adapter's supported features can be queried through queryFeatures()
        [[maybe_unused]] llri::adapter_features features = adapter->queryFeatures();
        
        // you may decide to rate the adapter higher for specific features, such as max texture size, vram, etc.
        int score = 0;

        // Discrete adapters tend to be more performant so we'll rate them much higher
        if (info.adapterType == llri::adapter_type::Discrete)
            score += 1000;

        sortedAdapters.emplace(score, adapter);
    }

    return (*sortedAdapters.begin()).second;
}
