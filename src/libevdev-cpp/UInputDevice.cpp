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

#include "UInputDevice.h"
#include "Device.h"
#include "logging.h"
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>

namespace InputActions::LibEvdev
{

UInputDevice::UInputDevice(libevdev_uinput *device)
    : m_device(device)
{
}

UInputDevice::UInputDevice(UInputDevice &&other)
{
    *this = std::move(other);
}

UInputDevice::~UInputDevice()
{
    if (!m_device) {
        return;
    }

    libevdev_uinput_destroy(m_device);
}

std::expected<UInputDevice, int> UInputDevice::createManaged(Device *device, const QString &name)
{
    const auto oldName = device->name();
    if (!name.isEmpty()) {
        device->setName(name);
    }

    libevdev_uinput *uinput;
    const auto error = libevdev_uinput_create_from_device(device->raw(), LIBEVDEV_UINPUT_OPEN_MANAGED, &uinput);

    if (!name.isEmpty()) {
        device->setName(oldName);
    }

    if (error) {
        qWarning(LIBEVDEV_CPP, "libevdev_uinput_create_from_device failed: %d", -error);
        return std::unexpected(-error);
    }

    return UInputDevice(uinput);
}

int UInputDevice::writeEvent(int type, int code, int value)
{
    return libevdev_uinput_write_event(m_device, type, code, value);
}

int UInputDevice::writeSynReportEvent()
{
    return writeEvent(EV_SYN, SYN_REPORT, 0);
}

int UInputDevice::fd() const
{
    return libevdev_uinput_get_fd(m_device);
}

QString UInputDevice::devNode() const
{
    return libevdev_uinput_get_devnode(m_device);
}

void UInputDevice::removeNonBlockFlag()
{
    fcntl(fd(), F_SETFL, fcntl(fd(), F_GETFL, 0) & ~O_NONBLOCK);
}

UInputDevice &UInputDevice::operator=(UInputDevice &&other)
{
    std::swap(m_device, other.m_device);
    return *this;
}

}