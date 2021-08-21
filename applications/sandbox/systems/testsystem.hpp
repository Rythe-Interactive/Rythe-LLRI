/**
 * @file testsystem.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <core/core.hpp>

namespace llri {
    class Instance;
    class Adapter;
    class Device;
    class Queue;
    class CommandGroup;
    class CommandList;
}

/**@class TestSystem
 * @brief Custom system.
 */
class TestSystem final : public lgn::System<TestSystem>
{
public:
    /**@brief Will automatically be called once at the start of the application.
     */
    void setup();

    /**@brief Default process marked in setup to run as fast as possible on the "Update" interval.
     */
    void update(lgn::time::span deltaTime);

private:
    llri::Instance* m_instance = nullptr;
    llri::Adapter* m_adapter = nullptr;
    llri::Device* m_device = nullptr;

    llri::Queue* m_graphicsQueue = nullptr;
    llri::Queue* m_computeQueue = nullptr;
    llri::Queue* m_transferQueue = nullptr;

    llri::CommandGroup* m_commandGroup = nullptr;
    llri::CommandList* m_commandList = nullptr;

    void createInstance();
    void selectAdapter();
    void createDevice();
    void createCommandLists();
};
