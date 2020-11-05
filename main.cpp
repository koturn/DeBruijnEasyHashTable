/*!
 * @brief バイナリDe Bruijn列とインデックステーブルを作成するプログラム
 * @author  koturn
 * @file    main.cpp
 */
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace
{

/*!
 * @brief 型引数に依らず常にfalseとなるメタ関数
 * @tparam Ts  型引数
 */
template <typename... Ts>
struct always_false
  : std::false_type
{};  // struct always_false


/*!
 * @brief always_false::value のエイリアスとなるコンパイル時定数
 * @see always_false
 * @tparam Ts  型引数
 */
template <typename... Ts>
inline constexpr bool always_false_v = always_false<Ts...>::value;


/*!
 * @brief 指定ビット数のバイナリDe Bruijn列を文字列として生成する
 * @param [in] n  ビット数
 * @return バイナリDe Bruijn列
 */
std::string
genDeBruijnSeqStr(int n) noexcept
{
  std::ostringstream iss;
  for (int i = 0; i < n; i++) {
    iss.put('0');
  }
  for (int i = n; i < (1 << n); i++) {
    const auto suffix = iss.str().substr(i - n + 1) + "1";
    if (iss.str().find(suffix) == std::string::npos) {
      iss.put('1');
    } else {
      iss.put('0');
    }
  }
  return iss.str();
}


/*!
 * @brief バイナリDe Bruijn列文字列を数値に変換する
 * @tparam T 返却値の型
 * @param [in] str  バイナリDe Bruijn列文字列
 * @return バイナリDe Bruijn列数値
 */
template <typename T>
constexpr T
convertBinStr(const std::string& str) noexcept
{
  static_assert(std::is_integral_v<T>, "[convertBinStr] Type parameter T must be integral");

  T n{};
  for (const auto& c : str) {
    n <<= 1;
    if (c == '1') {
      n |= 1;
    }
  }
  return n;
}


/*!
 * @brief 指定された数値の最上位ビットのみを残し，他のビットを0にする
 * @tparam T  xの型
 * @param [in] x  数値
 * @return 最上位ビット以外0となったx
 */
template <
  typename T,
  typename std::enable_if_t<
    std::is_integral_v<T> && std::is_unsigned_v<T>,
    std::nullptr_t
  > = nullptr
>
constexpr T
msb(T x) noexcept
{
  for (std::size_t shiftWidth = 1; shiftWidth < sizeof(T) * 8; shiftWidth <<= 1) {
    x = (x | (x >> shiftWidth));
  }
  return x ^ (x >> 1);
}


/*!
 * @brief 指定された数値の最上位ビットのみを残し，他のビットを0にする
 * @tparam T  xの型
 * @param [in] x  数値
 * @return 最上位ビット以外0となったx
 */
template <
  typename T,
  typename std::enable_if_t<
    std::is_integral_v<T> && std::is_signed_v<T>,
    std::nullptr_t
  > = nullptr
>
constexpr T
msb(T x) noexcept
{
  return msb(static_cast<std::make_signed_t<T>>(x));
}


/*!
 * @brief 整数型を除外するためのmsbのオーバーロード．実体化されたとき，常にコンパイルエラーとなる
 * @tparam T  xの型
 * @param [in] x  数値
 * @return 最上位ビット以外0となったx
 */
template <
  typename T,
  typename std::enable_if_t<!std::is_integral_v<T>, std::nullptr_t> = nullptr
>
constexpr T
msb(T x) noexcept
{
  static_assert(always_false_v<T>, "[msb] Type parameter T must be integral");
  return x;
}


/*!
 * @brief 最も最初に1が出現するビットのインデックスを得る
 * @tparam T  対象数値の型
 * @param [in] n  対象数値
 * @param [in] index  ビットインデックス
 * @retval n == 0 のとき，-1
 * @retval n != 0 のとき，最も最初に1が出現するビットのインデックス
 */
template <typename T>
constexpr int
bsf(T n, int index = 0) noexcept
{
  static_assert(std::is_integral_v<T>, "[bsf] Type parameter T must be integral");

  return n == 0 ? -1
    : ((n >> index) & 1) == 1 ? index
    : bsf(n, index + 1);
}


#if 0
template <
  typename T,
  typename std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr
>
T
constexpr int
msb(T n, int index = 0) noexcept
{
  return ((n >> index) & 1) == 1 ? index : bsf(n, index + 1);
}
#endif


#if 0
template <
  typename T,
  typename std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr
>
T
findBitPattern(T x, T p, T mask) noexcept
{
  do {

  } while ();
  x ^ p
}



template <
  typename T,
  typename std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr
>
T
genDeBruijnSeq() noexcept
{
  constexpr auto bitSize = sizeof(T) * 8
  // 8: 3
  // 16: 4
  // 32: 5
  // 64: 6
  constexpr auto nBits = bsf(bitSize);
  T seq{0};
  for (auto i = nBits; i < (1 << nBits); i++) {
    const auto suffix = T{(~T{} << (i - nBits + 1)) | (T{1} << (i - nBits + 2))};
    // const auto suffix = iss.str().substr(i - nBits + 1) + "1";
    if (iss.str().find(suffix) == std::string::npos) {
      iss.put('1');
    } else {
      iss.put('0');
    }
  }
  return iss.str();


  std::ostringstream iss;
  for (int i = 0; i < n; i++) {
    iss.put('0');
  }
  for (int i = n; i < (1 << n); i++) {
    const auto suffix = iss.str().substr(i - n + 1) + "1";
    if (iss.str().find(suffix) == std::string::npos) {
      iss.put('1');
    } else {
      iss.put('0');
    }
  }
  return iss.str();
}
#endif


/*!
 * @brief De Bruijn列を利用し，ハッシュ値を計算する
 * @tparam T  引数の型
 * @tparam std::enable_if_t<std::is_integral_v<T
 * @param [in] x  ハッシュ値計算対象値
 * @param [in] magic  De Bruijn列数値
 * @return ハッシュ値
 */
template <typename T>
T
calcHash(T x, T magic) noexcept
{
  static_assert(std::is_integral_v<T>, "[calcHash] Type parameter T must be integral");

  constexpr auto bitSize = sizeof(T) * 8;
  constexpr auto shiftWidth = bitSize - bsf(bitSize);

  return T{(x & -x) * magic} >> shiftWidth;
}


/*!
 * @brief C++のストリームで数値として印字可能な整数型を得るメタ関数
 * @tparam T 対象となる型
 * @tparam U デフォルトは std::int32_t
 * @tparam V デフォルトは std::uint32_t
 */
template <
  typename T,
  typename U = std::int32_t,
  typename V = std::make_unsigned_t<U>
>
struct to_printable_numeric_type
{
  static_assert(std::is_arithmetic_v<T>, "[to_printable_numeric_type] Type parameter T must be arithmetic");
  static_assert(std::is_integral_v<U>, "[to_printable_numeric_type] Type parameter U must be integral");
  static_assert(std::is_integral_v<V>, "[to_printable_numeric_type] Type parameter V must be integral");

  /*!
   * @brief C++のストリームで数値として印字可能な整数型
   */
  using type = std::conditional_t<
    !std::is_integral_v<T> || (sizeof(T) > 1),
    T,
    std::conditional_t<
      std::is_signed_v<T>,
      U,
      V
    >
  >;
};  // struct enlarge_to_64t


/*!
 * @brief to_printable_numeric_type::typeのエイリアス型
 * @see to_printable_numeric_type
 * @tparam T 対象となる型
 */
template <typename T>
using to_printable_numeric_type_t = typename to_printable_numeric_type<T>::type;


/*!
 * @brief C++のストリームで数値として印字可能な型にキャストする
 * @tparam T 引数xの型
 * @param [in] x キャストしたい型
 */
template <typename T>
decltype(auto)
printable_cast(T x) noexcept
{
  return static_cast<to_printable_numeric_type_t<T>>(x);
}


/*!
 * @brief De Bruijn列からインデックステーブルを作成する
 * @tparam T 対象の整数型
 * @return インデックステーブル
 */
template <typename T>
constexpr std::vector<int>
genDeBruijnHashTable() noexcept
{
  static_assert(std::is_integral_v<T>, "[genDeBruijnHashTable] Type parameter T must be integral");

  constexpr auto bitSize = sizeof(T) * 8;
  constexpr auto log2BitSize = bsf(bitSize);
  constexpr auto shiftWidth = bitSize - log2BitSize;

  std::cout << "=== table size: " << bitSize << " ===" << std::endl;
  std::cout << "log2BitSize = " << log2BitSize << "\n";
  std::cout << "shiftWidth = " << shiftWidth << "\n";

  const auto dbSeqStr = genDeBruijnSeqStr(log2BitSize);
  std::cout << "magic(bin) = 0b" << dbSeqStr << "\n";

  const auto magic = convertBinStr<T>(dbSeqStr);
  const auto coutFlags = std::cout.flags();
  std::cout << "magic(hex) = 0x" << std::hex << std::setw(sizeof(T) * 2) << std::setfill('0') << printable_cast(magic) << "\n";
  std::cout.flags(coutFlags);

  std::vector<std::pair<int, int>> vec{};
  vec.emplace_back(1, static_cast<int>(calcHash(T{1}, magic)));
  for (T i = 1; i < T{bitSize}; i++) {
    vec.emplace_back(static_cast<int>(i + 1), static_cast<int>(calcHash(T{T{1} << i}, magic)));
  }

  std::sort(
    std::begin(vec),
    std::end(vec),
    [](const auto& x, const auto& y) {
      return x.second < y.second;
    });

  std::vector<int> hashTable(vec.size());
  std::transform(
    std::cbegin(vec),
    std::cend(vec),
    std::begin(hashTable),
    [](const auto& e) {
      return e.first;
    });

  return hashTable;
}


/*!
 * @brief De Bruijn列とインデックステーブルを計算で求め，標準出力に出力する
 * @tparam T  対象となる型
 */
template <typename T>
void
execGen() noexcept
{
  static_assert(std::is_integral_v<T>, "[execGen] Type parameter T must be integral");

  const auto table = genDeBruijnHashTable<T>();
  std::cout << "table = [";
  if (!std::empty(table)) {
    std::copy(
      std::cbegin(table),
      std::prev(std::cend(table)),
      std::ostream_iterator<const int&>{std::cout, ", "});
    std::cout << *std::crbegin(table);
  }
  std::cout << "]\n" << std::endl;
}


/*!
 * @brief De Bruijn列とインデックステーブルを計算で求め，標準出力に出力する
 * @tparam T  対象となる型1
 * @tparam U  対象となる型2（再帰処理のために必要）
 * @tparam Us  残りの型
 */
template <
  typename T,
  typename U,
  typename... Us
>
void
execGen() noexcept
{
  execGen<T>();
  execGen<U, Us...>();
}


}  // namespace


/*!
 * @brief このプログラムのエントリポイント
 * @return  終了ステータス．0のみ．
 */
int
main()
{
  execGen<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>();
}
