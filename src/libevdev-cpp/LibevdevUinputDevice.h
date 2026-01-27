/*
    libevdev-cpp - Minimal C++ libevdev wrapper for InputActions
    Copyright (C) 2026 Marcin Woźniak

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

#pragma once

#include <QString>
#include <QtClassHelperMacros>
#include <expected>
#include <libevdev/libevdev-uinput.h>

namespace InputActions
{

class LibevdevDevice;

class LibevdevUinputDevice
{
public:
    LibevdevUinputDevice(LibevdevUinputDevice &&);
    ~LibevdevUinputDevice();

    /**
     * @param name If empty, the libevdevDevice's name is used instead.
     * @see libevdev_uinput_create_from_device
     */
    static std::expected<LibevdevUinputDevice, int> createManaged(LibevdevDevice *libevdevDevice, const QString &name = {});

    /**
     * @see libevdev_uinput_get_fd
     */
    int fd() const;
    /**
     * @see libevdev_uinput_get_devnode
     */
    QString devNode() const;

    /**
     * @see libevdev_uinput_write_event
     */
    int writeEvent(int type, int code, int value);
    /**
     * Writes a {EV_SYN, SYN_REPORT, 0} event.
     * @see writeEvent
     */
    int writeSynReportEvent();

    void removeNonBlockFlag();

    LibevdevUinputDevice &operator=(LibevdevUinputDevice &&);

private:
    LibevdevUinputDevice(libevdev_uinput *device);

    Q_DISABLE_COPY(LibevdevUinputDevice);

    libevdev_uinput *m_device{};
};

}