#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/File>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/Isosurface>
#include <kvs/OrthoSlice>
#include <kvs/Bounds>

#include <Lib/AVSUcdBinary64.h>
#include <Lib/ImportAVSUcdBinary64.h>

#include "Read.h"
#include "Crop.h"


int main( int argc, char** argv )
{
    std::string filename( argv[1] );
    ReadUCD::AVSUcdBinary64 ucd;
    //ucd.setElementType( ReadUCD::AVSUcdBinary64::Tet );
    ucd.setElementType( ReadUCD::AVSUcdBinary64::Prism );


    std::cout << "Read ..." << std::endl;

    local::ReadPressure( ucd, filename );
    //local::ReadDensity( ucd, filename );
    //local::ReadTemperature( ucd, filename );
    //local::ReadVelocity( ucd, filename );

    std::cout << "Import ..." << std::endl;

    kvs::UnstructuredVolumeObject* volume = ReadUCD::ImportAVSUcdBinary64( ucd );
    volume->print( std::cout );

//    local::Crop( volume, kvs::Vec3::All(-30), kvs::Vec3::All(30) );
//    volume->print( std::cout );

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );

    const double isolevel = kvs::Math::Mix( volume->minValue(), volume->maxValue(), 0.5 );
    const kvs::PolygonObject::NormalType normal_type = kvs::PolygonObject::PolygonNormal;
    const bool duplication = true;
    const kvs::TransferFunction tfunc( 256 );
    kvs::PolygonObject* object = new kvs::Isosurface( volume, isolevel, normal_type, duplication, tfunc );

    const float position = kvs::Math::Mix( volume->minObjectCoord().z(), volume->maxObjectCoord().z(), 0.5f );
    const kvs::OrthoSlice::AlignedAxis axis = kvs::OrthoSlice::ZAxis;
    kvs::PolygonObject* object2 = new kvs::OrthoSlice( volume, position, axis, tfunc );

    delete volume;

    screen.registerObject( object );
    screen.registerObject( object2 );
    screen.registerObject( object2, new kvs::Bounds() );
    screen.show();

    return app.run();
}
