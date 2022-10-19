int main() {
  static_assert(7 == ct::lift(7));
  ct::expect(7 == ct::lift(8));

  int const * const p = nullptr;
  ct::expect(
    not ct::lift(p) or *ct::lift(p) == 42);
  ct::expect(
    not ct::lift(p) or
    (*ct::lift(p) or false));









}