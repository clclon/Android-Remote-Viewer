
#include "libfake.h"
#include "libfakeVector.h"
#include "libfakeDisplayInfo.h"

/// typedef void native_handle_t;
/// typedef uint8_t HandleWrapMethod;

namespace android {

class IBinder;
class IGraphicBufferProducer;

/// TODO: add
//class String8;

/// declaration -> libinder ?
class Rect
{
public:
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
    Rect();
    Rect(Rect const &);
    Rect(int32_t, int32_t);
    Rect(int32_t, int32_t, int32_t, int32_t);
    ~Rect();
};

/// declaration -> libui
class GraphicBuffer
{
public:
    GraphicBuffer(uint32_t, uint32_t, int32_t, uint32_t, uint64_t, std::string);
    GraphicBuffer(uint32_t, uint32_t, int32_t, uint32_t, uint32_t, uint32_t, void*, bool);
    GraphicBuffer(const void*, uint8_t, uint32_t, uint32_t, int32_t, uint32_t, uint64_t, uint32_t);

    uint32_t unlock();
    uint32_t lock(uint32_t, void**);
    uint32_t lock(uint32_t, const Rect&, void**);

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getStride() const;
    uint64_t getUsage() const;
    int32_t  getPixelFormat() const;
    uint32_t getLayerCount() const;
    Rect     getBounds() const;
    uint64_t getId() const;

private:
    ~GraphicBuffer();
};


/// realisation
class SurfaceComposerClient
{
public:

    SurfaceComposerClient() {}
    virtual ~SurfaceComposerClient() {}

    /*
    /// TODO: add
    static sp<IBinder> createDisplay(const String8& displayName, bool secure);
    static int32_t getAnimationFrameStats(FrameStats* outStats) { return 0; }
    int32_t        setTransparentRegionHint(const sp<IBinder>& id, const Region& transparent) { return 0; }
    */
    static sp<IBinder> getBuiltInDisplay(int32_t);
    static int32_t     getDisplayConfigs(const sp<IBinder>&, Vector<DisplayInfo>*);
    static int32_t     getDisplayConfigs(const sp<IBinder>&, std::vector<DisplayInfo>*);
    static void        destroyDisplay(const sp<IBinder>&);
    static int32_t     getDisplayInfo(const sp<IBinder>&, DisplayInfo*);
    static int         getActiveConfig(const sp<IBinder>&);
    static int32_t     setActiveConfig(const sp<IBinder>&, int);
    static void        setDisplayPowerMode(const sp<IBinder>&, int);
    static void        openGlobalTransaction();
    static void        closeGlobalTransaction(bool);
    static void        setAnimationTransaction();
    static int32_t     clearAnimationFrameStats();

    static void setDisplaySurface(const sp<IBinder>&, const sp<IGraphicBufferProducer>&);
    static void setDisplayLayerStack(const sp<IBinder>&, uint32_t);
    static void setDisplaySize(const sp<IBinder>&, uint32_t, uint32_t);
    static void setDisplayProjection(const sp<IBinder>&, uint32_t, const Rect&, const Rect&);

