#include <boost/asio.hpp>
#include <clean-test/clean-test.h>
#include <thread>
#include <vector>
#include <optional>

namespace ct = clean_test;
namespace asio = boost::asio;
using namespace ct::literals;





class AsioPool {
  asio::io_context m_context;
  std::optional<asio::io_context::work> m_work;
  std::vector<std::jthread> m_workers;

public:
  AsioPool(
    ct::Observer & o,
    std::size_t const n)
  : m_context{},
    m_work{m_context}
  {
    while(m_workers.size() < n) {
      m_workers.emplace_back([&, this] {
        auto const os = ct::ObservationSetup{o};
        m_context.run();
      });
    }
  }

  ~AsioPool() {
    m_work.reset();
  }


  auto executor() {
    return m_context.get_executor();
  }
};


auto test = "test"_test = [](ct::Observer & o) {
  auto pool = AsioPool{o, 4};
  asio::post(pool.executor(), [] {
    ct::expect(7_i == 0);
  });
};