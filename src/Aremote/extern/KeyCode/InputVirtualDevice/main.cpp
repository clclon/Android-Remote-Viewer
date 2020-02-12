#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <atomic>
#include <memory>
#include <mutex>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#define UI_SET_PROPBIT _IOW(UINPUT_IOCTL_BASE, 110, int)

#include "main.h"

#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>

const char uinput_deivce_path[] = "/dev/uinput";
const char uinput_deivce_name[] = "ARemote Event";
int screen_width  = 1280;
int screen_Height = 800;


void send_event(int fd, int type, int code, int value)
{
    struct input_event event;
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = type;
    event.code = code;
    event.value = value;
    write(fd, &event, sizeof(event));
    usleep(500);
}

static int open_uinput_device()
{
    int uinp_fd = open(uinput_deivce_path, O_WRONLY | O_NDELAY);
    if (uinp_fd <= 0)
    {
        std::cout << "could not open " << uinput_deivce_path << " " << strerror(errno) << std::endl;
        return -1;
    }
    return uinp_fd;
}

static int init()
{
    try
    {
        struct uinput_user_dev ui_dev;
        int uinp_fd = open_uinput_device();
        if (uinp_fd < 0)
            return -1;

        memset(&ui_dev, 0, sizeof(ui_dev));
        memcpy(ui_dev.name, uinput_deivce_name, sizeof(uinput_deivce_name) - 1);
        //strncpy(ui_dev.name, UINPUT_MAX_NAME_SIZE, "ARemote Event");
        ui_dev.id.bustype = BUS_VIRTUAL;
        ui_dev.id.vendor = 0x0; //0x1341;
        ui_dev.id.product = 0x0001;
        ui_dev.id.version = 4;


        ui_dev.absmin[ABS_MT_SLOT] = 0;
        ui_dev.absmax[ABS_MT_SLOT] = 9;
        ui_dev.absmin[ABS_MT_TOUCH_MAJOR] = 0;
        ui_dev.absmax[ABS_MT_TOUCH_MAJOR] = 255;
        ui_dev.absmin[ABS_MT_POSITION_X] = 0;
        ui_dev.absmax[ABS_MT_POSITION_X] = screen_width - 1;
        ui_dev.absmin[ABS_MT_POSITION_Y] = 0;
        ui_dev.absmax[ABS_MT_POSITION_Y] = screen_Height - 1;
        ui_dev.absmin[ABS_MT_TRACKING_ID] = 0;
        ui_dev.absmax[ABS_MT_TRACKING_ID] = 65535;
        ui_dev.absmin[ABS_MT_PRESSURE] = 0;
        ui_dev.absmax[ABS_MT_PRESSURE] = 30;
        ui_dev.absmin[ABS_MT_TOOL_TYPE] = 0;
        ui_dev.absmax[ABS_MT_TOOL_TYPE] = 1;

        //enable direct
        ioctl(uinp_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
        //enable mouse event
        ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
        ioctl(uinp_fd, UI_SET_RELBIT, REL_X);
        ioctl(uinp_fd, UI_SET_RELBIT, REL_Y);
        //enable touch event
        ioctl(uinp_fd, UI_SET_EVBIT, EV_ABS);
        for (int i = 0; i <= ABS_CNT; i++)
            ioctl(uinp_fd, UI_SET_ABSBIT, i);


        ioctl(uinp_fd, UI_SET_EVBIT, EV_SYN);
        ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
        for (int i = 0; i < 256; i++)
            ioctl(uinp_fd, UI_SET_KEYBIT, i);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_TOUCH);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_LEFT);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MIDDLE);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_RIGHT);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_FORWARD);
        ioctl(uinp_fd, UI_SET_KEYBIT, BTN_BACK);


        write(uinp_fd, &ui_dev, sizeof(ui_dev));
        if (ioctl(uinp_fd, UI_DEV_CREATE))
        {
            std::cout << "Unable to create UINPUT device." << std::endl;
            return -1;
        }
        return uinp_fd;
    }
    catch (...)
    {
        //;
    }
    return -1;
}

int main(int32_t argc, char *argv[])
{
    android::sp<android::ProcessState> proc(android::ProcessState::self());
    android::ProcessState::self()->startThreadPool();


    try
    {
        int uinp_fd = init();
        if (uinp_fd < 0)
            return 0;

        send_event(uinp_fd, EV_ABS, ABS_MT_PRESSURE, 10);
        close (uinp_fd);
    }
    catch (...)
    {
        //;
    }
    return 0;
}
