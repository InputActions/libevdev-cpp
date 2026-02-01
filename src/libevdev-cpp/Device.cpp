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

#include "Device.h"
#include "logging.h"
#include <QLoggingCategory>
#include <fcntl.h>
#include <libevdev/libevdev.h>

Q_LOGGING_CATEGORY(LIBEVDEV_CPP, "inputactions.libevdev-cpp", QtWarningMsg);

namespace InputActions::libevdev
{

Device::Device()
    : Device(libevdev_new())
{
}

Device::Device(struct libevdev *device)
    : m_device(device)
{
    if (const auto fd = libevdev_get_fd(m_device); fd != -1) {
        m_notifier = std::make_unique<QSocketNotifier>(static_cast<qintptr>(fd), QSocketNotifier::Read);
        connect(m_notifier.get(), &QSocketNotifier::activated, this, [this]() {
            Q_EMIT eventsAvailable();
        });
    }
}

Device::~Device()
{
    if (const auto fd = libevdev_get_fd(m_device); fd != -1) {
        close(fd);
    }
    libevdev_free(m_device);
}

std::expected<std::unique_ptr<Device>, int> Device::createFromPath(const QString &path)
{
    const auto fd = open(path.toStdString().c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        qCDebug(LIBEVDEV_CPP, "Failed to open device \"%s\": %d", path.toStdString().c_str(), errno);
        return std::unexpected(errno);
    }

    fcntl(fd, F_SETFD, FD_CLOEXEC);
    struct libevdev *device;
    if (const auto error = libevdev_new_from_fd(fd, &device)) {
        qWarning(LIBEVDEV_CPP, "libevdev_new_from_fd failed: %d", -error);
        close(fd);
        return std::unexpected(-error);
    }

    return std::unique_ptr<Device>(new Device(device));
}

int Device::nextEvent(unsigned int flags, input_event &event)
{
    return libevdev_next_event(m_device, flags, &event);
}

void Device::setName(const QString &value)
{
    libevdev_set_name(m_device, value.toStdString().c_str());
}

void Device::grab()
{
    libevdev_grab(m_device, LIBEVDEV_GRAB);
}

int Device::enableEventCode(unsigned int type, unsigned int code, const void *data)
{
    return libevdev_enable_event_code(m_device, type, code, data);
}

int Device::enableEventType(unsigned int type)
{
    return libevdev_enable_event_type(m_device, type);
}

int Device::fd() const
{
    return libevdev_get_fd(m_device);
}

QString Device::name() const
{
    return libevdev_get_name(m_device);
}

bool Device::hasProperty(unsigned int prop) const
{
    return libevdev_has_property(m_device, prop) == 1;
}

bool Device::hasEventCode(unsigned int type, unsigned int code) const
{
    return libevdev_has_event_code(m_device, type, code) == 1;
}

bool Device::hasEventType(unsigned int type) const
{
    return libevdev_has_event_type(m_device, type) == 1;
}

int Device::eventValue(unsigned int type, unsigned int code) const
{
    return libevdev_get_event_value(m_device, type, code);
}

const input_absinfo *Device::absInfo(unsigned int code) const
{
    return libevdev_get_abs_info(m_device, code);
}

int Device::currentSlot() const
{
    return libevdev_get_current_slot(m_device);
}

int Device::slotCount() const
{
    return libevdev_get_num_slots(m_device);
}

int Device::slotValue(unsigned int slot, unsigned int code) const
{
    return libevdev_get_slot_value(m_device, slot, code);
}

}