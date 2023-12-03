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

    class FieldsTutorialAlgorithm : public VisAlgorithm {

    public:
        struct Options : public VisAlgorithm::Options {
            Options(fantom::Options::Control &control)
                    : VisAlgorithm::Options(control) {
                add< Field< 3, Vector3 > >( "Field", "A 3D vector field", definedOn< Grid< 3 > >( Grid< 3 >::Points ) );
                add<double>("threshold", "The time when to sample the field.", 0.0);
                add<Color>("color", "The color of the graphics.", Color(0.75, 0.75, 0.0));
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs {
            VisOutputs(fantom::VisOutputs::Control &control)
                    : VisAlgorithm::VisOutputs(control) {
                addGraphics("field");
            }
        };

        FieldsTutorialAlgorithm(InitData &data)
                : VisAlgorithm(data) {
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool &abortFlag) override {

            std::shared_ptr< const Function< Vector3 > > function = options.get< Function< Vector3 > >( "Field" );


            // if there is no input, do nothing
            if (!function) {
                debugLog() << "Input Field not set." << std::endl;
                return;
            }

            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath("utils/Graphics");
            auto const &system = graphics::GraphicsSystem::instance();

            double scale = options.get<double>("threshold");
            Color color = options.get<Color>("color");


            auto performanceObjectRenderer = std::make_shared<graphics::ObjectRenderer>(system);
            performanceObjectRenderer->reserve(graphics::ObjectRenderer::ObjectType::SPHERE, 18);

            performanceObjectRenderer->addSphere(Point3(2.0, 0.0, 0.0), 0.25, color);

            setGraphics("field", performanceObjectRenderer->commit());

        }
    };

    AlgorithmRegister <FieldsTutorialAlgorithm> dummy("Aufgabe3/Field",
                                                      "Show vector field glyphs at cell centers.");
} 