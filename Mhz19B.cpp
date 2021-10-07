//
// Created by kvitadmin on 07.10.2021.
//

#include <iostream>
#include "Mhz19B.h"
#include <iomanip>

Mhz19b::Mhz19b(void) : end_of_line_char_('\n')
{
}

Mhz19b::~Mhz19b(void)
{
    stop();
}

char Mhz19b::end_of_line_char() const
{
    return this->end_of_line_char_;
}

void Mhz19b::end_of_line_char(const char &c)
{
    this->end_of_line_char_ = c;
}

std::vector<std::string> Mhz19b::get_port_names()
{
    std::vector<std::string> names;

    names.push_back("/dev/ttyUSB0");
    return names;
}

int Mhz19b::get_port_number()
{
    std::vector<std::string> names = get_port_names();
    return names.size();
}

std::string Mhz19b::get_port_name(const unsigned int &idx)
{
    std::vector<std::string> names = get_port_names();
    if (idx >= names.size()) return std::string();
    return names[idx];
}

void Mhz19b::print_devices()
{
    std::cout << "Mhz19b::print_devices()" << std::endl;
    int n = Mhz19b::get_port_number();
    for (int i = 0; i < n; ++i) {
        std::string name = Mhz19b::get_port_name(i);
        std::cout << "\t" << name.c_str() << std::endl;
    }
}

bool Mhz19b::start(const char *com_port_name, int baud_rate)
{
    boost::system::error_code ec;

    if (port_) {
        std::cout << "error : port is already opened..." << std::endl;
        return false;
    }

    port_ = serial_port_ptr(new boost::asio::serial_port(io_service_));
    port_->open(com_port_name, ec);
    if (ec) {
        std::cout << "error : port_->open() failed...com_port_name="
        << com_port_name << ", e=" << ec.message().c_str() << std::endl;
        return false;
    }

    // option settings...
    port_->set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    port_->set_option(boost::asio::serial_port_base::character_size(8));
    port_->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    port_->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    port_->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service_));

    async_read_some_();

    return true;
}

void Mhz19b::stop()
{
    boost::mutex::scoped_lock look(mutex_);

    if (port_) {
        port_->cancel();
        port_->close();
        port_.reset();
    }
    io_service_.stop();
    io_service_.reset();
}

int Mhz19b::write_some(const std::string &buf)
{
    return write_some(buf.c_str(), buf.size());
}

int Mhz19b::write_some(const char *buf, const int &size)
{
    boost::system::error_code ec;

    if (!port_) return -1;
    if (size == 0) return 0;

    return port_->write_some(boost::asio::buffer(buf, size), ec);
}

void Mhz19b::async_read_some_()
{
    if (port_.get() == NULL || !port_->is_open()) return;

    port_->async_read_some(
            boost::asio::buffer(read_buf_raw_, SERIAL_PORT_READ_BUF_SIZE),
            boost::bind(
                    &Mhz19b::on_receive_,
                    this, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}


bool Mhz19b::check_answer()
{
    if(answer.size() != 9) return false;
    char checksum = 0;
    for(int i =0; i < 8; ++i){
        checksum += answer.at(i);
    }
    checksum = 0xff - checksum;
    checksum += 1;
    if(checksum != answer.at(8)) return false;
    else return true;
}

void Mhz19b::on_receive_(const boost::system::error_code& ec, size_t bytes_transferred)
{
    boost::mutex::scoped_lock look(mutex_);
    char hex[2];
    if (port_.get() == NULL || !port_->is_open()) return;
    if (ec) {
        async_read_some_();
        return;
    }

    for (unsigned int i = 0; i < bytes_transferred; ++i) {
        answer.push_back(read_buf_raw_[i]);
    }
    result = answer.at(2) * 256 + answer.at(3);
    answer.clear();
    async_read_some_();
}

void Mhz19b::on_receive_(const std::string &data)
{
    std::cout << "Mhz19b::on_receive_() : " << data << std::endl;
}

void Mhz19b::calculate_result()
{
    if(!answer.empty() && check_answer()){


    }
}

int Mhz19b::get_result()
{
    return result;
}

