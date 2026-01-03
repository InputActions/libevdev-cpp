/*
    libevdev-cpp - Minimal C++ libevdev wrapper for InputActions
    Copyright (C) 2006 Marcin Woźniak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "LibevdevUinputDevice.h"
#include "LibevdevDevice.h"
#include <fcntl.h>
#include "logging.h"
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

namespace InputActions
{

LibevdevUinputDevice::LibevdevUinputDevice(libevdev_uinput *device)
    : m_device(device)
{
}

LibevdevUinputDevice::~LibevdevUinputDevice()
{
    libevdev_uinput_destroy(m_device);
}

std::unique_ptr<LibevdevUinputDevice> LibevdevUinputDevice::createManaged(LibevdevDevice *libevdevDevice, const QString &name)
{
    const auto oldName = libevdevDevice->name();
    if (!name.isEmpty()) {
        libevdevDevice->setName(name);
    }

    libevdev_uinput *device;
    if (const auto error = libevdev_uinput_create_from_device(libevdevDevice->raw(), LIBEVDEV_UINPUT_OPEN_MANAGED, &device)) {
        qWarning(LIBEVDEV_CPP, "libevdev_uinput_create_from_device failed: %d", -error);
    }

    if (!name.isEmpty()) {
        libevdevDevice->setName(oldName);
    }
    if (!device) {
        return {};
    }

    return std::unique_ptr<LibevdevUinputDevice>(new LibevdevUinputDevice(device));
}

int LibevdevUinputDevice::writeEvent(int type, int code, int value)
{
    return libevdev_uinput_write_event(m_device, type, code, value);
}

int LibevdevUinputDevice::fd() const
{
    return libevdev_uinput_get_fd(m_device);
}

QString LibevdevUinputDevice::devNode() const
{
    return libevdev_uinput_get_devnode(m_device);
}

void LibevdevUinputDevice::removeNonBlockFlag()
{
    fcntl(fd(), F_SETFL, fcntl(fd(), F_GETFL, 0) & ~O_NONBLOCK);
}

}