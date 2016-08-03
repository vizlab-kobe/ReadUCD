#include <kvs/Message>
#include <kvs/Timer>
#include <kvs/File>
#include <kvs/Directory>
#include <kvs/UnstructuredVolumeObject>
#include <Lib/AVSUcdBinary64.h>
#include <Lib/ImportAVSUcdBinary64.h>
#include "Read.h"
#include "Crop.h"


int main( int argc, char** argv )
{
    kvs::Directory idirname( argv[1] ); // input directory
    kvs::Directory odirname( argv[2] ); // output directory

    // Read filenames stored in the input directory.
    kvs::FileList files = idirname.fileList();
    if ( files.size() == 0 )
    {
        kvsMessageError( "Files not found in %s.", idirname.directoryPath().c_str() );
        return false;
    }

    std::cout << "Input Directory : " << idirname.directoryPath() << std::endl;
    std::cout << "Output Directory : " << odirname.directoryPath() << std::endl;

    // Read filenames of inp data from the filename list.
    kvs::FileList inp_files;
    for ( size_t i = 0; i < files.size(); i++ )
    {
        kvs::File file = files[i];
        if ( file.extension() == "inp" )
        {
            inp_files.push_back( file );
        }
    }

    if ( inp_files.size() == 0 )
    {
        kvsMessageError( "Inp files not found in %s.", idirname.directoryPath().c_str() );
        return false;
    }

    std::cout << "Number of INP files : " << inp_files.size() << std::endl;

    std::cout << std::endl;

    // Read the data and write it in KVSML format.
    for ( size_t i = 0; i < inp_files.size(); i++ )
    {
        std::cout << "Convert [" << i + 1 << "/" << inp_files.size() << "]" << std::endl;

        std::string ifilename( inp_files[i].filePath() );

        ReadUCD::AVSUcdBinary64 ucd;
        ucd.setElementType( ReadUCD::AVSUcdBinary64::Prism );
        //ucd.setElementType( ReadUCD::AVSUcdBinary64::Tet );

        kvs::Indent indent(3);

        std::cout << indent << "Reading " << ifilename << " ... " << std::flush;
        kvs::Timer timer( kvs::Timer::Start );
        local::ReadTemperature( ucd, ifilename );
        //local::ReadPressure( ucd, filename );
        //local::ReadDensity( ucd, filename );
        //local::ReadVelocity( ucd, filename );
        timer.stop();
        std::cout << "done. [" << timer.sec() << " sec]" << std::endl;

        std::cout << indent << "Importing ... " << std::flush;
        timer.start();
        kvs::UnstructuredVolumeObject* volume = ReadUCD::ImportAVSUcdBinary64( ucd );
        timer.stop();
        std::cout << "done. [" << timer.sec() << " sec]" << std::endl;

        std::cout << indent << "Cropping ... " << std::flush;
        timer.start();
        local::Crop( volume, kvs::Vec3::All(-30), kvs::Vec3::All(30) );
        timer.stop();
        std::cout << "done. [" << timer.sec() << " sec]" << std::endl;

        std::string ofilename( odirname.directoryPath() + "/" + inp_files[i].baseName() + ".kvsml" );
        timer.start();
        std::cout << indent << "Writing " << ofilename << " ... " << std::flush;
        bool ascii = false;
        bool external = true;
        volume->write( ofilename, ascii, external );
        timer.stop();
        std::cout << "done. [" << timer.sec() << " sec]" << std::endl;

        delete volume;
    }

    return 0;
}
