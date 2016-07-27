#pragma once

#include <vector>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/Vector3>

namespace local
{

inline void Crop(
    kvs::UnstructuredVolumeObject* volume,
    const kvs::Vec3 min_coord,
    const kvs::Vec3 max_coord )
{
    const kvs::ValueArray<kvs::Real32>& coords = volume->coords();
    const kvs::ValueArray<kvs::UInt32>& connections = volume->connections();
    const size_t ncells = volume->numberOfCells();
    const size_t ncellnodes = volume->numberOfCellNodes();

    std::vector<kvs::UInt32> cropped_connections;
    for ( size_t i = 0; i < ncells; i++ )
    {
        const size_t index = connections[ ncellnodes * i ];
        const float x = coords[ 3 * index ];
        const float y = coords[ 3 * index + 1 ];
        const float z = coords[ 3 * index + 2 ];
        if ( min_coord.x() <= x && x <= max_coord.x() &&
             min_coord.y() <= y && y <= max_coord.y() &&
             min_coord.z() <= z && z <= max_coord.z() )
        {
            for ( size_t j = 0; j < ncellnodes; j++ )
            {
                cropped_connections.push_back( connections[ ncellnodes * i + j ] );
            }
        }
    }

    volume->setConnections( kvs::ValueArray<kvs::UInt32>( cropped_connections ) );
    volume->setNumberOfCells( cropped_connections.size() / ncellnodes );
    volume->setMinMaxObjectCoords( min_coord, max_coord );
    volume->setMinMaxExternalCoords( min_coord, max_coord );
}

} // end of namespace local
