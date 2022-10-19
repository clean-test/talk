#include "Base.h"


namespace ct {

struct Tag{};

template <typename T>
class Clause final : public Tag {
  T m_value;

public:
  template <typename U>
  constexpr Clause(U && value)
  : m_value{std::forward<U>(value)} {}

  constexpr decltype(auto) value() const {
    return m_value;
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(value());
  }

  friend std::ostream & operator<<(
    std::ostream & out, Clause const & clause) {
    return out << clause.value();
  }
};

template <typename T>
concept Expression =
  std::derived_from<std::remove_cvref_t<T>, Tag>;

template <typename T>
constexpr decltype(auto) lift(T && t) {
  if constexpr (Expression<T>) {
    return std::forward<T>(t);
  } else {
    return Clause<T>{std::forward<T>(t)};
  }
}

/////////////////////////////////////////////////

template <Expression Lhs, Expression Rhs>
class Equal final : public Tag {
  Lhs m_lhs;
  Rhs m_rhs;

public:
  constexpr Equal(auto && lhs, auto && rhs)
  : m_lhs{std::forward<decltype(lhs)>(lhs)},
    m_rhs{std::forward<decltype(rhs)>(rhs)} {}

  constexpr auto value() const {
    return m_lhs.value() == m_rhs.value();
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(value());
  }

  friend std::ostream & operator<<(
    std::ostream & out, Equal const & eq) {
    return out << "( " << eq.m_lhs
               << " == " << eq.m_rhs << " )";
  }
};

template <typename Lhs, typename Rhs>
Equal(Lhs &&, Rhs &&) -> Equal<Lhs, Rhs>;

template <typename Lhs, typename Rhs>
constexpr auto operator==(Lhs && lhs, Rhs && rhs) {
  return Equal{
    lift(std::forward<Lhs>(lhs)),
    lift(std::forward<Rhs>(rhs)),
  };
}

template <typename T>
void expect(T && t) {
  using Tags = std::array<std::string_view, 2>;
  static auto const tags = Tags{"FAIL", "OK  "};

  std::cout
    << tags[static_cast<bool>(t)]
    << ": " << t << std::endl;
}

template <typename Expression>
using Value =
  decltype(std::declval<Expression>().value());

template <Expression Lhs, Expression Rhs>
class CachingOr final : public Tag {
  Lhs m_lhs;
  Rhs m_rhs;
  Value<Lhs> m_lhs_value;
  std::optional<Value<Rhs>> m_rhs_value;

public:
  constexpr CachingOr(auto && lhs, auto && rhs)
  : m_lhs{std::forward<decltype(lhs)>(lhs)},
    m_rhs{std::forward<decltype(rhs)>(rhs)},
    m_lhs_value{m_lhs.value()},
    m_rhs_value{m_lhs_value
      ? std::nullopt
      : std::make_optional(m_rhs.value())}
  {}

  constexpr decltype(auto) value() const {
    return m_lhs_value or *m_rhs_value;
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(value());
  }

  friend std::ostream & operator<<(
    std::ostream & out, CachingOr const & o)
  {
    out << "( " << o.m_lhs << " or ";
    if (o.m_rhs_value) {
      out << o.m_rhs;
    } else {
      out << "<unknown>";
    }
    return out << " )";
  }
};

template <typename Lhs, typename Rhs>
CachingOr(Lhs &&, Rhs &&) -> CachingOr<Lhs, Rhs>;

template <typename Lhs, typename Rhs>
constexpr auto operator or(Lhs && lhs, Rhs && rhs) {
  return CachingOr{
    lift(std::forward<Lhs>(lhs)),
    lift(std::forward<Rhs>(rhs)),
  };
}

// Not shown ///////////////////////////////

template <Expression T>
class Not final : public Tag {
  T m_operand;

public:
  Not(auto&& otr) : m_operand{std::forward<decltype(otr)>(otr)} {}

  constexpr decltype(auto) value() const {
    return not m_operand.value();
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(value());
  }

  friend std::ostream& operator<<(std::ostream& out, Not const & n) {
    return out << "not " << n.m_operand;
  }
};

template <typename T>
Not(T&&) -> Not<T>;

template <typename T>
constexpr auto operator not(T && t) {
  return Not{lift(std::forward<T>(t))};
}

template <Expression T>
class Star final : public Tag {
  T m_operand;

public:
  Star(auto&& otr) : m_operand{std::forward<decltype(otr)>(otr)} {}

  constexpr decltype(auto) value() const {
    return *m_operand.value();
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(value());
  }

  friend std::ostream& operator<<(std::ostream& out, Star const & n) {
    return out << n.value();
  }
};

template <typename T>
Star(T&&) -> Star<T>;

template <typename T>
constexpr auto operator *(T && t) {
  return Star{lift(std::forward<T>(t))};
}


}

#include "ExpectAttemptMain.h"