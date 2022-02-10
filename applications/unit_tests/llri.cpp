/**
 * @file llri.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <helpers.hpp>

TEST_CASE("print info")
{
    printf("LLRI unit tests\n");
    llri::Instance* instance;
    const llri::instance_desc desc{ 0, nullptr, "unit test instance"};
    llri::createInstance(desc, &instance);
    
    std::vector<llri::Adapter*> adapters;
    instance->enumerateAdapters(&adapters);
    
    printf("Available adapters: (%lu)\n", adapters.size());
    for (auto* adapter : adapters)
    {
        llri::adapter_info info = adapter->queryInfo();
        printf("\t\"%s\"\n", info.adapterName.c_str());
    }
    
    if (strcmp(LLRI_TEST_ADAPTERS, "All") == 0)
    {
        printf("LLRI_TEST_ADAPTERS was set to \"All\", all adapters available are selected for unit test execution.\n");
        printf("This configuration is %s\n", adapters.size() > 0 ? "valid" : "invalid");
    }
    else
    {
        printf("LLRI_TEST_ADAPTERS was set to %s\n", LLRI_TEST_ADAPTERS);
        printf("Unit tests will attempt to run with the defined set of adapters. All adapters must be available for the unit tests to succeed.\n");
        
        auto selectedAdapterNames = helpers::split(LLRI_TEST_ADAPTERS, ';');
        std::vector<llri::Adapter*> selectedAdapters;

        for (auto* adapter : adapters)
        {
            llri::adapter_info info = adapter->queryInfo();
            
            if (std::find(selectedAdapterNames.begin(), selectedAdapterNames.end(), info.adapterName) != selectedAdapterNames.end())
            {
                selectedAdapters.push_back(adapter);
                printf("Found a required adapter: %s", info.adapterName.c_str());
            }
        }
        
        printf("Number of selected adapters as defined in LLRI_TEST_ADAPTERS: %lu\n", selectedAdapterNames.size());
        printf("Number of adapters that match LLRI_TEST_ADAPTERS names: %lu\n", selectedAdapters.size());
        printf("This configuration is: %s\n", selectedAdapters.size() == selectedAdapterNames.size() ? "valid" : "invalid");
    }
}

TEST_CASE("getImplementation()")
{
    SUBCASE("")
    {
        CHECK_UNARY(llri::getImplementation() <= llri::implementation::MaxEnum);
    }
}
