#include <image/opencl/Manager.hpp>

#include <cassert>

namespace image::opencl {

    static Manager *theManager_ { nullptr };

    Manager::Manager() noexcept
        : context(config.selectDevice())
        , queue(context)
        , bufferDevice(std::make_shared<memory::OpenCLDevice>(
            opencl::ContextHandle(context.getHandle()),
            opencl::CommandQueueHandle(queue.getHandle())
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

}
