#include <mbgl/gl/headless_backend.hpp>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>

#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>

#include <cassert>

namespace mbgl {
namespace gl {

class GlfwBackendImpl final : public HeadlessBackend::Impl {
public:
    GlfwBackendImpl() {
        printf("GlfwBackendImpl()\n");

        // GLFWwindow* window = glfwCreateWindow(1280, 720, "Headless backend", nullptr, nullptr);

        
        // EmscriptenWebGLContextAttributes attr;
        // printf("b\n");
        // emscripten_webgl_init_context_attributes(&attr);
        // printf("c\n");
        // attr.explicitSwapControl = EM_TRUE;
        // printf("d\n");
        // todo: inject ID of offscreen canvas
        // m_ctx = emscripten_webgl_create_context("#offscreenCanvas", &attr);
        // printf("e\n");
        // emscripten_webgl_make_context_current(m_ctx);
        // printf("f\n");

        // context.create();
        // surface.create();
    }
    ~GlfwBackendImpl() {
        // emscripten_webgl_make_context_current(0);
        // emscripten_webgl_destroy_context(m_ctx);
    };

    gl::ProcAddress getExtensionFunctionPointer(const char* name) {
        printf("getExtensionFunctionPointer()\n");
        // QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        // return thisContext->getProcAddress(name);
    }

    void activateContext() { 
        // pthread_attr_t attr;
        // pthread_attr_init(&attr);
        // emscripten_pthread_attr_settransferredcanvases(&attr, "#offscreenCanvas");
    }

    void deactivateContext() { 
        // context.doneCurrent(); 
    }

    bool glNeedsActiveContextOnDestruction() const { return true; }

private:
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_ctx;
    // QOpenGLContext context;
    // QOffscreenSurface surface;
};

void HeadlessBackend::createImpl() {
    assert(!impl);
    impl = std::make_unique<GlfwBackendImpl>();
}

} // namespace gl
} // namespace mbgl
