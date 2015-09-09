#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/IO/Nef_polyhedron_iostream_3.h>


#include <iostream>
#include <fstream>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Polyhedron_3<CGAL::Simple_cartesian<double> > Polyhedron_d;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef Kernel::Vector_3 Vector_3;
typedef Kernel::Point_3 Point_3;
typedef  Polyhedron::Vertex_handle Vertex_handle;
typedef Kernel::Aff_transformation_3 Aff_transformation_3;

typedef Polyhedron::allocator_type allocator_type;
typedef Polyhedron::HalfedgeDS HDS;


int main(int argc, char*argv[]) {
    Polyhedron P1;
    Polyhedron P2;
    Polyhedron P12;

    std::ifstream f1("/home/sway/myProjects/DentalMeshProject/NefSetUnion/data/elephant.off");
    f1 >> P1;

     std::cout << P1.size_of_vertices() << std::endl;
    //std::ifstream f2(argv[1]);
    //f2 >> P2;

    Nef_polyhedron N1(P1);
    Nef_polyhedron N2(P1);
    //Nef_polyhedron N2;
    Nef_polyhedron N12;

    Aff_transformation_3 aff(CGAL::TRANSLATION, Vector_3(0.1, 0.11, 0, 1));
    N2.transform(aff);

    std::cout <<"Mesh1: Number of volumes-- " <<N1.number_of_volumes() << std::endl;
    std::cout <<"Mesh2: Number of volumes-- " <<N2.number_of_volumes() << std::endl;

    std::cout<<"Computing the union of the two models"<<std::endl;
    //N1+=N2;  // model-dependent crash here
    N12 = N1.join(N2);

    std::cout <<"Union: Number of volumes-- " <<N12.number_of_volumes() << std::endl;
    N12.closure().convert_to_polyhedron(P12);


    // saves reconstructed surface mesh
    std::ofstream out("/home/sway/myProjects/DentalMeshProject/NefSetUnion/union.off");
    std::cout << P1.size_of_vertices() << std::endl;
    //std::cout << P2.size_of_vertices() << std::endl;
    std::cout << P12.size_of_vertices() << std::endl;
    out << P12;
    return 0;
}

