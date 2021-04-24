#pragma once

#include <memory>

#include <app/DataAPI.hpp>
#include <app/Model.hpp>
#include <app/NodeTreeModel.hpp>
#include <app/Updates.hpp>

namespace image {

    class ProgramModel : public NodeTreeModel {
        Q_OBJECT
    public:
        inline std::shared_ptr<Program> program() noexcept { return program_; }

        void setProgram(std::shared_ptr<Program> program) noexcept;

        ProgramModel() noexcept : NodeTreeModel() {}

    private:
        std::shared_ptr<Program> program_;
    };

}
