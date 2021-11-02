#ifndef profiler_h
#define profiler_h

//FP = function profiler
#ifdef FP_ENABLE

#include <boost/chrono.hpp>

#include <cstdio>
#include <ios>
#include <string>
#include <thread>

namespace fp {

// Agregator is used to collect data
class Agregator {
public:
  // Clocks
  using thread_clock = boost::chrono::thread_clock;
  using thread_time_point = thread_clock::time_point;
  using thread_accumulator = boost::chrono::duration<uint_least64_t, thread_clock::period>;

  using steady_clock = boost::chrono::steady_clock;
  using steady_time_point = steady_clock::time_point;
  using steady_accumulator = boost::chrono::duration<uint_least64_t, steady_clock::period>;

  using report_duration = boost::chrono::duration<double, boost::milli>;

  Agregator() = delete;
  Agregator(const std::string &name)
    : m_thread_last{thread_clock::now()},
      m_steady_last{steady_clock::now()},
      m_count{0},
      m_thread_accumulator{0},
      m_steady_accumulator{0},
      m_last_report{steady_clock::now()}
  {
    std::ostringstream os;
    os << "[FP] " << std::hex << std::this_thread::get_id() << " " << name << " #%-7lu %8.5F %8.5F %11.4F %11.4F\n";
    m_fmt = os.str();
  }

  ~Agregator() noexcept
  {
    report();
  }

  inline void start() noexcept
  {
    m_thread_last = thread_clock::now();
    m_steady_last = steady_clock::now();
  }

  inline void stop()
  {
    ++m_count;

    {
      const auto now = thread_clock::now();
      m_thread_accumulator += now - m_thread_last;
      m_thread_last = std::move(now);
    }
    {
      const auto now = steady_clock::now();
      m_steady_accumulator += now - m_steady_last;
      m_steady_last = std::move(now);
    }

    if (m_steady_last - m_last_report > report_interval) {
      m_last_report = m_steady_last;
      report();
    }
  }

private:
  inline void report() const
  {
    if (m_count == 0) return;
    const auto thread_ms = report_duration{m_thread_accumulator}.count();
    const auto steady_ms = report_duration{m_steady_accumulator}.count();
    std::printf(m_fmt.c_str(), m_count, steady_ms / m_count, thread_ms / m_count, steady_ms, thread_ms);
  }

private:
  static constexpr auto report_interval = boost::chrono::seconds{1};

  thread_time_point m_thread_last;
  steady_time_point m_steady_last;
  uint_least64_t m_count;
  thread_accumulator m_thread_accumulator;
  steady_accumulator m_steady_accumulator;
  steady_time_point m_last_report;
  std::string m_fmt;
};

// For updating Agregator
class profiler {
public:
  profiler(Agregator &Agregator_) : m_Agregator{Agregator_}
  {
    m_Agregator.start();
  }

  ~profiler()
  {
    m_Agregator.stop();
  }

  profiler() = delete;
  profiler(const profiler &) = delete;
  profiler(profiler &&) = delete;

  profiler &operator=(const profiler &) = delete;
  profiler &operator=(profiler &&) = delete;

private:
  Agregator &m_Agregator;
};

constexpr boost::chrono::seconds Agregator::report_interval;

} // namespace fp

#define PROFILE_FUNCTION()                                                                                             \
  thread_local fp::Agregator fp_info_fp{__func__};                                                                     \
  fp::profiler fp_profiler_fp{fp_info_fp};

#else // !FB_ENABLE

#define PROFILE_FUNCTION() static_cast<void>(0);

#endif // FB_ENABLE

#endif // profiler_h