    int32_t    hide(const sp<IBinder>&);
    int32_t    show(const sp<IBinder>&);
    int32_t    setFlags(const sp<IBinder>&, uint32_t, uint32_t);
    int32_t    setLayer(const sp<IBinder>&, uint32_t);
    int32_t    setAlpha(const sp<IBinder>&, float);
    int32_t    setMatrix(const sp<IBinder>&, float, float, float, float);
    int32_t    setPosition(const sp<IBinder>&, float, float);
    int32_t    setSize(const sp<IBinder>&, uint32_t, uint32_t);
    int32_t    setCrop(const sp<IBinder>&, const Rect&);
    int32_t    setLayerStack(const sp<IBinder>&, uint32_t);
    int32_t    destroySurface(const sp<IBinder>&);
    int32_t    clearLayerFrameStats(const sp<IBinder>&) const;
    int32_t    getLayerFrameStats(const sp<IBinder>&, void*) const;

};

sp<IBinder> SurfaceComposerClient::getBuiltInDisplay(int id) {
          LOGPF();
          sp<IBinder> p;
          p.m_ptr = NULL;
          return p;
    }
int32_t SurfaceComposerClient::getDisplayConfigs(const sp<IBinder>& display, Vector<DisplayInfo>* configs) {
          LOGPF();
          return 0;
    }
int32_t SurfaceComposerClient::getDisplayConfigs(const sp<IBinder>& display, std::vector<DisplayInfo>* configs) {
          LOGPF();
          return 0;
    }
void    SurfaceComposerClient::destroyDisplay(const sp<IBinder>& b) {
          LOGPF();
    }
int32_t SurfaceComposerClient::getDisplayInfo(const sp<IBinder>& display, DisplayInfo* info) {
          LOGPF();
          return 0;
    }
int32_t SurfaceComposerClient::getActiveConfig(const sp<IBinder>& display) {
          LOGPF();
          return 0;
    }
int32_t SurfaceComposerClient::setActiveConfig(const sp<IBinder>& display, int id) {
          LOGPF();
          return 0;
    }
void    SurfaceComposerClient::setDisplayPowerMode(const sp<IBinder>& display, int mode) {
          LOGPF();
    }
void    SurfaceComposerClient::openGlobalTransaction() {
          LOGPF();
    }
void    SurfaceComposerClient::closeGlobalTransaction(bool synchronous) {
          LOGPF();
    }
void    SurfaceComposerClient::setAnimationTransaction() {
          LOGPF();
    }
int32_t SurfaceComposerClient::clearAnimationFrameStats() {
          LOGPF();
          return 0;
    }
void    SurfaceComposerClient::setDisplaySurface(const sp<IBinder>& token, const sp<IGraphicBufferProducer>& bufferProducer) {
          LOGPF();
    }
void    SurfaceComposerClient::setDisplayLayerStack(const sp<IBinder>& token, uint32_t layerStack) {
          LOGPF();
    }
void    SurfaceComposerClient::setDisplaySize(const sp<IBinder>& token, uint32_t width, uint32_t height) {
          LOGPF();
    }
void    SurfaceComposerClient::setDisplayProjection(const sp<IBinder>& token, uint32_t orientation, const Rect& layerStackRect, const Rect& displayRect) {
          LOGPF();
    }
int32_t    SurfaceComposerClient::hide(const sp<IBinder>& id) { return 0; }
int32_t    SurfaceComposerClient::show(const sp<IBinder>& id) { return 0; }
int32_t    SurfaceComposerClient::setFlags(const sp<IBinder>& id, uint32_t flags, uint32_t mask) { return 0; }
int32_t    SurfaceComposerClient::setLayer(const sp<IBinder>& id, uint32_t layer) { return 0; }
int32_t    SurfaceComposerClient::setAlpha(const sp<IBinder>& id, float alpha) { return 0; }
int32_t    SurfaceComposerClient::setMatrix(const sp<IBinder>& id, float dsdx, float dtdx, float dsdy, float dtdy) { return 0; }
int32_t    SurfaceComposerClient::setPosition(const sp<IBinder>& id, float x, float y) { return 0; }
int32_t    SurfaceComposerClient::setSize(const sp<IBinder>& id, uint32_t w, uint32_t h) { return 0; }
int32_t    SurfaceComposerClient::setCrop(const sp<IBinder>& id, const Rect& crop) { return 0; }
int32_t    SurfaceComposerClient::setLayerStack(const sp<IBinder>& id, uint32_t layerStack) { return 0; }
int32_t    SurfaceComposerClient::destroySurface(const sp<IBinder>& id) { return 0; }
int32_t    SurfaceComposerClient::clearLayerFrameStats(const sp<IBinder>& token) const  { return 0; }
int32_t    SurfaceComposerClient::getLayerFrameStats(const sp<IBinder>& token, void* outStats) const { return 0; }


/// realisation
class ScreenshotClient
{
    /*
    sp<IMemoryHeap> mHeap;
    uint32_t mWidth;
    uint32_t mHeight;
    PixelFormat mFormat;
    char data[64];
    */
public:
    ScreenshotClient();
    ~ScreenshotClient();

    /// < 4.0
    int32_t update ();
    /// 4.0 - 8.0 work *
    int32_t update (const sp<IBinder>&, Rect, bool);
    /// 5.0 - 8.0
    int32_t update (const sp<IBinder>&, Rect, uint32_t, uint32_t, bool);
    /// 6.0 - 7.0 (android-6.0.1_r81 / android-7.1.2_r36)
    int32_t update (const sp<IBinder>&, Rect, uint32_t, uint32_t, uint32_t, uint32_t, bool);
    int32_t update (const sp<IBinder>&, Rect, uint32_t, uint32_t, uint32_t, uint32_t, bool, uint32_t);
    /// 8.0
    int32_t update (const sp<IBinder>&, Rect, uint32_t, uint32_t, int32_t, int32_t, bool);
    int32_t update (const sp<IBinder>&, Rect, uint32_t, uint32_t, int32_t, int32_t, bool, uint32_t);
    static int32_t capture(const sp<IBinder>&, const sp<IGraphicBufferProducer>&, Rect, uint32_t, uint32_t, int32_t, int32_t, bool);
    /// 9.0
    static int32_t capture(const sp<IBinder>&, Rect, uint32_t, uint32_t, int32_t, int32_t, bool, uint32_t, sp<GraphicBuffer>*);
    static int32_t capture(const sp<IBinder>&, Rect, uint32_t, uint32_t, int32_t, int32_t, bool, uint32_t, bool, sp<GraphicBuffer>*, bool&);

