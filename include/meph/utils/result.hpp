#include <concepts>
#include <type_traits>
#include <utility>
#include <variant>

namespace meph::util
{

template <typename T, typename E> class Result
{
private:
  struct Succ
  {
    T value;
  };
  struct Err
  {
    E value;
  };
  std::variant<Succ, Err> m_value;

public:
  Result() = delete;

  template <typename U>
    requires std::constructible_from<T, U>
  Result(std::in_place_index_t<0>, U&& success)
      : m_value(std::in_place_index<0>, Succ{std::forward<U>(success)})
  {
  }

  template <typename U>
    requires std::constructible_from<E, U>
  Result(std::in_place_index_t<1>, U&& error)
      : m_value(std::in_place_index<1>, Err{std::forward<U>(error)})
  {
  }

  constexpr bool is_success() const
  {
    return std::holds_alternative<Succ>(m_value);
  }
  constexpr bool is_error() const
  {
    return std::holds_alternative<Err>(m_value);
  }

  // Success Getters
  constexpr T const& get() const&
  {
    return std::get<Succ>(m_value).value;
  }
  constexpr T& get() &
  {
    return std::get<Succ>(m_value).value;
  }
  constexpr T&& get() &&
  {
    return std::move(std::get<Succ>(m_value).value);
  }

  // Error Getters
  constexpr E const& get_err() const&
  {
    return std::get<Err>(m_value).value;
  }
  constexpr E& get_err() &
  {
    return std::get<Err>(m_value).value;
  }
  constexpr E&& get_err() &&
  {
    return std::move(std::get<Err>(m_value).value);
  }

  // --- Monadic operations ---

  template <typename F, typename U = std::invoke_result_t<F, const T&>>
  constexpr Result<U, E> map(F&& f) const&
  {
    if (is_success())
    {
      return Result<U, E>(std::in_place_index<0>, std::forward<F>(f)(get()));
    }
    return Result<U, E>(std::in_place_index<1>, get_err());
  }

  template <typename F, typename U = std::invoke_result_t<F, T&&>>
  constexpr Result<U, E> map(F&& f) &&
  {
    if (is_success())
    {
      return Result<U, E>(std::in_place_index<0>, std::forward<F>(f)(std::move(get())));
    }
    return Result<U, E>(std::in_place_index<1>, std::move(get_err()));
  }

  template <typename F, typename U = std::invoke_result_t<F, const E&>>
  constexpr Result<T, U> map_err(F&& f) const&
  {
    if (is_error())
      return Result<T, U>(std::in_place_index<1>, std::forward<F>(f)(get_err()));
    return Result<T, U>(std::in_place_index<0>, get());
  }

  template <typename F, typename U = std::invoke_result_t<F, E&&>>
  constexpr Result<T, U> map_err(F&& f) &&
  {
    if (is_error())
    {
      return Result<T, U>(std::in_place_index<1>, std::forward<F>(f)(std::move(get_err())));
    }
    return Result<T, U>(std::in_place_index<0>, std::move(get()));
  }

  template <typename F, typename U = std::invoke_result_t<F, const T&>>
    requires std::constructible_from<U, std::in_place_index_t<1>, E> &&
             std::convertible_to<std::invoke_result_t<F, const T&>, U>
  constexpr U flat_map(F&& f) const&
  {
    if (is_success())
    {
      return std::forward<F>(f)(get());
    }
    return U(std::in_place_index<1>, get_err());
  }

  template <typename F, typename U = std::invoke_result_t<F, T&&>>
    requires std::constructible_from<U, std::in_place_index_t<1>, E> &&
             std::convertible_to<std::invoke_result_t<F, T&&>, U>
  constexpr U flat_map(F&& f) &&
  {
    if (is_success())
    {
      return std::forward<F>(f)(std::move(get()));
    }
    return U(std::in_place_index<1>, std::move(get_err()));
  }

  template <typename F, typename U = std::invoke_result_t<F, const E&>>
  constexpr U recover_with(F&& f) const&
  {
    if (is_error())
    {
      return std::forward<F>(f)(get_err());
    }
    return U(std::in_place_index<0>, get());
  }

  template <typename F, typename U = std::invoke_result_t<F, E&&>>
  constexpr U recover_with(F&& f) &&
  {
    if (is_error())
    {
      return std::forward<F>(f)(std::move(get_err()));
    }
    return U(std::in_place_index<0>, std::move(get()));
  }

  // Fold (Lvalue version)
  template <typename F1, typename F2, typename U = std::invoke_result_t<F1, const T&>>
  constexpr U fold(F1&& succ, F2&& err) const&
  {
    if (is_success())
    {
      return std::forward<F1>(succ)(get());
    }
    return std::forward<F2>(err)(get_err());
  }

  // Fold (Rvalue version)
  template <typename F1, typename F2, typename U = std::invoke_result_t<F1, T&&>>
  constexpr U fold(F1&& succ, F2&& err) &&
  {
    if (is_success())
    {
      return std::forward<F1>(succ)(std::move(get()));
    }
    return std::forward<F2>(err)(std::move(get_err()));
  }

  template <typename F> constexpr Result<T, E>& consume(F&& f) &
  {
    if (is_success())
    {
      std::forward<F>(f)(get());
    }
    return *this;
  }

  template <typename F> constexpr Result<T, E> const& consume(F&& f) const&
  {
    if (is_success())
    {
      std::forward<F>(f)(get());
    }
    return *this;
  }

  template <typename F> constexpr Result<T, E>&& consume(F&& f) &&
  {
    if (is_success())
    {
      std::forward<F>(f)(std::move(get()));
    }
    return std::move(*this);
  }

  template <typename F> constexpr Result<T, E> const&& consume(F&& f) const&&
  {
    if (is_success())
    {
      std::forward<F>(f)(get());
    }
    return std::move(*this);
  }

  template <typename F> constexpr Result<T, E>& consume_err(F&& f) &
  {
    if (is_error())
    {
      std::forward<F>(f)(get_err());
    }
    return *this;
  }

  template <typename F> constexpr Result<T, E> const& consume_err(F&& f) const&
  {
    if (is_error())
    {
      std::forward<F>(f)(get_err());
    }
    return *this;
  }

  template <typename F> constexpr Result<T, E>&& consume_err(F&& f) &&
  {
    if (is_error())
    {
      std::forward<F>(f)(std::move(get_err()));
    }
    return std::move(*this);
  }

  template <typename F> constexpr Result<T, E> const&& consume_err(F&& f) const&&
  {
    if (is_error())
    {
      std::forward<F>(f)(get_err());
    }
    return std::move(*this);
  }
};

// --- Helper Factory Functions ---

template <typename T, typename E = std::monostate> auto make_success(T&& succ)
{
  return Result<std::decay_t<T>, E>(std::in_place_index<0>, std::forward<T>(succ));
}

template <typename E, typename T = void*> auto make_error(E&& err)
{
  return Result<T, std::decay_t<E>>(std::in_place_index<1>, std::forward<E>(err));
}

} // namespace meph::util
