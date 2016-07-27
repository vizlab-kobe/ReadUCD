#pragma once
#include "AVSUcdBinary64.h"
#include <kvs/UnstructuredVolumeObject>


namespace ReadUCD
{

kvs::UnstructuredVolumeObject* ImportAVSUcdBinary64( const ReadUCD::AVSUcdBinary64& ucd );

} // end of namespace ReadUCD