    /*  Call param: 6.0
	update (const sp<IBinder>&, Rect, 0, 0, 0, -1U, false, 0|1|2|3);
    */
    /*  Call param: 8.0
	update (const sp<IBinder>&, Rect, 0, 0, INT32_MIN, INT32_MAX, false, 0|1|2|3);
    */
    /*  Call param: 9.0
	capture(const sp<IBinder>&, Rect, 0, 0, INT32_MIN, INT32_MAX, false, uint32_t, sp<GraphicBuffer>*); 
	  void* base = NULL;
	  result = outBuffer->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, &base);
	    w = outBuffer->getWidth();
	    h = outBuffer->getHeight();
	    s = outBuffer->getStride();
	    f = outBuffer->getPixelFormat();
	    d = HAL_DATASPACE_UNKNOWN;
	    size = s * h * bytesPerPixel(f);
    */


    void const* getPixels() const;
    uint32_t    getWidth() const;
    uint32_t    getHeight() const;
    uint32_t    getStride() const;
    int32_t     getFormat() const;
    size_t      getSize() const;
    void        release();
    /// 6.0 - 8.0
    int32_t     getDataSpace();
};

    /// < 4.0
int32_t ScreenshotClient::update () { LOGPF(); return 1; }
    /// 4.0 - 8.0 work *
int32_t ScreenshotClient::update (const sp<IBinder>& d, Rect r, bool b) { LOGPF(); return 1; }
    /// 5.0 - 8.0
int32_t ScreenshotClient::update (const sp<IBinder>& d, Rect r, uint32_t w, uint32_t h, bool b) { LOGPF(); return 1; }
    /// 6.0 - 7.0 (android-6.0.1_r81 / android-7.1.2_r36)
int32_t ScreenshotClient::update (const sp<IBinder>& d, Rect r, uint32_t w, uint32_t h, uint32_t x, uint32_t y, bool b) { LOGPF(); return 1; }
int32_t ScreenshotClient::update (const sp<IBinder>& d, Rect r, uint32_t w, uint32_t h, uint32_t x, uint32_t y, bool b, uint32_t o) { LOGPF(); return 1; }
    /// 8.0
int32_t ScreenshotClient::update (const sp<IBinder>& d, Rect r, uint32_t w, uint32_t h, int32_t x, int32_t y, bool b) { LOGPF(); return 1; }
int32_t ScreenshotClient::update (const sp<IBinder>& d, Rect r, uint32_t w, uint32_t h, int32_t x, int32_t y, bool b, uint32_t o) { LOGPF(); return 1; }
int32_t ScreenshotClient::capture(const sp<IBinder>& d, const sp<IGraphicBufferProducer>& p, Rect r, uint32_t w, uint32_t h, int32_t x, int32_t y, bool b) { LOGPF(); return 1; }
    /// 9.0
int32_t ScreenshotClient::capture(const sp<IBinder>& d, Rect r, uint32_t w, uint32_t h, int32_t x, int32_t y, bool b, uint32_t o, sp<GraphicBuffer> *p) { LOGPF(); return 1; }
int32_t ScreenshotClient::capture(const sp<IBinder>& d, Rect r, uint32_t w, uint32_t h, int32_t x, int32_t y, bool b, uint32_t o, bool bb, sp<GraphicBuffer> *p, bool& bbb) { LOGPF(); return 1; }

ScreenshotClient::ScreenshotClient() {
    LOGPF();
}
ScreenshotClient::~ScreenshotClient() {
    LOGPF();
}
void const* ScreenshotClient::getPixels() const {
    LOGPF();
    return NULL;
}
uint32_t ScreenshotClient::getWidth() const {
    LOGPF();
    return 0;
}
uint32_t ScreenshotClient::getHeight() const {
    LOGPF();
    return 0;
}
uint32_t ScreenshotClient::getStride() const {
    LOGPF();
    return 0;
}
int32_t ScreenshotClient::getFormat() const {
    LOGPF();
    return 0;
}
size_t ScreenshotClient::getSize() const {
    LOGPF();
    return 0;
}
int32_t ScreenshotClient::getDataSpace() {  /// >= 6.0
    LOGPF();
    return 0;
}
void ScreenshotClient::release() {
    LOGPF();
}

};

