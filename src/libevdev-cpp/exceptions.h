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

#include <exception>

namespace InputActions::libevdev
{

class Exception : public std::exception
{
protected:
    Exception() = default;
};

class ErrorCodeException : public Exception
{
public:
    ErrorCodeException(int error);

    int error() const { return m_error; }

private:
    int m_error;
};

/**
 * Error code is the errno set by open().
 */
class DeviceOpenException : public ErrorCodeException
{
public:
    using ErrorCodeException::ErrorCodeException;
};

/**
 * Error code is the value returned by libevdev_new_from_fd.
 */
class DeviceCreationException : public ErrorCodeException
{
public:
    using ErrorCodeException::ErrorCodeException;
};

/**
 * Error code is the value returned by libevdev_uinput_create_from_device.
 */
class UInputDeviceCreationException : public ErrorCodeException
{
public:
    using ErrorCodeException::ErrorCodeException;
};

}