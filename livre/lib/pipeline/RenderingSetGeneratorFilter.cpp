/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
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

#include <livre/lib/pipeline/RenderingSetGeneratorFilter.h>
#include <livre/lib/cache/TextureCache.h>

#include <livre/core/pipeline/PipeFilter.h>
#include <livre/core/pipeline/InputPort.h>
#include <livre/core/pipeline/Workers.h>
#include <livre/core/pipeline/PortData.h>

namespace livre
{

struct RenderingSetGenerator
{
    explicit RenderingSetGenerator( const TextureCache& textureCache )
        : _textureCache( textureCache )
    {}

    bool hasParentInMap( const NodeId& childRenderNode,
                         const ConstCacheMap& cacheMap ) const
    {
        const NodeIds& parentNodeIds = childRenderNode.getParents();

        for( const NodeId& parentId : parentNodeIds )
            if( cacheMap.find( parentId.getId( )) != cacheMap.end() )
                return true;

        return false;
    }

    void collectLoadedTextures( const NodeId& nodeId,
                                ConstCacheMap& cacheMap ) const
    {
        NodeId current = nodeId;
        while( current.isValid( ))
        {
            const NodeId& currentNodeId = current;
            const ConstCacheObjectPtr texture = _textureCache.get( currentNodeId.getId( ));
            if( texture && texture->isLoaded( ))
            {
                cacheMap[ currentNodeId.getId() ] = texture;
                break;
            }

            current = currentNodeId.isRoot() ? NodeId() :
                                               currentNodeId.getParent();
        }
    }

    ConstCacheObjects generateRenderingSet( const NodeIds& visibles,
                                            size_t& nAvailable,
                                            size_t& nNotAvailable ) const
    {
        ConstCacheMap cacheMap;
        for( const NodeId& nodeId : visibles )
        {
            collectLoadedTextures( nodeId, cacheMap );
            cacheMap.count( nodeId.getId( )) > 0 ? ++nAvailable : ++nNotAvailable;
        }

        if( visibles.size() != cacheMap.size( ))
        {
            ConstCacheMap::const_iterator it = cacheMap.begin();
            size_t previousSize = 0;
            do
            {
                previousSize = cacheMap.size();
                while( it != cacheMap.end( ))
                {
                    if( hasParentInMap( NodeId( it->first ), cacheMap ))
                        it = cacheMap.erase( it );
                    else
                        ++it;
                }
            }
            while( previousSize != cacheMap.size( ));
        }

        ConstCacheObjects cacheObjects;
        cacheObjects.reserve( cacheMap.size( ));
        for( ConstCacheMap::const_iterator it = cacheMap.begin();
             it != cacheMap.end(); ++it )
        {
            cacheObjects.push_back( it->second );
        }

        return cacheObjects;
    }

    const TextureCache& _textureCache;
};

struct RenderingSetGeneratorFilter::Impl
{
    explicit Impl( const TextureCache& cache )
        : _cache( cache )
    {}

    void execute( const FutureMap& input,
                  PromiseMap& output ) const
    {
        RenderingSetGenerator renderSetGenerator( _cache );

        ConstCacheObjects cacheObjects;
        size_t nVisible = 0;
        size_t nAvailable = 0;
        size_t nNotAvailable = 0;
        for( const auto& visibles: input.get< NodeIds >( "VisibleNodes" ))
        {
            size_t available = 0;
            size_t notAvailable = 0;
            const ConstCacheObjects& objs = renderSetGenerator.generateRenderingSet( visibles,
                                                                                     available,
                                                                                     notAvailable );
            cacheObjects.insert( cacheObjects.end(), objs.begin(), objs.end( ));
            nVisible += visibles.size();
            nAvailable += available;
            nNotAvailable += notAvailable;
        }

        output.set( "CacheObjects", cacheObjects );
        output.set( "RenderingDone", cacheObjects.size() == nVisible );
        output.set( "AvailableCount", nAvailable );
        output.set( "NotAvailableCount", nNotAvailable );
    }

    DataInfos getInputDataInfos() const
    {
        return
        {
            { "VisibleNodes", getType< NodeIds >( )}
        };
    }

    DataInfos getOutputDataInfos() const
    {
        return
        {
            { "CacheObjects", getType< ConstCacheObjects >( )},
            { "RenderingDone", getType< bool >() },
            { "AvailableCount", getType< size_t >() },
            { "NotAvailableCount", getType< size_t >() },
        };
    }

    const TextureCache& _cache;
};

RenderingSetGeneratorFilter::RenderingSetGeneratorFilter( const TextureCache& cache )
    : _impl( new RenderingSetGeneratorFilter::Impl( cache ))
{
}

RenderingSetGeneratorFilter::~RenderingSetGeneratorFilter()
{
}

void RenderingSetGeneratorFilter::execute( const FutureMap& input,
                                           PromiseMap& output ) const
{
    _impl->execute( input, output );
}

DataInfos RenderingSetGeneratorFilter::getInputDataInfos() const
{
    return _impl->getInputDataInfos();
}

DataInfos RenderingSetGeneratorFilter::getOutputDataInfos() const
{
    return _impl->getOutputDataInfos();
}


}
