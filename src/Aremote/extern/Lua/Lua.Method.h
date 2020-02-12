
#pragma once

/** @file
 @page lua_methods Lua methods API
 @brief Lua virtual methods help.
 @var class aremote {};
 @class aremote
 */

#if !defined(DOXYGEN_BUILD)
namespace Lua
{
    namespace Method
    {
#endif
        /**
         @brief Check time period, time string format: "HH:mm:ss" or "HH:mm" or "HH"
         @luamethod aremote:checkTime(string time_start, string time_end)
         @param string time_start
         @param string time_end
         @return int, -1 is error, 0 is not time period and 1 is period active
         @luacode
         @verbatim
                local x = aremote:checkTime("10:00:59","20:30:35")
                if x then
                   -- time period active
                end
         @endverbatim
         */
        int32_t checkTime(Lua::Engine*, sol::string_view, sol::string_view);

        /**
         @brief Check pixel by buffer position
         @luamethod aremote:checkPixelByPos(numeric position, numeric R, numeric G, numeric B)
         @param number position - buffer position
         @param R - value range: 0-255
         @param G - value range: 0-255
         @param B - value range: 0-255
         @return boolean
         @luacode
         @verbatim
                local x = aremote:checkPixelByPos(50678, 255, 255, 127)
                if x then
                   -- pixel equals
                end
         @endverbatim
         */
        bool checkPixelByPos(Lua::Engine*, uint32_t, uint8_t, uint8_t, uint8_t);

        /**
         @brief Check pixel by buffer position, table RGB array input
         @luamethod aremote:checkPixelByPos(numeric position, table pixel)
         @verbatim
              aremote:checkPixelByPos(numeric position, { numeric R, numeric G, numeric B })
         @endverbatim
         @param number position - buffer position
         @param table RGB
         @return boolean
         @luacode
         @verbatim
                local t = { 255, 255, 127 }
                local x = aremote:checkPixelByPos(50678, t)
                if x then
                   -- pixel equals
                end
         @endverbatim
         */
        bool checkPixelByPos(Lua::Engine*, uint32_t, sol::as_table_t<std::vector<uint8_t>>);

        /**
         @brief Check pixel by x/y coordinates
         @luamethod aremote:checkPixelByCord(numeric x, numeric y, numeric R, numeric G, numeric B)
         @param number x - width value
         @param number y - height value
         @param R - value range: 0-255
         @param G - value range: 0-255
         @param B - value range: 0-255
         @return boolean
         @luacode
         @verbatim
                local x = aremote:checkPixelByCord( 1021, 620, 255, 255, 127)
                if x then
                   -- pixel equals
                end
         @endverbatim
         */
        bool checkPixelByCord(Lua::Engine*, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);

        /**
         @brief Check pixel by x/y coordinates, table RGB array input
         @luamethod aremote:checkPixelByCord(numeric x, numeric y, table pixel)
         @verbatim
              aremote:checkPixelByCord(numeric x, numeric y, { numeric R, numeric G, numeric B })
         @endverbatim
         @param number x - width value
         @param number y - height value
         @param table RGB pixel
         @return boolean
         @luacode
         @verbatim
                local t = { 255, 255, 127 }
                local x = aremote:checkPixelByCord( 1021, 620, t)
                if x then
                   -- pixel equals
                end
         @endverbatim
         */
        bool checkPixelByCord(Lua::Engine*, uint32_t, uint32_t, sol::as_table_t<std::vector<uint8_t>>);

        /**
         @brief Check pixels by buffer positions, table array input
         @luamethod aremote:checkPixelsByPos(table pixels)
         @verbatim
              aremote:checkPixelsByPos( { {numeric position, { numeric R, numeric G, numeric B }}, ..})
         @endverbatim
         @param table - {{ position, { R, G, B }, ...}
         @return boolean
         @luacode
         @verbatim
                local t = { { 50678, { 255, 255, 127 }}, { 50682, { 255, 255, 127 }}, ..}
                local x = aremote:checkPixelsByPos(t)
                if x then
                   -- all pixels equals
                end
         @endverbatim
         @seeimgarr4
         */
        bool checkPixelsByPos(Lua::Engine*, ImageLite::LuaArray4);

