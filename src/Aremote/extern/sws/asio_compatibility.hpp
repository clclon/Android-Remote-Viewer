#ifndef SIMPLE_WEB_ASIO_COMPATIBILITY_HPP
#define SIMPLE_WEB_ASIO_COMPATIBILITY_HPP

#include <memory>

#include <asio.hpp>
#include <asio/steady_timer.hpp>
namespace SimpleWeb {
  namespace error = asio::error;
  using error_code = std::error_code;
  using errc = std::errc;
  using system_error = std::system_error;
  namespace make_error_code = std;
} // namespace SimpleWeb

namespace SimpleWeb {
  using io_context = asio::io_context;
  using resolver_results = asio::ip::tcp::resolver::results_type;
  using async_connect_endpoint = asio::ip::tcp::endpoint;

  template <typename handler_type>
  inline void post(io_context &context, handler_type &&handler) {
    asio::post(context, std::forward<handler_type>(handler));
  }
  inline void restart(io_context &context) noexcept {
    context.restart();
  }
  inline asio::ip::address make_address(const std::string &str) noexcept {
    return asio::ip::make_address(str);
  }
  template <typename socket_type>
  asio::executor get_socket_executor(socket_type &socket) {
    return socket.get_executor();
  }
  template <typename handler_type>
  void async_resolve(asio::ip::tcp::resolver &resolver, const std::pair<std::string, std::string> &host_port, handler_type &&handler) {
    resolver.async_resolve(host_port.first, host_port.second, std::forward<handler_type>(handler));
  }
} // namespace SimpleWeb

#endif /* SIMPLE_WEB_ASIO_COMPATIBILITY_HPP */
