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

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QtClassHelperMacros>
#include <expected>
#include <linux/input.h>

struct libevdev;

namespace InputActions::libevdev
{

class Device : public QObject
{
    Q_OBJECT

public:
    /**
     * @see libevdev_new
     */
    Device();
    ~Device() override;

    /**
     * @return Errno on failure.
     * @see libevdev_new_from_fd
     */
    static std::expected<std::unique_ptr<Device>, int> createFromPath(const QString &path);

    struct libevdev *raw() { return m_device; }
    const struct libevdev *raw() const { return m_device; }

    /**
     * @see libevdev_get_fd
     */
    int fd() const;
    /**
     * @see libevdev_get_name
     */
    QString name() const;

    /**
     * @see libevdev_has_property
     */
    bool hasProperty(unsigned int prop) const;

    /**
     * @see libevdev_has_event_code
     */
    bool hasEventCode(unsigned int type, unsigned int code) const;
    /**
     * @see libevdev_has_event_type
     */
    bool hasEventType(unsigned int type) const;
    /**
     * @see libevdev_get_event_value
     */
    int eventValue(unsigned int type, unsigned int code) const;

    /**
     * @see libevdev_get_abs_info
     */
    const input_absinfo *absInfo(unsigned int code) const;

    /**
     * @see libevdev_get_current_slot
     */
    int currentSlot() const;
    /**
     * @see libevdev_get_num_slots
     */
    int slotCount() const;
    /**
     * @see libevdev_get_slot_value
     */
    int slotValue(unsigned int slot, unsigned int code) const;

    /**
     * @see libevdev_set_name
     */
    void setName(const QString &value);
    /**
     * @see libevdev_grab
     */
    void grab();

    /**
     * @see libevdev_next_event
     */
    int nextEvent(unsigned int flags, input_event &event);

    /**
     * @see libevdev_enable_event_code
     */
    int enableEventCode(unsigned int type, unsigned int code, const void *data);
    /**
     * @see libevdev_enable_event_type
     */
    int enableEventType(unsigned int type);

signals:
    void eventsAvailable();

private:
    Device(struct libevdev *device);

    Q_DISABLE_COPY_MOVE(Device);

    struct libevdev *m_device;
    std::unique_ptr<QSocketNotifier> m_notifier;
};

}