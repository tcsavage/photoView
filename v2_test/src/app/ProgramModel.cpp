#include <app/ProgramModel.hpp>

#include <app/DataAPI.hpp>

namespace image {

    namespace {

        std::unique_ptr<Node> buildNodeTree(std::shared_ptr<Program> program) noexcept {
            auto root = std::make_unique<Node>(program);

            for (auto &&function : program->functions) {
                root->addChildUnchecked(function, &dynInfo(*function));
            }

            return root;
        }

    }

    void ProgramModel::setProgram(std::shared_ptr<Program> program) noexcept {
        program_ = program;
        setRoot(buildNodeTree(program));
    }

}
