#include <fantom/algorithm.hpp>
#include <fantom/dataset.hpp>
#include <fantom/register.hpp>

using namespace fantom;

namespace {
    class StructuredGridAlgorithm : public DataAlgorithm {

    public:
        struct Options : public DataAlgorithm::Options {
            Options(fantom::Options::Control &control)
                    : DataAlgorithm::Options(control) {
                add<Point3>("origin", "Grid origin", Point3());
                add<Point3>("extent", "Cell count ", Point3());
                add<Point3>("spacing", "Cell size ", Point3());
            }
        };

        struct DataOutputs : public DataAlgorithm::DataOutputs {
            DataOutputs(fantom::DataOutputs::Control &control)
                    : DataAlgorithm::DataOutputs(control) {
                add<const Grid<3> >("grid");
            }
        };


        StructuredGridAlgorithm(InitData &data)
                : DataAlgorithm(data) {
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool & /*abortFlag*/ ) override {

            Point3 origin = options.get<Point3>("origin");
            Point3 extent = options.get<Point3>("extent");
            Point3 spacing = options.get<Point3>("spacing");


            size_t extentArray[3] = {static_cast<size_t>(extent[0]), static_cast<size_t>(extent[1]),
                                     static_cast<size_t>(extent[2])};
            double offsetArray[3] = {origin[0], origin[1], origin[2]};
            double spacingArray[3] = {spacing[0], spacing[1], spacing[2]};

            std::shared_ptr<const Grid<3> > grid = DomainFactory::makeUniformGrid(extentArray, offsetArray,
                                                                                  spacingArray);

            setResult("grid", grid);

        }
    };

    class UnstructuredGridAlgorithm : public DataAlgorithm {

    public:
        struct Options : public DataAlgorithm::Options {
            Options(fantom::Options::Control &control)
                    : DataAlgorithm::Options(control) {
            }
        };

        struct DataOutputs : public DataAlgorithm::DataOutputs {
            DataOutputs(fantom::DataOutputs::Control &control)
                    : DataAlgorithm::DataOutputs(control) {
                add<const Grid<3> >("grid");
            }
        };


        UnstructuredGridAlgorithm(InitData &data)
                : DataAlgorithm(data) {
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool & /*abortFlag*/ ) override {


            const std::vector <Point<3>> points = {
                    Point<3>(0, 0, 0),// 0
                    Point<3>(1, 0, 0),// 1
                    Point<3>(1, 0, 1),// 2
                    Point<3>(0, 0, 1),// 3
                    Point<3>(0, 1, 1),// 4
                    Point<3>(1, 1, 1),// 5
                    Point<3>(1, 1, 0),// 6
                    Point<3>(0, 1, 0),// 7
                    Point<3>(0.5, 1.5, 1),// 8
                    Point<3>(0.5, 1.5, 0),// 9
            };

            size_t numCellTypes = 3;

            std::pair <Cell::Type, size_t> cellCounts[] = {
                    {Cell::Type::HEXAHEDRON, 1}, // Four walls
                    {Cell::Type::TRIANGLE, 2}, // Four walls
                    {Cell::Type::QUAD, 2}, // Four walls
            };

            /**
                            3                        4--------5
           3              / |\             4        /|       /|
          /|\           /   | \          //|\      / |      / |
         / | \        5--------4        // | \    7--------6  |
        /  |  \       |     |  |       //  |  \   |  |     |  |
       /   2   \      |     2  |      /3---|--2   |  3-----|--2
      /  /   \  \     |   /  \ |     //    | /    | /      | /
     / /       \ \    | /     \|    //     |/     |/       |/
    0-------------1   0--------1   0-------1      0--------1
 */

            const std::vector <size_t> indices = {
                    // HEXAHEDRON
                    0, 1, 2, 3, 4, 5, 6, 7,
                    // Triangles
                    7, 6, 9,
                    4, 5, 8,
                    // QUADS
                    4, 7, 9, 8,
                    6, 5, 8, 9
            };


            std::shared_ptr<const Grid<3> > grid = DomainFactory::makeGrid(points, numCellTypes, cellCounts,
                                                                           indices);

            setResult("grid", grid);
        }
    };

    AlgorithmRegister <StructuredGridAlgorithm> dummy("Aufgabe1/StructuredGrid", "Generate a simple field.");
    AlgorithmRegister <UnstructuredGridAlgorithm> dummy2("Aufgabe1/UnstructuredGrid (House)", "Generate unstructured field.");
}