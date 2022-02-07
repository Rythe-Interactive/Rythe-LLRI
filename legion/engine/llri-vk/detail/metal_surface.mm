/**
 * @file metal_surface.mm
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>
#include <Carbon/Carbon.h>

namespace llri
{
    namespace internal
    {
        void* getCAMetalLayer(void* nsWindow)
        {
            @autoreleasepool
            {
                NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
                if (!bundle)
                    return nullptr;
                
                id ns = static_cast<id>(nsWindow);
                id view = [ns contentView];
                
                id layer = [[bundle classNamed:@"CAMetalLayer"] layer];
                if (!layer)
                    return nullptr;

                [layer setContentsScale:[ns backingScaleFactor]];
                
                [view setLayer:layer];
                [view setWantsLayer:YES];
        
                return reinterpret_cast<void*>(layer);
            }
        }
    }
}
