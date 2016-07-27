#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <kvs/FileFormatBase>
#include <kvs/ValueArray>
#include <kvs/Type>

#define VERBOSE 0

namespace ReadUCD
{

class AVSUcdBinary64 : public kvs::FileFormatBase
{
public:
    typedef kvs::FileFormatBase BaseClass;
    typedef kvs::ValueArray<kvs::Real32> Coords;
    typedef kvs::ValueArray<kvs::UInt32> Connections;
    typedef kvs::ValueArray<kvs::Real32> Values;

    enum CycleType
    {
        UnknownCycleType = -1,
        Data = 0,
        Geom,
        DataGeom
    };

    enum ElementType
    {
        UnknownElementType = -1,
        Point = 0,
        Line,
        Tri,
        Quad,
        Tet,
        Pyr,
        Prism,
        Hex,
        Line2,
        Tri2,
        Quad2,
        Tet2,
        Pyr2,
        Prism2,
        Hex2
    };

private:

    CycleType m_cycle_type;
    ElementType m_element_type;
    std::vector<std::string> m_data_filenames;
    size_t m_total_nnodes; ///< total num. of nodes
    size_t m_total_nelems; ///< total num. of elems
    size_t m_nelems; ///< num. of elems specified by the given element type
    size_t m_ncomponents;
    size_t m_component_id;
    size_t m_component_veclen;
    Coords m_coords; ///< coordinate array
    Connections m_connections; ///< connection array
    Values m_values; ///< value array

public:

    AVSUcdBinary64();
    void setElementType( ElementType element_type ) { m_element_type = element_type; }
    void setComponentID( size_t id ) { m_component_id = id; }
    void setComponentVeclen( size_t veclen ) { m_component_veclen = veclen; }
    size_t totalNumberOfNodes() const { return m_total_nnodes; }
    size_t totalNumberOfElements() const { return m_total_nelems; }
    size_t numberOfElements() const { return m_nelems; }
    size_t veclen() const { return m_component_veclen; }
    ElementType elementType() const { return m_element_type; }
    const Coords& coords() const { return m_coords; }
    const Connections& connections() const { return m_connections; }
    const Values& values() const { return m_values; }
    bool read( const std::string& filename );

private:

    void read_control_file( const std::string& filename );
    void read_data_file( const std::string& filename );
    void read_file_info( std::ifstream& ifs );
    void read_step_info( std::ifstream& ifs );
    void read_node_info( std::ifstream& ifs );
    void read_elem_info( std::ifstream& ifs );
    void read_node_data( std::ifstream& ifs );

private:

    bool write( const std::string& ) { return false; }
};

} // end of namespace ReadUCD
