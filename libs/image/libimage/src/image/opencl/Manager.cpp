#include <image/opencl/Manager.hpp>

#include <cassert>

#include <cmrc/cmrc.hpp>

#include <image/opencl/BufferDevice.hpp>
#include <image/opencl/Program.hpp>

CMRC_DECLARE(image::rc);

namespace image::opencl {

    static Manager *theManager_ { nullptr };

    Manager::Manager() noexcept
        : context(config.selectDevice())
        , queue(context)
        , bufferDevice(std::make_shared<memory::OpenCLDevice>(
            ContextHandle(context.getHandle()),
            CommandQueueHandle(queue.getHandle())
        ))
    {
        assert(theManager_ == nullptr);
        theManager_ = this;
    }

    Manager::~Manager() noexcept {
        assert(theManager_ == this);
        theManager_ = nullptr;
    }

    Manager *Manager::the() noexcept {
        return theManager_;
    }

    Expected<Program, Error> Manager::programFromResource(const String &filename) noexcept { 
        if (auto it = programs.find(filename); it != programs.end()) {
            return it->second;
        }

        auto fs = cmrc::image::rc::get_filesystem();
        auto f = fs.open(filename);
        String src { f.begin(), f.end() };

        auto maybeProg = Program::fromSource(context, src);
        if (maybeProg.hasError()) {
            return Unexpected(maybeProg.error());
        }
        auto prog = std::move(*maybeProg);

        auto buildResult = prog.build();
        if (buildResult.hasError()) {
            return Unexpected(maybeProg.error());
        }

        programs.insert({filename, prog});
        return prog;
    }

}
