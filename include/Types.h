// -----------------------------------------------------------------------------
// libDDG -- Types.h
// -----------------------------------------------------------------------------
//
// This file contains forward declarations of common types and definitions of
// convenience types for standard iterators.
//

#ifndef DDG_TYPES_H
#define DDG_TYPES_H

#include <map>
#include <vector>

namespace SW
{
   // forward declarations
   class Edge;
   class Face;
   class HalfEdge;
   class Image;
   class Mesh;
   class MeshIO;
   class Vector;
   class Vertex;
   class Variable;

   // convenience types for iterators
   typedef std::map<Variable*,double>::iterator            TermIter;
   typedef std::map<Variable*,double>::const_iterator     TermCIter;
   typedef std::map<Variable*,int>::iterator              IndexIter;
   typedef std::map<Variable*,int>::const_iterator       IndexCIter;
   typedef std::vector<HalfEdge>::iterator             HalfEdgeIter;
   typedef std::vector<HalfEdge>::const_iterator      HalfEdgeCIter;
   typedef std::vector<Vertex>::iterator                 VertexIter;
   typedef std::vector<Vertex>::const_iterator          VertexCIter;
   typedef std::vector<Edge>::iterator                     EdgeIter;
   typedef std::vector<Edge>::const_iterator              EdgeCIter;
   typedef std::vector<Face>::iterator                     FaceIter;
   typedef std::vector<Face>::const_iterator              FaceCIter;
}

#endif
