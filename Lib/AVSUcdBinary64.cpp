#include "AVSUcdBinary64.h"
#include <iostream>
#include <kvs/File>
#include <kvs/IgnoreUnusedVariable>


namespace
{

std::string GetString( std::ifstream& ifs, size_t n )
{
    std::string ret( n, '\0' );
    ifs.read( &ret[0], n );
    return ret;
}

template <typename T>
T GetValue( std::ifstream& ifs )
{
    T ret = 0;
    ifs.read( (char*)( &ret ), sizeof(T) );
    return ret;
}

template <typename T>
kvs::ValueArray<T> GetValues( std::ifstream& ifs, const size_t size )
{
    kvs::ValueArray<T> ret( size );
    ifs.read( (char*)( ret.data() ), sizeof(T) * size );
    return ret;
}

ReadUCD::AVSUcdBinary64::Coords GetCoords1( std::ifstream& ifs, size_t nnodes )
{
    ReadUCD::AVSUcdBinary64::Coords coords( nnodes * 3 );
    for ( size_t i = 0; i < nnodes; i++ )
    {
        const size_t index = GetValue<long>( ifs );
        const float x = GetValue<float>( ifs );
        const float y = GetValue<float>( ifs );
        const float z = GetValue<float>( ifs );
        coords[ 3 * index + 0 ] = x;
        coords[ 3 * index + 1 ] = y;
        coords[ 3 * index + 2 ] = z;
    }

    return coords;
}

ReadUCD::AVSUcdBinary64::Coords GetCoords2( std::ifstream& ifs, size_t nnodes )
{
    ReadUCD::AVSUcdBinary64::Coords coords( nnodes * 3 );
    kvs::ValueArray<long> indices = GetValues<long>( ifs, nnodes );
    kvs::ValueArray<float> xcoords = GetValues<float>( ifs, nnodes );
    kvs::ValueArray<float> ycoords = GetValues<float>( ifs, nnodes );
    kvs::ValueArray<float> zcoords = GetValues<float>( ifs, nnodes );
    for ( size_t i = 0; i < nnodes; i++ )
    {
        const size_t index = indices[i];
        coords[ 3 * index + 0 ] = xcoords[index];
        coords[ 3 * index + 1 ] = ycoords[index];
        coords[ 3 * index + 2 ] = zcoords[index];
    }

    return coords;
}

}