        /**
         @brief Check pixels by x/y coordinates, table array input
         @luamethod aremote:checkPixelsByCord(table pixels)
         @verbatim
              aremote:checkPixelsByCord( { {numeric x, numeric y, { numeric R, numeric G, numeric B }}, ..})
         @endverbatim
         @param table - {{ width, height, { R, G, B }, ...}
         @return boolean
         @luacode
         @verbatim
                local t = { { 1020, 720, { 255, 255, 127 }}, { 1021, 720, { 255, 255, 127 }}, ..}
                local x = aremote:checkPixelsByCord(t)
                if x then
                   -- all pixels equals
                end
         @endverbatim
         @seeimgarr5
         */
        bool checkPixelsByCord(Lua::Engine*, ImageLite::LuaArray5);

        /**
         @brief Get Android full frame buffer data
         @luamethod aremote:screenGet()
         @return ImageTable format type
         @luacode
         @verbatim
                local imgTbl = aremote:screenGet()
         @endverbatim
         @seeimgtbl
         */
        ImageLite::ImageTable screenGet(Lua::Engine*);

        /**
         @brief Get region from Android frame buffer data
         @luamethod aremote:screenGetRegion(numeric x, numeric y, numeric w, numeric h)
         @param number x - start width value
         @param number y - start height value
         @param number w - region width value
         @param number h - region height value
         @return ImageTable format type
         @luacode
         @verbatim
                local imgTbl = aremote:screenGetRegion(100, 200, 300, 300)
         @endverbatim
         @seeimgtbl
         */
        ImageLite::ImageTable screenGetRegion(Lua::Engine*, uint32_t, uint32_t, uint32_t, uint32_t);

        /**
         @brief Set default bmp, png image as Android frame buffer
         @luamethod aremote:imageDefault(string filename)
         @param string filename
         @return none
         @luacode
         @verbatim
                aremote:imageDefault("filename.png")
         @endverbatim
         @warning Development only,
                  This code is not included in the script server assembly and is only available in the assembly script editor.
         */
        void imageDefault(Lua::Engine*, sol::string_view);

        /**
         @brief Displays an image of the bmp, png, raw type in the built-in image viewer
         @luamethod aremote:imageShow(string filename)
         @param string filename
         @return none
         @luacode
         @verbatim
                aremote:imageShow("filename.png")
         @endverbatim
         @warning Development only,
                  This code is not included in the script server assembly and is only available in the assembly script editor.
                  Source code supports assembly for platforms WIN32/64.
         */
        void imageShow(Lua::Engine*, sol::string_view);

        /**
         @brief Displays an image of the ImageTable type in the built-in image viewer
         @luamethod aremote:imageTableShow(table ImageTable)
         @param table ImageTable
         @return none
         @luacode
         @verbatim
                local imgTbl = { .. }
                aremote:imageTableShow(imgTbl)
         @endverbatim
         @warning Development only,
                  This code is not included in the script server assembly and is only available in the assembly script editor.
                  Source code supports assembly for platforms WIN32/64.
         @seeimgtbl
         */
        void imageTableShow(Lua::Engine*, ImageLite::ImageTable);

        /**
         @brief Save to file from Android frame buffer data, support png, jpg, bmp and more raw formats..
                Upon reaching the maximum number of saves, the function does not save the files until the end of the script.
                Restriction applies to the entire executable script.
         @luamethod aremote:imageSave(string filename, numeric count)
         @param string filename
         @param number total count for save
         @return none
         @luacode
         @verbatim
                aremote:imageSave("filename.png", 7)
         @endverbatim
         */
        void imageSave(Lua::Engine*, sol::string_view, uint32_t);

        /**
         @brief Compare image with Android frame-buffer
                Input image support: png, bmp, raw, Lua ImageTable
         @luamethod aremote:imageCompare(string filename)
         @param string filename
         @return double percentage
         @luacode
         @verbatim
                local d = aremote:imageCompare("filename.png")
         @endverbatim
         */
        double imageCompare(Lua::Engine*, sol::string_view);

        /**
         @brief Compare image with region Android frame-buffer
                Input image support: png, bmp, raw, Lua ImageTable
         @luamethod aremote:imageCompareRegion(string filename, numeric x, numeric y, numeric w, numeric h)
         @param string filename
         @param number x - start width value
         @param number y - start height value
         @param number w - region width value
         @param number h - region height value
         @return double percentage
         @luacode
         @verbatim
                local d = aremote:imageCompareRegion("filename.png", 100, 50, 300, 300)
         @endverbatim
         */
        double imageCompareRegion(Lua::Engine*, sol::string_view, uint32_t, uint32_t, uint32_t, uint32_t);

