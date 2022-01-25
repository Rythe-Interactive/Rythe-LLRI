/**
 * @file metal_surface.mm
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
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

                if (false) // ns.retina
                    [layer setContentsScale:[ns backingScaleFactor]];
                
                [view setLayer:layer];
                [view setWantsLayer:YES];
        
                return reinterpret_cast<void*>(layer);
            }
        }

    }
}
