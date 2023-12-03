#include <fantom/algorithm.hpp>
#include <fantom/dataset.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
#include <math.h>

// needed for BoundinSphere-Calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

#include <stdexcept>
#include <vector>

using namespace fantom;


namespace {

    class FieldVisualizationAlgorithm : public VisAlgorithm {

    public:
        struct Options : public VisAlgorithm::Options {
            Options(fantom::Options::Control &control)
                    : VisAlgorithm::Options(control) {
                add < Function < Scalar >> ("Field", "A 3D vector field");
                add<double>("threshold", "The time when to sample the field.", 0.0);
                add<double>("radius", "The time when to sample the field.", 0.25);
                add<bool>("scaleSpheres", "The time when to sample the field.", false);
                add<Color>("color", "The color of the graphics.", Color(0.75, 0.75, 0.0));
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs {
            VisOutputs(fantom::VisOutputs::Control &control)
                    : VisAlgorithm::VisOutputs(control) {
                addGraphics("field");
            }
        };

        FieldVisualizationAlgorithm(InitData &data)
                : VisAlgorithm(data) {
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool &abortFlag) override {

            std::shared_ptr<const Function <Scalar> > function = options.get < Function < Scalar > > ("Field");
            if (!function) {
                debugLog() << "Input Field not set or supported." << std::endl;
                return;
            }
            std::shared_ptr<const Grid<3> > grid = std::dynamic_pointer_cast<const Grid<3>>(function->domain());

            // if there is no input, do nothing


            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath("utils/Graphics");
            auto const &system = graphics::GraphicsSystem::instance();

            double threshold = options.get<double>("threshold");
            double radius = options.get<double>("radius");
            bool scaleSpheres = options.get<bool>("scaleSpheres");
            Color color = options.get<Color>("color");


            auto performanceObjectRenderer = std::make_shared<graphics::ObjectRenderer>(system);

            auto evaluator = function->makeDiscreteEvaluator();

            performanceObjectRenderer->reserve(graphics::ObjectRenderer::ObjectType::SPHERE, grid->points().size());

            for (size_t i = 0; i < grid->points().size(); i++) {
                Point3 p = grid->points()[i];
                double value = evaluator->value(i)[0];
                if (value > threshold) {
                    performanceObjectRenderer->addSphere(p, scaleSpheres ? value * radius : radius, color);
                }
            }


            setGraphics("field", performanceObjectRenderer->commit());

        }
    };

    AlgorithmRegister <FieldVisualizationAlgorithm> dummy("Aufgabe3/Field",
                                                          "Show vector field glyphs at cell centers.");
} 