        /**
         @brief Calculate position from x/y coordinates
         @luamethod aremote:imageGetPosition(numeric x, numeric y)
         @param number x - start width value
         @param number y - start height value
         @return number position
         @luacode
         @verbatim
                local x = aremote:imageGetPosition(100, 50)
         @endverbatim
         */
        uint32_t imageGetPosition(Lua::Engine*, uint32_t, uint32_t);

        /**
         @brief Emulate Android Tap/Click from x/y coordinates
         @luamethod aremote:adbClicke(numeric x, numeric y)
         @param number x - width value
         @param number y - height value
         @return none
         @luacode
         @verbatim
                aremote:adbClick(100, 50)
         @endverbatim
         */
        void adbClick(Lua::Engine*, uint32_t, uint32_t);

        /**
         @brief Emulate Android Swap from x/y, xx/yy coordinates
         @luamethod aremote:adbSwipe(numeric x, numeric y, numeric xx, numeric yy, numeric t)
         @param number x - width  start value
         @param number y - height start value
         @param number xx - width  end value
         @param number yy - height end value
         @param number t - millisecond value
         @return none
         @luacode
         @verbatim
                aremote:adbSwipe(100, 50, 200, 150, 75)
         @endverbatim
         */
        void adbSwipe(Lua::Engine*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

        /**
         @brief Emulate Android Key pressing
         @luamethod aremote:adbKey(numeric key)
         @param number k - Android keys numeric type
         @return none
         @luacode
         @verbatim
                aremote:adbKey(223)
         @endverbatim
         @see Android developer: https://developer.android.com/reference/android/view/KeyEvent.html \n
              Used Android keys table: https://github.com/clclon/Android-Remote-Viewer/blob/master/src/Aremote/extern/KeyCode
         */
        void adbKey(Lua::Engine*, uint16_t);

        /**
         @brief Send text to Android
                Any input text field on Android must be open in advance for receiving text.
         @luamethod aremote:adbText(string text)
         @param string text
         @return none
         @luacode
         @verbatim
                aremote:adbText("some text..")
         @endverbatim
         @warning Only supports ASCII text encoded in UTF8.
         */
        void adbText(Lua::Engine*, sol::string_view);

        /**
         @brief Get user defined state in run script
                This state send as argument in call function main(state).
         @luamethod aremote:stateGet()
         @return numeric
         @luacode
         @verbatim
                local x = aremote:stateGet()
         @endverbatim
         */
        uint32_t stateGet(Lua::Engine*);

        /**
         @brief Set user defined state in run script
                This state send as argument in call function main(state).
         @luamethod aremote:stateSet(numeric state)
         @param number current state
         @return none
         @luacode
         @verbatim
                local state = aremote:stateGet()
                aremote:stateSet(state + 1)
         @endverbatim
         */
        void stateSet(Lua::Engine*, uint32_t);

        /**
         @brief Sleep running script user defined time
         @luamethod aremote:stateSleep(numeric seconds)
         @param number time sleep in seconds
         @return none
         @luacode
         @verbatim
                aremote:stateSleep(3)
         @endverbatim
         */
        void stateSleep(Lua::Engine*, uint32_t);

        /**
         @brief Set trace message enabled
         @luamethod aremote:traceOn()
         @return none
         @warning Development only,
                  This code is not included in the script server assembly and is only available in the assembly script editor.
         */
        void traceOn(Lua::Engine*);

        /**
         @brief Set trace message disabled
         @luamethod aremote:traceOff()
         @return none
         @warning Development only,
                  This code is not included in the script server assembly and is only available in the assembly script editor.
         */
        void traceOff(Lua::Engine*);

        /**
         @brief Set manually capture flag enabled
         @luamethod aremote:captureOn()
         @return none
         @warning Don't use this, all capture process is automatically.
                  Development option.
         */
        void captureOn(Lua::Engine*);

        /**
         @brief Set manually capture flag disabled
         @luamethod aremote:captureOff()
         @return none
         @warning Don't use this, all capture process is automatically.
                  Development option.
         */
        void captureOff(Lua::Engine*);

        /**
         @brief Finish the script immediately
         @luamethod aremote:exitNow()
         @return none
         @warning Don't use this.
                  Development option.
         */
        void exitNow(Lua::Engine*);

#if !defined(DOXYGEN_BUILD)
    }
}
#endif
