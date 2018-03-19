#include <stdlib.h>
#include <EGL/egl.h>

#include "tools.h"
#include "elf_reader.h"

#if defined(__arm__)
#define LIBSF_PATH "/system/lib/libsurfaceflinger.so"
#elif defined(__aarch64__)
#define LIBSF_PATH "/system/lib64/libsurfaceflinger.so"
#else
#error "unknown architecture!"
#endif

typedef EGLBoolean (*t_eglSwapBuffers_fun)(EGLDisplay dpy, EGLSurface surf);

static t_eglSwapBuffers_fun original = NULL;
static t_eglSwapBuffers_fun original2 = NULL;
static EGLBoolean new_eglSwapBuffers(EGLDisplay dpy, EGLSurface surf) {
    LOGD("new_eglSwapBuffers()");
    if (NULL == original) {
        LOGE("failed to get original eglSwapBuffers");

        return EGL_FALSE;
    }

    return original(dpy, surf);
}

void __attribute__ ((constructor)) hooker_main() {
    void *start = ElfHooker::get_module_base(getpid(), LIBSF_PATH);
    if (NULL == start) {
        return;
    }


    ElfReader elfReader(LIBSF_PATH, start);
    if (0 != elfReader.parse()) {
        LOGE("failed to parse %s in %d maps at %p", LIBSF_PATH, getpid(), start);
        return;
    }
    elfReader.hook("eglSwapBuffers", reinterpret_cast<void *>(new_eglSwapBuffers),
                   reinterpret_cast<void **>(&original));
    // elfReader.dumpElfHeader();
    // elfReader.dumpProgramHeaders();
    // elfReader.dumpDynamicSegment();
    // elfReader.dumpDynamicRel();
}
