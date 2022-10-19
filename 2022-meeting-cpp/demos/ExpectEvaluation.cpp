#include "Base.h"


namespace ct {

struct Tag{};

template <typename C>
class ClauseEvaluation final {
  C const & m_clause;

public:
  constexpr ClauseEvaluation(C const & clause)
  : m_clause{clause} {}

  constexpr decltype(auto) value() const {
    return m_clause.m_value;
  }

  friend std::ostream & operator<<(
    std::ostream & out, ClauseEvaluation const & eval) {
    return out << eval.value();
  }
};

template <typename T>
class Clause final : public Tag {
  T m_value;

public:
  using Evaluation = ClauseEvaluation<Clause<T>>;
  friend Evaluation;

  template <typename U>
  constexpr Clause(U && value)
  : m_value{std::forward<U>(value)} {}

  [[nodiscard]] constexpr auto evaluation() const {
    return Evaluation{*this};
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(evaluation().value());
  }
};

/////////////////////////////////////////////////
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

template <Expression T>
void expect(T && t) {
  using Tags = std::array<std::string, 2>;
  static auto const tags
    = Tags{"FAIL", "OK  "};

  auto const eval = t.evaluation();
  std::cout
    << tags[static_cast<bool>(eval.value())]
    << ": " << eval << std::endl;
}

/////////////////////////////////////////////////

template <typename Lhs, typename Rhs>
class EqualityEvaluation final {
  Lhs m_lhs;
  Rhs m_rhs;
  bool m_value;

public:
  constexpr EqualityEvaluation(auto && lhs, auto && rhs)
  : m_lhs{std::forward<decltype(lhs)>(lhs)},
    m_rhs{std::forward<decltype(rhs)>(rhs)},
    m_value{m_lhs.value() == m_rhs.value()}
  {}

  constexpr decltype(auto) value() const {
    return m_value;
  }

  friend std::ostream & operator<<(
    std::ostream & out, EqualityEvaluation const & eval) {
    return out
      << "( " << eval.m_lhs << " == "
      << eval.m_rhs << " )";
  }
};

template <Expression Lhs, Expression Rhs>
class Equal final : public Tag {
  Lhs m_lhs;
  Rhs m_rhs;

public:
  using Evaluation = EqualityEvaluation<
    typename Lhs::Evaluation,
    typename Rhs::Evaluation>;
  friend Evaluation;

  constexpr Equal(auto && lhs, auto && rhs)
  : m_lhs{std::forward<decltype(lhs)>(lhs)},
    m_rhs{std::forward<decltype(rhs)>(rhs)} {}

  [[nodiscard]] constexpr auto evaluation() const {
    return Evaluation{m_lhs.evaluation(), m_rhs.evaluation()};
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(evaluation().value());
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

/////////////////////////////////////////////////

template <typename Lhs, typename Rhs>
class OrEvaluation final {
  Lhs m_lhs;
  std::optional<Rhs> m_rhs;
  bool m_value;

public:
  OrEvaluation(auto && expr)
  : m_lhs{expr.m_lhs.evaluation()},
    m_rhs{
      static_cast<bool>(m_lhs.value())
      ? std::nullopt
      : std::optional{
          expr.m_rhs.evaluation()
        }
    },
    m_value{
      m_rhs
      ? static_cast<bool>(m_rhs->value())
      : static_cast<bool>(m_lhs.value())
    }
  {}

  constexpr decltype(auto) value() const {
    return m_value;
  }

  friend std::ostream & operator<<(
    std::ostream & out, OrEvaluation const & eval) {
    out << "( " << eval.m_lhs << " or ";
    if (eval.m_rhs) {
      out << *eval.m_rhs;
    } else {
      out << "<unknown>";
    }
    return out << " )";
  }
};

template <Expression Lhs, Expression Rhs>
class Or final : public Tag {
  Lhs m_lhs;
  Rhs m_rhs;

public:
  using Evaluation = OrEvaluation<typename Lhs::Evaluation, typename Rhs::Evaluation>;
  friend Evaluation;

  constexpr Or(auto && lhs, auto && rhs)
  : m_lhs{std::forward<decltype(lhs)>(lhs)},
    m_rhs{std::forward<decltype(rhs)>(rhs)}
  {}

  [[nodiscard]] constexpr auto evaluation() const {
    return Evaluation{*this};
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(evaluation().value());
  }
};

template <typename Lhs, typename Rhs>
Or(Lhs &&, Rhs &&) -> Or<Lhs, Rhs>;

template <typename Lhs, typename Rhs>
constexpr auto operator or(Lhs && lhs, Rhs && rhs) {
  return Or{
    lift(std::forward<Lhs>(lhs)),
    lift(std::forward<Rhs>(rhs)),
  };
}

// Not shown ///////////////////////////////

template <typename Eval>
class NotEvaluation final {
  Eval m_evaluation;
  decltype(not std::declval<Eval>().value()) m_value;

public:
  constexpr NotEvaluation(auto && eval)
  : m_evaluation{std::forward<decltype(eval)>(eval)},
    m_value{not m_evaluation.value()}
  {}

  constexpr decltype(auto) value() const {
    return m_value;
  }

  friend std::ostream & operator<<(
    std::ostream & out, NotEvaluation const & eval) {
    return out << "not " << eval.m_evaluation;
  }
};

template <Expression T>
class Not final : public Tag {
  T m_operand;

public:
  using Evaluation = NotEvaluation<typename T::Evaluation>;
  friend Evaluation;

  Not(auto&& otr) : m_operand{std::forward<decltype(otr)>(otr)} {}

  [[nodiscard]] constexpr auto evaluation() const {
    return Evaluation{m_operand.evaluation()};
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(evaluation().value());
  }
};

template <typename T>
Not(T&&) -> Not<T>;

template <typename T>
constexpr auto operator not(T && t) {
  return Not{lift(std::forward<T>(t))};
}

/////////////////////////////////////////////////

template <typename Eval>
class StarEvaluation final {
  Eval m_evaluation;
  decltype(*std::declval<Eval>().value()) m_value;

public:
  constexpr StarEvaluation(auto && eval)
  : m_evaluation{std::forward<decltype(eval)>(eval)},
    m_value{*m_evaluation.value()}
  {}

  constexpr decltype(auto) value() const {
    return m_value;
  }

  friend std::ostream & operator<<(
    std::ostream & out, StarEvaluation const & eval) {
    // Intentionally dont' print '*' << m_evaluation
    // since this is the user-expected output for e.g.
    // pointers.
    return out << eval.value();
  }
};

template <Expression T>
struct Star final : public Tag {
  T m_operand;

public:
  using Evaluation = StarEvaluation<typename T::Evaluation>;
  friend Evaluation;
  Star(auto&& otr) : m_operand{std::forward<decltype(otr)>(otr)} {}

  [[nodiscard]] constexpr auto evaluation() const {
    return Evaluation{m_operand.evaluation()};
  }

  constexpr explicit operator bool() const {
    return static_cast<bool>(evaluation().value());
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