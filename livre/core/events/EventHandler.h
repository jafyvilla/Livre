/* Copyright (c) 2011-2014, EPFL/Blue Brain Project
 *                     Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *
 * This file is part of Livre <https://github.com/BlueBrain/Livre>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _EventHandler_h_
#define _EventHandler_h_

#include <livre/core/types.h>

namespace livre
{

/**
 * The EventHandler class is the abstract class for event handling.
 */
class EventHandler
{
public:

    /**
     * Executes the event function.
     * @param eventInfo The needed information for the event. \see livre::EventInfo
     */
    virtual bool operator()( EventInfo& eventInfo ) = 0;

    virtual ~EventHandler() {}
};

}

#endif //_EventHandler_h_
