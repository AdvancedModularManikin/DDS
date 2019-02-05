#include "AMM/SerialPort.h"
#include <boost/exception/all.hpp>
#include <chrono>
#include <thread>

namespace AMM {
    SerialPort::SerialPort(boost::asio::io_service &rIoService, uint32_t baud,
                           std::string const &device)
            : m_bInitialized(false), m_rIoService(rIoService),
              m_serialPort(m_rIoService, device), m_bytesTransferred(0) {
        boost::asio::serial_port::baud_rate baudRate(baud);
        m_serialPort.set_option(baudRate);
    }

    SerialPort::~SerialPort() { m_serialPort.close(); }

    void SerialPort::setRTS(bool enabled) {
        int data = TIOCM_RTS;
        if (!enabled)
            ioctl(fd, TIOCMBIC, &data);
        else
            ioctl(fd, TIOCMBIS, &data);
    }

    void SerialPort::setDTR(bool enabled) {

        int data = TIOCM_DTR;
        if (!enabled)
            ioctl(fd, TIOCMBIC, &data); // Clears the DTR pin
        else
            ioctl(fd, TIOCMBIS, &data); // Sets the DTR pin
    }

    int SerialPort::Initialize() {
        if (m_bInitialized) {
            return -1;
        }
        if (!m_serialPort.is_open()) {
            return -1;
        }

        LOG_DEBUG << "Connecting to serial port ";

        m_bInitialized = true;

        this->BeginRead_();
        return 0;
    }

    void SerialPort::Close() {
        m_rIoService.post(
                boost::bind(&SerialPort::DoClose_, this, boost::system::error_code()));
    }

    void SerialPort::Write(std::string const &msg) {
        if (!m_bInitialized)
            return;

        SerialPort::SharedBufferPtr_t pBuf(
                new std::vector<char>(msg.begin(), msg.end()));

        m_rIoService.post(boost::bind(&SerialPort::DoWrite_, this, pBuf));
    }

    void SerialPort::BeginRead_() {
        m_serialPort.async_read_some(
                boost::asio::buffer(m_rdBuf, m_rdBuf.size()),
                boost::bind(&SerialPort::EndRead_, this, boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }

    void SerialPort::EndRead_(boost::system::error_code const &error,
                              size_t bytesTransferred) {
        if (!error) {
            DataRead(m_rdBuf, bytesTransferred);
        }
        BeginRead_();
    }

    void SerialPort::DoWrite_(SharedBufferPtr_t pBuffer) {
        bool bWriteInProgress = !m_wrBuf.empty();
        m_wrBuf.push_back(pBuffer);
        if (!bWriteInProgress)
            BeginWrite_();
    }

    void SerialPort::BeginWrite_() {
        boost::asio::async_write(
                m_serialPort,
                boost::asio::buffer(*m_wrBuf.front(), m_wrBuf.front()->size()),
                boost::asio::transfer_all(),
                boost::bind(&SerialPort::EndWrite_, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }

    void SerialPort::EndWrite_(boost::system::error_code const &error,
                               size_t /*bytes_transferred*/) {
        if (!error) {
            m_wrBuf.pop_front();
            if (!m_wrBuf.empty())
                BeginWrite_();
        } else {
            std::cout << error.message() << std::endl;
        }
    }

    void SerialPort::DoClose_(boost::system::error_code const &error) {
        if (error == boost::asio::error::operation_aborted)
            return;
        if (error)
            std::cout << "ERROR:" << error.message() << std::endl;

        m_serialPort.cancel();
        m_serialPort.close();
    }
}
