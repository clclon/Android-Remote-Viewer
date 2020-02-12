
[![Git HOME](https://img.shields.io/badge/Home-Git-brightgreen.svg?style=flat)](https://github.com/clclon/Android-Remote-Viewer)
[![Documentation](https://img.shields.io/badge/Documentation-DOC-brightgreen.svg?style=flat)](https://github.com/clclon/Android-Remote-Viewer/docs)
[![Repo size](https://img.shields.io/github/repo-size/clclon/Android-Remote-Viewer.svg?style=flat)](https://github.com/clclon/Android-Remote-Viewer)
[![Code size](https://img.shields.io/github/languages/code-size/clclon/Android-Remote-Viewer.svg?style=flat)](https://github.com/clclon/Android-Remote-Viewer)

# Android remote Lua script API
----------

coming soon..

## Example

```
     
     -- skeleton.lua
     -- Default ADB Remote Lua script skeleton.

     function main(arg)
         -- pass older return numeric value main(arg) from returned prevision call
         local state = aremote:stateGet()
         print("state = ", state, ", old return value = ", arg)

         -- logical code body
         if arg == 0 then
            aremote:stateSet(1)
            -- other code
            return 0
         end

         local x = 55
         local y = 120

         -- example loop
             if state == 0 then aremote:adbClick(x + 10, y + 20)
         elseif state == 1 then aremote:adbClick(x + 15, y + 30)
         elseif state == 2 then aremote:adbClick(x + 20, y + 40)
         -- etc ..
         elseif state == 30 then
            aremote:stateSet(0)
            aremote:stateSleep(35)
            return 0
         end

         -- save state numeric value
         aremote:stateSet(state + 1)
         -- sleep script 10 seconds
         aremote:stateSleep(10)
         -- returned as argument main(arg) from next call
         return arg + 1
     end
     return 0
     
```

## License

_MIT_

