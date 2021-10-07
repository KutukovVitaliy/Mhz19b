//
// Created by kvitadmin on 07.10.2021.
//

#ifndef MHZ19B_MHZ19B_H
#define MHZ19B_MHZ19B_H


#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <string>
#include <vector>

typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;

#define SERIAL_PORT_READ_BUF_SIZE 256

class Mhz19b
{
protected:
    boost::asio::io_service io_service_;
    serial_port_ptr port_;
    boost::mutex mutex_;

    char read_buf_raw_[SERIAL_PORT_READ_BUF_SIZE];
    std::string read_buf_str_;

    char end_of_line_char_;
    std::vector<char> answer;
    int result;

private:
    Mhz19b(const Mhz19b &p);
    Mhz19b &operator=(const Mhz19b &p);

public:
    Mhz19b(void);
    virtual ~Mhz19b(void);

    char end_of_line_char() const;
    void end_of_line_char(const char &c);

    virtual bool start(const char *com_port_name, int baud_rate=9600);
    virtual void stop();

    int write_some(const std::string &buf);
    int write_some(const char *buf, const int &size);
    int get_result(void);

    static int get_port_number();
    static std::string get_port_name(const unsigned int &idx);
    static std::vector<std::string> get_port_names();
    static void print_devices();

protected:
    virtual void async_read_some_();
    virtual void on_receive_(const boost::system::error_code& ec, size_t bytes_transferred);
    virtual void on_receive_(const std::string &data);
    bool check_answer();
    void calculate_result();

};



#endif //MHZ19B_MHZ19B_H
