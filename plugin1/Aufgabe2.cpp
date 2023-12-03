#include <fantom/algorithm.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
#include <fantom/dataset.hpp>
#include <math.h>

// needed for BoundinSphere-Calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

using namespace fantom;
using namespace fantom::graphics;

namespace {

    class GraphicsAlgorithm : public VisAlgorithm {

    public:
        struct Options : public VisAlgorithm::Options {
            Options(fantom::Options::Control &control)
                    : VisAlgorithm::Options(control) {
                add < Grid < 3 > > ("grid", "A 3D vector field");
                add<Color>("color", "The color of the graphics.", Color(0.75, 0.75, 0.0));
                add<bool>("singleCell", "An example boolean value.", false);
                add<int>("cellIndex", "An example integral value.", 0);
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs {
            // These are the graphic outputs which can be toggled on and off in the interface.
            VisOutputs(fantom::VisOutputs::Control &control)
                    : VisAlgorithm::VisOutputs(control) {
                addGraphics("gridCells");
                addGraphics("gridLines");
            }
        };

        GraphicsAlgorithm(InitData &data)
                : VisAlgorithm(data) {
        }


        void addQuadCellIndices(std::vector<unsigned int> &indices, std::vector <PointF<3>> &lineVertices,
                                const std::vector <PointF<3>> &points, const std::vector <size_t> &indexedVerts) {
            indices.push_back(indexedVerts[0]);
            indices.push_back(indexedVerts[1]);
            indices.push_back(indexedVerts[2]);
            indices.push_back(indexedVerts[0]);
            indices.push_back(indexedVerts[2]);
            indices.push_back(indexedVerts[3]);
            addQuadCellLines(lineVertices, {
                    points[indexedVerts[0]], points[indexedVerts[1]], points[indexedVerts[2]], points[indexedVerts[3]]
            });
        }


        void addQuadCellLines(std::vector <PointF<3>> &lineVertices, const std::vector <PointF<3>> &points) {
            for (int i = 0; i < points.size(); i++) {
                lineVertices.push_back(points[i]);
                lineVertices.push_back(points[(i + 1) % points.size()]);
            }
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool &abortFlag) override {


            std::shared_ptr<const Grid<3>> grid = options.get < Grid < 3 >> ("grid");
            Color color = options.get<Color>("color");


            if (!grid) {
                debugLog() << "No Grid";
                return;
            }

            const bool singleCell = options.get<bool>("singleCell");
            const int cellIndex = options.get<int>("cellIndex");


            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath("utils/Graphics");
            auto const &system = graphics::GraphicsSystem::instance();
            std::vector <PointF<3>> vertices;
            std::vector<unsigned int> indices;
            std::vector <PointF<3>> lineVertices;

            const ValueArray <Point3> &points = grid->points();
            for (size_t i = 0; i < points.size(); i++) {
                vertices.push_back(PointF<3>(
                        points[i][0],
                        points[i][1],
                        points[i][2]
                ));
            }


            for (size_t i = 0; i < grid->numCells(); i++) {

                if (singleCell && cellIndex != i) continue;

                Cell cell = grid->cell(i);
                if (cell.type() == Cell::Type::TRIANGLE) {
                    for (size_t j = 0; j != cell.numVertices(); ++j) {
                        indices.push_back(cell.index(j));

                        lineVertices.push_back(vertices[cell.index(j)]);
                        lineVertices.push_back(vertices[cell.index((j + 1) % cell.numVertices())]);

                    }
                }
                if (cell.type() == Cell::Type::QUAD) {
                    addQuadCellIndices(indices, lineVertices, vertices,
                                       {cell.index(0), cell.index(1), cell.index(2), cell.index(3)});
                }
                if (cell.type() == Cell::Type::HEXAHEDRON) {
                    addQuadCellIndices(indices, lineVertices, vertices,
                                       {cell.index(0), cell.index(1), cell.index(2), cell.index(3)});
                    addQuadCellIndices(indices, lineVertices, vertices,
                                       {cell.index(0), cell.index(1), cell.index(6), cell.index(7)});
                    addQuadCellIndices(indices, lineVertices, vertices,
                                       {cell.index(0), cell.index(3), cell.index(4), cell.index(7)});
                    addQuadCellIndices(indices, lineVertices, vertices,
                                       {cell.index(5), cell.index(6), cell.index(1), cell.index(2)});
                    addQuadCellIndices(indices, lineVertices, vertices,
                                       {cell.index(5), cell.index(6), cell.index(7), cell.index(4)});
                    addQuadCellIndices(indices, lineVertices, vertices,
                                       {cell.index(5), cell.index(2), cell.index(3), cell.index(4)});
                }
            }

            auto bs = computeBoundingSphere(vertices);
            auto norm = computeNormals(vertices, indices);

            std::shared_ptr <graphics::Drawable> triangles = system.makePrimitive(
                    graphics::PrimitiveConfig{graphics::RenderPrimitives::TRIANGLES}
                            .vertexBuffer("position", system.makeBuffer(vertices))
                            .vertexBuffer("normal", system.makeBuffer(norm))
                            .indexBuffer(system.makeIndexBuffer(indices))
                            .uniform("color", color)
                            .renderOption(graphics::RenderOption::Blend, true)
                            .boundingSphere(bs),
                    system.makeProgramFromFiles(resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
                                                resourcePath + "shader/surface/phong/singleColor/fragment.glsl"));

            setGraphics("gridCells", triangles);


            std::shared_ptr <graphics::Drawable> lines
                    = system.makePrimitive(graphics::PrimitiveConfig{graphics::RenderPrimitives::LINES}
                                                   .vertexBuffer("in_vertex", system.makeBuffer(lineVertices))
                                                   .uniform("u_color", color)
                                                   .uniform("u_lineWidth", 2)
                                                   .boundingSphere(bs),
                                           system.makeProgramFromFiles(
                                                   resourcePath + "shader/line/noShading/singleColor/vertex.glsl",
                                                   resourcePath + "shader/line/noShading/singleColor/fragment.glsl"));
            setGraphics("gridLines", lines);
        }
    };

    AlgorithmRegister <GraphicsAlgorithm> dummy3("Aufgabe2/Visualization", "Custom Vis Alg");
}