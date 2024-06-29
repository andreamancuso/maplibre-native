#include <mbgl/gl/headless_backend.hpp>

#include <cassert>

namespace mbgl {
namespace gl {

class GlfwBackendImpl final : public HeadlessBackend::Impl {
public:
    GlfwBackendImpl() {
        printf("GlfwBackendImpl()\n");
        // context.create();
        // surface.create();
    }
    ~GlfwBackendImpl() = default;

    gl::ProcAddress getExtensionFunctionPointer(const char* name) {
        printf("getExtensionFunctionPointer()\n");
        // QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        // return thisContext->getProcAddress(name);
    }

    void activateContext() { 
        // context.makeCurrent(&surface); 
    }

    void deactivateContext() { 
        // context.doneCurrent(); 
    }

    bool glNeedsActiveContextOnDestruction() const { return true; }

private:
    // QOpenGLContext context;
    // QOffscreenSurface surface;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<GlfwBackendImpl>();
}

} // namespace gl
} // namespace mbgl
