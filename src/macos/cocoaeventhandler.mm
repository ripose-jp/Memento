////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "cocoaeventhandler.h"

#import <Cocoa/Cocoa.h>

/**
 * Objective-C class for actually handling native events.
 */
@interface EVHandler : NSObject
{
    /* Callback functions */
    CocoaEventHandlerCallback *m_cb;
}
- (id) initWithCallback:(CocoaEventHandlerCallback *)cb;

/**
 * Pauses mpv and saves the pause state before the window changes.
 * @param notification The NSNotification to handle.
 */
- (void) handleWindowWillChange:(NSNotification *)notification;

/**
 * Restores mpv's pause state after the window has changed.
 * @param notification The NSNotification to handle.
 */
- (void) handleWindowDidChange:(NSNotification *)notification;
@end

struct _NSContainer
{
    EVHandler *eh;
};

CocoaEventHandler::CocoaEventHandler(CocoaEventHandlerCallback *cb)
{
    nsc = new _NSContainer;
    nsc->eh = [ [ EVHandler alloc ] initWithCallback:cb ];
}

CocoaEventHandler::~CocoaEventHandler()
{
    [ nsc->eh dealloc ];
    delete nsc;
}

@implementation EVHandler

- (id) initWithCallback:(CocoaEventHandlerCallback *)cb
{
    m_cb = cb;

    [ [ NSNotificationCenter defaultCenter ]
        addObserver:self
        selector:@selector(handleWindowWillChange:)
        name:NSWindowWillEnterFullScreenNotification
        object:nil
    ];
    [ [ NSNotificationCenter defaultCenter ]
        addObserver:self
        selector:@selector(handleWindowWillChange:)
        name:NSWindowWillExitFullScreenNotification
        object:nil
    ];

    [ [ NSNotificationCenter defaultCenter ]
        addObserver:self
        selector:@selector(handleWindowDidChange:)
        name:NSWindowDidEnterFullScreenNotification
        object:nil
    ];
    [ [ NSNotificationCenter defaultCenter ]
        addObserver:self
        selector:@selector(handleWindowDidChange:)
        name:NSWindowDidExitFullScreenNotification
        object:nil
    ];

    return self;
}

- (void) handleWindowWillChange:(NSNotification *)notification
{
    m_cb->beforeTransition();
}

- (void) handleWindowDidChange:(NSNotification *)notification
{
    m_cb->afterTransition();
}

@end
