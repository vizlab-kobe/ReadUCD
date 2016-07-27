#include "ImportAVSUcdBinary64.h"


namespace ReadUCD
{

kvs::UnstructuredVolumeObject* ImportAVSUcdBinary64( const ReadUCD::AVSUcdBinary64& ucd )
{
    kvs::UnstructuredVolumeObject::CellType cell_type =
        ( ucd.elementType() == ReadUCD::AVSUcdBinary64::Tet ) ?
        kvs::UnstructuredVolumeObject::Tetrahedra :
        ( ucd.elementType() == ReadUCD::AVSUcdBinary64::Prism ) ?
        kvs::UnstructuredVolumeObject::Prism :
        kvs::UnstructuredVolumeObject::UnknownCellType;

    kvs::UnstructuredVolumeObject* volume = new kvs::UnstructuredVolumeObject();
    volume->setVeclen( ucd.veclen() );
    volume->setNumberOfNodes( ucd.totalNumberOfNodes() );
    volume->setNumberOfCells( ucd.numberOfElements() );
    volume->setCellType( cell_type );
    volume->setCoords( ucd.coords() );
    volume->setConnections( ucd.connections() );
    volume->setValues( ucd.values() );
    volume->updateMinMaxCoords();
    volume->updateMinMaxValues();

    return volume;
}

} // end of namespace ReadUCD
