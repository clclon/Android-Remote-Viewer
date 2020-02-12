
#include "ARemote.h"
#include "Event.Display.Debug.h"

namespace Event
{
    // Auto rotate deduction,
    //       Orientation: [sys][user]
    const uint16_t OrientRMap[7][7] =
    {
        /* O_0       x */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }, // 0
        /* O_90      x */ { dspo::O_270,    dspo::O_0,     dspo::O_90,    dspo::O_180,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }, // 1
        /* O_180     x */ { dspo::O_180,    dspo::O_270,   dspo::O_90,    dspo::O_0,     dspo::O_MIRROR, dspo::O_180,   dspo::O_270     }, // 2
        /* O_270     x */ { dspo::O_90,     dspo::O_180,   dspo::O_270,   dspo::O_0,     dspo::O_MIRROR, dspo::O_180,   dspo::O_270     }, // 3
        /* O_MIRROR    */ { dspo::O_MIRROR, dspo::O_MIRROR,dspo::O_MIRROR,dspo::O_MIRROR,dspo::O_MIRROR, dspo::O_MIRROR, dspo::O_MIRROR }, // 4
        /* O_PORTRAIT  */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270,    }, // 5
        /* O_LANDSCAPE */ { dspo::O_90,     dspo::O_180,   dspo::O_270,   dspo::O_0,     dspo::O_MIRROR, dspo::O_0,     dspo::O_270,    }  // 6
    };
    //
    const uint16_t OrientNMap[7][7] =
    {
        /* O_0       x */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }, // 0
        /* O_90      x */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }, // 1
        /* O_180     x */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }, // 2
        /* O_270     x */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }, // 3
        /* O_MIRROR    */ { dspo::O_MIRROR, dspo::O_MIRROR,dspo::O_MIRROR,dspo::O_MIRROR,dspo::O_MIRROR, dspo::O_MIRROR, dspo::O_MIRROR }, // 4
        /* O_PORTRAIT  */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }, // 5
        /* O_LANDSCAPE */ { dspo::O_0,      dspo::O_90,    dspo::O_180,   dspo::O_270,   dspo::O_MIRROR, dspo::O_0,     dspo::O_270     }  // 6
    };
    //
    StreamRawData::StreamRawData()
      : x(0), y(0), sizeIn(0),
        qualityOut(0), orientSwap(false), orientOut(dspo::O_0),
        ratioOut(erat::R_1_1) {}
    //
    StreamRawData::StreamRawData(
                    App::Config & cnf,
                    StreamRatioEnum r,
                    dspo o,
                    uint32_t q)
      : x(0), y(0), sizeIn(0),
        qualityOut(0), orientSwap(false), orientOut(dspo::O_0),
        ratioOut(erat::R_1_1)
    {
        init(cnf, r, o, q);
    }

    void StreamRawData::init(App::Config & cnf,
                    StreamRatioEnum r,
                    dspo o,
                    uint32_t q)
    {
        x               = cnf.display.w;
        y               = cnf.display.h;
        sizeIn          = cnf.display.sz;
        qualityOut      = q;
        orientSwap      = false;
        orientOut       = o;
        ratioOut        = r;
        idata[I_BPP]    = cnf.display.b;
        idata[I_STRIDE] = ((cnf.display.s > cnf.display.w) ?
                            (cnf.display.s - cnf.display.w) : 0U);
        //
        if (ratioOut != erat::R_1_1)
        {
            double d;
            switch (ratioOut)
            {
                case erat::R_1_2: d = 2.0; break;
                case erat::R_1_3: d = 3.0; break;
                default:
                    throw std::system_error(App::make_error_code(App::ErrorId::error_display_not_ratio));
            }
            idata[I_X]      = static_cast<uint32_t>(std::round(x / d));
            idata[I_Y]      = static_cast<uint32_t>(std::round(y / d));
            idata[I_X]     += (idata[I_X] % 2);
            idata[I_Y]     += (idata[I_Y] % 2);
            idata[I_SZDST]  = (idata[I_X] * 3) * idata[I_Y];
            idata[I_SZDST] += (idata[I_SZDST] % 3) + 3;
            idata[I_PAD]    = static_cast<uint32_t>(std::round(d));
        }
        else
        {
            idata[I_X]     = x;
            idata[I_Y]     = y;
            idata[I_SZDST] = (x * 3) * y;
            idata[I_PAD]   = 1U;
        }
        idata[I_ROTATE] = ((cnf.display.changexy) ?
                            Event::OrientNMap[cnf.display.o][orientOut] :
                            Event::OrientRMap[cnf.display.o][orientOut]
                        );
        switch (idata[I_ROTATE])
        {
            case dspo::O_90:
            case dspo::O_270:
            case dspo::O_LANDSCAPE:
                orientSwap = true;
                break;
            default:
                orientSwap = false;
                break;
        }
        //
        LOGDI("StreamRawData rotate:",
              "display =", cnf.display.o,
              ", user =", orientOut,
              ", calculate result =", idata[I_ROTATE],
              ", size swap =", ((orientSwap) ? "yes" : "no" )
            );
        LOGDI("StreamRawData out:",
              "X =", idata[I_X],
              ", Y =", idata[I_Y],
              ", pad =", idata[I_PAD],
              ", size =", idata[I_SZDST],
              ", ratio =", ratioOut
            );
    }
    StreamRawData::~StreamRawData() {}
};
