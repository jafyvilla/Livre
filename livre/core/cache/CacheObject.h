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

#ifndef _CacheObject_h_
#define _CacheObject_h_

#include <livre/core/api.h>
#include <livre/core/types.h>
#include <livre/core/lunchboxTypes.h>
#include <livre/core/util/ThreadClock.h>

namespace livre
{

/**
 * The CacheObject class for memory objects that can be managed with \see Cache.
 * If it is generated by cache it is valid and one reference is hold by the cache, otherwise
 * it is an invalid cache object.
 */

class CacheObject
{
public:
    LIVRECORE_API virtual ~CacheObject();

    /**
     * @return True if the object is valid.
     */
    LIVRECORE_API bool isValid() const;

    /**
     * @return The unique cache id.
     */
    LIVRECORE_API CacheId getId() const;

    /**
     * @return The object is loaded in cache. The function is thread safe.
     */
    LIVRECORE_API bool isLoaded() const;

    /**
     * @return The memory size of the object in bytes.
     */
    LIVRECORE_API size_t getSize() const;

    /**
     * @return On default returns true if cache ids are same
     */
    virtual bool operator==( const CacheObject& cacheObject ) const;

protected:

    friend class Cache;

    LIVRECORE_API explicit CacheObject( const CacheId& cacheId = INVALID_CACHE_ID );

    /**
     * Implemented by the derived class, for loading data to memory. Thread safety is satisfied for
     * loading and unloading.
     * @return True if data is loaded.
     */
    virtual bool _load() = 0;

    /**
     * Implemented by the derived class, for unloading data from memory. Thread safety is satisfied for
     * loading and unloading.
     */
    virtual void _unload() = 0;

    /**
     * @return The validity of derived class.
     */
    virtual bool _isValid() const;

    /**
     * @return True if object is loaded into memory.
     */
    virtual bool _isLoaded() const = 0;

    /**
     * @return The memory size of the object in bytes.
     */
    virtual size_t _getSize() const;

private:

    /** Used by cache to load the data */
    bool _notifyLoad();

    /** Used by cache to unload the data */
    void _notifyUnload();

    struct Status;
    mutable std::shared_ptr< Status > _status;
};

}

#endif // _CacheObject_h_
