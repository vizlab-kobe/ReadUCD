#pragma once

#include <string>
#include <Lib/AVSUcdBinary64.h>


namespace local
{

inline void ReadPressure( ReadUCD::AVSUcdBinary64& ucd, std::string& filename )
{
    ucd.setComponentID( 0 );
    ucd.setComponentVeclen( 1 );
    ucd.read( filename );
}

inline void ReadDensity( ReadUCD::AVSUcdBinary64& ucd, std::string& filename )
{
    ucd.setComponentID( 2 );
    ucd.setComponentVeclen( 1 );
    ucd.read( filename );
}

inline void ReadTemperature( ReadUCD::AVSUcdBinary64& ucd, std::string& filename )
{
    ucd.setComponentID( 3 );
    ucd.setComponentVeclen( 1 );
    ucd.read( filename );
}

inline void ReadVelocity( ReadUCD::AVSUcdBinary64& ucd, std::string& filename )
{
    ucd.setComponentID( 5 );
    ucd.setComponentVeclen( 3 );
    ucd.read( filename );
}

} // end of namespace local
