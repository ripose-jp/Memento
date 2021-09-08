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

#include "../../util/globalmediator.h"
#include "../playeradapter.h"

/**
 * Objective-C class for actually handling native events.
 */
@interface EVHandler : NSObject
{
    /* The mpv handle to manipulate. */
    mpv_handle *m_mpv;

    /* The last saved pause state. Set in handleWindowWillChange and then used
     * in handleWindowDidChange. */
    int m_paused;
}

- (id) initWithMpvHandle:(mpv_handle *)mpv;

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

CocoaEventHandler::CocoaEventHandler(mpv_handle *mpv)
{
    nsc = new _NSContainer;
    nsc->eh = [ [ EVHandler alloc ] initWithMpvHandle:mpv ];
}

CocoaEventHandler::~CocoaEventHandler()
{
    [ nsc->eh dealloc ];
    delete nsc;
}

@implementation EVHandler

- (id) initWithMpvHandle:(mpv_handle *)mpv
{
    m_mpv = mpv;
    m_paused = 1;

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
    mpv_get_property(m_mpv, "pause", MPV_FORMAT_FLAG, &m_paused);
    int pause = 1;
    mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &pause);
}

- (void) handleWindowDidChange:(NSNotification *)notification
{
    mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &m_paused);
}

@end