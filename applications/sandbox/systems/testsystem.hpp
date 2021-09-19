/**
 * @file testsystem.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <core/core.hpp>

//#define LLRI_DISABLE_VALIDATION //uncommenting this disables API validation (see docs)
//#define LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING //uncommenting this disables implementation message polling
#include <llri/llri.hpp>

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

    ~TestSystem() override;
private:
    llri::Instance* m_instance = nullptr;
    llri::Adapter* m_adapter = nullptr;
    llri::Device* m_device = nullptr;

    llri::Queue* m_graphicsQueue = nullptr;
    llri::Queue* m_computeQueue = nullptr;
    llri::Queue* m_transferQueue = nullptr;

    llri::CommandGroup* m_commandGroup = nullptr;
    llri::CommandList* m_commandList = nullptr;

    llri::Fence* m_fence = nullptr;
    llri::Semaphore* m_semaphore = nullptr;
    
    void createInstance();
    void selectAdapter();
    void createDevice();
    void createCommandLists();
    void createSynchronization();
};