namespace ReadUCD
{

AVSUcdBinary64::AVSUcdBinary64():
    m_cycle_type( AVSUcdBinary64::UnknownCycleType ),
    m_element_type( AVSUcdBinary64::UnknownElementType ),
    m_total_nnodes( 0 ),
    m_total_nelems( 0 ),
    m_nelems( 0 ),
    m_ncomponents( 0 ),
    m_component_id( 0 ),
    m_component_veclen( 0 )
{
}

bool AVSUcdBinary64::read( const std::string& filename )
{
    BaseClass::setFilename( filename );
    BaseClass::setSuccess( true );

    try
    {
        this->read_control_file( filename );
        this->read_data_file( m_data_filenames[0] );
    }
    catch ( const char* const error )
    {
        kvsMessageError( "%s: %s", filename.c_str(), error );
        BaseClass::setSuccess( false );
        return false;
    }

    return true;
}

void AVSUcdBinary64::read_control_file( const std::string& filename )
{
    std::ifstream ifs( filename.c_str(), std::ios::in );
    if ( !ifs.is_open() ) { throw "Cannot open file."; }

    const std::string path = kvs::File( BaseClass::filename() ).pathName( true );

    std::string buffer;
    while ( ifs && std::getline( ifs, buffer ) )
    {
        if ( buffer[0] == '#' ) { continue; }
        if ( buffer == "data" ) { m_cycle_type = Data; break; }
        if ( buffer == "geom" ) { m_cycle_type = Geom; break; }
        if ( buffer == "data_geom" ) { m_cycle_type = DataGeom; break; }
    }

    while ( ifs && std::getline( ifs, buffer ) )
    {
        if ( buffer[0] == '#' ) { continue; }
        const std::string data_filename = path + kvs::File::Separator() + buffer;
        m_data_filenames.push_back( data_filename );
    }

    ifs.close();
}

void AVSUcdBinary64::read_data_file( const std::string& filename )
{
    std::ifstream ifs( filename.c_str(), std::ios::in | std::ios::binary );
    if ( !ifs.is_open() ) { throw "Cannot open file."; }

    this->read_file_info( ifs );
    this->read_step_info( ifs );
    this->read_node_info( ifs );
    this->read_elem_info( ifs );
    this->read_node_data( ifs );

    ifs.close();
}

void AVSUcdBinary64::read_file_info( std::ifstream& ifs )
{
    std::string keyword = ::GetString( ifs, 7 );
    float version = ::GetValue<float>( ifs );
    kvs::IgnoreUnusedVariable( version );

    if ( keyword != "AVSUC64" ) throw "Not supported for reading 32bit binary data.";

#if VERBOSE
    std::cout << "keyword = " << keyword << std::endl;
    std::cout << "version = " << version << std::endl;
#endif
}

void AVSUcdBinary64::read_step_info( std::ifstream& ifs )
{
    std::string title = ::GetString( ifs, 70 );
    int step_number = ::GetValue<int>( ifs );
    int step_time = ::GetValue<int>( ifs );
    kvs::IgnoreUnusedVariable( step_number );
    kvs::IgnoreUnusedVariable( step_time );

#if VERBOSE
    std::cout << "title = " << title << std::endl;
    std::cout << "step number = " << step_number << std::endl;
    std::cout << "step time = " << step_time << std::endl;
#endif
}

void AVSUcdBinary64::read_node_info( std::ifstream& ifs )
{
    m_total_nnodes = ::GetValue<long>( ifs );
    int desc_type = ::GetValue<int>( ifs );

    m_coords = desc_type == 1 ?
        ::GetCoords1( ifs, m_total_nnodes ) :
        ::GetCoords2( ifs, m_total_nnodes );

#if VERBOSE
    std::cout << "total nnodes = " << m_total_nnodes << std::endl;
    std::cout << "description type = " << desc_type << std::endl;
#endif
}

void AVSUcdBinary64::read_elem_info( std::ifstream& ifs )
{
    m_total_nelems = ::GetValue<long>( ifs );

    // Element IDs
    ifs.seekg( sizeof(long) * m_total_nelems, std::ios_base::cur );

    // Material numbers
    ifs.seekg( sizeof(int) * m_total_nelems, std::ios_base::cur );

    // Element types
    kvs::ValueArray<char> element_types = ::GetValues<char>( ifs, m_total_nelems );

    kvs::ValueArray<long> element_counter( 15 );
    element_counter.fill(0);
    for ( size_t i = 0; i < m_total_nelems; i++ )
    {
        const int element_type = element_types[i];
        element_counter[ element_type ]++;
    }

#if VERBOSE
    std::cout << "total nelems = " << m_total_nelems << std::endl;
    const std::string element_type_names[15] = {
        "Point", "Line", "Tri", "Quad", "Tet", "Pyr", "Prism", "Hex",
        "Line2", "Tri2", "Quad2", "Tet2", "Pyr2", "Prism2", "Hex2"
    };
    for ( size_t i = 0; i < 15; i++ )
    {
        std::cout << "\t" << element_type_names[i] << ": " << element_counter[i] << std::endl;
    }
#endif

    const size_t nnodes_per_element[15] = {
        1, 2, 3, 4, 4, 5, 6, 8,
        3, 6, 8, 10, 13, 15, 20
    };

    if ( m_element_type == UnknownElementType ) throw "Reading element type is not spefied.";

    const long nelems = element_counter[ m_element_type ];
    const long nnodes = nnodes_per_element[ m_element_type ];
    kvs::ValueArray<kvs::UInt32> connections( nelems * nnodes );
    kvs::UInt32* pconnections = connections.data();
    for ( size_t i = 0; i < m_total_nelems; i++ )
    {
        int element_type( element_types[i] );
        if ( element_type == int( m_element_type ) )
        {
            for ( size_t j = 0; j < nnodes_per_element[ element_type ]; j++ )
            {
                *(pconnections++) = ::GetValue<long>( ifs );
            }
        }
        else
        {
            ifs.seekg( nnodes_per_element[ element_type ] * sizeof(long), std::ios_base::cur );
        }
    }

    m_nelems = nelems;
    m_connections = connections;
}

void AVSUcdBinary64::read_node_data( std::ifstream& ifs )
{
    m_ncomponents = ::GetValue<int>( ifs );
    int desc_type = ::GetValue<int>( ifs );

#if VERBOSE
    std::cout << "ncomponents = " << m_ncomponents << std::endl;
    std::cout << "description type = " << desc_type << std::endl;
#endif

    switch ( desc_type )
    {
    case 1:
    {
        // Not yet implemented
        break;
    }
    case 2:
    {
        for ( size_t i = 0; i < m_ncomponents; i++ )
        {
            std::string name = ::GetString( ifs, 16 );
            std::string unit = ::GetString( ifs, 16 );
            int veclen = ::GetValue<int>( ifs );
            int flag = ::GetValue<int>( ifs );
            float value = ::GetValue<float>( ifs );
#if VERBOSE
            std::cout << "\tcomp id = " << i << std::endl;
            std::cout << "\tname = " << name << std::endl;
            std::cout << "\tunit = " << unit << std::endl;
            std::cout << "\tveclen = " << veclen << std::endl;
            std::cout << "\tflag = " << flag << std::endl;
            std::cout << "\tvalue = " << value << std::endl;
#else
            kvs::IgnoreUnusedVariable( name );
            kvs::IgnoreUnusedVariable( unit );
            kvs::IgnoreUnusedVariable( veclen );
            kvs::IgnoreUnusedVariable( flag );
            kvs::IgnoreUnusedVariable( value );
#endif
        }

        std::vector<kvs::ValueArray<float> > values_set;
        size_t counter = 0;
        size_t component_id = m_component_id;
        for ( size_t i = 0; i < m_ncomponents; i++ )
        {
            kvs::ValueArray<float> values = ::GetValues<float>( ifs, m_total_nnodes );
            if ( component_id == i )
            {
                values_set.push_back( values );
                counter++;
                if ( m_component_veclen != 1 ) { component_id++; }
            }

            if ( counter >= m_component_veclen ) { break; }
        }

        if ( m_component_veclen == 1 ) { m_values = values_set[0]; }
        else if ( m_component_veclen == 3 )
        {
            kvs::ValueArray<float> values( m_total_nnodes * 3 );
            for ( size_t i = 0; i < m_total_nnodes; i++ )
            {
                values[ 3 * i + 0 ] = values_set[0][i];
                values[ 3 * i + 1 ] = values_set[1][i];
                values[ 3 * i + 2 ] = values_set[2][i];
            }
            m_values = values;
        }
        break;
    }
    case 3:
    {
        // Not yet implemented
        break;
    }
    case 4:
    {
        // Not yet implemented
        break;
    }
    default: break;
    }
}

} // end of namespace ReadUCD
