#define BOOST_SIGNALS_NO_DEPRECATION_WARNING true

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>

#include <vector>
#include <deque>

namespace AMM {
    class SerialPort
            : public boost::enable_shared_from_this<SerialPort> {
    public:
        static uint32_t const k_readBufferSize = 1024;

        typedef boost::shared_ptr <std::vector<char>> SharedBufferPtr_t;

        SerialPort(
                boost::asio::io_service &rIoService,
                uint32_t baud,
                std::string const &device);

        //~SerialPort() override = default;

        int Initialize();

        void Close();

        void Write(std::string const &msg);

        boost::signal<
        void(boost::array<char, k_readBufferSize>
        const &,
        size_t bytesTransferred
        )>
        DataRead;

    private:
        void BeginRead_();

        void EndRead_(boost::system::error_code const &error, size_t bytesTransferred);

        void DoWrite_(SharedBufferPtr_t pBuffer);

        void BeginWrite_();

        void EndWrite_(boost::system::error_code const &error, size_t bytes_transferred);

        void DoClose_(boost::system::error_code const &error);

        bool m_bInitialized;
        boost::asio::io_service &m_rIoService;
        boost::asio::serial_port m_serialPort;
        boost::array<char, k_readBufferSize> m_rdBuf;
        std::deque <SharedBufferPtr_t> m_wrBuf;

        uint32_t m_bytesTransferred;
    };

}