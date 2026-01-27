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
#include "logging.h"
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>

namespace InputActions
{

LibevdevUinputDevice::LibevdevUinputDevice(libevdev_uinput *device)
    : m_device(device)
{
}

LibevdevUinputDevice::LibevdevUinputDevice(LibevdevUinputDevice &&other)
{
    *this = std::move(other);
}

LibevdevUinputDevice::~LibevdevUinputDevice()
{
    if (!m_device) {
        return;
    }

    libevdev_uinput_destroy(m_device);
}

std::expected<LibevdevUinputDevice, int> LibevdevUinputDevice::createManaged(LibevdevDevice *libevdevDevice, const QString &name)
{
    const auto oldName = libevdevDevice->name();
    if (!name.isEmpty()) {
        libevdevDevice->setName(name);
    }

    libevdev_uinput *device;
    const auto error = libevdev_uinput_create_from_device(libevdevDevice->raw(), LIBEVDEV_UINPUT_OPEN_MANAGED, &device);

    if (!name.isEmpty()) {
        libevdevDevice->setName(oldName);
    }

    if (error) {
        qWarning(LIBEVDEV_CPP, "libevdev_uinput_create_from_device failed: %d", -error);
        return std::unexpected(-error);
    }

    return LibevdevUinputDevice(device);
}

int LibevdevUinputDevice::writeEvent(int type, int code, int value)
{
    return libevdev_uinput_write_event(m_device, type, code, value);
}

int LibevdevUinputDevice::writeSynReportEvent()
{
    return writeEvent(EV_SYN, SYN_REPORT, 0);
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

LibevdevUinputDevice &LibevdevUinputDevice::operator=(LibevdevUinputDevice &&other)
{
    std::swap(m_device, other.m_device);
    return *this;
}

}