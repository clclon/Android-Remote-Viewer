
#pragma once

/// android::GraphicBuffer method
#if (__ANDROID_VER__ >= 9)

# define acap_init()

#  define acap_release()  \
    if (m_sc != nullptr) m_sc->unlock()

#  define acap_size()  \
    static_cast<size_t>(m_sc->getStride() * m_sc->getHeight() * android::bytesPerPixel(m_sc->getPixelFormat()))

#  define acap_getFormat()  \
    m_sc->getPixelFormat()

#  define acap_getPixels()  \
    vbdata

#  define acap_getWidth()  \
    m_sc->getWidth()

#  define acap_getHeight()  \
    m_sc->getHeight()

#  define acap_getStride()  \
    m_sc->getStride()


/// android::ScreenshotClient method
#else

# define acap_init()  \
    m_sc = android::ScreenshotClient()

# define acap_release()  \
    m_sc.release()

# define acap_size()  \
    static_cast<size_t>(m_sc.getSize())

#  define acap_getFormat()  \
    m_sc.getFormat()

#  define acap_getPixels()  \
    m_sc.getPixels()

#  define acap_getWidth()  \
    m_sc.getWidth()

#  define acap_getHeight()  \
    m_sc.getHeight()

#  define acap_getStride()  \
    m_sc.getStride()

#endif

#   if (__ANDROID_VER__ >= 9)
#     define acap_capture()  \
        android::ScreenshotClient::capture( \
            m_dsp, android::Rect(), 0, 0, INT32_MIN, INT32_MAX, false, 0U, &m_sc)

#   elif (__ANDROID_VER__ == 8)
#     define acap_capture()  \
        m_sc.update(m_dsp, android::Rect(0, 0), 0, 0, INT32_MIN, INT32_MAX, false, 0)

#   elif (__ANDROID_VER__ == 7)
#     define acap_capture()  \
        m_sc.update(m_dsp, android::Rect(0, 0), 0U, 0U, 0U, -1U, false, 0)

#   elif ((__ANDROID_VER__ == 6) || (__ANDROID_VER__ == 5))
#     define acap_capture()  \
        m_sc.update(m_dsp, android::Rect(0, 0), false)

#   elif (__ANDROID_API__ >= 17)
#     define acap_capture()  \
        m_sc.update(m_dsp)

#   else
#     define acap_capture()  \
        m_sc.update()

#   endif

#   define acap_lock(A) \
      m_sc->lock(android::GraphicBuffer::USAGE_SW_READ_OFTEN, A)
